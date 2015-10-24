/*
* Data.cpp
*
*  Created on: 2014å¹´jjjjdf 0æœ?31æ—?
*      Author: darren
*/

#include "Data.h"

Data::Data(unsigned _type, unsigned _length) :
	type(_type), length(_length) {

}

Data::~Data() {
	// TODO Auto-generated destructor stub
}

int Data::getType() {
	return type;
}
unsigned Data::getLength() {
	return length;
}

Int::Int(int _value) :
	Data(INT, sizeof(int)), value(_value) {
}
Int::~Int() {
}
void* Int::getValue() {
	return &value;
}

Float::Float(float _value) :
	Data(FLOAT, sizeof(float)), value(_value) {
}
Float::~Float() {
}
void* Float::getValue() {
	return &value;
}

Char::Char(string _value) :
	Data(CHAR, _value.length()) {
	for (unsigned i = 0; i < _value.length(); i++) {
		value[i] = _value[i];
	}
	value[_value.length()] = 0;
}
Char::~Char() {
}
void* Char::getValue() {
	return value;
}

int Table::getKeyIndex() {
	for (unsigned i = 0; i < fields.size(); i++) {
		if (fields[i].attribute == PRIMARY)
			return i;
	}
	return -1;
}
int Table::getIndexOf(string name) {
	for (unsigned i = 0; i < fields.size(); i++) {
		if (fields[i].name == name)
			return i;
	}
	return -1;
}

bool Table::findField(string _name)
{
	for (size_t i = 0; i<fields.size(); i++)
	{
		if (fields[i].name.compare(_name) == 0)
		{
			return true;
		}
	}
	return false;
}

Field& Table::getFieldInfo(string _name)
{
	size_t i;
	for (i = 0; i<fields.size(); i++)
	{
		if (fields[i].name.compare(_name) == 0) break;
	}
	Field& f = fields[i];
	return f;
}

Field& Table::getFieldInfoAtIndex(size_t index)
{
	Field& f = fields[index];
	return f;
}

void Table::show() {
	cout << hex << "name:           " << name << "\n"
		<< "locationOfData: " << locationOfData << "\n"
		<< "locationOfTable:" << locationOfTable << "\n"
		<< "numOfField:     " << numOfField << "\n"
		<< "size:           " << size << "\n"
		<< "****************" << endl;
	cout.fill(' ');
	for (vector<Field>::iterator iter = (fields).begin();
	iter != (fields).end(); iter++) {
		cout << setiosflags(ios::left)
			<< "    Name:" << setw(8) << iter->name << "\n"
			<< "\tType:" << ((iter->type == INT) ? "INT  " :
				((iter->type == FLOAT) ? "FLOAT" : "CHAR ")) << "\n"
			<< "\tAttribute=" << ((iter->attribute == PRIMARY) ? "PRIMARY" :
				((iter->attribute == UNIQUE) ? "UNIQUE" : "NORMAL")) << "\n"
			<< "\tLength=" << iter->length << "\n";
	}
	cout << "=================================" << endl;
}

