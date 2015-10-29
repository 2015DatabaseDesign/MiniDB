#pragma once
//add by hedejin 10/28
#include "datatype.h"
const size_t BLOCK_LEN = 4096;
const size_t MAX_FILE_ACTIVE = 5;
const size_t MAX_BLOCK = 40;

#include "string"
using namespace std;

void useDatabase(string DB_Name);
blockInfo *findBlock();
string get_file_path(string table_name, int file_type);
void replace(fileInfo *m_fileInfo, blockInfo *m_blockInfo);
blockInfo *get_file_block(string DBName, string Table_Name, int fileType, int blockNum);
void closeDatabase(string DB_Name, bool m_flag);
void closeFile(size_t num);
void writeBlock(size_t m_file_num, blockInfo *block);
fileInfo *get_file_info(string fileName, int m_fileType);
blockInfo *readBlock(string DBName, string m_fileName, int m_blockNum, int m_fileType);
void quitProg(string DB_Name);
inline void writeRootBlock(const string &database, const string &table_name, blockInfo *block) {};
inline void deleteBlock(const string &database, const string &table_name, blockInfo *block) {};
void Get_Table_Info(string DB_Name, string Table_Name, int &record_count, int &record_len, int &free);
void Write_Table_Info(string DB_Name, string Table_Name, int &record_count, int &record_len, int &free);
void Write_Index_Info(string DB_Name, string Index_Name, int &record_count, int &record_len, int &free);
//add by hedejin
inline blockInfo *getNewBlock(string DBName, string m_fileName, int m_blockNum, int m_fileType) { return NULL; }
inline blockInfo *get_new_block(string DBName, string m_fileName) { return NULL; }
