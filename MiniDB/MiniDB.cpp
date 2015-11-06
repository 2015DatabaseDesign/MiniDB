#include "MiniDB.h"
#include "io.h"
#include <regex>


#include <sstream>
void MiniDB::Print(Selector & selector, const Table* tableDesc, const vector<string> &AttrsName)
{
	string Attrs[3] = { "INT", "FLOAT", "CHAR" };
	bool first = true;
	int cnt = 0;
	//hedejin 11/5
	vector<int> Alig;

	while (selector.hasNext())
	{
		cnt++;
		if (first)
		{
			std::cout << left << setw(5) << "";
			for (string AttrName : AttrsName)
			{
				const Field &f = tableDesc->getFieldInfo(AttrName);
				string output = f.name + ":" + Attrs[f.type];
				if (f.type == CHAR)
				{
					stringstream ss;
					ss << f.length;
					string str;
					ss >> str;
					output += "(" + str + ")";
				}
				Alig.push_back(output.size() + 3);
				std::cout << left << setw(5) << std::setw(output.size() + 3) << output;
			}
			first = false;
			std::cout << endl;
		}

		const Tuple& t = selector.next();
		std::cout << left << setw(5) << cnt;
		for (string AttrName : AttrsName)
		{
			int i = tableDesc->getIndexOf(AttrName);
			//decide data type and print
			switch (t.datas[i]->getType())
			{
			case DataType::INT: {
				int Integer;
				memcpy(&Integer, t.datas[i]->getValue(), sizeof(int));
				//std::cout << left << std::setw(8) << dec << Integer;
				std::cout << left << std::setw(Alig[i]) << dec << Integer;
				break;
			}
			case DataType::FLOAT: {
				float FloatNumber;
				memcpy(&FloatNumber, t.datas[i]->getValue(), sizeof(float));
				std::cout << left << std::setw(Alig[i]) << FloatNumber;
				break;
			}
			case DataType::CHAR: {
				char *charValue = new char[t.datas[i]->getLength()];
				memcpy(charValue, t.datas[i]->getValue(), t.datas[i]->getLength());	///!!!
				string str(charValue, t.datas[i]->getLength());
				std::cout << left << std::setw(Alig[i]) << str;
				delete charValue;
				break;
			}
			}
		}
		std::cout << endl;
		//std::cout << left << setw(5) << cnt << t << endl;
	}
#ifdef DEBUG
	std::cout << "REPORT: Command(s) completed successfully." << endl
		<< setw(8) << "" << "(" << cnt << " row(s) affected.)" << endl;
#endif
}
//add by hedejin 11/4
void MiniDB::Print(const map<int, Tuple> tuples, const Table* tableDesc, const vector<string> &AttrsName)
{
	string Attrs[3] = { "INT", "FLOAT", "CHAR" };
	bool first = true;
	int cnt = 0;
	for(auto iter:tuples)
	{
		cnt++;
		if (first)
		{
			std::cout << left << setw(5) << "";
			for (string AttrName : AttrsName)
			{
				const Field &f = tableDesc->getFieldInfo(AttrName);
				string output = f.name + ":" + Attrs[f.type];
				if (f.type == CHAR)
				{
					stringstream ss;
					ss << f.length;
					string str;
					ss >> str;
					output += "(" + str + ")";
				}
				std::cout << left << setw(5) << std::setw(output.size() + 3) << output;
			}
			first = false;
			std::cout << endl;
		}

		const Tuple& t = iter.second;
		std::cout << left << setw(5) << cnt;
		for (string AttrName : AttrsName)
		{
			int i = tableDesc->getIndexOf(AttrName);
			//decide data type and print
			switch (t.datas[i]->getType())
			{
			case DataType::INT: {
				int Integer;
				memcpy(&Integer, t.datas[i]->getValue(), sizeof(int));
				std::cout << left << std::setw(8) << dec << Integer;
				break;
			}
			case DataType::FLOAT: {
				float FloatNumber;
				memcpy(&FloatNumber, t.datas[i]->getValue(), sizeof(float));
				std::cout << left << std::setw(10) << FloatNumber;
				break;
			}
			case DataType::CHAR: {
				char *charValue = new char[t.datas[i]->getLength()];
				memcpy(charValue, t.datas[i]->getValue(), t.datas[i]->getLength());	///!!!
				string str(charValue, t.datas[i]->getLength());
				std::cout << left << std::setw(12) << str;
				delete charValue;
				break;
			}
			}
		}
		std::cout << endl;
		//std::cout << left << setw(5) << cnt << t << endl;
	}
#ifdef DEBUG
	std::cout << "REPORT: Command(s) completed successfully." << endl
		<< setw(8) << "" << "(" << cnt << " row(s) affected.)" << endl;
#endif
}

