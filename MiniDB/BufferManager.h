#pragma once

const size_t BLOCK_LEN = 4096;
const size_t MAX_FILE_ACTIVE = 5;
const size_t MAX_BLOCK = 40;

#include "string"
using namespace std;


struct blockInfo;
struct fileInfo {
	int type;
	string fileName;
	int recordAmount;
	int freeNum;
	int recordLength;
	fileInfo *next;
	blockInfo *firstBlock;
};

struct blockInfo {
	int blockNum;
	bool dirtyBit;
	blockInfo *prev;
	blockInfo *next;
	fileInfo *file;
	int charNum;
	char *cBlock;
	int iTime;
	int lock;
};

void useDatabase(string DB_Name);
blockInfo *findBlock();
string get_file_path(string table_name, int file_type);
void replace(fileInfo *m_fileInfo, blockInfo *m_blockInfo);
blockInfo *get_file_block(string Table_Name, int fileType, int blockNum);
void closeDatabase(string DB_Name, bool m_flag);
void closeFile(size_t num);
void writeBlock(size_t m_file_num, blockInfo *block);
fileInfo *get_file_info(string fileName, int m_fileType);
blockInfo *readBlock(string m_fileName, int m_blockNum, int m_fileType);
void quitProg(string DB_Name);
void writeRootBlock(const string &database, const string &table_name, blockInfo *block);
void deleteBlock(const string &database, const string &table_name, blockInfo *block);
void Get_Table_Info(string DB_Name, string Table_Name, int &record_count, int &record_len, int &free);
void Write_Table_Info(string DB_Name, string Table_Name, int &record_count, int &record_len, int &free);
void Write_Index_Info(string DB_Name, string Index_Name, int &record_count, int &record_len, int &free);