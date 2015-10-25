// Buffer_Manager.cpp : Defines the entry point for the console application.
//
#include "stdlib.h"
#include "iostream"
#include "BufferManager.h"

using namespace std;
static fileInfo *FileHandle = NULL;
static blockInfo *BlockHandle = NULL;
static string database;
static size_t file_out_num = 0;
static FILE *file_in[5];
static size_t FILE_NUM = 0;

//************************************
// Method:    main
// FullName:  main
// Access:    public 
// Returns:   int
// Qualifier:
//************************************
int main()
{
	string tableName("hello");
	fileInfo *a;
	useDatabase(string("hac"));
	a = get_file_info(tableName, 1);
	getchar();
	return 0;
}

void mem_init() {
	FileHandle = (fileInfo *)malloc(sizeof(fileInfo) * MAX_FILE_ACTIVE);
	BlockHandle = (blockInfo *)malloc(sizeof(blockInfo) * MAX_BLOCK);
	if (FileHandle == NULL || BlockHandle == NULL) {
		/************************************************************************/
		/* throw                                                                */
		/************************************************************************/
		exit(1);
	}
	/* Initialization */
	for (int i = 0; i < MAX_FILE_ACTIVE; i++) {
		FileHandle[i].fileName = "";
		FileHandle[i].firstBlock = NULL;
		FileHandle[i].next = (i == MAX_FILE_ACTIVE) ? NULL : (FileHandle + i + 1);
	}
	for (int i = 0; i < MAX_BLOCK; i++) {
		BlockHandle[i].file = NULL;
		BlockHandle[i].prev = (i == 0) ? NULL : (BlockHandle + i - 1);
		BlockHandle[i].next = (i == MAX_BLOCK - 1) ? NULL : (BlockHandle + i + 1);
	}
}
void mem_quit() {
	free(FileHandle);
	free(BlockHandle);
	exit(0);
}
void useDatabase(string DB_Name) {
	database = string("E://MiniSQL//") + DB_Name;
}

blockInfo *findBlock() {
	fileInfo *filenode = FileHandle;
	blockInfo *blocknode;
	int iTime_max = 0;
	blockInfo *LRU_node = NULL;
	blockInfo *IDLE_node = NULL;
	/* Idle Block */
	for (int i = 0; i < MAX_BLOCK; i++)
		if (BlockHandle[i].file == NULL)
			return (BlockHandle + i);
	/* LRU Block */
	while (filenode != NULL) {
		blocknode = filenode->firstBlock;
		while (blocknode != NULL) {
			if (blocknode->lock == 0) {
				int m_iTime = blocknode->iTime--;
				if (iTime_max < m_iTime) {
					iTime_max = m_iTime;
					LRU_node = blocknode;
				}
			}
			blocknode = blocknode->next;
		}
		filenode = filenode->next;
	}
	LRU_node->prev->next = LRU_node->next;
	return LRU_node;
}

void replace(fileInfo *m_fileInfo, blockInfo *m_blockInfo) {
	blockInfo *block_node = m_fileInfo->firstBlock;
	while (block_node != NULL)
		block_node = block_node->next;
	block_node->next = m_blockInfo;
	m_blockInfo->prev = block_node;
	m_blockInfo->file = m_fileInfo;
}

blockInfo *get_file_block(string Table_Name, int fileType, int blockNum) {
	fileInfo *filenode = FileHandle, *new_file = NULL;
	blockInfo *blocknode, *new_block = NULL;
	FILE *file_read = NULL;
	while (filenode != NULL) {
		/* File in Buffer? */
		if (filenode->fileName == Table_Name) {
			file_read = file_in[(filenode - FileHandle) / sizeof(fileInfo)];
			if ((blocknode = filenode->firstBlock) == NULL) {			// No Block after The File
				filenode->firstBlock = new_block = findBlock();
				new_block->next = NULL;
				break;
			}
			while (blocknode->next != NULL)								// Find the Tail-Block Node
				blocknode = blocknode->next;
			new_block = findBlock();
			new_block->next = NULL;
			blocknode->next = new_block;
			break;
		}
		filenode = filenode->next;
	}
	/* File Not in Buffer */
	if (new_block == NULL) {
		new_file = get_file_info(Table_Name, fileType);
		file_read = file_in[(new_file - FileHandle) / sizeof(fileInfo)];
		new_block = findBlock();
		new_block->next = NULL;
		new_file->firstBlock = new_block;
	}
	/************************************************************************/
	/* Read Block Data From File                                            */
	/************************************************************************/
	if (!fseek(file_read, MAX_BLOCK * blockNum, SEEK_SET))
		cout << "out" << endl;
	if (fread((void *)new_block->cBlock, MAX_BLOCK, 1, file_read) == -1)
		cout << "file read error" << endl;
	new_block->blockNum = blockNum;
	return new_block;
}

void closeDatabase() {
	for (size_t i = 0; i < 5; i++)
		closeFile(i);
}

void closeFile(size_t m_file_num) {
	fileInfo *filenode = FileHandle + m_file_num;
	//if (filenode->fileName != m_fileName || filenode->type != m_fileType)
	//	/************************************************************************/
	//	/* ERROR                                                                */
	//	/************************************************************************/
	//	exit(1);
	blockInfo *blocknode = filenode->firstBlock;
	while (blocknode) {
		if (blocknode->dirtyBit)
			/************************************************************************/
			/* dirty bit                                                            */
			/************************************************************************/
			writeBlock(m_file_num, blocknode);
		blocknode->file = NULL;
	}
	fclose(file_in[m_file_num]);
}

void writeBlock(size_t m_file_num, blockInfo *block) {
	if (!fseek(file_in[m_file_num], MAX_BLOCK * block->blockNum, SEEK_SET)) {
		cout << "write back, seek error" << endl;
		exit(1);
	}
	if (fwrite((void *)block->cBlock, MAX_BLOCK, 1, file_in[m_file_num]) == -1) {
		cout << "write back, write error" << endl;
		exit(1);
	}
}

fileInfo *get_file_info(string fileName, int m_fileType) {
	fileInfo *node = FileHandle, *ret_node;
	/* File numbers < 5 */
	if (FILE_NUM < 5) {
		while (node->fileName != string(""))
			node = node->next;
		return node;
	}
	/* File numbers = 5 */
	for (size_t i = 0; i < file_out_num; i++)
		node = node->next;
	closeFile(file_out_num);
	file_in[file_out_num] = fopen(get_file_path(fileName, m_fileType).data(), "wb+");
	file_out_num = (file_out_num == 4) ? 0 : file_out_num++;
	node->fileName = fileName;
	node->type = m_fileType;
	node->firstBlock = NULL;
	return node;
}

blockInfo *readBlock(string m_fileName, int m_blockNum, int m_fileType) {
	blockInfo *ret_block = NULL;
	fileInfo *filenode = FileHandle;
	blockInfo *blocknode;
	/* Block in Buffer? */
	while (filenode != NULL) {
		if (filenode->fileName == m_fileName && filenode->type == m_fileType) {
			blocknode = filenode->firstBlock;
			while (blocknode != NULL) {
				if (blocknode->blockNum == m_blockNum)
					return blocknode;
				blocknode = blocknode->next;
			}
		}
		filenode = filenode->next;
	}
	/* Block Not in Buffer */
	ret_block = get_file_block(m_fileName, m_fileType, m_blockNum);
	return ret_block;
}

string get_file_path(string table_name, int file_type) {
	string path = string("E://MiniSQL//") + database + string("//")
		+ table_name + (file_type ? string(".dat") : string(".idx"));
	return path;
}