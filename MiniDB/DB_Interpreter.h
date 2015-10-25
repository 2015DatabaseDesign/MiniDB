#pragma once

// DBMS state
#define UndefinedCMD		1
#define IllegalIndexList	2
#define IllegalAttr			3
#define TooLongArgs			4
#define TooManyArgs			5
#define CMD_HELP			0

// SQL argument
#define CREATE				1
#define	INSERT				2
#define DROP				3
#define SELECT				4
#define DELETE				5
#define USE					6
#define TABLE				7
#define DATABASE			8
#define INDEX				9
#define VIEW				10

// SQL encode
#define USE_SQL				0x00 /* 0b0000 for use								*/

#define CREATE_TABLE		0x11 /* 0b0001 for create, 0b0000 for table			*/
#define CREATE_DATABASE		0x12 /* 0b0001 for create, 0b0001 for database		*/
#define CREATE_INDEX		0x13 /* 0b0001 for create, 0b0010 for index			*/

#define SELECT_ORDINARY		0x20 /* 0b0010 for select, 0b0000 for ordinary		*/
#define SELECT_WHERE		0x21 /* 0b0010 for select, 0b0001 for where			*/

#define INSERT_SQL			0x30 /* 0b0011 for insert							*/

#define DELETE_ORDINARY		0x40 /* 0b0100 for delete, 0b0000 for ordinary		*/
#define DELETE_WHERE		0x41 /* 0b0100 for delete, 0b0001 for where			*/

#define DROP_INDEX			0x50 /* 0b0101 for drop,   0b0000 for index			*/
#define DROP_TABLE			0x51 /* 0b0101 for drop,   0b0001 for table			*/
#define DROP_DATABASE		0x52 /* 0b0101 for drop,   0b0010 for database		*/




#include "iostream"
#include "string"
#include "map"
#include "vector"

using namespace std;
typedef unsigned char byte;

static map<string, int> OperMap = {
	map<string, int>::value_type("=", 0),
	map<string, int>::value_type("<>", 1),
	map<string, int>::value_type("<", 2),
	map<string, int>::value_type(">", 3),
	map<string, int>::value_type("<=", 4),
	map<string, int>::value_type(">=", 5),
};
/**
* \class	IllegalCommand
*
* \brief	An illegal command.
*
* \author	Hac
* \date	10/16/2015
*/

class IllegalCommand {
private:
	size_t ErrorID = 0;
public:
	size_t getErrorID() {
		return ErrorID;
	};
	IllegalCommand(size_t ID) { ErrorID = ID; };
	~IllegalCommand() {};
	void DBerror();
};

/**
* \class	sql_node
*
* \brief	A SQL node.
*
* \author	Hac
* \date	10/16/2015
*/

class sql_node {
public:
	// detailed info about sql statement
	/* See MACRO definition */
	byte m_type;

	// assisted variable
	/* support complicated functions encoding for INTERPRETER */
	byte m_func;

	// first argument(s)
	/***********************************************************************************/
	/* E.g. create database ***, select *** from ..., drop index ***                   */
	/***********************************************************************************/
	size_t m_fa_len;
	char **m_fa;
	// second argument(s)
	/************************************************************************************/
	/* E.g. create table sth(***), select sth from ***, delete from sth where ***       */
	/************************************************************************************/
	size_t m_sa_len;
	char **m_sa;
	// third argument(s)
	/************************************************************************************/
	/* E.g. select sth from sth where ***                                               */
	/************************************************************************************/
	size_t m_ta_len;
	char **m_ta;
	sql_node &operator=(sql_node &);
	sql_node() {};
	sql_node(sql_node &node) { *this = node; };
	sql_node(byte type, byte func, size_t fa_len, char **fa, size_t sa_len = 0, char **sa = NULL, size_t ta_len = 0, char **ta = NULL) {
		m_type = type;
		m_func = func;
		m_fa = fa;
		m_fa_len = fa_len;
		m_sa = sa;
		m_sa_len = sa_len;
		m_ta = ta;
		m_ta_len = ta_len;
	};
	~sql_node() {};
	void show();
};

/**
* \class	DB_interpreter
*
* \brief	A database interpreter.
*
* \author	Hac
* \date	10/16/2015
*/

class DB_interpreter {
private:
	map<string, int> str_map = {
		map<string, int>::value_type("create", CREATE),
		map<string, int>::value_type("insert", INSERT),
		map<string, int>::value_type("drop", DROP),
		map<string, int>::value_type("select", SELECT),
		map<string, int>::value_type("delete", DELETE),
		map<string, int>::value_type("use", USE),

		map<string, int>::value_type("table", TABLE),
		map<string, int>::value_type("database", DATABASE),
		map<string, int>::value_type("index", INDEX),
		map<string, int>::value_type("view", VIEW)
	};
public:
	string toLower(string statement);

	string read_input();

	void Interpreter(string statement); /*command interpreting, return state*/
	void create_clause(string SQL);
	sql_node create_database(smatch);
	sql_node create_table(smatch);
	sql_node create_index(smatch);

	void drop_clause(string SQL);
	void drop_database(string SQL);
	void drop_table(string SQL);
	void drop_index(string SQL);

	void select_clause(string SQL);

	void insert_clause(string SQL);

	void delete_clause(string SQL);
	void delete_from_where(string SQL);

	void use_clause(string SQL);

	void execfile_clause(string SQL);

	void quit_clause(string SQL);

	void get_expression(string SQL_CMD);

	void DBhelp();
};


void bin_show(byte);
void argu_show(size_t, char **);
void split(const string&, const string&, vector<string>&);