/*
* Data.cpp
*
*  Created on: 2014å¹´jjjjdf 0æœ?31æ—?
*      Author: darren
*/

#include "Data.h"
#include <sstream>
Data::Data(unsigned _type, unsigned _length) :
	type(_type), length(_length) {

}

Data::~Data() {
	// TODO Auto-generated destructor stub
}

int Data::getType() const {
	return type;
}
unsigned Data::getLength() const {
	return length;
}

//add by hedejin 10/28
bool Data::compare(Op op, const Data* data)
{
	//can't be called
	throw MethodNotImplementException();
	return false;
}

Int::Int(int _value) :
	Data(INT, sizeof(int)), value(_value) {
}
Int::~Int() {
}

void * Int::getValue() const
{
	return (void *)&value;
}

Float::Float(float _value) :
	Data(FLOAT, sizeof(float)), value(_value) {
}

Float::~Float() {
}


void* Float::getValue() const {
	return (void*)&value;
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

void* Char::getValue() const {
	return (void*)value;
}

//add by hedejin 10/28
bool Int::compare(Op op, const Data * data)
{
	switch (data->getType())
	{
	case DataType::INT: {
		int Integer;
		memcpy(&Integer, data->getValue(), sizeof(int));
		switch (op)
		{
		case Op::EQUALS: return value == Integer;
		case Op::GREATER_THAN: return value > Integer;
		case Op::GREATE_THAN_OR_EQUAL: return value >= Integer;
		case Op::LESS_THAN:return value < Integer;
		case Op::LESS_THAN_OR_EQUAL:return value <= Integer;
		case Op::NOT_EQUAL:return value != Integer;
		}
		break;
	}
	case DataType::FLOAT: {
		float FloatNumber;
		memcpy(&FloatNumber, data->getValue(), sizeof(float));
		switch (op)
		{
		case Op::EQUALS: return value == FloatNumber;
		case Op::GREATER_THAN: return value > FloatNumber;
		case Op::GREATE_THAN_OR_EQUAL: return value >= FloatNumber;
		case Op::LESS_THAN:return value < FloatNumber;
		case Op::LESS_THAN_OR_EQUAL:return value <= FloatNumber;
		case Op::NOT_EQUAL:return value != FloatNumber;
		}
		break;
	}
	case DataType::CHAR: {
		throw DataNotComparableException("Int", "Char/String");
	}
	}
}

//add by hedejin 10/28
bool Float::compare(Op op, const Data * data)
{
	switch (data->getType())
	{
	case DataType::INT: {
		int Integer;
		memcpy(&Integer, data->getValue(), sizeof(int));
		switch (op)
		{
		case Op::EQUALS: return value == Integer;
		case Op::GREATER_THAN: return value > Integer;
		case Op::GREATE_THAN_OR_EQUAL: return value >= Integer;
		case Op::LESS_THAN:return value < Integer;
		case Op::LESS_THAN_OR_EQUAL:return value <= Integer;
		case Op::NOT_EQUAL:return value != Integer;
		}
		break;
	}
	case DataType::FLOAT: {
		float FloatNumber;
		memcpy(&FloatNumber, data->getValue(), sizeof(float));
		switch (op)
		{
		case Op::EQUALS: return value == FloatNumber;
		case Op::GREATER_THAN: return value > FloatNumber;
		case Op::GREATE_THAN_OR_EQUAL: return value >= FloatNumber;
		case Op::LESS_THAN:return value < FloatNumber;
		case Op::LESS_THAN_OR_EQUAL:return value <= FloatNumber;
		case Op::NOT_EQUAL:return value != FloatNumber;
		}
		break;
	}
	case DataType::CHAR: {
		throw DataNotComparableException("Float", "Char/String");
	}
	}
}

//add by hedejin 10/28
bool Char::compare(Op op, const Data * data)
{
	switch (data->getType())
	{
	case DataType::INT: {
		throw DataNotComparableException("Char/String", "Int");
	}
	case DataType::FLOAT: {
		throw DataNotComparableException("Char/String", "Float");
	}
	case DataType::CHAR: {
		//char *charValue = new char[data->getLength()];
		char *src = (char *)data->getValue();
		int len = data->getLength();
		//memcpy(charValue, src, data->getLength());
		//string strToCompare(charValue, data->getLength());
		string strToCompare(src);
		string operand(value);
		while (operand.back() == ' ')
			operand.pop_back();
		while (strToCompare.back() == ' ')
			strToCompare.pop_back();

		switch (op)
		{
		case Op::EQUALS: return operand == strToCompare;
		case Op::GREATER_THAN: return operand > strToCompare;
		case Op::GREATE_THAN_OR_EQUAL: return operand >= strToCompare;
		case Op::LESS_THAN:return operand < strToCompare;
		case Op::LESS_THAN_OR_EQUAL:return operand <= strToCompare;
		case Op::NOT_EQUAL:return operand != strToCompare;
		}
	}
	}
}

ostream &operator<<(ostream &os, const Table &t)
{
	string Attrs[3] = { "INT", "FLOAT", "CHAR" };
	std::stringstream ss;
	
	for (auto f : t.fields)
	{
		string output = f.name + ":" + Attrs[f.type];
		if (f.type == CHAR)
		{
			ss << f.length;
			string str;
			ss >> str;
			output += "(" + str + ")";
		}
		os << left << setw(5) << std::setw(output.size() + 3) << output;
	}
	return os;
}

int Table::getKeyIndex() {
	for (unsigned i = 0; i < fields.size(); i++) {
		if (fields[i].attribute == PRIMARY)
			return i;
	}
	return -1;
}
int Table::getIndexOf(string name)const {
	for (unsigned i = 0; i < fields.size(); i++) {
		if (fields[i].name == name)
			return i;
	}
	return -1;
}

bool Table::findField(string _name)
{
	for (size_t i = 0; i < fields.size(); i++)
	{
		if (fields[i].name.compare(_name) == 0)
		{
			return true;
		}
	}
	return false;
}

const Field& Table::getFieldInfo(string _name) const
{
	size_t i;
	for (i = 0; i < fields.size(); i++)
	{
		if (fields[i].name.compare(_name) == 0) break;
	}
	const Field& f = fields[i];
	return f;
}

const Field& Table::getFieldInfoAtIndex(size_t index) const
{
	if (index >= fields.size()) 
		throw(TableException("too many fields"));

	const Field& f = fields[index];
	return f;
}

//void Table::show() {
//	cout << hex << "name:           " << name << "\n"
//		<< "locationOfData: " << locationOfData << "\n"
//		<< "locationOfTable:" << locationOfTable << "\n"
//		<< "numOfField:     " << numOfField << "\n"
//		<< "size:           " << size << "\n"
//		<< "****************" << endl;
//	cout.fill(' ');
//	for (vector<Field>::iterator iter = (fields).begin();
//	iter != (fields).end(); iter++) {
//		cout << setiosflags(ios::left)
//			<< "    Name:" << setw(8) << iter->name << "\n"
//			<< "\tType:" << ((iter->type == INT) ? "INT  " :
//				((iter->type == FLOAT) ? "FLOAT" : "CHAR ")) << "\n"
//			<< "\tAttribute=" << ((iter->attribute == PRIMARY) ? "PRIMARY" :
//				((iter->attribute == UNIQUE) ? "UNIQUE" : "NORMAL")) << "\n"
//			<< "\tLength=" << iter->length << "\n";
//	}
//	cout << "=================================" << endl;
//}
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
		if (iter->hasIndex == true) cout << "\thasIndex=" << "yes" << "\n";
		else cout << "\thasIndex=" << "no" << "\n";
	}
	cout << "=================================" << endl;
}
