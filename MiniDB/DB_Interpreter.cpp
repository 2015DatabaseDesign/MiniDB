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
#define MAXNUM		100

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

void DB_interpreter::Interpreter(string statement) {
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
		case CREATE: create_clause(statement); break;
		case INSERT: insert_clause(statement); break;
		case DROP: drop_clause(statement); break;
		case SELECT: select_clause(statement); break;
		case DELETE: cout << "delete" << endl; break;
		case USE: cout << "use" << endl; break;
		default:
			throw IllegalCommand(UndefinedCMD);
		}
	}
	else
		throw IllegalCommand(UndefinedCMD);

}


/**
* \fn	void DB_interpreter::create_clause(string SQL)
*
* \brief	Create Clause.
*
* \author	Hac
* \date	10/16/2015
*
* \param	SQL	The SQL.
*/

void DB_interpreter::create_clause(string SQL) {
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
	const regex create_split("^\\s*create\\s+(\\w+)\\s(\\w+)(?:(?:\\s*\\((.+)\\))|(?:\\s+on\\s+(\\w+)\\s*\\(([^)]+)\\)))?\\s*;\\s*");
	// split SQL statement
	if (regex_match(SQL, create_match, create_split)) {
		map<string, int>::iterator iter = str_map.find(string(create_match[1]));
		int type;
		if (iter != str_map.end())
			type = iter->second;
		else {
			throw IllegalCommand(UndefinedCMD);
		}
		cout << create_match[1] << "*" << create_match[2] << "*" << create_match[3] << "*" << create_match[4] << "*" << create_match[5] << "*" << endl;
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
}

/**
* \fn	sql_node DB_interpreter::create_database(smatch cmd_match)
*
* \brief	Creates a database.
*
* \author	Hac
* \date	10/16/2015
*
* \param	cmd_match	A match specifying the command.
*
* \return	The new database.
*/

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

/**
* \fn	sql_node DB_interpreter::create_index(smatch cmd_match)
*
* \brief	Creates an index.
*
* \author	Hac
* \date	10/16/2015
*
* \param	cmd_match	A match specifying the command.
*
* \return	The new index.
*/

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

/**
* \fn	sql_node DB_interpreter::create_table(smatch cmd_match)
*
* \brief	Creates a table.
*
* \author	Hac
* \date	10/16/2015
*
* \param	cmd_match	A match specifying the command.
*
* \return	The new table.
*/

sql_node DB_interpreter::create_table(smatch cmd_match) {
	byte type = CREATE_TABLE, func = 0;
	//	cout << cmd_match[1] << "*" << cmd_match[2] << "*" << cmd_match[3] << "*" << cmd_match[4] << "*" << cmd_match[5] << endl;
	string table_name = string(cmd_match[2]), attribute_str = string(cmd_match[3]);
	vector<string> attr_vector;
	vector<string>::iterator iter;
	split(attribute_str, ",", attr_vector);
	static char fa_chr[MAXLENGTH];
	strcpy(fa_chr, table_name.data());
	static char *fa[] = { fa_chr };
	size_t fa_len = 1, sa_len = 0;
	static char *sa[MAXNUM];
	static char sa_chr[MAXNUM][MAXLENGTH];
	string attr[2];
	regex attribute_split("\\s*(\\w+)\\s+(\\w[^,]+)\\s*");
	smatch attribute_list;

	//regex sp("(\\w+)\\s+(\\S+)\\s*(,|$)");
	//sregex_iterator pos(attribute_str.cbegin(), attribute_str.cend(), sp);
	//sregex_iterator end;
	//for (; pos != end; ++pos) {
	//	cout << pos->str(1) << endl;
	//	cout << pos->str(2) << endl;
	//}
	//

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
	}
	/*last*/
	return sql_node(type, func, fa_len, fa, sa_len, sa);
}

