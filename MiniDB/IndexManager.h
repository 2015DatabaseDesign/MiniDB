#ifndef __INDEXMANAGER_H_
#define __INDEXMANAGER_H_

#include<string>
#include<deque>
#include "datatype.h"
#include "BufferManager.h"

//blockInfo *get_file_block(const std::string& database, const std::string& table_name, int fileType, int blockNum);
//void writeRootBlock(const std::string& database, const std::string& table_name, blockInfo *block);
//void deleteBlock(const std::string& database, const std::string& table_name, blockInfo *block);
//blockInfo *get_new_block(const std::string& database, const std::string& table_name);

class IndexManager {
private:
	int N; // number of pointers in each node
	int StrLen;
	int LeafLeast;
	int InnodeLeast;

	void setN(int n);
	int search_leaf(const std::string& database, const std::string& table_name, const index_info& inform);
	int findNextLeafSibling(const std::string& database, const std::string& table_name, int blocknum);
	int findLeftMostSibling(const std::string& database, const std::string& table_name);
	int findPrevLeafSibling(const std::string& database, const std::string& table_name, int blocknum);
	int findParent(const std::string& database, const std::string& table_name, const index_info& inform, int blocknum);
	int getoffset(const std::string& database, const std::string& table_name, const index_info& inform, int type, int blocknum);
	void insert_divide(const std::string& database, const std::string& table_name, struct index_info& inform, int leaf1, int leaf2, char* leafpoint2);
	void write(blockInfo *const node, const std::string& s);
	void delete_entry(const std::string& database, const std::string& table_name, struct index_info& inform, int n, const std::string& K, int nod);
	void insert_leaf(const std::string& database, const std::string& table_name, struct index_info& inform, int Node);
	void insert_parent(const std::string& database, const std::string& table_name, struct index_info& inform, int Node, const std::string& K, int n);
public:
	IndexManager();
	//返回查找的block的block号。
	int search_one(const std::string& database, const std::string& table_name, struct index_info& inform);
	void search_many(const std::string& database, const std::string& table_name, int& start, int& end, int type, struct index_info& inform);
// !!!根据10/24的讨论，将search_many的signatrue做修改，返回一个行号的集合，方便RecordManager的使用
	//void search_many(const std::string& database, const std::string& table_name, int type, struct index_info& inform, deque<int>& LineNums);

	void insert_one(const std::string& database, const std::string& table_name, struct index_info& inform);
	void delete_one(const std::string& database, const std::string& table_name, struct index_info& inform);
};
#endif
