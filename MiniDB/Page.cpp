#include "Page.h"
#include "RecordManager.h"
#include <sstream>

string Tuple::toString()const
{
	string output;
	for (unsigned i = 0; i < datas.size(); i++)
	{
		switch (datas[i]->getType())
		{
		case DataType::INT: {
			int Integer;
			memcpy(&Integer, datas[i]->getValue(), sizeof(int));
			char *str = new char[10];
			sprintf(str, "%d", Integer);
			output += "\t" + string(str);
			delete str;
			break;
		}
		case DataType::FLOAT: {
			float FloatNumber;
			memcpy(&FloatNumber, datas[i]->getValue(), sizeof(float));
			char *str = new char[10];
			sprintf(str, "%f", FloatNumber);
			output += "\t" + string(str);
			delete str;
			break;
		}
		case DataType::CHAR: {
			char *charValue = new char[datas[i]->getLength()];
			memcpy(charValue, datas[i]->getValue(), datas[i]->getLength());	///!!!
			string str(charValue, datas[i]->getLength());
			output += "\t" + str;
			delete charValue;
			break;
		}
		}
	}
	return output;
}

Tuple::Tuple(char *dataStart, const Table* tabledesc)
{
	const vector<Field> &fields = tabledesc->fields;
	int offset = 0;
	for (unsigned i = 0; i < fields.size(); i++)
	{
		Field f = fields[i];
		shared_ptr<Data> data;
		//read bytes from dataStart into Data object including Int, Float and Char/string
		switch (f.type)
		{
		case DataType::INT: {
			int Integer;
			memcpy(&Integer, dataStart + offset , sizeof(int));

			data = make_shared<Int>(Integer);
			break;
		}
		case DataType::FLOAT: {
			float FloatNumber;
			memcpy(&FloatNumber, dataStart + offset, sizeof(float));
			data = make_shared<Float>(FloatNumber);
			break;
		}
		case DataType::CHAR: {
			char *charValue = new char[f.length];
			memcpy(charValue, dataStart + offset, f.length);	///!!!
			string str(charValue, f.length);
			data = make_shared<Char>(str);
			delete charValue;
			break;
		}
		default:throw logic_error("Unsuported data type");
		}
		datas.push_back(data);
		offset += f.length;
	}
}

void Tuple::writeData(char *dataToWrite, const Table* table) const
{
	int offset = 0;
	char *dest = dataToWrite; //destination
	for (unsigned i = 0; i < datas.size();i++)
	{
		int len = table->getFieldInfoAtIndex(i).length;
		if (table->getFieldInfoAtIndex(i).type == DataType::CHAR)
		{
			string str = (char *)datas[i]->getValue();
			while (str.size() < (len - 1))
			{
				str += " ";
			}
			memcpy(dest+offset, str.data(), len);
		}
		else
		{
			memcpy(dest+offset, datas[i]->getValue(), len);
		}
		offset += len;
	}

	////test
	//for (int i = 0; i < datas.size(); i++)
	//{
	//	shared_ptr<Data> data;
	//	switch (datas[i]->getType())
	//	{
	//	case DataType::INT: {
	//		int Integer;
	//		memcpy(&Integer, datas[i]->getValue(), sizeof(int));
	//		data = make_shared<Int>(Integer);
	//		break;
	//	}
	//	case DataType::FLOAT: {
	//		float FloatNumber;
	//		memcpy(&FloatNumber, datas[i]->getValue(), sizeof(float));
	//		data = make_shared<Float>(FloatNumber);
	//		break;
	//	}
	//	case DataType::CHAR: {
	//		char *charValue = new char[datas[i]->getLength()];
	//		memcpy(charValue, datas[i]->getValue(), datas[i]->getLength());	///!!!
	//		string str(charValue, datas[i]->getLength());
	//		data = make_shared<Char>(str);
	//		break;
	//	}
	//	}
	//}
}

ostream &operator<<(ostream &os, const Tuple &t)
{
//	for (auto data : t.datas)
	for (unsigned i = 0; i < t.datas.size(); i++)
	{
		switch (t.datas[i]->getType())
		{
		case DataType::INT: {
			int Integer;
			memcpy(&Integer, t.datas[i]->getValue(), sizeof(int));
			os << left << std::setw(8) << dec <<  Integer;
			break;
		}
		case DataType::FLOAT: {
			float FloatNumber;
			memcpy(&FloatNumber, t.datas[i]->getValue(), sizeof(float));
			os << left << std::setw(10) << FloatNumber;
			break;
		}
		case DataType::CHAR: {
			char *charValue = new char[t.datas[i]->getLength()];
			memcpy(charValue, t.datas[i]->getValue(), t.datas[i]->getLength());	///!!!
			string str(charValue, t.datas[i]->getLength());
			os << left << std::setw(12) << str;
			delete charValue;
			break;
		}
		}
	}
	return os;
}

