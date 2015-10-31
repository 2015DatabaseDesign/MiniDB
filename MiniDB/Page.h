#pragma once
#include <iostream>
#include "Data.h"
#include <memory>
#include "PageException.h"
//class Tuple
//{
//	friend ostream &operator<<(ostream &os, const Tuple &t);
//private:
//	vector<Data*> datas;
//
//public:
//	Tuple(){}
//	Tuple(const vector<Data*>& _datas)
//		: datas(_datas) {}
//	Tuple(char *dataStart, const Table*);
//	void writeData(char* dataToWrite) const;
//	Data* getData(int index) const;
//	virtual ~Tuple();
//};
class Tuple
{
	friend ostream &operator<<(ostream &os, const Tuple &t);
	friend bool operator==(const Tuple& t0, const Tuple &t1);
private:
public:
	string toString()const;
	vector<shared_ptr<Data>> datas;
	Tuple() {}
	Tuple(const vector<shared_ptr<Data>>& _datas)
		: datas(_datas) {}
	Tuple(char *dataStart, const Table*);
	void writeData(char* dataToWrite, const Table *table) const;
	shared_ptr<Data> getData(int index) const;
	virtual ~Tuple();
};

class Page
{
private:
	char *data;
	const Table *tabledesc;
	int TupleNum;

public:
	Page() {}
	static const int PageSize = 4096;
	static int calcTupleNum(const Table *td);
	//record the number of tuples, 
	//it would be used frequently 
	int getTupleNum() const { return TupleNum; }

	//the first parameter 'data' comes from blockinfo->cBlock
	//the second parameter 'td' comes from Catalog manager
	Page(char *data,const Table *td)
	{
		this->data = data;
		this->tabledesc = td;
		TupleNum = calcTupleNum(td);
		for (int i = 0; i < TupleNum; i++)
		{
			this->SetHeader(i, 0);
		}
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