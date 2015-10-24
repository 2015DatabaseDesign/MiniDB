#include "RecordManager.h"

Tuple::Tuple(char *dataStart, Table* tabledesc)
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
			string str;
			memcpy(&str, dataStart + offset, data->getLength());	///!!!
			data = new Char(str);
			break;
		}
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

Tuple& Page::ReadTuple(int index)
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


void RecordManager::Insert(const string& DBName, const string& TableName, Table* tableDesc, const Tuple& t)
{
	int blocknum = 0;
	while (true)
	{
		blockInfo *info = readBlock(DBName, TableName, blocknum, DataFile);
		if (info == NULL)
		{
			//error
		}

		Page page(info->cBlock, tableDesc);
		int index = page.InsertableIndex();
		if (index != -1)
		{
			page.WriteTuple(t, index);
			break;
		}
	}
}

void RecordManager::Select(const string & DBName, const string & TableName, Table * tableDesc, const Condition & condition)
{
	vector<Tuple> result;
	int blocknum = 0;
	while (true)
	{
		blockInfo *info = readBlock(DBName, TableName, blocknum, DataFile);
		if (info == NULL)
			break;

		Page page(info->cBlock, tableDesc);
		for (int i = 0; i < page.TupleNum; i++)
		{
			if (page.isTupleValid(i))
			{
				const Tuple& t = page.ReadTuple(i);
				if (condition.match(t))
				{
					result.push_back(t);
					//...to be continued
				}
			}
		}

	}
}

void RecordManager::SelectWithIndex(const string & DBName, const string & TableName, Table * tableDesc, const Condition & condition, struct index_info& inform)
{
	int blocknum = 0;
	Tree t;	//!!!should be done in API
	if (condition.getOp() == Op::EQUALS)
	{
		t.search_one(DBName, TableName, inform);
		int tupleIndex = inform.offset;
		//
		blockInfo *info = readBlock(DBName, TableName, blocknum, DataFile);	//!!!should be done in API
		//read data from IndexFile block
	}
	else
	{
		int start, end;
		t.search_many(DBName, TableName, start, end, condition.getDataType(), inform);
		for (int i = start; i <= end; i++)
		{
			blockInfo *info = readBlock(DBName, TableName, i, DataFile);	//!!!should be done in API
			//read data from IndexFile block
		}
	}



}

void RecordManager::Delete(const string & DBName, const string & TableName, Table * tableDesc, const Condition & condition)
{
	int blocknum = 0;
	while (true)
	{
		blockInfo *info = readBlock(DBName, TableName, blocknum, DataFile);	//!!!should be done in API
		if (info == NULL)
		{
			//error :no such tuple
		}

		Page page(info->cBlock, tableDesc);
		for (int i = 0; i < page.TupleNum; i++)
		{
			if (page.isTupleValid(i))
			{
				const Tuple& t = page.ReadTuple(i);
				if (condition.match(t))
				{
					page.SetHeader(i, false);
				}
			}
		}

	}
}

void RecordManager::DeleteWithIndex(const string & DBName, const string & TableName, Table * tableDesc, const Condition & condition, index_info & inform)
{
	int blocknum = 0;
	Tree t;	//!!!should be done in API
	if (condition.getOp() == Op::EQUALS)
	{
		int blocknum = t.search_one(DBName, TableName, inform);
		blockInfo *info = readBlock(DBName, TableName, blocknum, IndexFile);	//!!!should be done in API
		//read data from IndexFile block
	}
	else
	{
		int start, end;
		t.search_many(DBName, TableName, start, end, condition.getDataType(), inform);
		for (int i = start; i <= end; i++)
		{
			blockInfo *info = readBlock(DBName, TableName, i, IndexFile);
			//read data from IndexFile block
		}
	}

}


