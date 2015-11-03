#ifndef CATALOGMANAGER_CATALOGMANAGER_H_
#define CATALOGMANAGER_CATALOGMANAGER_H_
#include <fstream>
#include <string>
#include <iostream>
#include <vector>
#include <string>
#include "CatalogException.h"

#include "Data.h"

const string FCATALOG = "catalog";
const string FDATA = "data";
const int BLOCK_SIZE = 4096;
const int FIRST_TABLE_LOCATION = 0x20;
const int DATA_END = 0xee; 
const int DATA_LOCATION = 0x1;
const int DELETE = 0xe5;

class CatalogManager {
private:
	fstream fCatalog;
	void init(unsigned numOfBlock);
	int findNextEmptyTable();
public:
	CatalogManager();
	virtual ~CatalogManager();

	void displayAllTable();
	bool findTable(string name);
	bool dropTable(string name);
	Table* getTableInfo(string name);
	bool creatTable(string name, vector<Field> fields);
	void verifyFirstDataLocation(int tableLocation, int newLocation);
	vector<string> getAllTable();

	bool deleteAll(string _name);
	void setIndex(Table* table, string indexname, int i, bool hasIndex) {
		char buffer[24];
		table->fields[i].hasIndex = true;
		table->fields[i].indexname = indexname;
		fCatalog.seekg(table->locationOfTable + 48 + 24 * i, ios::beg);
		fCatalog.read(buffer, 24);
		buffer[12] = hasIndex ? 1 : 0;
		strcpy(buffer + 16, indexname.data());
		fCatalog.seekp(-24, ios::cur);
		fCatalog.write(buffer, 24);
		table->show();
	}

	void dropIndex(Table* table, string indexname, int i, bool hasIndex) {
		char buffer[24];
		table->fields[i].hasIndex = false;
		table->fields[i].indexname = "noindex";
		fCatalog.seekg(table->locationOfTable + 48 + 24 * i, ios::beg);
		fCatalog.read(buffer, 24);
		buffer[12] = hasIndex ? 1 : 0;
		strcpy(buffer + 16, "noindex");
		fCatalog.seekp(-24, ios::cur);
		fCatalog.write(buffer, 24);
	}
};
#endif /* CATALOGMANAGER_CATALOGMANAGER_H_ */
