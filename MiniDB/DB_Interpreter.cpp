// DB_Interpreter.cpp : Defines the entry point for the console application.
//
#include "iostream"
#include "regex"
#include "string"
#include "map"
#include "DB_Interpreter.h"
typedef unsigned char byte;
using namespace std;

/**
* \def	MAXLENGTH MAXNUM
*
* \brief	Macros that defines maxlength and maxnum.
*
* \author	Hac
* \date	10/16/2015
*/

#define MAXLENGTH	20
#define MAXNUM		50

/**
* \fn	string DB_interpreter::read_input()
*
* \brief	Reads the input.
*
* \author	Hac
* \date	10/16/2015
*
* \return	The input.
*/

string DB_interpreter::read_input() {
	string statement, str_temp;
	smatch cap_str;
	const regex clause_pattern("[^;]*;(.*)");
	cout << "MiniSQL> ";
	while (true) {
		getline(cin, str_temp);
		statement = statement.append(str_temp);
		// match 'quit' and 'exit'
		if (regex_match(statement, regex("^[\\s]*(quit|exit)(\\s|;)*")))
			exit(0);
		// match 'help'
		if (regex_match(statement, regex("^[\\s]*help(\\s|;)*"))) {
			DBhelp();
			return "";
		}
		// match ';' 
		// judge whether the statement is over
		if (regex_match(statement, cap_str, clause_pattern))
			if (regex_match(string(cap_str[1]), regex("\\s*")))
				return statement;
			else
				throw IllegalCommand(UndefinedCMD);
		else {
			statement.append(" ");
			cout << "      -> ";
		}

	}
}

/**
* \fn	void DB_interpreter::Interpreter(string statement)
*
* \brief	Interpreters.
*
* \author	Hac
* \date	10/16/2015
*
* \param	statement	The statement.
*/

sql_node DB_interpreter::Interpreter(string statement) {
	smatch cap_str;
	statement = toLower(statement);

	const regex command_split("^\\s*(\\w+)\\s+(\\S+)\\s.*");

	// find function of the SQL statement
	// E.g. create, insert, drop, select, delete, use
	if (regex_match(statement, cap_str, command_split)) {
		map<string, int>::iterator iter = str_map.find(string(cap_str[1]));
		int type;
		if (iter != str_map.end())
			type = iter->second;
		else {
			throw IllegalCommand(UndefinedCMD);
		}

		// function switch
		switch (type) {
		case CREATE: return create_clause(statement); break;
		case INSERT: return insert_clause(statement); break;
		case DROP: return drop_clause(statement); break;
		case SELECT: return select_clause(statement); break;
		case DELETE: return delete_clause(statement); break;
		case USE: return use_clause(statement); break;
		default:
			throw IllegalCommand(UndefinedCMD);
		}
	}
	else
		throw IllegalCommand(UndefinedCMD);

}



sql_node DB_interpreter::create_clause(string SQL) {
	sql_node node;
	/**
	* 'create' grammar
	* create (table | database | index) (name)
	* ('('info')' | ';' | 'on') (';' | NONE | 'table_name')
	* (NONE | NONE | '('info')') (NONE | NONE | ';')
	*/
	smatch create_match;
	/***************************************************************************************************/
	/*     create	table|database|index	name	(table_info);|;|on tablename (attribute_info);     */
	/***************************************************************************************************/
	const regex create_split("^\\s*create\\s+(\\w+)\\s(\\w+)(?:(?:\\s*\\(\\s*(.+)\\s*\\))|(?:\\s+on\\s+(\\w+)\\s*\\(([^)]+)\\)))?\\s*;\\s*");
	// split SQL statement
	if (regex_match(SQL, create_match, create_split)) {
		map<string, int>::iterator iter = str_map.find(string(create_match[1]));
		int type;
		if (iter != str_map.end())
			type = iter->second;
		else {
			throw IllegalCommand(UndefinedCMD);
		}
		// cout << create_match[1] << "*" << create_match[2] << "*" << create_match[3] << "*" << create_match[4] << "*" << create_match[5] << "*" << endl;
		// 'create' function switch
		switch (type) {
		case TABLE: node = create_table(create_match); break;
		case DATABASE: node = create_database(create_match); break;
		case INDEX: node = create_index(create_match); break;
		default: break;
		}
	}
	else
		throw IllegalCommand(UndefinedCMD);
	node.show();
	/*
	Call API
	*/
	cout << "done" << endl;
	return node;
}


