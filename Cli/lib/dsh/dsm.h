#pragma once

#include <iostream>
#include <Cli/lib/core/lib.h>
#include <Cli/routing/routing_table.h>
#include <Cli/lib/hierarchy_iterator.h>


namespace dsm {

	// methods
	void printRoutesWithAddress(RoutingTable& rt);
	void printRoutesWithLifetime(RoutingTable& rt);
	void runHierarchyTraversal(RoutingTable& rt);
	void runNextHopTable(RoutingTable& rt);

	// helper methods
	void matchSubHierarchyWithAddress(RoutingTable& rt, HierarchyIterator& hit);
	void matchSubHierarchyWithLifetime(RoutingTable& rt, HierarchyIterator& hit);
}