//###
void MiniDB::getCondsAndLinkOp(char ** input, int size, byte func, const Table* table, vector<Condition>& conds, LinkOp & lop, vector<string>& values)
{
	if (size == 2)
	{
		lop = AND;
	}
	else
	{
		if (func & 0x80)
			lop = LinkOp::AND;
		else
			lop = LinkOp::OR;
	}


	int code1 = (int)(func & 0x7);
	Op op = getOpFromFunc(code1);
	if (op < 0 || op >= 6)
	{
		std::cout << "ERROR: Column type of supplied values does not match table definition." << endl;
		throw InvalidInputForConstructData();
	}
	//string attrname = input[1];
	//string tocomp = input[2];
	string AttrName = input[0];
	string toComp = input[1];
	if (toComp.front() == '\'' && toComp.back() == '\'')
	{
		values.push_back(toComp.substr(1, toComp.size() - 2));
	}
	else
		values.push_back(toComp);	
	while (AttrName.back() == ' ') AttrName.pop_back();
	int index = table->getIndexOf(AttrName);
	if (index == -1)
	{
		throw NoSuchAttributeInTable(AttrName, table->name);
	}
	shared_ptr<Data> data(getDataFromStr(toComp));
	//if(data->getType() == CHAR && table->getFieldInfo(AttrName).type == INT)

	Condition cond(op, data, index);
	conds.push_back(cond);
	int code2 = (int)((func >> 4) & 0x7);
	if (code2 != 0)
	{
		Op op2 = getOpFromFunc(code2);
		if (op2 < 0 || op2 >= 6)
		{
			std::cout << "ERROR: Column type of supplied values does not match table definition." << endl;
			return;
		}
		string AttrName = input[2];
		string toComp = input[3];
		if (toComp.front() == '\'' && toComp.back() == '\'')
		{
			values.push_back(toComp.substr(1, toComp.size() - 2));
		}
		else
			values.push_back(toComp);
		int index2 = table->getIndexOf(AttrName);
		//Data* data = getDataFromStr(toComp);
		shared_ptr<Data> data2(getDataFromStr(toComp));
		Condition cond(op2, data2, index2);
		conds.push_back(cond);
	}

}

Data* MiniDB::getDataFromStr(string input)
{
	Data* data;
	try
	{
		if ((input.find('.') != string::npos) && (input.find_first_of('.') == input.find_last_of('.')))//float
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
		/*default:throw InvalidInputForConstructData();*/
	}
}

vector<int> MiniDB::getLineNumsAndLeftConds(const Table* table, vector<Condition>& conds, LinkOp lop, const vector<string>& values, bool &useIndex)
{
	vector<int> LineNums;
	vector<Condition> LeftConds;
	bool first = true;
	useIndex = false;
	for (unsigned i = 0; i < conds.size(); i++)
	{
		Condition cond = conds[i];
		int index = cond.getIndexOfData();
		const Field& f = table->getFieldInfoAtIndex(index);
		vector<int> Lines;
		int useindexcnt = 0;
		if (f.hasIndex)	//if this condition can make the use of IndexManager, use it to get LineNums
		{
			useIndex = true;
			useindexcnt++;
			Lines.clear();
			Data* data = cond.getData();
			struct index_info inform(table->name, f, values[i].data());
			string IndexFileName = table->name + "_" + f.name;
			//bool getLine;
			//if (cond.getOp() == Op::EQUALS && im.search_one(getDatabaseName(), IndexFileName, inform) >= 0) //if Op is equal, use search_one
			//{
			//	getLine = true;

			//}
			//else
			//{
			//	int start, end;
			//	vector<int> LinesFromSearchMany;
			//	im.search_many("###", IndexFileName, start, end, cond.getOp(), inform, LinesFromSearchMany);
			//	getLine = (LinesFromSearchMany.size() > 0);

			//}
			if (cond.getOp() == Op::EQUALS) //if Op is equal, use search_one
			{
				if (im.search_one(getDatabaseName(), IndexFileName, inform) >= 0)
					Lines.push_back(inform.offset);
			}
			else
			{
				int start;
				int end;
				im.search_many("###", IndexFileName, start, end, cond.getOp(), inform, Lines);
			}

			if (i == 0)
				LineNums = Lines;
			else//if conditions are linked by AND,OR	
			{
				if (useindexcnt == 2)
				{
					if (lop == LinkOp::AND) //calculate intersection between Lines get from condition using index and origin line numbers
					{
						vector<int> newLineNums(LineNums.size());
						vector<int>::iterator it = set_intersection(LineNums.begin(), LineNums.end(), Lines.begin(), Lines.end(), newLineNums.begin());
						newLineNums.resize(it - newLineNums.begin());
						LineNums = newLineNums;
					}
					else//OR //calculate union between Lines get from condition using index and origin line numbers
					{
						vector<int> newLineNums(LineNums.size() + Lines.size());
						vector<int>::iterator it = set_union(LineNums.begin(), LineNums.end(), Lines.begin(), Lines.end(), newLineNums.begin());
						newLineNums.resize(it - newLineNums.begin());
						LineNums = newLineNums;
					}
				}
				else
				{
					LineNums = Lines;
				}
			}
		}
		else	//else store conditions in LeftConds
		{
			LeftConds.push_back(cond);
		}
		//if (!first)
		//first = false;
	}
	//return conditions left
	conds = LeftConds;
	return LineNums;
}

