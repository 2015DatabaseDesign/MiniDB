/*
* Data.h
*
*  Created on: 2014å¹?10æœ?31æ—?
*      Author: darren
*/
#ifndef RECORDMANAGER_DATA_H_
#define RECORDMANAGER_DATA_H_

#include <iostream>
#include <iomanip>
#include <string>
#include <vector>
#include "DBException.h" //add by hedejin 10/28
using namespace std;

enum KeyType{NORMAL, UNIQUE, PRIMARY};
enum DataType{INT, FLOAT, CHAR};
const int LENGTH_OF_INT = 4;
const int LENGTH_OF_FLOAT = 4;
const int LENGTH_OF_CHAR = 1;
enum Op { EQUALS, GREATER_THAN, GREATE_THAN_OR_EQUAL, LESS_THAN, LESS_THAN_OR_EQUAL, NOT_EQUAL };//add by hedejin 10/28

class Data {
private:
	unsigned type;
	unsigned length;
	//string name;
public:
	Data(unsigned _type, unsigned _length);
	//string getName();
	virtual ~Data();
	int getType() const;
	unsigned getLength();
	//add by hedejin 10/28
	virtual bool compare(Op op, const Data* data);
	virtual void* getValue() const = 0;

};

class Int : public Data {
private:
	int value;
public:
	Int(int _value);
	~Int();
	virtual bool compare(Op op, const Data* data);
	virtual void* getValue() const;
};

class Float : public Data {
private:
	float value;
public:
	Float(float _value);
	~Float();
	virtual bool compare(Op op, const Data* data);
	virtual void* getValue() const;
};

class Char : public Data {
private:
	char value[256];
public:
	Char(string _value);
	~Char();
	virtual bool compare(Op op, const Data* data);
	virtual void* getValue() const;
};

class Field {
public:
	string name;
	int type;
	int attribute;
	int length;
	bool hasIndex;

	Field(string _name, int _type, int _attribute = NORMAL, int _length = 4, bool _hasIndex = false) :
		name(_name), type(_type), attribute(_attribute), length(_length), hasIndex(_hasIndex) {
		if (type == INT)
			length = LENGTH_OF_INT;
		if (type == FLOAT)
			length = LENGTH_OF_FLOAT;
	}
	virtual ~Field() {
	}
};

class Table {
	//add by hedejin
	friend ostream &operator<<(ostream &os, const Table &t);
public:
	string name;
	int numOfField;
	vector<Field> fields;
	int size;
	int locationOfTable;
	int locationOfData;
	int key;
	Table(string _name, vector<Field> _fields, int _size, int _locationOfTable,
		int _locationOfData) :
		name(_name), fields(_fields), size(_size), locationOfTable(
			_locationOfTable), locationOfData(_locationOfData) {
		numOfField = fields.size();
		key = getKeyIndex();
	}
	virtual ~Table() {
	}
	int getKeyIndex();
	int getIndexOf(string name);
	bool findField(string _name);
	Field& getFieldInfo(string _name);
	const Field& getFieldInfoAtIndex(size_t index) const;

	void show();

	//add by hedejin 10.27
	vector<int> getUniqueIndexs() {
		vector<int> UniqueIndexs;
		for (int i = 0; i < fields.size(); i++)
		{
			const Field &f = fields[i];
			if (f.attribute == KeyType::UNIQUE)
			{
				UniqueIndexs.push_back(i);
			}
		}
		return UniqueIndexs;
	}

	vector<int> getTupleWithIndexs() const{
		vector<int> TupleWithIndexs;
		for (int i = 0; i < fields.size(); i++)
		{
			const Field &f = fields[i];
			if (f.hasIndex == true)
			{
				TupleWithIndexs.push_back(i);
			}
		}
		return TupleWithIndexs;
	}
};
#endif /* RECORDMANAGER_DATA_H_ */

