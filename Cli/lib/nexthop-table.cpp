#include <Cli/lib/nexthop-table.h>

NextHopTable::~NextHopTable() {

	for (ds::adt::TableItem<unsigned int, List*>& item : *this->table_) {
		delete item.data_;
		item.data_ = nullptr;
	}

	delete this->table_;
	this->table_ = nullptr;
}

void NextHopTable::insert(Route* route) {

	List** list = nullptr;

	if (this->table_->tryFind(route->nextHop, list)) {
		(*list)->insertFirst(route);
	} else {
		List* otherList = new List();
		otherList->insertFirst(route);
		this->table_->insert(route->nextHop, otherList);
	}
}

bool NextHopTable::tryFind(unsigned int key, List& data) const {

	List** list = nullptr;

	if (!this->table_->tryFind(key, list)) {
		return false;
	}

	data.assign(**list);
	return true;
}