void MiniDB::exec_file(string &path) {
	if (_access(path.data(), 0) == 0) {
		FILE *exec_file = fopen(path.data(), "rt");
		char SQL_c[200]; /* MAXWORDS */
		while (fgets(SQL_c, 200 /*MAXWORDS*/, exec_file)) {
			string SQL(SQL_c);
			if (SQL.back() == '\n')
				SQL.pop_back();
			sql_node node = ip.Interpreter(SQL);
			getOperation(node);
		}
	}
	else
		throw IllegalCommand(FileNotExist, path);
}

bool MiniDB::getOperation(const sql_node & node)
{
	if (node.m_type == EXEC)
	{
		exec_file(string(node.m_fa[0]));
	}
	if (node.m_type == CREATE_DATABASE)
	{
		createDatabase(node.m_fa[0]);
		return true;
	}

	if (node.m_type != USE_SQL && getDatabaseName() == "")
	{
		std::cout << "ERROR: No database is selected." << endl;
#ifndef DEBUG
		system("pause");
		exit(1);
#endif
		return false;
	}

	if (node.m_type == EXIT)
	{
		closeDatabase();
		cout << "Good bye" << endl;
		getchar();
		exit(1);
		return true;
	}
	switch (node.m_type)
	{
	case SHOW_TABLES: {
		vector<string> TableNames = cm.getAllTable();
		for (string name : TableNames)
			cout << name << endl;
		break;
	}
	case DESCRIBE:{
		string TableName = node.m_fa[0];
		Table * table = cm.getTableInfo(TableName);
		table->show();
		break;
	}

	case USE_SQL: {
		closeDatabase();
		if (!useDatabase(string(node.m_fa[0])))
		{
			std::cout << "ERROR: Database '" << node.m_fa[0] << "' does not exist!" << endl;
			return false;
		}
		
		break;
	}

	case CREATE_TABLE: {
		CreateTable(node);
		break;
	}
	case CREATE_DATABASE: {
		createDatabase(node.m_sa[0]);
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
		deleteDatabase(node.m_sa[0]);
		break;
	}
	}

	return true;
}

void MiniDB::readinput()
{
	string statement = ip.read_input();
	sql_node node = ip.Interpreter(statement);
	getOperation(node);
}

