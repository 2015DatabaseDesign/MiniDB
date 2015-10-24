#ifndef __RECORDMANAGER_H__
#define __RECORDMANAGER_H__

#include <vector>
#include <string>
#include "Data.h"

using namespace std;

enum FileType { DataFile, IndexFile };

class Tuple
{
private:
	vector<Data*> datas;

public:
	Tuple(char *dataStart, Table*);
	void writeData(char* dataToWrite) const;
	Data* getData(int index) const;
	virtual ~Tuple();
};

class Page
{
	static const int PageSize = 4096;
private:
	char *data;
	Table *tabledesc;

public:
	//record the number of tuples, 
	//it would be use frequently so I set it to be a public member
	int TupleNum;

	//the first parameter 'data' comes from blockinfo->cBlock
	//the second parameter 'td' comes from Catalog manager
	Page(char *data, Table *td)
	{
		this->data = data;
		this->tabledesc = td;
	}

	//check if specific tuple is valid
	bool isTupleValid(int index);

	//check if page is insertable, 
	//if it is, return the index of the invalid tuple
	//else return -1
	int InsertableIndex();

	//insert Tuple
	//index is the position to insert
	void WriteTuple(const Tuple&, int index);

	//read a tuple specified by index
	Tuple& ReadTuple(int index);

	//set certain position of header to be set or reset
	void SetHeader(int index, bool isSet);
};

enum Op { EQUALS, GREATER_THAN, GREATE_THAN_OR_EQUAL, LESS_THAN,  LESS_THAN_OR_EQUAL, NOT_EQUAL };

class Condition
{
private:
	Op op;
	Data *operand;
	int IndexofData;

public:
	Condition(Op _op, Data* _operand, int _IndexofData)
		:op(_op), operand(_operand), IndexofData(_IndexofData) {}

	//check if tuple matches the predicate
	bool match(const Tuple& t) const
	{
		Data* data = t.getData(IndexofData);
		switch (data->getType)
		{
		case DataType::INT: {
			int Integer;
			memcpy(&Integer, data->getValue(), sizeof(int));
			data = new Int(Integer);
			break;
		}
		case DataType::FLOAT: {
			float FloatNumber;
			memcpy(&FloatNumber, data->getValue(), sizeof(float));
			data = new Float(FloatNumber);
			break;
		}
		case DataType::CHAR: {
			string str;
			memcpy(&str, data->getValue(), sizeof(string));
			data = new Char(str);
			break;
		}
		}
	}

	Op getOp() const { return op; }

	DataType getDataType() const { return DataType(operand->getType()); }
};

class RecordManager
{

public:

	//DBName -- name of database
	//TableName -- name of table
	//tableDesc -- a point to Table object which contains imformations for reading datas
	//t -- tuple to be inserted
	void Insert(const string& DBName, const string& TableName, Table* tableDesc, const Tuple& t);

	//DBName -- name of database
	//TableName -- name of table
	//tableDesc -- a point to Table object which contains imformations for reading datas
	//predicate -- a predicate used to filter tuples
	//return a vector of tuple which matches
	void Select(const string& DBName, const string& TableName, Table* tableDesc, const Condition& condition);
	void SelectWithIndex(const string& DBName, const string& TableName, Table* tableDesc, const Condition& condition, struct index_info& inform);
	void Delete(const string& DBName, const string& TableName, Table* tableDesc, const Condition& condition);
	void DeleteWithIndex(const string& DBName, const string& TableName, Table* tableDesc, const Condition& condition, struct index_info& inform);
};

#endif