bool operator==(const Tuple& t0, const Tuple &t1)
{
	int sz = t0.datas.size();
	for (int i = 0; i < sz; i++)
	{
		bool comp = t0.datas[i]->compare(Op::EQUALS, t1.datas[i].get());
		if (!comp)
			return false;
	}
	return true;
}

shared_ptr<Data> Tuple::getData(int index) const
{
	return datas[index];
}

string Tuple::getValueStr(int index) const
{
	shared_ptr<Data> data = datas[index];
	std::stringstream ss;
	string output;
	switch (data->getType())
	{
	case DataType::INT: {
		int Integer;
		memcpy(&Integer, data->getValue(), sizeof(int));
		ss << Integer;
		break;
	}
	case DataType::FLOAT: {
		float FloatNumber;
		memcpy(&FloatNumber, data->getValue(), sizeof(float));
		ss << FloatNumber;
		break;
	}
	case DataType::CHAR: {
		char *charValue = new char[data->getLength()];
		memcpy(charValue, data->getValue(), data->getLength());	///!!!
		string str(charValue, data->getLength());
		delete charValue;
		ss << str;
		break;
	}
	}
	ss >> output;
	return output;
}

Tuple::~Tuple()
{
	//for (auto data : datas)
	//{
	//	delete data;
	//}
}

int Page::calcTupleNum(const Table *td)
{
	const vector<Field> &fields = td->fields;
	int sizeOfLine = td->size + 1;

	return Page::PageSize / sizeOfLine;
}

bool Page::isTupleValid(int index)
{
	if (index < TupleNum)
	{
		char isValid = data[index] & 0x1;
		return isValid > 0;
	}
	else
	{
		throw logic_error("index out of numbers of tuples");
	}
}

int Page::InsertableIndex()
{
	for (int i = 0; i < TupleNum; i += JUMPLENGTH) {
		if ((data[i] & 0xFE) == 0)
			continue;
		for (int j = 0; j < JUMPLENGTH && (i + j) < TupleNum; j++)
			if ((data[i + j] & 0x1) == 0)
				return (i + j);
	}
	return -1;
}

void Page::WriteTuple(const Tuple& t, int index)
{
	if (index >= TupleNum)
	{
		throw TupleIndexOutofRange(index);
	}
	//skip the part of header
	char *TupleStart = data + TupleNum;

	//get address of Target
	char *Target = TupleStart + index * tabledesc->size;
	t.writeData(Target, tabledesc);
}

Tuple Page::ReadTuple(int index)
{
	if (index >= TupleNum)
	{
		throw TupleIndexOutofRange(index);
	}

	//skip the part of header
	char *TupleStart = data + TupleNum;

	//get address of Target
	char *Target = TupleStart + index * tabledesc->size;
	Tuple tuple(Target, this->tabledesc);
	return tuple;
}

void Page::SetHeader(int index, unsigned char isSet)
{
	if (index >= TupleNum)
		throw TupleIndexOutofRange(index);
	switch (isSet) {
	case 0:
		if ((this->data[index] & 0x1) == 0) //error:This tuple is already deleted
			throw TupleAlreadyDeletedButDeleteItAgain(index);
		else
			this->data[index] &= 0xFE; // reset 0
		break;
	case 1:
		if ((this->data[index] & 0x1) == 1) //error:This tuple is already written
			throw TupleAlreadyWrittenButInsertOnIt(index);
		else
			this->data[index] |= 0x1; // set 1
		break;
	case 2:
		if ((this->data[index] >> 1) < JUMPLENGTH)
			this->data[index] += 2;
		break;
	case 3:
		if ((this->data[index] >> 1) > 0)
			this->data[index] -= 2;
		break;
	case 4:
		this->data[index] = ((unsigned char)JUMPLENGTH) << 1;
		break;
	default:
		break;
	}
	//if (index >= TupleNum)
	//{
	//	throw TupleIndexOutofRange(index);
	//}
	//if (isSet)
	//{
	//	if (this->data[index] == 1)
	//	{
	//		//error:This tuple is already written
	//		throw TupleAlreadyWrittenButInsertOnIt(index);
	//	}
	//	else
	//		this->data[index] = (unsigned char)1;
	//}
	//else
	//{
	//	if (this->data[index] == 0)
	//	{
	//		//error:This tuple is already deleted
	//		throw TupleAlreadyDeletedButDeleteItAgain(index);
	//	}
	//	else
	//		this->data[index] = (unsigned char)0;
	//}
}