bool MiniDB::CreateTable(const sql_node & node)
{
	string TableName = node.m_fa[0];
	vector<Field> fields;
	string IsPrim = node.m_sa[node.m_sa_len - 1];
	int end;
	if (IsPrim == "primary")
		end = node.m_sa_len - 2;
	else
		end = node.m_sa_len;

	for (int i = 0; i < end; i += 2)
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

	if (IsPrim != "primary")
	{
		cm.creatTable(TableName, fields);
		create_file(TableName, DataFile);
	}
	else
	{
		int len = node.m_sa_len;
		string primarykeyField = node.m_sa[len - 2];
		int keyIndex = -1;
		for (unsigned i = 0; i < fields.size(); i++)
		{
			if (fields[i].name == primarykeyField)
			{
				fields[i].attribute = PRIMARY;
				fields[i].hasIndex = true;
				keyIndex = i;
				break;
			}
		}

		if (keyIndex == -1)
		{
			std::cout << "No attribute name " + primarykeyField + "in this table." << endl;
			std::cout << "The statement has been terminated." << endl;
			return false;
		}

		cm.creatTable(TableName, fields);
		create_file(TableName, DataFile);
		//!!!!!create index
		string IndexFileName = TableName + "_" + primarykeyField;
		create_file(IndexFileName, IndexFile);
		//updata catalogmanager
		Table * table = cm.getTableInfo(TableName);
		//### DefaultIndex is name of index automatically created when declared primary key
		char IndexNum[10];
		sprintf(IndexNum, "%d", table->getIndexOf(primarykeyField));
		string IndexNumStr = IndexNum;
		string DefaultIndexName = TableName + IndexNumStr;
		if (DefaultIndexName.size() > 7)
		{
			cout << "Catalog does not support index file name longer than 7" << endl;
			return false;
		}
		cm.setIndex(table, DefaultIndexName, keyIndex, true);
		std::cout << "REPORT: Command(s) completed successfully." << endl //+++
			<< setw(8) << "" << "(1 table(s) affected)" << endl;
	}
}

bool MiniDB::CreateIndex(const sql_node & node)
{
	//need to check uniqueness
	string IndexName = node.m_fa[0];
	string TableName = node.m_sa[0];
	string AttriName = node.m_ta[0];

	Table *table = cm.getTableInfo(TableName);
	if (table == NULL)
	{
		std::cout << "ERROR: Table '" << TableName << "' does not exist." << endl;
		return false;
	}
	else
	{
		int index = table->getIndexOf(AttriName);
		Field field = table->getFieldInfoAtIndex(index);
		if (field.attribute == PRIMARY)
		{
			std::cout << "ERROR: Default index has been created on primary key '" + field.name + "'." << endl;
			return false;
		}
		else if (field.attribute == NORMAL)
		{
			//std::cout << "This attribute is not unique, index is only create on unique." << endl;
			std::cout << "ERROR: Unable to create index on non-unique attributes '" + field.name + "'." << endl;
			return false;
		}
		//cm.setIndex(table, index, true);
		if (field.hasIndex)
		{
			//std::cout << "There is an index already created on this attribute." << endl;
			std::cout << "ERROR: Index has been created on attribute '" + field.name + "'." << endl;
			return false;
		}
		else
		{
			//call buffermanager to create index file
			string IndexFileName = TableName + "_" + AttriName;
			create_file(IndexFileName, IndexFile);

			//updata catalogmanager
			cm.setIndex(table, IndexName, index, true);
			//iterate tuples and insert one it index manager
			Selector selector(getDatabaseName(), TableName, table);
			while (selector.hasNext())
			{
				const Tuple& t = selector.next();
				struct index_info inform(TableName, field, "###");
				//inform.offset = selector.CurrentIndex - 1;
				inform.offset = selector.CurrentIndex;
				shared_ptr<Data> data = t.datas[index];
				string value;
				std::stringstream ss;
				switch (data->getType())
				{
				case DataType::INT: {
					int Integer;
					memcpy(&Integer, data->getValue(), sizeof(int));
					//std::cout << "Integer : " << Integer;
					ss << Integer;
					break;
				}
				case DataType::FLOAT: {
					float FloatNumber;
					memcpy(&FloatNumber, data->getValue(), sizeof(float));
					//std::cout << "FloatNumber : " << FloatNumber;
					ss << FloatNumber;
					break;
				}
				case DataType::CHAR: {
					char *charValue = new char[data->getLength()];
					memcpy(charValue, data->getValue(), data->getLength());	///!!!
					string str(charValue, data->getLength());
					//std::cout << "Char : " << str;
					ss << str;
					delete charValue;
					break;
				}
				}
				ss >> value;
				//std::cout << ">> Value : " << value << endl;
				//inform.offset = selector.CurrentIndex - 1;
				inform.offset = selector.CurrentIndex;
				inform.value = value;

				im.insert_one(getDatabaseName(), IndexFileName, inform);
			}
		}

	}
	return true;
}