sql_node DB_interpreter::create_database(smatch cmd_match) {
	byte type = CREATE_DATABASE, func = 0;
	string database_name = string(cmd_match[2]);
	size_t fa_len = 1;
	static char chr[MAXLENGTH];
	if (database_name.size() >= MAXLENGTH)
		throw IllegalCommand(TooLongArgs);
	strcpy(chr, database_name.data());
	static char *fa[] = { chr };
	//const static char *fa[] = { database_name.data() };
	return sql_node(type, func, fa_len, fa);
}


sql_node DB_interpreter::create_index(smatch cmd_match) {
	size_t fa_len = 1, sa_len = 1, ta_len = 0;
	byte type = CREATE_INDEX, func = 0;
	string index_name = string(cmd_match[2]), table_name = string(cmd_match[4]);
	string argu[2];
	static char fa_chr[MAXLENGTH], sa_chr[MAXLENGTH];
	if (index_name.size() >= MAXLENGTH || table_name.size() >= MAXLENGTH)
		throw IllegalCommand(TooLongArgs);
	strcpy(fa_chr, index_name.data());
	strcpy(sa_chr, table_name.data());
	static char *fa[] = { fa_chr }, *sa[] = { sa_chr };
	//	cout << cmd_match[1] << "*" << cmd_match[1] << "*" << cmd_match[2] << "*" << cmd_match[3] << "*" << cmd_match[4] << "*" << cmd_match[5] << "*" << endl;
	smatch attribute_list;
	string attribute_str = string(cmd_match[5]);
	const regex attribute_split("\\s*(\\w+)(?:\\s*,\\s*(\\w+))?\\s*");
	if (regex_match(attribute_str, attribute_list, attribute_split)) {
		int i = 0;
		while (i < 2) {
			argu[i] = string(attribute_list[i + 1]);
			if (argu[i].size() >= MAXLENGTH)
				throw IllegalCommand(TooLongArgs);
			ta_len++;
			i++;
		}
	}
	else
		throw IllegalCommand(IllegalIndexList);
	static char ta_chr[2][MAXLENGTH];
	strcpy(ta_chr[0], argu[0].data());
	strcpy(ta_chr[1], argu[1].data());
	static char *ta[] = { ta_chr[0], ta_chr[1] };
	return sql_node(type, func, fa_len, fa, sa_len, sa, ta_len, ta);
}


