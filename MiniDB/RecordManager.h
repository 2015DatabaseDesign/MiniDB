#ifndef __RECORDMANAGER_H__
#define __RECORDMANAGER_H__
#define JUMPLENGTH	28
#include <vector>
#include <string>
#include <deque>
//hey man test github
#include "Page.h"
#include "BufferManager.h" 
#include "RecordManagerException.h"
#include "DBException.h"

using namespace std;

enum FileType { DataFile, IndexFile };

class Condition
{
private:
	Op op;
	shared_ptr<Data> operand;
	int IndexofData;

public:
	bool operator<(const Condition& c) {
		if (op == EQUALS)
			return true;
		if (c.getOp() == EQUALS)
			return false;
	}
	Condition(Op _op, shared_ptr<Data> _operand, int _IndexofData)
		:op(_op), operand(_operand), IndexofData(_IndexofData) {}

	//check if tuple matches the predicate
	bool match(const Tuple& t) const
	{
		shared_ptr<Data> data = t.getData(IndexofData);
		return data->compare(op, operand.get());
	}

	Op getOp() const { return op; }

	DataType getDataType() const { return DataType(operand->getType()); }

	int getIndexOfData() const { return IndexofData; }

	Data* getData() const { return operand.get(); }
};

enum LinkOp{AND, OR};


class Operation {
protected:
	string DBName;
	string TableName;
	const Table *tableDesc;
	Tuple nextTuple;
public:
	int CurrentIndex;
	int givenLineNumIndex;
	blockInfo *CurrentBlock;
	Operation() {}
	Operation(string _DBName, string _TableName, const Table* _tabledesc)
		:DBName(_DBName), TableName(_TableName), tableDesc(_tabledesc), CurrentIndex(-1), givenLineNumIndex(-1), CurrentBlock(NULL) {}
	virtual const Tuple &next() const;
	virtual bool hasNext();
	virtual ~Operation() = 0 {};
};

class Selector :public Operation {
public:
	Selector() {}
	Selector(string _DBName, string _TableName, const Table* _tabledesc,  const vector<Condition>& _conds, const vector<int>& _LineNums, LinkOp _lop)
		:Operation(_DBName, _TableName, _tabledesc ), conds(_conds), LineNums(_LineNums), lop(_lop) {}

	Selector(string _DBName, string _TableName, const Table* _tabledesc)
		:Operation(_DBName, _TableName, _tabledesc){
		lop = LinkOp::AND;
	}

	virtual const Tuple &next() const;
	virtual bool hasNext() ;
	virtual ~Selector() {};
private:
	vector<Condition> conds;
	vector<int> LineNums;
	LinkOp lop;
};

class Deleter :public Operation {
public:
	Deleter() {}
	Deleter(string _DBName, string _TableName, const Table* _tabledesc)
		:Operation(_DBName, _TableName, _tabledesc), lop(LinkOp::AND){}
	Deleter(string _DBName, string _TableName, const Table* _tabledesc, const vector<Condition> &_conds, const vector<int> &_LineNums, LinkOp _lop)
		:Operation(_DBName, _TableName, _tabledesc), conds(_conds), LineNums(_LineNums), lop(_lop) {}
	virtual const Tuple &next() const;
	virtual bool hasNext();
	virtual ~Deleter() {};
private:
	vector<Condition> conds;
	vector<int> LineNums;
	LinkOp lop;
};
 

class RecordManager
{
public:

	//DBName -- name of database
	//TableName -- name of table
	//tableDesc -- a point to Table object which contains imformations for reading datas
	//t -- tuple to be inserted
	int Insert(const string& DBName, const string& TableName, Table* tableDesc, const Tuple& t);
	
	//check if there are mutiple tuples which should be unique
	//return false if there is a tuple with same value for same attribute as the target tuple
	//unique attribute and their values are stored in conditions
	bool CheckUnique(const string& DBName, const string& TableName, Table* tableDesc, const vector<Condition>& conditions);
};

#endif