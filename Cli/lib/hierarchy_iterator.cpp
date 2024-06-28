#include "hierarchy_iterator.h"

HierarchyIterator::HierarchyIterator(ds::amt::MultiWayExplicitHierarchy<TreeNode>& addressHierarchy) {
	this->addressHierarchy_ = &addressHierarchy;
	this->currentBlock_ = addressHierarchy.accessRoot();
}

HierarchyIterator::~HierarchyIterator() {
	this->addressHierarchy_ = nullptr;
	this->currentBlock_ = nullptr;
}

void HierarchyIterator::showNextBlocks() {

	std::cout << "Your current position : "
		<< (this->currentAddress_.octetA == INVALID_OCTET ? "x" : std::to_string(this->currentAddress_.octetA)) << "."
		<< (this->currentAddress_.octetB == INVALID_OCTET ? "x" : std::to_string(this->currentAddress_.octetB)) << "."
		<< (this->currentAddress_.octetC == INVALID_OCTET ? "x" : std::to_string(this->currentAddress_.octetC)) << "."
		<< (this->currentAddress_.octetD == INVALID_OCTET ? "x" : std::to_string(this->currentAddress_.octetD)) << std::endl;

	if (this->addressHierarchy_->isLeaf(*this->currentBlock_)) {
		std::cout << "Leaf node." << std::endl;
		return;
	}

	size_t count = 0; // for formatting
	std::cout << "Next available nodes: " << std::endl;
	this->currentBlock_->sons_->processBlocksForward(this->currentBlock_->sons_->accessFirst(), [&count, this](const auto* block) {
		std::cout << std::setw(3) << std::setfill(' ') << block->data_->data_.getCurrentOctet() << " ";
		if (++count % 10 == 0 && count != currentBlock_->sons_->size()) std::cout << std::endl;
	});

	std::cout << std::endl; // break line for formatting

	if (this->currentBlock_->parent_ != nullptr) {
		std::cout << "Parent available." << std::endl;
	}
}

void HierarchyIterator::moveToChild(unsigned int index) {
	
	if (this->addressHierarchy_->isLeaf(*this->currentBlock_)) {
		std::cout << "Leaf node, cannot move to " << index << "." << std::endl;
		return;
	}

	const auto predicate = [&index](const RoutingTable::ImplicitSequenceBlock* currentBlock) {
		return currentBlock->data_->data_.getCurrentOctet() == index;
	};

	if (this->currentBlock_->sons_->findBlockWithProperty(predicate) == nullptr) {
		std::cout << "Node " << index << " not found." << std::endl;
		return;
	}

	RoutingTable::TreeNodeBlock* nextBlock = this->currentBlock_->sons_->findBlockWithProperty(predicate)->data_;

	this->currentBlock_ = nextBlock;
	this->addOctetToAddress(index);
	std::cout << "Moved to " << index << "." << std::endl;
	std::cout << std::endl; // break line for formatting
}

void HierarchyIterator::moveToParent() {
	
	if (this->currentBlock_->parent_ == nullptr) {
		std::cout << "No parent available." << std::endl;
		return;
	}

	this->removeOctetFromAddress();
	this->currentBlock_ = this->addressHierarchy_->accessParent(*currentBlock_);
	std::cout << "Moved to parent." << std::endl;
}

void HierarchyIterator::addOctetToAddress(unsigned int octet) {

	if (this->currentOctet_ == 4) {
		std::cout << "Address is already complete." << std::endl;
		return;
	}

	switch (this->currentOctet_++) {
		case 0: {
			this->currentAddress_.octetA = octet;
			break;
		} case 1: {
			this->currentAddress_.octetB = octet;
			break;
		} case 2: {
			this->currentAddress_.octetC = octet;
			break;
		} case 3: {
			this->currentAddress_.octetD = octet;
			break;
		}
	}
}

void HierarchyIterator::removeOctetFromAddress() {

	if (this->currentOctet_ == 0) {
		std::cout << "Address is already empty." << std::endl;
		return;
	}

	switch (this->currentOctet_--) {
		case 0: {
			this->currentAddress_.octetA = INVALID_OCTET;
			break;
		} case 1: {
			this->currentAddress_.octetB = INVALID_OCTET;
			break;
		} case 2: {
			this->currentAddress_.octetC = INVALID_OCTET;
			break;
		} case 3: {
			this->currentAddress_.octetD = INVALID_OCTET;
			break;
		}
	}
}