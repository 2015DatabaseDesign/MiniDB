#include "MiniDB.h"
#include <regex>
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

void MiniDB::getCondsAndLinkOp(char ** input, int size, byte func, const Table* table, vector<Condition>& conds, LinkOp & lop)
{
	if (func & 0x80)
		lop = LinkOp::AND;
	else
	{
		lop = LinkOp::OR;

		int code1 = (int)(func & 0x7);
		Op op = getOpFromFunc(code1);
		string AttrName = input[0];
		string toComp = input[1];
		int index = table->getIndexOf(AttrName);
		shared_ptr<Data> data(getDataFromStr(toComp));
		Condition cond(op, data, index);
		conds.push_back(cond);
		int code2 = (int)((func >> 4) & 0x7);
		if (code2 != 0)
		{
			Op op2 = getOpFromFunc(code2);
			string AttrName = input[2];
			string toComp = input[3];
			int index = table->getIndexOf(AttrName);
			//Data* data = getDataFromStr(toComp);
			shared_ptr<Data> data(getDataFromStr(toComp));
			Condition cond(op, data, index);
			conds.push_back(cond);
		}

	}

}

Data* MiniDB::getDataFromStr(string input)
{
	Data* data;
	try
	{
		if (input.find_first_of('.') == input.find_last_of('.'))//float
		{
			float value = stof(input);
			data = new Float(value);
		}
		else if (input.front() == '\'' && input.back() == '\'') //string
		{
			string cut = input.substr(1, input.size() - 2);
			data = new Char(cut);
		}
		else //int
		{
			int value = stoi(input);
			data = new Int(value);
		}
	}
	catch (const exception& e)
	{
		throw InvalidInputForConstructData();
	}
	return data;
}

Op MiniDB::getOpFromFunc(int input)
{
	switch (input)
	{
	case 1:return Op::EQUALS; break;
	case 2:return Op::NOT_EQUAL; break;
	case 3:return Op::LESS_THAN; break;
	case 4:return Op::GREATER_THAN; break;
	case 5:return Op::LESS_THAN_OR_EQUAL; break;
	case 6:return Op::GREATE_THAN_OR_EQUAL; break;
	}
}

