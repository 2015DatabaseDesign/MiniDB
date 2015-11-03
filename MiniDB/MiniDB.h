#pragma once
#include "IndexManager.h"
#include "BufferManager.h"
#include "RecordManager.h"
#include "CatalogManager.h"
#include "DB_Interpreter.h"
#include <set>

class MiniDB
{
private:
	IndexManager im;
	CatalogManager cm;			
	RecordManager rm;
	DB_interpreter ip;
	bool IsPrint;

	void Print(Selector& selector, const Table* tableDesc, const vector<string>& Attrs);
	void getCondsAndLinkOp(char ** input, int size, byte func, const Table * table, vector<Condition>& conds, LinkOp & lop, vector<string>& values);
	Data * getDataFromStr(string input);
	Op getOpFromFunc(int input);
	vector<int> getLineNumsAndLeftConds(const Table * table, vector<Condition>& conds, LinkOp lop, const vector<string>& values, bool & useIndex);
	void exec_file(string & path);
public:
	MiniDB() {
		mem_init();
		IsPrint = true;
	}
	bool getOperation(const sql_node& node);
	void readinput();
	bool CreateTable(const sql_node &node);
	bool CreateIndex(const sql_node &node);
	bool DropIndex(const sql_node &node);
	bool DropTable(const sql_node &node);
	int InsertTuple(const string& TableName, const Tuple& t, char** valueStr, int valuelen);
	bool Insert(const sql_node &node);
	int GetDataTypeFromStr(string input);
	bool Select(const sql_node &node);
	bool Delete(const sql_node &node);
	//void DeleteAll(string TableName);
	bool checkTupleFormat(const Table* table, const Tuple & t);
	virtual ~MiniDB() {}
};