bool MiniDB::DropIndex(const sql_node & node)
{
	string IndexName = node.m_sa[0];
	string IndexFileName;
	const vector<string> &tableNames = cm.getAllTable();
	bool found = false;
	for (string name : tableNames)
	{
		Table* table = cm.getTableInfo(name);
		const vector<Field> &fields = table->fields;

		for (unsigned i = 0; i < fields.size(); i++)
		{
			const Field &f = fields[i];
			if (f.hasIndex && f.indexname == IndexName)
			{
				cm.dropIndex(table, IndexName, i, false);
				IndexFileName = table->name + "_" + f.name;
				found = true;
				break;
			}
		}
		if (found)
			break;
		else
		{
			cout << "ERROR: Index named" + IndexName + "does not exist. " << endl;
			cout << "The statement has been terminated. " << endl;
			return false;
		}
	}
	//!!! call buffer manager to delete the index file
	return delete_file(IndexFileName, IndexFile) > 0;
}

bool MiniDB::DropTable(const sql_node & node)
{
	string TableName = node.m_sa[0];
	//first delete tuples and indexs
	//DeleteAll(TableName);                   // ????
	Table* table = cm.getTableInfo(TableName);
	if (table == NULL)
	{
		std::cout << "ERROR: Table does not exist." << endl;
		return false;
	}
	else
	{
		const vector<int> &TupleWithIndex = table->getTupleWithIndexs();
		//get every tuple to delete and delete associated index
		for (int ti : TupleWithIndex)
		{
			string indexname = table->getFieldInfoAtIndex(ti).indexname;
			cm.dropIndex(table, indexname, ti, false); //
			string IndexFileName = TableName + "_" + table->getFieldInfoAtIndex(ti).name;
			delete_file(IndexFileName, IndexFile);
		}
		//and then delete information stored in CatalogManager
		cm.dropTable(TableName);
		//call buffer manager to delete file
		int result = delete_file(TableName, DataFile);
		std::cout << "REPORT: Command(s) completed successfully." << endl //+++
			<< setw(8) << "" << "(1 table(s) affected)" << endl;
		return result > 0;
	}
}

