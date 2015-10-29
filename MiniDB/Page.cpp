#include "Page.h"
Tuple::Tuple(char *dataStart, const Table* tabledesc)
{
	const vector<Field> &fields = tabledesc->fields;
	int offset = 0;
	for (int i = 0; i < fields.size(); i++)
	{
		Field f = fields[i];
		Data *data;
		//read bytes from dataStart into Data object including Int, Float and Char/string
		switch (f.type)
		{
		case DataType::INT: {
			int Integer;
			memcpy(&Integer, dataStart + offset, sizeof(int));
			data = new Int(Integer);
			break;
		}
		case DataType::FLOAT: {
			float FloatNumber;
			memcpy(&FloatNumber, dataStart + offset, sizeof(float));
			data = new Float(FloatNumber);
			break;
		}
		case DataType::CHAR: {
			char *charValue = new char[f.length];
			memcpy(&charValue, dataStart + offset, f.length);	///!!!
			string str = charValue;
			data = new Char(str);
			break;
		}
		default:throw logic_error("Unsuported data type");
		}
		datas.push_back(data);
		offset += f.length;
	}
}

void Tuple::writeData(char *dataToWrite) const
{
	int offset = 0;
	char *dest = dataToWrite; //destination
	for (auto data : datas)
	{
		int len = data->getLength();
		memcpy(dest, data->getValue(), len);
		dest += len;
	}
}

ostream &operator<<(ostream &os, const Tuple &t)
{
	for (auto data : t.datas)
	{
		switch (data->getType())
		{
		case DataType::INT: {
			int Integer;
			memcpy(&Integer, data->getValue(), sizeof(int));
			os << std::setw(8) << Integer;
			break;
		}
		case DataType::FLOAT: {
			float FloatNumber;
			memcpy(&FloatNumber, data->getValue(), sizeof(float));
			os << std::setw(8) << FloatNumber;
			break;
		}
		case DataType::CHAR: {
			char *charValue = new char[data->getLength()];
			memcpy(&charValue, data->getValue(), data->getLength());	///!!!
			string str = charValue;
			os << std::setw(8) << str;
			break;
		}
		}
	}
	return os;
}

Data* Tuple::getData(int index) const
{
	return datas[index];
}

Tuple::~Tuple()
{
	for (auto data : datas)
	{
		delete data;
	}
}

int Page::getTupleNum(const Table *td)
{
	const vector<Field> &fields = td->fields;
	int sizeOfLine = 0;
	for (auto f : fields)
	{
		sizeOfLine += f.length;
	}

	return (Page::PageSize + sizeOfLine) / (sizeOfLine + 1);
}

bool Page::isTupleValid(int index)
{
	if (index < TupleNum)
	{
		char isValid = data[index];
		return isValid > 0;
	}
	else
	{
		throw logic_error("index out of numbers of tuples");
	}
}

int Page::InsertableIndex()
{
	for (int i = 0; i < TupleNum; i++)
	{
		char isValid = data[i];
		if (isValid == 0)
			return i;
	}

	return -1;
}

void Page::WriteTuple(const Tuple& t, int index)
{
	if (index >= TupleNum)
	{
		throw logic_error("index out of amount of tuples");
	}
	//skip the part of header
	char *TupleStart = data + TupleNum;

	//get address of Target
	char *Target = TupleStart + index * tabledesc->size;
	t.writeData(Target);
}

Tuple Page::ReadTuple(int index)
{
	if (index >= TupleNum)
	{
		throw logic_error("index out of amount of tuples");
	}

	//skip the part of header
	char *TupleStart = data + TupleNum;

	//get address of Target
	char *Target = TupleStart + index * tabledesc->size;
	Tuple tuple(Target, this->tabledesc);
	return tuple;
}

void Page::SetHeader(int index, bool isSet)
{
	if (isSet)
		this->data[index] = 1;
	else
		this->data[index] = 0;
}