sql_node DB_interpreter::create_table(smatch cmd_match) {
	byte type = CREATE_TABLE, func = 0;
	size_t fa_len = 1, sa_len = 0;
	//	cout << cmd_match[1] << "*" << cmd_match[2] << "*" << cmd_match[3] << "*" << cmd_match[4] << "*" << cmd_match[5] << endl;
	string table_name = string(cmd_match[2]), attribute_str = string(cmd_match[3]);
	static char fa_chr[MAXLENGTH];
	strcpy(fa_chr, table_name.data());
	static char *fa[] = { fa_chr };

	static char *sa[MAXNUM];
	static char sa_chr[MAXNUM][MAXLENGTH];
	vector<string> attr_vector;
	vector<string>::iterator iter;
	/* Cannot end with a ',' */
	if (regex_match(attribute_str, regex(".*,\\s*"))) {
		cout << "Syntax error in '" << attribute_str << "'" << endl;
		throw IllegalCommand(UndefinedCMD);
	}
	split(attribute_str, ",", attr_vector);
	for (iter = attr_vector.begin(); iter < attr_vector.end(); iter++) {
		if (attr_func(*iter, sa, sa_len) == false)
			throw IllegalCommand(UndefinedCMD);
	}

	/*regex attribute_split("\\s*(\\w+)\\s+(\\w[^,]+)\\s*");
	smatch attribute_list;

	for (iter = attr_vector.begin(); iter < attr_vector.end(); iter++) {
	if (regex_match(*iter, attribute_list, attribute_split)) {
	attr[0] = string(attribute_list[1]);
	attr[1] = string(attribute_list[2]);
	if (attr[0].size() >= MAXLENGTH || attr[1].size() >= MAXLENGTH)
	throw IllegalCommand(TooLongArgs);
	if ((sa_len + 2) >= MAXNUM)
	throw IllegalCommand(TooManyArgs);
	strcpy(sa_chr[sa_len], attr[0].data());
	sa[sa_len] = sa_chr[sa_len];
	sa_len++;
	strcpy(sa_chr[sa_len], attr[1].data());
	sa[sa_len] = sa_chr[sa_len];
	sa_len++;
	}
	else
	throw IllegalCommand(IllegalAttr);
	}*/
	/*last*/
	return sql_node(type, func, fa_len, fa, sa_len, sa);
}

/* SELECT */
sql_node DB_interpreter::select_clause(string SQL) {
	/**
	* 'select' grammar
	* select (info) from (info) (where (info))?
	*/
	smatch select_match;
	/***************************************************************************************************/
	/*                select	attribute_info	from	table_name	where	cond_info                  */
	/***************************************************************************************************/
	const regex select_split1("^\\s*select\\s+(\\*)\\s+from\\s+(\\S.*\\S)(?:\\s+where\\s+(\\S.*\\S))\\s*;\\s*"); // with 'where'
	const regex select_split2("^\\s*select\\s+(\\*)\\s+from\\s+(\\S.*\\S)\\s*;\\s*");	// without 'where'
	byte type = 0, func = 0, temp = 0;
	size_t fa_len = 1, sa_len = 1, ta_len = 0;
	// split SQL statement
	if (regex_match(SQL, select_match, select_split1))
		type = SELECT_WHERE;
	else if (regex_match(SQL, select_match, select_split2))
		type = SELECT_ORDINARY;
	else
		throw IllegalCommand(UndefinedCMD);
	static string table_name = string(select_match[2]);
	static char *fa[1], *sa[1], *ta[7]; /* At 2 condition statements */
	static char fa_chr[MAXLENGTH], sa_chr[MAXLENGTH], ta_chr[7][MAXLENGTH];
	string attr_list = string(select_match[1]), cond_list;
	strcpy(fa_chr, string(select_match[1]).data());
	fa[0] = fa_chr;
	strcpy(sa_chr, table_name.data());
	sa[0] = sa_chr;

	if (type == SELECT_WHERE) {
		smatch cond_split;
		string cond_str(select_match[3]);
		if (regex_match(cond_str, cond_split, regex("\\s*(\\S.*\\S)\\s+(and|or)\\s+(\\S.*\\S)\\s*"))) {
			int i = 1;
			while (i <= 3) {
				if (cond_func(string(cond_split[i]), ta, ta_len, temp)) {
					func |= (i == 1 ? temp : temp << 4);
					i = i + 2;
				}
				else
					throw IllegalCommand(UndefinedCMD);
			}
			if (string(cond_split[2]) == "and")
				func |= 0x80;
		}
		else if (cond_func(cond_str, ta, ta_len, temp)) {
			func |= temp;
		}
		else
			throw IllegalCommand(UndefinedCMD);
	}
	sql_node node(type, func, fa_len, fa, sa_len, sa, ta_len, ta);
	node.show();
	return node;
}
//	const regex select_split("^\\s*select\\s(\\w)+\\sfrom\\s(select\\s(\\w+)\\sfrom(\\w+)\\s(where\\s(.*))?)|(\\^((select)|(where).*))\\swhere\\s(.*)))");

