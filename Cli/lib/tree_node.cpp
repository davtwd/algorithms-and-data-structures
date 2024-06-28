#include <Cli/lib/tree_node.h>

// Constructors

TreeNode::TreeNode() : TreeNode(0, false) {}

TreeNode::TreeNode(unsigned int octet, bool isLeaf)
	: routesList_(isLeaf ? new ds::adt::ImplicitList<Route*> : nullptr), currentOctet_(octet) {}

TreeNode::TreeNode(const TreeNode& other) 
	: routesList_(other.routesList_ ? new ds::adt::ImplicitList<Route*>(*other.routesList_) : nullptr), currentOctet_(other.currentOctet_) {}

TreeNode::~TreeNode() {
	delete this->routesList_;
	this->currentOctet_ = 0;
	this->routesList_ = nullptr;
}

// Methods

unsigned int TreeNode::getCurrentOctet() const {
	return this->currentOctet_;
}

void TreeNode::insertRoute(Route* route) {
	if (this->isLeaf()) {
		this->routesList_->insertLast(route);
	}
}

bool TreeNode::isLeaf() const {
	return this->routesList_ != nullptr;
}

// Operators

bool TreeNode::operator==(const TreeNode& other) const {
	return this->currentOctet_ == other.currentOctet_ && this->routesList_->equals(*other.routesList_);
}

bool TreeNode::operator!=(const TreeNode& other) const {
	return this->currentOctet_ != other.currentOctet_ && !this->routesList_->equals(*other.routesList_);
}

TreeNode& TreeNode::operator=(const TreeNode& other) {
	if (this != &other) {
		delete this->routesList_;
		this->currentOctet_ = other.currentOctet_;
		this->routesList_ = other.routesList_ ? new ds::adt::ImplicitList<Route*>(*other.routesList_) : nullptr;
	}
	return *this;
}

TreeNode& TreeNode::operator=(TreeNode&& other) noexcept {
	if (this != &other) {
		delete this->routesList_;
		this->currentOctet_ = other.currentOctet_;
		this->routesList_ = other.routesList_;
		other.routesList_ = nullptr;
	}
	return *this;
}