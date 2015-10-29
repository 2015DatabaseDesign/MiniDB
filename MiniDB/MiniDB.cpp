#include "MiniDB.h"

void MiniDB::Print(Selector & selector, const Table* tableDesc)
{
	bool first = true;
	while (selector.hasNext())
	{
		if (first)
		{
			cout << *tableDesc << endl;
			first = false;
		}
		const Tuple& t = selector.next();
		cout << t << endl;
	}
}

void MiniDB::getOperation(const sql_node & node)
{
	switch (node.m_type)
	{
	case USE_SQL: {
		DBName = node.m_fa[0];
	}

	case CREATE_TABLE: {
		CreateTable(node);
		break;
	}
	case CREATE_DATABASE: {
		break;
	}
	case CREATE_INDEX: {
		CreateIndex(node);
		break;
	}

	case SELECT_ORDINARY:case SELECT_WHERE:{
		Select(node);
		break;
	}
	case INSERT_SQL: {
		Insert(node);
		break;
	}

	case DELETE_ORDINARY: case DELETE_WHERE: {
		Delete(node);
		break;
	}

	case DROP_INDEX: {
		DropIndex(node);
		break;
	}
	case DROP_TABLE: {
		DropTable(node);
		break;
	}
	case DROP_DATABASE:{
		break;
	}

	}
}

void MiniDB::readinput()
{
	string statement = ip.read_input();
	sql_node node = ip.Interpreter(statement);
	getOperation(node);
}

void MiniDB::CreateTable(const sql_node & node)
{
	string TableName = node.m_fa[0];
	vector<Field> fields;
	for (int i = 0; i < node.m_sa_len; i+=2)
	{
		string AttrName = node.m_sa[i];
		char* DataType = node.m_sa[i + 1];
		int type;
		if (strcmp(DataType, "int") == 0)
			type = 0;
		else if (strcmp(DataType, "float") == 0)
			type = 1;
		else
			type = 2;//not finish yet!!!use regex to get number of char

		int lenght = 4;
		Field f(AttrName, type);
		fields.push_back(f);
	}
	cm.creatTable(TableName, fields);
}

void MiniDB::CreateIndex(const sql_node & node)
{
	//need to check more!!!
	string IndexName = node.m_fa[0];
	string TableName = node.m_sa[0];
	string AttriName = node.m_ta[0];
	Table *table = cm.getTableInfo(TableName);
	int index = table->getIndexOf(AttriName);
	cm.setIndex(table, index, true);
}

void MiniDB::DropIndex(const sql_node & node)
{
	string IndexName = node.m_sa[0];
	throw MethodNotImplementException();
}

void MiniDB::DropTable(const sql_node & node)
{
	string TableName = node.m_sa[0];
	//first delete tuples and indexs
	DeleteAll(TableName);
	//and then delete information stored in CatalogManager
	cm.dropTable(TableName);
}

void MiniDB::InsertTuple(const string & TableName, const Tuple & t)
{
	Table* table = cm.getTableInfo(TableName);
	int keyIndex = table->getKeyIndex();

	//construct index_info
	char *value = (char *)t.getData(keyIndex);
	const Field &f = table->getFieldInfoAtIndex(keyIndex);
	struct index_info inform(TableName, f, value);							//!!!!the name of index file may not be TableName

	//search if there is a tuple has the same primary key
	//because we create index on primary key automatically
	//we can use IndexManagaer to find if there is a tuple with the same key
	bool InsertSucc = false;
	if (im.search_one(DBName, TableName, inform) < 0)	//uniqueness of primary key is kept
	{
		//if some attribute is declared to be unique,
		//more check should be done
		vector<int> UniqueIndexs = table->getUniqueIndexs();
		if (UniqueIndexs.size() == 0)
		{
			//no need to check unique tuples;
			rm.Insert(DBName, TableName, table, t);
			InsertSucc = true;
		}
		else
		{
			//if all the unique attribute has index created on it
			//then we can check uniqueness by IndexManager
			//else it's better to iterate tuples by RecordManager
			bool HastoIter = false;
			for (int index : UniqueIndexs)
			{
				if ((table->getFieldInfoAtIndex(index)).hasIndex == false)
				{
					HastoIter = true;
					break;
				}
			}

			if (HastoIter)
			{
				//construct conditions to select tuples which break uniqueness
				vector<Condition> conditions;
				for (int index : UniqueIndexs)
				{
					Data* data = t.getData(index);
					Condition condition(Op::EQUALS, data, index);
					conditions.push_back(condition);
				}

				if (rm.CheckUnique(DBName, TableName, table, conditions))
				{
					//can insert
					rm.Insert(DBName, TableName, table, t);
					InsertSucc = true;

				}
				else
				{
					//error : insertion will break the uniqueness
					
				}
			}
			else //no need to iterate by RecordManager, just use IndexManager
			{
				bool UniquenessBreak = false;
				for (int index : UniqueIndexs)
				{
					char *value = (char *)t.getData(index);
					const Field &f = table->getFieldInfoAtIndex(index);
					struct index_info inform(TableName, f, value);				//!!!!the name of index file may not be TableName
					if (im.search_one(DBName, TableName, inform) >= 0)
					{
						//there is a tuple breaks the uniqueness
						//error : insertion will break the uniqueness
						UniquenessBreak = true;
						break;
					}
				}

				if (!UniquenessBreak)
				{
					rm.Insert(DBName, TableName, table, t);
					InsertSucc = true;

				}
			}

		}
	}

	if (InsertSucc)//if insertion succeed, update index files
	{
		const vector<int>& TupleWithIndex = table->getTupleWithIndexs();
		for (int index : TupleWithIndex)
		{
			char *value = (char *)t.getData(index);
			const Field &f = table->getFieldInfoAtIndex(index);
			struct index_info inform(TableName, f, value);//!!!!the name of index file may not be TableName
			im.insert_one(DBName, TableName, inform);
		}
	}
}

