#pragma once

#include <Cli/lib/core/structs.h>
#include <DataStructures/libds/adt/list.h>

// heapmonitor
#include <DataStructures/libds/heap_monitor.h>

class TreeNode {
private:
	bool isLeaf_;
	unsigned int currentOctet_;
	ds::adt::ImplicitList<Route*>* routesList_;
public:
	
	// Constructors
	TreeNode();
	TreeNode(unsigned int currentOctet, bool isLeaf);
	TreeNode(const TreeNode& other);
	~TreeNode();

	// Methods
	unsigned int getCurrentOctet() const;
	void insertRoute(Route* route);
	bool isLeaf() const;

	// Operators
	bool operator==(const TreeNode& other) const;
	bool operator!=(const TreeNode& other) const;
	TreeNode& operator=(const TreeNode& other);
	TreeNode& operator=(TreeNode&& other) noexcept;

	// Iterators
	auto begin() { return this->routesList_->begin(); }
	auto end() { return this->routesList_->end(); }
};