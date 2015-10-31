#include <string>
#include <string.h>
#include "datatype.h"
#include "BufferManager.h"
using namespace std;
const int Int(0), Float(1), Char_n(2);
const int DataFile(0), IndexFile(1);

blockInfo *get_new_block(const std::string& table_name, int fileType, int blockNum);

blockInfo *get_my_file_block(const std::string& database, const std::string& table_name, int fileType, int blockNum) {
	blockInfo *node = readBlock(table_name, blockNum, fileType );
	if (node == NULL) {
		node = get_new_block(table_name, fileType, blockNum);
		node->charNum = 0;
		return node;
	}
	int i = -1;
	while (node->cBlock[++i] != '\0');
	node->charNum = i;
	return node;
}

blockInfo *get_my_new_block(const std::string& database, const std::string& table_name) {
	int blockNum = -1;
	blockInfo *node;
	while (node = readBlock(table_name, ++blockNum, IndexFile)) {
		if (node->cBlock[0] == '\0')
			break;
	}
	if (node == NULL)
		node = get_new_block(table_name, IndexFile, blockNum);
	node->charNum = 0;
	return node;
}