sql_node DB_interpreter::insert_clause(string SQL) {
	byte type = INSERT, func = 0;
	size_t fa_len = 1, sa_len = 0;
	static char *fa[1], *sa[MAXNUM];
	static char fa_chr[MAXLENGTH], sa_chr[MAXNUM][MAXLENGTH];
	smatch insert_match;
	const regex insert_split("\\s*insert\\s+into\\s+(\\w+)\\s+values\\s*\\((.*)\\)\\s*;\\s*");
	SQL = toLower(SQL);
	if (regex_match(SQL, insert_match, insert_split)) {
		strcpy(fa_chr, string(insert_match[1]).data());
		fa[0] = fa_chr;
		string value_list(insert_match[2]);
		const regex value_split("\\s*(\\S+)\\s*(,|$)\\s*");
		sregex_iterator pos(value_list.cbegin(), value_list.cend(), value_split);
		sregex_iterator end;
		for (; pos != end; ++pos) {
			strcpy(sa_chr[sa_len], pos->str(1).data());
			sa[sa_len] = sa_chr[sa_len];
			sa_len++;
		}
	}
	else
		throw IllegalCommand(UndefinedCMD);
	sql_node node(type, func, fa_len, fa, sa_len, sa);
	node.show();
	return node;
}

sql_node DB_interpreter::drop_clause(string SQL) {
	const regex drop_split("\\s*drop\\s+(\\w+)\\s+(\\w+)\\s*;\\s*");
	smatch drop_match;
	size_t fa_len = 1, sa_len = 1;
	static char *fa[1], *sa[1];
	static char fa_chr[MAXLENGTH], sa_chr[MAXLENGTH];
	byte type, func = 0;
	if (regex_match(SQL, drop_match, drop_split)) {
		switch (str_map[string(drop_match[1])]) {
		case TABLE: type = DROP_TABLE; break;
		case INDEX: type = DROP_INDEX; break;
		case DATABASE: type = DROP_DATABASE; break;
		}
		strcpy(fa_chr, string(drop_match[1]).data());
		strcpy(sa_chr, string(drop_match[2]).data());
		fa[0] = fa_chr;
		sa[0] = sa_chr;
	}
	else
		throw IllegalCommand(UndefinedCMD);
	sql_node node(type, func, fa_len, fa, sa_len, sa);
	node.show();
	return node;
}

sql_node DB_interpreter::use_clause(string SQL) {
	const regex use_split("\\s*drop\\s+database\\s+(\\w+)\\s*;\\s*");
	smatch use_match;
	size_t fa_len = 1;
	static char *fa[1];
	static char fa_chr[MAXLENGTH];
	byte type = USE_SQL, func = 0;
	if (regex_match(SQL, use_match, use_split)) {
		strcpy(fa_chr, string(use_match[1]).data());
		fa[0] = fa_chr;
	}
	else
		throw IllegalCommand(UndefinedCMD);
	sql_node node(type, func, fa_len, fa);
	node.show();
	return node;
}

