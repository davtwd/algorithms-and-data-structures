#pragma once

#include <Cli/routing/routing_table.h>
#include <DataStructures/libds/amt/explicit_hierarchy.h>
#include <DataStructures/libds/amt/hierarchy.h>

// heapmonitor
#include <DataStructures/libds/heap_monitor.h>

#define INVALID_OCTET 999

class HierarchyIterator {
private:
	ds::amt::MultiWayExplicitHierarchy<TreeNode>* addressHierarchy_;
	RoutingTable::TreeNodeBlock* currentBlock_;
	Address currentAddress_ = { INVALID_OCTET, INVALID_OCTET, INVALID_OCTET, INVALID_OCTET };
	unsigned int currentOctet_ = 0;
public:
	using PreOrderHierarchyIterator = ds::amt::MultiWayExplicitHierarchy<TreeNode>::PreOrderHierarchyIterator;

	HierarchyIterator(ds::amt::MultiWayExplicitHierarchy<TreeNode>& addressHierarchy);
	~HierarchyIterator();

	void showNextBlocks();
	void moveToChild(unsigned int index);
	void moveToParent();
	void addOctetToAddress(unsigned int octet);
	void removeOctetFromAddress();

	PreOrderHierarchyIterator begin() const { return {this->addressHierarchy_, this->currentBlock_}; };
	PreOrderHierarchyIterator end() const { return {this->addressHierarchy_, nullptr}; };
};