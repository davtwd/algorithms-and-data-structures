#pragma once

#include <string>

#include <Cli/lib/core/structs.h>
#include <Cli/lib/nexthop-table.h>
#include <Cli/lib/tree_node.h>
#include <DataStructures/libds/adt/sorts.h>
#include <DataStructures/libds/amt/implicit_sequence.h>

// heapmonitor
#include <DataStructures/libds/heap_monitor.h>

class RoutingTable {
public:
	using TreeNodeBlock = ds::amt::MultiWayExplicitHierarchy<TreeNode>::BlockType;
	using ImplicitSequenceBlock = ds::amt::ImplicitSequence<TreeNodeBlock*>::BlockType;
private:
	ds::amt::MultiWayExplicitHierarchy<TreeNode> addressHierarchy_;
	ds::amt::ImplicitSequence<Route> routes_;
	NextHopTable table_;
public:

	// Constructor
	RoutingTable();

	// Core functionality
	void loadRoutes(const char* filename);
	void printAllRoutes();
	void printRoute(const Route& route) const;
	void printRoute(const Route* route) const;
	void printRoutes(ds::amt::ImplicitSequence<Route>& routes) const;
	void printRoutes(ds::amt::ImplicitSequence<Route*>& routes) const;
	auto getRoutes() { return this->routes_; }

	// Iterators
	auto begin() { return this->routes_.begin(); }
	auto end() { return this->routes_.end(); }

	// Address Hierarchy
	void createAddressHierarchy();
	void addAddressToHierarchy(Route& route);
	ds::amt::MultiWayExplicitHierarchy<TreeNode>& getAddressHierarchy() { return this->addressHierarchy_; }

	// Next-Hop Table
	void createNextHopTable();
	bool tryFindInTable(const unsigned int key, NextHopTable::List& data) const;

	// Universal Filter Algorithm
	template<typename Iterator, typename Predicate, typename T>
	void matchWith(
		Iterator begin,
		Iterator end,
		Predicate& predicate,
		ds::amt::ImplicitSequence<T>& matchedResults
	) {
		for (Iterator it = begin; it != end; ++it) {
			if (predicate(*it)) {
				matchedResults.insertLast().data_ = *it;
			}
		}
	}

	// Universal Sorting Algorithm (using HeapSort)
	template<typename Comparator, typename T>
	void compare(
		Comparator& compare,
		ds::amt::ImplicitSequence<T>& is
	) {
		if (is.size() < 2) return;	// check if there is anything to sort
		ds::adt::HeapSort<T>().sort(is, compare); // merge sort can be used as well
	}
};