int MiniDB::InsertTuple(const string & TableName, const Tuple & t, char** valueStr, int valuelen)
{
	Table* table = cm.getTableInfo(TableName);
	bool hasPrim = table->HasPrim();

	//search if there is a tuple has the same primary key
	//because we create index on primary key automatically
	//we can use IndexManagaer to find if there is a tuple with the same key
	bool InsertSucc = false;
	if (hasPrim)
	{
		int keyIndex = table->getKeyIndex();

		//construct index_info
		//char *value = (char *)(t.getData(keyIndex).get());
		const Field &f = table->getFieldInfoAtIndex(keyIndex);
		string IndexFileName = TableName + "_" + f.name;
		string value = valueStr[keyIndex];
		if (f.type == CHAR)
		{
			if (value.back() == '\'')
				value.pop_back();
			if (value.front() == '\'')
				value = value.substr(1, value.size() - 1);
		}
		struct index_info inform(TableName, f, value.data());

		//struct index_info inform(TableName, f, value);	//!!!!the name of index file may not be TableName

		if (im.search_one(getDatabaseName(), IndexFileName, inform) >= 0)
		{
			/*throw BreakUniqueness(f.name);*/
			std::cout << "ERROR: Violation of UNIQUE KEY constraint '" +
				TableName << "_" << f.name << "'.";
			std::cout << "The duplicate key value occurs in: "
				<< "Line number :" << inform.offset << ". The statement has been terminated." << endl;
			return -1;
		}
	}
	//if some attribute is declared to be unique,
	//more check should be done
	vector<int> UniqueIndexs = table->getUniqueIndexs();
	if (UniqueIndexs.size() == 0)
	{
		//no need to check unique tuples;
		return rm.Insert(getDatabaseName(), TableName, table, t);
		//InsertSucc = true;
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

			if (rm.CheckUnique(getDatabaseName(), TableName, table, conditions))
			{
				//can insert
				return rm.Insert(getDatabaseName(), TableName, table, t);
				//InsertSucc = true;
				//return true;

			}
			else
			{
				InsertSucc = false;
				//error : insertion will break the uniqueness
				return -1;
			}
		}
		else //no need to iterate by RecordManager, just use IndexManager
		{
			bool UniquenessBreak = false;
			for (int index : UniqueIndexs)
			{
				//char *value = (char *)(t.getData(index).get());//????
				const Field &f = table->getFieldInfoAtIndex(index);
				//char * value = valueStr[index];   hedejin 11/5
				string value = valueStr[index];
				if (f.type == CHAR)
				{
					if (value.back() == '\'')
						value.pop_back();
					if (value.front() == '\'')
						value = value.substr(1, value.size() - 1);
				}
				struct index_info inform(TableName, f, value.data());				//!!!!the name of index file may not be TableName
				string IndexFileName = TableName + "_" + f.name;
				if (im.search_one(getDatabaseName(), IndexFileName, inform) >= 0)
				{
					//there is a tuple breaks the uniqueness
					//error : insertion will break the uniqueness
					int linenum = inform.offset;
					vector<Condition> conds;
					vector<int> LineNums;
					LineNums.push_back(linenum);
					Selector selector(getDatabaseName(), TableName, table, conds, LineNums, LinkOp::AND);
					if (!selector.hasNext())
					{
						throw IndexManagerIncompatiblewithRecordManager();
					}
					std::cout << "ERROR: Violation of UNIQUE KEY constraint '" +
						TableName << "_" << f.name << "'.";
					std::cout << "The duplicate key value occurs in: "
						<< selector.next() << ". The statement has been terminated." << endl;
					/*std::cout << "Cannot insert duplicate key in " << TableName << "." << endl;
					std::cout << ". The duplicate key value occurs in :" << endl;
					std::cout << selector.next() << endl;
					std::cout << ".The statement has been terminated." << endl;*/
					UniquenessBreak = true;
					break;
				}
			}

			if (!UniquenessBreak)
			{
				return rm.Insert(getDatabaseName(), TableName, table, t);
				//InsertSucc = true;
				//return true;
			}
			else
				return -1;
		}

	}

	//if (InsertSucc)//if insertion succeed, update index files
	//{
	//	const vector<int>& TupleWithIndex = table->getTupleWithIndexs();
	//	for (int index : TupleWithIndex)
	//	{
	//		string valuestr = (char *)t.getData(index)->getValue();
	//		char *value = (char *)(t.getData(index).get());
	//		const Field &f = table->getFieldInfoAtIndex(index);
	//		struct index_info inform(TableName, f, value);//!!!!the name of index file may not be TableName
	//		
	//		inform.offset = 
	//		im.insert_one(getDatabaseName(), TableName, inform);
	//	}
	//}
}

bool MiniDB::Insert(const sql_node & node)
{
	string TableName = node.m_fa[0];
	Table* table = cm.getTableInfo(TableName);
	if (table == NULL)
	{
		std::cout << "ERROR: Table does not exist." << endl;
		return false;
	}
	else
	{
		//construct tuple
		//vector<Data*> datas;
		vector<shared_ptr<Data>> datas;
		if (node.m_sa_len != table->fields.size())
		{
			std::cout << "ERROR: Column name or number of supplied values does not match table definition." << endl;
			//std::cout << "Tuple:" << t << endl;
			//std::cout << "Table:" << *table << endl;
			return false;
		}
		for (unsigned i = 0; i < node.m_sa_len; i++)
		{
			const Field &f = table->getFieldInfoAtIndex(i);
			const char* value = node.m_sa[i];
			//Data *data;
			shared_ptr<Data> data;
			stringstream ss;
			string input = value;
			int dataType = GetDataTypeFromStr(input);
			if (dataType == -1)
			{
				std::cout << "ERROR: Syntax error near " << input  << endl;
				std::cout << " The statement has been terminated." << endl;
				return false;
			}
			if (!(dataType == DataType::INT && f.type == DataType::FLOAT) && dataType != f.type)
			{
				/*throw InvalidInputForConstructData();
				return false;*/
				std::cout << "ERROR: Column type of supplied values does not match table definition." << endl;
				return false;
			}
			switch (f.type)
			{
			case DataType::INT: {
				//int IntValue = atoi(value);
				//data = new Int(IntValue);
				int IntValue = atoi(value);
				data = make_shared<Int>(IntValue);
				break;
			}
			case DataType::FLOAT: {
				float FloatValue = atof(value);
				data = make_shared<Float>(FloatValue);
				break;
			}
			case DataType::CHAR: {
				string str = value;
				// drop the '' of string
				str.pop_back();
				int sz = str.size();
				data = make_shared<Char>(str.substr(1, sz - 1));
				break;
			}
			}
			datas.push_back(data);
		}
		Tuple t(datas);
		if (!checkTupleFormat(table, t))
		{
			std::cout << "ERROR: Column type of supplied values does not match table definition." << endl;
			return false;
		}
		else
		{
			//Insert succeed and then updata indexmanager
			int lineNum = InsertTuple(TableName, t, node.m_sa, node.m_sa_len);
			if (lineNum >= 0)
			{
				vector<int> TupleWithIndex = table->getTupleWithIndexs();
				for (int ti : TupleWithIndex)
				{
					const Field &f = table->getFieldInfoAtIndex(ti);
					string value = node.m_sa[ti];
					if (value.back() == '\'')
						value.pop_back();
					if (value.front() == '\'')
						value = value.substr(1, value.size() - 1);

					struct index_info inform(TableName, f, value.data());
					inform.offset = lineNum;
#ifdef DEBUG
					std::cout << "REPORT: Command completed successfully." << endl;
					std::cout << setw(8) << "" << "(1 row(s) affected)" << endl;
#endif
					//cout << "Insert Index on " << t << " : " << f.name << endl;
					string IndexFileName = TableName + "_" + f.name;
					blockInfo *info = readBlock(TableName, 0, DataFile);

					im.insert_one(getDatabaseName(), IndexFileName, inform); //!!

				}
			}
			else
			{
				return false;
			}

		}
	}
	return true;
}