/* SELECT */
void DB_interpreter::select_clause(string SQL) {
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
	vector<string> vec1, vec2;
	vector<string>::iterator iter;
	byte type, func;
	size_t fa_len = 1, sa_len = 1, ta_len = 0;
	// split SQL statement
	if (regex_match(SQL, select_match, select_split1)) {
		func = 0;
		type = SELECT_WHERE;
	}
	else if (regex_match(SQL, select_match, select_split2)) {
		ta_len = 0;
		func = 0;
		type = SELECT_ORDINARY;
	}
	else
		throw IllegalCommand(UndefinedCMD);
	static string table_name = string(select_match[2]);
	static char *fa[1], *sa[1], *ta[3]; /* At 2 condition statements */
	static char fa_chr[MAXLENGTH], sa_chr[MAXLENGTH], ta_chr[3][MAXLENGTH];
	string attr_list = string(select_match[1]), cond_list;
	strcpy(fa_chr, string(select_match[1]).data());
	fa[0] = fa_chr;
	strcpy(sa_chr, table_name.data());
	sa[0] = sa_chr;

	//cout << select_match[1] << endl;
	//split(string(select_match[1]), ",", vec1);
	//for (iter = vec1.begin(); iter < vec1.end(); iter++) {
	//	attr_list = *iter;
	//	strcpy(fa_chr[fa_len], attr_list.data());
	//	fa[fa_len] = fa_chr[fa_len];
	//	fa_len++;
	//}

	if (type == SELECT_WHERE) {
		cout << select_match[3] << endl;
		split(string(select_match[3]), " ", vec2);
		for (iter = vec2.begin(); iter < vec2.end(); iter++) {
			cond_list = *iter;
			if (ta_len == 1) {
				func |= OperMap[cond_list];
			}
			if (ta_len == 3) {
				string cond = toLower(cond_list);
				if (cond == "and")
					func |= 8;
				else if (cond == "or");
				else throw IllegalCommand(UndefinedCMD);

			}
			if (ta_len == 5) {
				func |= (OperMap[cond_list] << 4);

			}
			strcpy(ta_chr[ta_len], cond_list.data());
			ta[ta_len] = ta_chr[ta_len];
			ta_len++;
		}
	}
	sql_node node(type, func, fa_len, fa, sa_len, sa, ta_len, ta);
	node.show();
}
//	const regex select_split("^\\s*select\\s(\\w)+\\sfrom\\s(select\\s(\\w+)\\sfrom(\\w+)\\s(where\\s(.*))?)|(\\^((select)|(where).*))\\swhere\\s(.*)))");

void DB_interpreter::insert_clause(string SQL) {
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
}

void DB_interpreter::drop_clause(string SQL) {

}
/**
* \fn	string DB_interpreter::toLower(string statement)
*
* \brief	Converts a statement to a lower.
*
* \author	Hac
* \date	10/16/2015
*
* \param	statement	The statement.
*
* \return	statement as a string.
*/

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

/**
* \fn	sql_node &sql_node::operator=(sql_node &node)
*
* \brief	Assignment operator.
*
* \author	Hac
* \date	10/16/2015
*
* \param [in,out]	node	The node.
*
* \return	A shallow copy of this object.
*/

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

/**
* \fn	void sql_node::show()
*
* \brief	Shows this object.
*
* \author	Hac
* \date	10/16/2015
*/

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

/**
* \fn	void split(const string& src, const string& separator, vector<string>& dest)
*
* \brief	Splits.
*
* \author	Hac
* \date	10/16/2015
*
* \param	src				Source for the.
* \param	separator   	The separator.
* \param [in,out]	dest	Destination for the.
*/

void split(const string& src, const string& separator, vector<string>& dest)
{
	string str = src;
	string substring;
	string::size_type start = 0, index;
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

/**
* \fn	int main(int argc, char *argv[])
*
* \brief	Main entry-point for this application.
*
* \author	Hac
* \date	10/16/2015
*
* \param	argc	Number of command-line arguments.
* \param	argv	Array of command-line argument strings.
*
* \return	Exit-code for the process - 0 for success, else an error code.
*/

int main(int argc, char *argv[])
{
	string statement;
	DB_interpreter a;
	while (true) {
		try {
			statement = a.read_input();
			if (statement != "") {
				cout << "statement = " << statement << endl;
				a.Interpreter(statement);
			}
		}
		catch (IllegalCommand e) {
			e.DBerror();
		}

	}
	/*string statement;
	while (true) {
	statement = read_input();
	try {
	Interpreter(statement);
	}
	catch () {

	}
	}
	return 0;*/
}