sql_node DB_interpreter::delete_clause(string SQL) {
	smatch delete_match;
	//	const regex delete_split1("^\\s*delete\\s+from\\s+(\\w+)\\s*;\\s*");
	//	const regex delete_split2("^\\s*delete\\s+from\\s+(\\w+)\\s+where\\s+(\\S.*\\S))\\s*;\\s*");
	const regex delete_split1("^\\s*delete\\s+from\\s+(\\w+)(?:\\s+where\\s+(\\S.*\\S))\\s*;\\s*"); // with 'where'
	const regex delete_split2("^\\s*delete\\s+from\\s+(\\w+)\\s*;\\s*");	// without 'where'

	byte type = 0, func = 0, temp = 0;
	size_t fa_len = 1, sa_len = 0;
	// split SQL statement
	if (regex_match(SQL, delete_match, delete_split1))
		type = DELETE_WHERE;
	else if (regex_match(SQL, delete_match, delete_split2))
		type = DELETE_ORDINARY;
	else
		throw IllegalCommand(UndefinedCMD);

	static char *fa[1], *sa[7]; /* At 2 condition statements */
	static char fa_chr[MAXLENGTH], sa_chr[7][MAXLENGTH];
	static string table_name = string(delete_match[1]);
	strcpy(fa_chr, table_name.data());
	fa[0] = fa_chr;

	if (type == DELETE_WHERE) {
		smatch cond_split;
		string cond_str(delete_match[2]);
		if (regex_match(cond_str, cond_split, regex("\\s*(\\S.*\\S)\\s+(and|or)\\s+(\\S.*\\S)\\s*"))) {
			int i = 1;
			while (i <= 3) {
				if (cond_func(string(cond_split[i]), sa, sa_len, temp)) {
					func |= (i == 1 ? temp : temp << 4);
					i = i + 2;
				}
				else
					throw IllegalCommand(UndefinedCMD);
			}
			if (string(cond_split[2]) == "and")
				func |= 0x80;
		}
		else if (cond_func(cond_str, sa, sa_len, temp)) {
			func |= temp;
		}
		else
			throw IllegalCommand(UndefinedCMD);
	}
	sql_node node(type, func, fa_len, fa, sa_len, sa);
	node.show();
	return node;
}
string DB_interpreter::toLower(string statement) {
	string::iterator iter = statement.begin();
	string replace;

	while (iter != statement.end()) {
		replace = replace.append(1, (*iter >= 'A' && *iter <= 'Z') ? (*iter + 0x20) : *iter);
		iter++;
	}
	return replace;
}

/* ERROR */
void IllegalCommand::DBerror() {
	switch (ErrorID) {
	case 0:
		cout << "Unknown Error!" << endl;
		break;
	case UndefinedCMD:
		cout << "Undefined Command!" << endl;
		break;
	case IllegalIndexList:
		cout << "Illegal Index List" << endl;
		break;
	case IllegalAttr:
		cout << "Illegal Attribute List" << endl;
		break;
	case TooLongArgs:
		cout << "Argument '***' is Too Long" << endl;
		break;
	case TooManyArgs:
		cout << "Too Many Arguments" << endl;
		break;
	default:
		break;
	}
}

/* HELP DOC */
void DB_interpreter::DBhelp() {
	cout << "help" << endl;
}


sql_node &sql_node::operator=(sql_node &node) {
	m_type = node.m_type;
	m_func = node.m_func;
	m_fa = node.m_fa;
	m_fa_len = node.m_fa_len;
	m_sa = node.m_sa;
	m_sa_len = node.m_sa_len;
	m_ta = node.m_ta;
	m_ta_len = node.m_ta_len;
	return *this;
}


void sql_node::show() {
	cout << "m_type = ";
	bin_show(m_type);
	cout << "m_func = ";
	bin_show(m_func);
	cout << "m_fa = ";
	argu_show(m_fa_len, m_fa);
	cout << "m_sa = ";
	argu_show(m_sa_len, m_sa);
	cout << "m_ta = ";
	argu_show(m_ta_len, m_ta);
}

void bin_show(byte num) {
	for (int i = 0; i < 8; i++) {
		int a = (num & 0x80) == 0 ? 0 : 1;
		cout << a;
		num = num << 1;
	}
	cout << endl;
}
void argu_show(size_t length, char **argv) {
	cout << length << " * ";
	if (!length) {
		cout << "NULL" << endl;
		return;
	}
	for (size_t i = 0; i < length; i++) {
		cout << argv[i];
		if (i != length - 1)
			cout << " * ";
	}
	cout << endl;
}


