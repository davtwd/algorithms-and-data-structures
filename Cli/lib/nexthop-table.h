#pragma once

#include <Cli/lib/core/lib.h>
#include <DataStructures/libds/adt/list.h>
#include <DataStructures/libds/adt/table.h>

class NextHopTable {
public:
	using List = ds::adt::ImplicitList<Route*>;
	using Table = ds::adt::Treap<unsigned int, List*>;
	using TableIteratorType = Table::IteratorType;
private:
	Table* table_;
public:
	NextHopTable() : table_(new Table()) {}
	~NextHopTable();
	void insert(Route* route);
	bool tryFind(unsigned int key, List& data) const;
	TableIteratorType begin() const { return this->table_->begin(); }
	TableIteratorType end() const { return this->table_->end(); }
};