void MiniDB::Insert(const sql_node & node)
{
	string TableName = node.m_fa[0];
	Table* table = cm.getTableInfo(TableName);
	//construct tuple
	vector<Data*> datas;
	for (int i = 0; i < node.m_sa_len; i++)
	{
		const Field &f = table->getFieldInfoAtIndex(i);
		const char* value = node.m_sa[i];
		Data *data;
		switch (f.type)
		{
		case DataType::INT: {
			int IntValue = atoi(value);
			data = new Int(IntValue);
		}
		case DataType::FLOAT: {
			float FloatValue = atof(value);
			data = new Float(FloatValue);
		}
		case DataType::CHAR: {
			string str = value;
			data = new Char(str);
		}
		}
		datas.push_back(data);
	}
	Tuple t(datas);
	InsertTuple(TableName, t);
}

void MiniDB::Select(const sql_node &node)
{
	string TableName = node.m_sa[0];
	Table* table = cm.getTableInfo(TableName);
	Selector selector;
	if (node.m_type == SELECT_WHERE)
	{
		vector<Condition> conds;
		vector<int> LineNums;
		//construct conds and LineNums
		//...
		selector = Selector(DBName, TableName, table, conds, LineNums, LinkOp::AND);
	}
	else //select ordinary
		selector = Selector(DBName, TableName, table);

	Print(selector, table);
}

void MiniDB::Delete(const sql_node & node)
{
	string TableName = node.m_sa[0];
	const Table* table = cm.getTableInfo(TableName);
	Deleter deleter;
	if (node.m_type == DELETE_WHERE)
	{
		vector<Condition> conds;
		vector<int> LineNums;
		//construct conds and LineNums
		//...
		deleter = Deleter(DBName, TableName, table, conds, LineNums, LinkOp::AND);
	}
	else//delete ordinary
		deleter = Deleter(DBName, TableName, table);

	const vector<int> &TupleWithIndex = table->getTupleWithIndexs();
	//get every tuple to delete and delete associated index
	while (deleter.hasNext())
	{
		const Tuple &t = deleter.next();
		for (int tupleIndex : TupleWithIndex)
		{
			const Field &f = table->getFieldInfoAtIndex(tupleIndex);
			struct index_info inform(TableName, f, (char *)t.getData(tupleIndex));
			im.delete_one(DBName, TableName, inform);
		}
	}
}

void MiniDB::DeleteAll(string TableName)
{
	const Table* table = cm.getTableInfo(TableName);
	Deleter deleter(DBName, TableName, table);
	const vector<int> &TupleWithIndex = table->getTupleWithIndexs();
	//get every tuple to delete and delete associated index
	while (deleter.hasNext())
	{
		const Tuple &t = deleter.next();
		for (int tupleIndex : TupleWithIndex)
		{
			const Field &f = table->getFieldInfoAtIndex(tupleIndex);
			struct index_info inform(TableName, f, (char *)t.getData(tupleIndex));
			im.delete_one(DBName, TableName, inform);
		}
	}
}
