#pragma once
#include <iostream>
#include "Data.h"
class Tuple
{
	friend ostream &operator<<(ostream &os, const Tuple &t);
private:
	vector<Data*> datas;

public:
	Tuple(){}
	Tuple(const vector<Data*>& _datas)
		: datas(_datas) {}
	Tuple(char *dataStart, const Table*);
	void writeData(char* dataToWrite) const;
	Data* getData(int index) const;
	virtual ~Tuple();
};

class Page
{
private:
	static const int PageSize = 4096;
	char *data;
	const Table *tabledesc;

public:
	static int getTupleNum(const Table *td);
	//record the number of tuples, 
	//it would be use frequently so I set it to be a public member
	int TupleNum;

	//the first parameter 'data' comes from blockinfo->cBlock
	//the second parameter 'td' comes from Catalog manager
	Page(char *data,const Table *td)
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
	Tuple ReadTuple(int index);

	//set certain position of header to be set or reset
	void SetHeader(int index, bool isSet);
};