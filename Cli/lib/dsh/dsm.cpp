#include <map>
#include <Cli/lib/dsh/dsm.h>

namespace dsm {

    void printRoutesWithAddress(RoutingTable& rt) {

        std::string searchAddress;
        std::cout << "Type the address (e.g., 1.1.135.254): ";
        std::cin >> searchAddress;

        if (!Library::isValidAddress(searchAddress)) {
			std::cout << "Invalid address." << std::endl;
			return;
		}

        ds::amt::ImplicitSequence<Route> matchedResults;
        auto fun = [&searchAddress](const Route& route) -> bool {
            return Library::matchWithAddress(route, searchAddress);
        };

        rt.matchWith(rt.begin(), rt.end(), fun, matchedResults);
        rt.compare(Library::getAddressComparator(), matchedResults);
        rt.printRoutes(matchedResults);
    }

    void printRoutesWithLifetime(RoutingTable& rt) {

        std::string from, to;
        std::cout << "Type the lifetime range (e.g., 2w 2w6d): ";
        std::cin >> from >> to;

        if (!Library::isValidLifetime(from) || !Library::isValidLifetime(to)) {
            std::cout << "Invalid time format." << std::endl;
            return;
        }

        unsigned int searchFrom = Library::parseTimeToSeconds(from), searchTo = Library::parseTimeToSeconds(to);

        ds::amt::ImplicitSequence<Route> matchedResults;
        auto fun = [&searchFrom, &searchTo](const Route& route) -> bool {
            return Library::matchWithLifetime(route, searchFrom, searchTo);
        };

        rt.matchWith(rt.begin(), rt.end(), fun, matchedResults);
        rt.compare(Library::getLifetimeComparator(), matchedResults);
        rt.printRoutes(matchedResults);
    }

    /*
        TODO:
        - implement hierarchy iterator
    */

    void runHierarchyTraversal(RoutingTable& rt) {
    
        ds::amt::ImplicitSequence<Route*> list;
        HierarchyIterator hit(rt.getAddressHierarchy());

        std::map<char, std::function<void()>> options = {
            {'a', [&hit]() { hit.moveToParent(); hit.showNextBlocks(); }},
            {'b', [&hit]() {
                unsigned int child;
                std::cout << "Type the child value: ";
                std::cin >> child;
                hit.moveToChild(child);
                hit.showNextBlocks();
            }},
            {'c', [&hit]() { hit.showNextBlocks(); }},
            {'d', [&rt, &hit]() { matchSubHierarchyWithAddress(rt, hit); }},
            {'e', [&rt, &hit]() { matchSubHierarchyWithLifetime(rt, hit); }}
        };

        char input = '\0';
        while (input != 'q') {

            // print the title
            std::cout << "  _     _                         _                " << std::endl;
            std::cout << " | |   (_)                       | |               " << std::endl;
            std::cout << " | |__  _  ___ _ __ __ _ _ __ ___| |__  _   _      " << std::endl;
            std::cout << " | '_ \\| |/ _ \\ '__/ _` | '__/ __| '_ \\| | | |  " << std::endl;
            std::cout << " | | | | |  __/ | | (_| | | | (__| | | | |_| |     " << std::endl;
            std::cout << " |_| |_|_|\\___|_|  \\__,_|_|  \\___|_| |_|\\__, | " << std::endl;
            std::cout << "                                         __/ |     " << std::endl;
            std::cout << "                                        |___/      " << std::endl;

            // print menu
            std::cout << "+> a - move to parent" << std::endl;
            std::cout << "+> b - move to child" << std::endl;
            std::cout << "+> c - show next available blocks" << std::endl;
            std::cout << "+> d - print routes that match with address (subhierarchy)" << std::endl;
            std::cout << "+> e - print routes that match with lifetime (subhierarchy)" << std::endl;
            std::cout << "+> q - quit to main menu" << std::endl;
            std::cout << "Input: ";
            std::cin >> input;
            std::cout << std::endl; // break line for formatting

            auto it = options.find(input);
            if (it != options.end()) {
				it->second();
				std::cout << std::endl;
            } else if (input != 'q') {
				std::cout << "Invalid input." << std::endl << std::endl;
			}
        }

        std::cout << "Quitting..." << std::endl;
    }

    void runNextHopTable(RoutingTable& rt) {
    	
        rt.createNextHopTable();

        std::string input;
        std::cout << "Type the address (e.g., 203.181.248.195): " << std::endl;
        std::cin >> input;
        
        if (!Library::isValidAddress(input)) {
            std::cout << "Invalid address." << std::endl;
            return;
        }

        unsigned int address = Library::getDecFromAddress(input);
        NextHopTable::List results;
        if (!rt.tryFindInTable(address, results)) {
            std::cout << "Next-hop address not found." << std::endl;
			return;
        }

        for (auto& route : results) { // cannot use printRoutes because it is a list not a sequence
            rt.printRoute(route);
        }
    }

    // helper methods

    void matchSubHierarchyWithAddress(RoutingTable& rt, HierarchyIterator& hit) {
    	
        std::string searchAddress;
        std::cout << "Type the address (e.g., 1.1.135.254): ";
        std::cin >> searchAddress;

        if (!Library::isValidAddress(searchAddress)) {
            std::cout << "Invalid address." << std::endl;
            return;
        }

        ds::amt::ImplicitSequence<TreeNode> temp;
        auto funcNodes = [&rt, &searchAddress](TreeNode& node) -> bool {

            if (!node.isLeaf()) return false;

            ds::amt::ImplicitSequence<Route*> matchedResults;
            auto funcRoutes = [&searchAddress](Route* route) -> bool {
                return Library::matchWithAddress(*route, searchAddress);
            };

            rt.matchWith(node.begin(), node.end(), funcRoutes, matchedResults);
            rt.compare(Library::getAddressComparatorPtr(), matchedResults);
            rt.printRoutes(matchedResults);

            return false;
        };

        rt.matchWith(hit.begin(), hit.end(), funcNodes, temp);
    }

    void matchSubHierarchyWithLifetime(RoutingTable& rt, HierarchyIterator& hit) {
    
        std::string from, to;
        std::cout << "Type the lifetime range (e.g., 2w 2w6d): ";
        std::cin >> from >> to;

        if (!Library::isValidLifetime(from) || !Library::isValidLifetime(to)) {
            std::cout << "Invalid time format." << std::endl;
            return;
        }

        unsigned int searchFrom = Library::parseTimeToSeconds(from), searchTo = Library::parseTimeToSeconds(to);

        ds::amt::ImplicitSequence<TreeNode> temp;
        ds::amt::ImplicitSequence<Route*> matchedResults;

        auto funcNodes = [&rt, &searchFrom, &searchTo, &matchedResults](TreeNode& node) -> bool {

            if (!node.isLeaf()) return false;

            auto funcRoutes = [&searchFrom, &searchTo](Route* route) -> bool {
                return Library::matchWithLifetime(*route, searchFrom, searchTo);
            };

            rt.matchWith(node.begin(), node.end(), funcRoutes, matchedResults);
            return false;
        };

        rt.matchWith(hit.begin(), hit.end(), funcNodes, temp);

        rt.compare(Library::getLifetimeComparatorPtr(), matchedResults);
        rt.printRoutes(matchedResults);
    }

} // namespace datastructures menu