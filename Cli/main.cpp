#include <iostream>
#include <map>

#include <Cli/lib/dsh/dsm.h>
#include <Cli/lib/dsh/dst.h>
#include <Cli/routing/routing_table.h>

// heapmonitor
#include <DataStructures/libds/heap_monitor.h>

#ifndef ANALYZER_OUTPUT_ENABLED
#define ANALYZER_OUTPUT_ENABLED false
#endif

#define INIT_HEAPMONITOR false

// create tests for DataStructures
static std::vector<std::unique_ptr<ds::tests::Test>> createTests() {

	auto root = std::make_unique<ds::tests::CompositeTest>("root");
	auto mm = std::make_unique<ds::tests::CompositeTest>("mm");
	auto amt = std::make_unique<ds::tests::CompositeTest>("amt");
	auto adt = std::make_unique<ds::tests::CompositeTest>("adt");

	// 01 - MemoryManager
	mm->add_test(std::make_unique<ds::tests::MemoryManagerTest>());

	// 02 - CompactMemoryManager
	mm->add_test(std::make_unique<ds::tests::CompactMemoryManagerTest>());

	// 03 - ImplicitSequence
	amt->add_test(std::make_unique<ds::tests::ImplicitSequenceTest>());

	// 04 - ExplicitSequence
	amt->add_test(std::make_unique<ds::tests::ExplicitSequenceTest>());

	// 05 -	ImplicitHierarchy
	amt->add_test(std::make_unique<ds::tests::ImplicitHierarchyTest>());

	// 06 - ExplicitHierarchy
	amt->add_test(std::make_unique<ds::tests::ExplicitHierarchyTest>());
	amt->add_test(std::make_unique<ds::tests::HierarchyTest>());

	// 07 - Lists, Arrays
	adt->add_test(std::make_unique<ds::tests::ListTest>());
	adt->add_test(std::make_unique<ds::tests::ArraysTest>());

	// 08 - Stacks, Queues, PriorityQueues
	adt->add_test(std::make_unique<ds::tests::StackTest>());
	adt->add_test(std::make_unique<ds::tests::QueueTest>());
	adt->add_test(std::make_unique<ds::tests::PriorityQueueTest>());

	// 09 - Tables
	adt->add_test(std::make_unique<ds::tests::SequenceTableTest>());

	// 10 - NonSequenceTable
	adt->add_test(std::make_unique<ds::tests::NonSequenceTableTest>());

	// 11 - Sort
	adt->add_test(std::make_unique<ds::tests::SortTest>());

	root->add_test(std::move(mm));
	root->add_test(std::move(amt));
	root->add_test(std::move(adt));
	std::vector<std::unique_ptr<ds::tests::Test>> tests;
	tests.emplace_back(std::move(root));
	return tests;
}

static void runDSTests() {

	std::cout << "Running datastructures tests..." << std::endl << std::endl;

	std::vector<std::unique_ptr<ds::tests::Test>> tests = createTests();
	dst::runTests(tests);
	dst::printResults(tests);

	#if ANALYZER_OUTPUT_ENABLED

	std::vector<std::unique_ptr<ds::utils::Analyzer>> analyzers = dst::createAnalyzers();	
	std::cout << std::endl << "Running analyzers..." << std::endl;

	dst::setAnalyzersParameters(analyzers, ANALYZER_OUTPUT);
	dst::runAnalyzers(analyzers);

	std::cout << "Analyzers finished." << std::endl;
	#endif
}

int main() {

	#if INIT_HEAPMONITOR
	initHeapMonitor();
	#endif

	RoutingTable rt;
	rt.loadRoutes("resources/RT.csv");
	rt.createAddressHierarchy();

	std::map<char, std::function<void(RoutingTable&)>> options = {
		{'a', [](RoutingTable& rt) { rt.printAllRoutes(); }},
		{'b', dsm::printRoutesWithAddress},
		{'c', dsm::printRoutesWithLifetime},
		{'d', dsm::runHierarchyTraversal},
		{'e', dsm::runNextHopTable},
		{'t', [](RoutingTable& rt) { runDSTests(); }}
	};

	char input = '\0';
	while (input != 'q') {

		// print the title 
		std::cout << "      _              _  _    " << std::endl;
		std::cout << "     | |            | |(_)   " << std::endl;
		std::cout << "   __| | ___    ___ | | _    " << std::endl;
		std::cout << "  / _` |/ __|  / __|| || |   " << std::endl;
		std::cout << " | (_| |\\__ \\ | (__ | || | " << std::endl;
		std::cout << "  \\__,_||___/  \\___||_||_| " << std::endl;
		std::cout << std::endl;

		// print menu
		std::cout << "a - print all routes" << std::endl;
		std::cout << "b - print routes that match with address" << std::endl;
		std::cout << "c - print routes that match with lifetime" << std::endl;
		std::cout << "d - traversal in a hierarchy of routes" << std::endl;
		std::cout << "e - print next-hop addresses from the table" << std::endl;
		std::cout << "t - run datastructures tests" << std::endl;
		std::cout << "q - quit" << std::endl;
		std::cout << "Input: ";
		std::cin >> input;
		std::cout << std::endl; // break line for formatting

		// execute the option
		auto it = options.find(input);
		if (it != options.end()) {
			it->second(rt);
			std::cout << std::endl;
		} else if (input != 'q') {
			std::cout << "Invalid input." << std::endl << std::endl;
		}
	}

	// end of program
	std::cout << "Fin.";
	return 0;
}