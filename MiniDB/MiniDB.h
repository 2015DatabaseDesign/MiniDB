#pragma once
#include "IndexManager.h"
#include "BufferManager.h"
#include "RecordManager.h"
#include "CatalogManager.h"
#include "DB_Interpreter.h"

class MiniDB
{
private:
	IndexManager im;
	CatalogManager cm;			
	RecordManager rm;
	string DBName;
	DB_interpreter ip;

	void Print(Selector& selector, const Table* tableDesc);
public:
	MiniDB() {}
	void getOperation(const sql_node& node);
	void readinput();
	void CreateTable(const sql_node &node);
	void CreateIndex(const sql_node &node);
	void DropIndex(const sql_node &node);
	void DropTable(const sql_node &node);
	void InsertTuple(const string& TableName, const Tuple& t);
	void Insert(const sql_node &node);
	void Select(const sql_node &node);
	void Delete(const sql_node &node);
	void DeleteAll(string TableName);
	virtual ~MiniDB() {}
};