int MiniDB::GetDataTypeFromStr(string input)
{
	try
	{
		if (input.front() == '\'' && input.back() == '\'') //string
		{
			string cut = input.substr(1, input.size() - 2);
			return DataType::CHAR;
		}
		else if ((input.find('.') != string::npos) && (input.find_first_of('.') == input.find_last_of('.')))//float
		{
			float value = stof(input);
			return DataType::FLOAT;
		}
		else //int
		{
			int value = stoi(input);
			return DataType::INT;
		}
	}
	catch (const exception& e)
	{
		return -1;
		//throw InvalidInputForConstructData();
	}
}

bool MiniDB::Select(const sql_node &node)
{
	vector<string> Attrs;
	//####
	for (unsigned i = 0; i < node.m_fa_len; i++)
	{
		if (strcmp(node.m_fa[0], "*") == 0)
			break;
		string AttrName = node.m_fa[i];
		Attrs.push_back(AttrName);
	}

	string TableName = node.m_sa[0];
	Table* table = cm.getTableInfo(TableName);

	if (Attrs.size() == 0)
	{
		for (Field f : table->fields)
			Attrs.push_back(f.name);
	}

	if (table == NULL)
	{
		std::cout << "ERROR: Table does not exist." << endl;
		return false;
	}
	else
	{
		Selector selector;
		if (node.m_type == SELECT_WHERE)
		{
			//construct conds and LineNums
			vector<Condition> conds;
			vector<string> values;
			LinkOp lop;
			getCondsAndLinkOp(node.m_ta, node.m_ta_len, node.m_func, table, conds, lop, values);
			bool useIndex;
			const vector<int> &LineNums = getLineNumsAndLeftConds(table, conds, lop, values, useIndex);
			if (useIndex && lop == LinkOp::AND && LineNums.size() == 0)//hedejin 11/4
			{
#ifdef DEBUG
				std::cout << "REPORT: Command(s) completed successfully." << endl
					<< setw(8) << "" << "(" << 0 << " row(s) affected.)" << endl;
#endif
				return  true;
			}
			if (useIndex && lop == LinkOp::OR && conds.size() > 0) //if use index and there are conditions left combinating with operator or
			{
				vector<int> EmptyNums;
				vector<Condition> EmptyConds;
				Selector selector = Selector(getDatabaseName(), TableName, table, conds, EmptyNums, LinkOp::AND);
				Selector selFromLineNum = Selector(getDatabaseName(), TableName, table, EmptyConds, LineNums, LinkOp::AND);
				map<int, Tuple> tuples;
				while (selector.hasNext())
				{
					int lineNum = selector.CurrentIndex;
					Tuple t(selector.next());
					tuples.insert({ lineNum, t });
				}
				while (selFromLineNum.hasNext())
				{
					int lineNum = selFromLineNum.CurrentIndex;
					Tuple t(selFromLineNum.next());
					if (tuples.find(lineNum) == tuples.end())
						tuples.insert({ lineNum, t });
				}
				Print(tuples, table, Attrs);
				return true;
			}
			if (node.m_ta_len == 2) lop = LinkOp::AND;
			selector = Selector(getDatabaseName(), TableName, table, conds, LineNums, lop);
		}
		else //select ordinary
			selector = Selector(getDatabaseName(), TableName, table);

		Print(selector, table, Attrs);
	}
	return true;
}