vector<int> MiniDB::getLineNumsAndLeftConds(const Table* table, vector<Condition>& conds, LinkOp lop)
{
	vector<int> LineNums;
	vector<Condition> LeftConds;
	bool first = true;
	for (Condition cond : conds)
	{
		int index = cond.getIndexOfData();
		const Field& f = table->getFieldInfoAtIndex(index);
		vector<int> Lines;

		if (f.hasIndex)	//if this condition can make the use of IndexManager, use it to get LineNums
		{
			Data* data = cond.getData();
			struct index_info inform(table->name, f, (char *)(data->getValue()));
			if (cond.getOp() == Op::EQUALS) //if Op is equal, use search_one
			{
				im.search_one(DBName, table->name, inform);
				if (first)
					LineNums.push_back(inform.offset);
				else
					Lines.push_back(inform.offset);
			}
			else
			{
				throw MethodNotImplementException();
				//im.search_many(DBName, table->name, );			//search_many not complete yet!!!
				//LineNums.push_back(LineNum);
			}
		}
		else	//else store conditions in LeftConds
		{
			LeftConds.push_back(cond);
		}

		//if conditions are linked by AND,OR
		if (!first)
		{
			if (lop == LinkOp::AND)
			{
				vector<int> newLineNums(LineNums.size());
				vector<int>::iterator it = set_intersection(LineNums.begin(), LineNums.end(), Lines.begin(), Lines.end(), newLineNums.begin());
				newLineNums.resize(it - newLineNums.begin());
				LineNums = newLineNums;
			}
			else//OR
			{
				vector<int> newLineNums(LineNums.size() + Lines.size());
				vector<int>::iterator it = set_union(LineNums.begin(), LineNums.end(), Lines.begin(), Lines.end(), newLineNums.begin());
				newLineNums.resize(it - newLineNums.begin());
				LineNums = newLineNums;
			}
		}

		first = false;
	}

	//return conditions left
	conds = LeftConds;
	return LineNums;
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
		string DBName = node.m_sa[0];
		throw MethodNotImplementException();
		break;
	}
	case CREATE_INDEX: {
		CreateIndex(node);
		break;
	}

	case SELECT_ORDINARY:case SELECT_WHERE: {
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
	case DROP_DATABASE: {
		throw MethodNotImplementException();
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
	for (int i = 0; i < node.m_sa_len - 2; i += 2)
	{
		string AttrName = node.m_sa[i];
		string TypeAttr = node.m_sa[i + 1];
		int pos = TypeAttr.find_first_of("_");
		string Type = TypeAttr;
		bool isUnique;
		if (pos > 0)
		{
			Type = TypeAttr.substr(0, pos);
			isUnique = true;
		}
		else
			isUnique = false;

		if (Type == "int")
		{
			Field f(AttrName, 0);
			fields.push_back(f);
		}
		else if (Type == "float")
		{
			Field f(AttrName, 1);
			fields.push_back(f);
		}
		else//Type == char
		{
			string input = Type;
			regex num_split("\\s*char\\s*\\(\\s*(\\d+)\\s*\\)\\s*");
			smatch num_match;
			regex_match(input, num_match, num_split);
			string output(num_match[1]);
			int charsize = stoi(output);
			Field f(AttrName, 2, NORMAL, charsize);
			fields.push_back(f);
		}
		if (isUnique)
			fields.back().attribute = UNIQUE;
	}
	int len = node.m_sa_len;
	string primarykeyField = node.m_sa[len - 2];
	for (int i = 0; i < fields.size(); i++)
	{
		if (fields[i].name == primarykeyField)
		{
			fields[i].attribute = PRIMARY;
			fields[i].hasIndex = true;
		}
	}
	cm.creatTable(TableName, fields);

	//create Index
}

void MiniDB::CreateIndex(const sql_node & node)
{
	//need to check uniqueness
	string IndexName = node.m_fa[0];
	string TableName = node.m_sa[0];
	string AttriName = node.m_ta[0];
	Table *table = cm.getTableInfo(TableName);
	int index = table->getIndexOf(AttriName);
	Field field = table->getFieldInfoAtIndex(index);
	if (field.attribute == PRIMARY)
	{
		cout << "This attribute is primary key, index is created on it acquiescently when created table." << endl;
		cout << "The statement has been terminated." << endl;
	}
	else if (field.attribute == NORMAL)
	{
		cout << "This attribute is not unique, index is only create on unique." << endl;
		cout << "The statement has been terminated." << endl;
	}
	//cm.setIndex(table, index, true);
	if (field.hasIndex)
	{
		cout << "There is an index already created on this attribute." << endl;
		cout << "The statement has been terminated." << endl;
	}
	else
	{
		cm.setIndex(table, IndexName, index, true);
		Selector selector(DBName, TableName, table);
		while (selector.hasNext())
		{
			const Tuple& t = selector.next();
			struct index_info inform(TableName, field, (char *)(t.getData(index).get()));
			im.insert_one(DBName, TableName, inform);
		}
		//!!! call buffer manager to create a new file 
	}
}

void MiniDB::DropIndex(const sql_node & node)
{
	string IndexName = node.m_sa[0];
	//throw MethodNotImplementException();
	const vector<string> &tableNames = cm.getAllTable();
	bool found = false;
	for (string name : tableNames)
	{
		Table* table = cm.getTableInfo(name);
		const vector<Field> &fields = table->fields;

		for (int i = 0; i < fields.size(); i++)
		{
			const Field &f = fields[i];
			if (f.hasIndex && f.indexname == IndexName)
			{
				cm.dropIndex(table, IndexName, i, false);
				found = true;
				break;
			}
		}
		if (found)
			break;
	}
	//!!! call buffer manager to delete the index file
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
	char *value = (char *)(t.getData(keyIndex).get());
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
					//Data* data = t.getData(index);
					shared_ptr<Data> data = t.getData(index);
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
					InsertSucc = false;
					//error : insertion will break the uniqueness
					cout << "The statement has been terminated." << endl;
				}
			}
			else //no need to iterate by RecordManager, just use IndexManager
			{
				bool UniquenessBreak = false;
				for (int index : UniqueIndexs)
				{
					char *value = (char *)(t.getData(index).get());
					const Field &f = table->getFieldInfoAtIndex(index);
					struct index_info inform(TableName, f, value);				//!!!!the name of index file may not be TableName
					if (im.search_one(DBName, TableName, inform) >= 0)
					{
						//there is a tuple breaks the uniqueness
						//error : insertion will break the uniqueness
						int linenum = inform.offset;
						vector<Condition> conds;
						vector<int> LineNums;
						LineNums.push_back(linenum);
						Selector selector(DBName, TableName, table, conds, LineNums, LinkOp::AND);
						if (!selector.hasNext())
						{
							throw IndexManagerIncompatiblewithRecordManager();
						}
						cout << "Cannot insert duplicate key in " << TableName << "." << endl;
						cout << ". The duplicate key value occurs in :" << endl;
						cout << selector.next() << endl;
						cout << ".The statement has been terminated." << endl;
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
			char *value = (char *)(t.getData(index).get());
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
	//vector<Data*> datas;
	vector<shared_ptr<Data>> datas;
	for (int i = 0; i < node.m_sa_len; i++)
	{
		const Field &f = table->getFieldInfoAtIndex(i);
		const char* value = node.m_sa[i];
		//Data *data;
		shared_ptr<Data> data;
		switch (f.type)
		{
		case DataType::INT: {
			int IntValue = atoi(value);
			//data = new Int(IntValue);
			data = make_shared<Int>(IntValue);
		}
		case DataType::FLOAT: {
			float FloatValue = atof(value);
			data = make_shared<Float>(FloatValue);
		}
		case DataType::CHAR: {
			string str = value;
			data = make_shared<Char>(str);
		}
		}
		datas.push_back(data);
	}
	Tuple t(datas);
	if (!checkTupleFormat(table, t))
	{
		cout << "The format of tuple is inconsistent with table" << endl;
		cout << "Tuple:" << t << endl;
		cout << "Table:" << *table << endl;
	}
	else
	{
		InsertTuple(TableName, t);
	}
}

void MiniDB::Select(const sql_node &node)
{
	string TableName = node.m_sa[0];
	Table* table = cm.getTableInfo(TableName);
	Selector selector;
	if (node.m_type == SELECT_WHERE)
	{
		//construct conds and LineNums
		vector<Condition> conds;
		LinkOp lop;
		getCondsAndLinkOp(node.m_sa, node.m_sa_len, node.m_func, table, conds, lop);
		const vector<int> &LineNums = getLineNumsAndLeftConds(table, conds, lop);
		selector = Selector(DBName, TableName, table, conds, LineNums, lop);
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
		//construct conds and LineNums
		vector<Condition> conds;
		LinkOp lop;
		getCondsAndLinkOp(node.m_sa, node.m_sa_len, node.m_func, table, conds, lop);
		const vector<int> &LineNums = getLineNumsAndLeftConds(table, conds, lop);
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
			struct index_info inform(TableName, f, (char *)(t.getData(tupleIndex)).get());
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
			struct index_info inform(TableName, f, (char *)(t.getData(tupleIndex).get()));
			im.delete_one(DBName, TableName, inform);
		}
	}
}

bool MiniDB::checkTupleFormat(const Table * table, const Tuple & t)
{
	for (int i = 0; i < table->fields.size(); i++)
	{
		if (table->fields[i].type != (t.datas[i])->getType())
		{
			return false;
		}
	}

	return true;

}
