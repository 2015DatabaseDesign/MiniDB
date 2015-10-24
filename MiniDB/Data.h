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
using namespace std;

enum KeyType{NORMAL, UNIQUE, PRIMARY};
enum DataType{INT, FLOAT, CHAR};
const int LENGTH_OF_INT = 4;
const int LENGTH_OF_FLOAT = 4;
const int LENGTH_OF_CHAR = 1;

class Data {
private:
	unsigned type;
	unsigned length;
	//string name;
public:
	Data(unsigned _type, unsigned _length);
	//string getName();
	virtual ~Data();
	int getType();
	unsigned getLength();
	virtual void* getValue() = 0;
};

class Int : public Data {
private:
	int value;
public:
	Int(int _value);
	~Int();
	virtual void* getValue();
};

class Float : public Data {
private:
	float value;
public:
	Float(float _value);
	~Float();
	virtual void* getValue();
};

class Char : public Data {
private:
	char value[256];
public:
	Char(string _value);
	~Char();
	virtual void* getValue();
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
	Field& getFieldInfoAtIndex(size_t index);

	void show();
};
#endif /* RECORDMANAGER_DATA_H_ */

