#include <fstream>
#include <iostream>

#include <Cli/lib/core/lib.h>
#include <Cli/routing/routing_table.h>

RoutingTable::RoutingTable() {
	this->addressHierarchy_.emplaceRoot().data_ = TreeNode();
}

void RoutingTable::loadRoutes(const char* filename) {
	
	std::ifstream file(filename);
	if (!file.is_open()) {
		std::cerr << "Error: could not open file " << filename << std::endl;
		return;
	}

	std::string line;
	std::getline(file, line); // skip the first line
	while (std::getline(file, line)) {

		if (line.empty()) continue;

		unsigned int network, prefix, nextHop, lifetime;
		size_t pos = 0;

		pos = line.find(';'); line.erase(0, pos + 1); // skip source
		pos = line.find('/'); network = Library::getDecFromAddress(line.substr(0, pos)); line.erase(0, pos + 1);
		pos = line.find(';'); prefix = std::stoi(line.substr(0, pos)); line.erase(0, pos + 1);
		pos = line.find(';'); line.erase(0, pos + 1); // skip metric
		pos = line.find(';'); nextHop = Library::getDecFromAddress(line.substr(4, pos - 4)); line.erase(0, pos + 1);
		lifetime = Library::parseTimeToSeconds(line);

		Route temp = {
			network,
			prefix,
			nextHop,
			lifetime
		};

		this->routes_.insertLast().data_ = temp;
	}

	file.close();
}

// print the route (reference)
void RoutingTable::printRoute(const Route& route) const {
	std::cout << Library::getAddressFromDec(route.network) << "/" << route.prefix << "\u0020"
		<< "via " << Library::getAddressFromDec(route.nextHop) << ",\u0020"
		<< Library::formatSecondsToTime(route.lifetime) << std::endl;
}

// print the route (pointer)
void RoutingTable::printRoute(const Route* route) const {
	std::cout << Library::getAddressFromDec(route->network) << "/" << route->prefix << "\u0020"
		<< "via " << Library::getAddressFromDec(route->nextHop) << ",\u0020"
		<< Library::formatSecondsToTime(route->lifetime) << std::endl;
}

// print the routes (reference)
void RoutingTable::printRoutes(ds::amt::ImplicitSequence<Route>& routes) const {
	for (auto& route : routes) {
		this->printRoute(route);
	}
}

// print the routes (pointer)
void RoutingTable::printRoutes(ds::amt::ImplicitSequence<Route*>& routes) const {
	for (auto& route : routes) {
		this->printRoute(route);
	}
}

// print all routes from the implicit sequence of routes
void RoutingTable::printAllRoutes() {
	for (auto& route : this->routes_) {
		this->printRoute(route);
	}
}

// ---------

void RoutingTable::createAddressHierarchy() {
	for (auto& route : this->routes_) {
		this->addAddressToHierarchy(route);
	}
}

void RoutingTable::addAddressToHierarchy(Route& route) {

	TreeNodeBlock* currentBlock = this->addressHierarchy_.accessRoot();
	Address network = Library::getNetworkAddress(route.network);
	static size_t leafIndex = 3;

	for (size_t i = 0; i < 4; ++i) {

		TreeNodeBlock* nextBlock = nullptr;
		unsigned int octet = network.operator[](i);
		const bool isLeaf = (leafIndex == i); // check if the current node is a leaf

		if (this->addressHierarchy_.degree(*currentBlock) > 0) {
			const auto predicate = [&octet](ImplicitSequenceBlock* currentBlock) {
				return currentBlock->data_->data_.getCurrentOctet() == octet;
			};

			ImplicitSequenceBlock* child = currentBlock->sons_->findBlockWithProperty(predicate);
			if (child) {
				nextBlock = child->data_;
			}
		}

		if (nextBlock == nullptr) {
			nextBlock = &this->addressHierarchy_.emplaceSon(*currentBlock, currentBlock->sons_->size());
			nextBlock->data_ = TreeNode(octet, isLeaf);
		}

		if (isLeaf) {
			nextBlock->data_.insertRoute(&route);
		}

		currentBlock = nextBlock;
	}
} // REAAAAAAAAAAAAAAL

// ---------

void RoutingTable::createNextHopTable() {
	for (auto& route : this->routes_) {
		this->table_.insert(&route);
	}
}

bool RoutingTable::tryFindInTable(const unsigned int key, NextHopTable::List& data) const {
	return this->table_.tryFind(key, data);
}