void split(const string& src, const string& separator, vector<string>& dest)
{
	string::size_type m_start = src.find_first_not_of(separator, 0);
	string str = src.substr(m_start, src.size() - m_start);
	string::size_type start = 0, index;
	string substring;
	dest.clear();
	do
	{
		index = str.find_first_of(separator, start);
		if (index != string::npos)
		{
			substring = str.substr(start, index - start);
			dest.push_back(substring);
			start = str.find_first_not_of(separator, index);
			if (start == string::npos) return;
		}
	} while (index != string::npos);

	//the last token
	substring = str.substr(start);
	dest.push_back(substring);
}
bool attr_func(string attr_str, char **sa, size_t &sa_len) {
	if (sa_len + 1 >= MAXNUM) {
		cout << "Too Many Attributes" << endl;
		return false;
	}
	smatch attr_match;
	static char sa_chr[MAXNUM][MAXLENGTH];
	if (regex_match(attr_str, attr_match, regex("\\s*primary\\s+key\\s*\\(\\s*(\\S+)\\s*\\)\\s*"))) {
		for (int i = 0; i < sa_len; i++) {
			if (string(attr_match[1]) == string(sa[i])) {
				strcpy(sa_chr[sa_len], string(attr_match[1]).data());
				sa[sa_len] = sa_chr[sa_len++];
				strcpy(sa_chr[sa_len], (char *)"primary");
				sa[sa_len] = sa_chr[sa_len++];
				return true;
			}
		}
		cout << "Attribute '" << attr_match[1] << "' Not Found!" << endl;
		return false;
	}
	vector<string> attr_vec;
	split(attr_str, " ", attr_vec);
	if (attr_vec.size() == 2 || (attr_vec.size() == 3 && attr_vec[2] == "unique")) {
		if (regex_match(attr_vec[1], regex("int|float|char\\(\\d\\d?\\)"))) {
			strcpy(sa_chr[sa_len], attr_vec[0].c_str());
			sa[sa_len] = sa_chr[sa_len++];
			if (attr_vec.size() == 3) {
				string type_unique = attr_vec[1] + "_u";
				strcpy(sa_chr[sa_len], type_unique.data());
			}
			else
				strcpy(sa_chr[sa_len], attr_vec[1].c_str());
			sa[sa_len] = sa_chr[sa_len++];
			return true;
		}
		else {
			cout << "Undefined Data Type: '" << attr_vec[1] << "'" << endl;
			return false;
		}
	}
	else {
		cout << "Error in '" << attr_str << "'." << endl;
		return false;
	}
}
bool cond_func(string cond_str, char **ta, size_t &ta_len, byte &func_temp) {
	smatch cond_match;//\\s*(\\S+)\\s*(\\S+)\\s*)
	const regex cond("([^<=>]+)\\s*(<|<=|=|<>|>|>=)\\s*([^<=>]+)\\s*");
	static char ta_chr[4][MAXLENGTH];
	if (regex_match(cond_str, cond_match, cond)) {
		strcpy(ta_chr[ta_len], string(cond_match[1]).data());
		ta[ta_len++] = ta_chr[ta_len];
		strcpy(ta_chr[ta_len], string(cond_match[3]).data());
		ta[ta_len++] = ta_chr[ta_len];
		if (OperMap.find(string(cond_match[2])) != OperMap.end()) {
			func_temp = OperMap[string(cond_match[2])] & 0xF;
			bin_show(func_temp);
			return true;
		}
		else
			return false;
	}
	else
		return false;
}
//int main(int argc, char *argv[])
//{
//	string statement;
//	DB_interpreter a;
//	while (true) {
//		try {
//			statement = a.read_input();
//			if (statement != "") {
//				cout << "statement = " << statement << endl;
//				a.Interpreter(statement);
//			}
//		}
//		catch (IllegalCommand e) {
//			e.DBerror();
//		}
//	}
//}