bool MiniDB::Delete(const sql_node & node)
{
	string TableName = node.m_fa[0];
	const Table* table = cm.getTableInfo(TableName);
	if (table == NULL)
	{
		std::cout << "ERROR: Table does not exist." << endl;
		return false;
	}
	else
	{
		Deleter deleter;
		if (node.m_type == DELETE_WHERE)
		{
			//construct conds and LineNums
			vector<Condition> conds;
			LinkOp lop;
			vector<string> values;
			getCondsAndLinkOp(node.m_sa, node.m_sa_len, node.m_func, table, conds, lop, values);
			bool useIndex;
			const vector<int> &LineNums = getLineNumsAndLeftConds(table, conds, lop, values, useIndex);
			/*if (useIndex && LineNums.size() == 0)
			{
				std::cout << "REPORT: Command(s) completed successfully." << endl
					<< setw(8) << "" << "(" << 0 << " row(s) affected.)" << endl;
				return  true;
			}*/
			if (useIndex && lop == LinkOp::AND && LineNums.size() == 0)//hedejin 11/4
			{
#ifdef DEBUG
				std::cout << "REPORT: Command(s) completed successfully." << endl
					<< setw(8) << "" << "(" << 0 << " row(s) affected.)" << endl;
#endif
				return  true;
			}
			if (useIndex && lop == LinkOp::OR && conds.size() > 0) //if use index and there are conditions left combinating with operator or
			{
				vector<int> EmptyNums;
				vector<Condition> EmptyConds;
				Deleter deleter = Deleter(getDatabaseName(), TableName, table, conds, EmptyNums, LinkOp::AND);
				Deleter delFromLineNum = Deleter(getDatabaseName(), TableName, table, EmptyConds, LineNums, LinkOp::AND);
				map<int, Tuple> tuples;
				while (deleter.hasNext())
				{
					int lineNum = deleter.CurrentIndex;
					Tuple t(deleter.next());
					tuples.insert({ lineNum, t });
				}
				while (delFromLineNum.hasNext())
				{
					int lineNum = delFromLineNum.CurrentIndex;
					Tuple t(delFromLineNum.next());
					if (tuples.find(lineNum) == tuples.end())
						tuples.insert({ lineNum, t });
				}
				const vector<int> &TupleWithIndex = table->getTupleWithIndexs();
				for (auto iter : tuples)
				{
					const Tuple &t = iter.second;
					for (int tupleIndex : TupleWithIndex)
					{
						const Field &f = table->getFieldInfoAtIndex(tupleIndex);
						string value = t.getValueStr(tupleIndex);
						struct index_info inform(TableName, f, value.data()); //!!!
						string IndexFileName = TableName + "_" + f.name;
						im.delete_one(getDatabaseName(), IndexFileName, inform);
					}
				}
				return true;
			}
			if (node.m_ta_len == 2) lop = LinkOp::AND;
			deleter = Deleter(getDatabaseName(), TableName, table, conds, LineNums, lop);
		}
		else//delete ordinary
			deleter = Deleter(getDatabaseName(), TableName, table);

		const vector<int> &TupleWithIndex = table->getTupleWithIndexs();
		//get every tuple to delete and delete associated index
		while (deleter.hasNext())
		{
			const Tuple &t = deleter.next();
			for (int tupleIndex : TupleWithIndex)
			{
				const Field &f = table->getFieldInfoAtIndex(tupleIndex);
				string value = t.getValueStr(tupleIndex);
				struct index_info inform(TableName, f, value.data()); //!!!
				string IndexFileName = TableName + "_" + f.name;
				im.delete_one(getDatabaseName(), IndexFileName, inform);
			}
		}
	}
	return true;
}

bool MiniDB::checkTupleFormat(const Table * table, const Tuple & t)
{
	if (table->fields.size() != t.datas.size())
		return false;
	for (unsigned i = 0; i < table->fields.size(); i++)
	{
		if (table->fields[i].type != (t.datas[i])->getType())
		{
			return false;
		}
	}

	return true;

}
