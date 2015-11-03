// Buffer_Manager.cpp : Defines the entry point for the console application.
//
//#include "stdlib.h"
//#include "iostream"
//#include "BufferManager.h"
#include "stdlib.h"
#include "io.h"
#include "iomanip"
#include "iostream"
#include "BufferManager.h"
//#include "windows.h"
#include "TCHAR.H"
#include "sys/types.h"
#include "direct.h"
//using namespace std;
//static fileInfo *FileHandle = NULL;
//static blockInfo *BlockHandle = NULL;
//static string database;
//static size_t file_out_num = 0;
//static FILE *file_in[5];
//static size_t FILE_NUM = 0;

//************************************
// Method:    main
// FullName:  main
// Access:    public 
// Returns:   int
// Qualifier:
//************************************
//int main()
//{
//	string tableName("hello");
//	fileInfo *a;
//	useDatabase(string("hac"));
//	a = get_file_info(tableName, 1);
//	getchar();
//	return 0;
//}
//
//void mem_init() {
//	FileHandle = (fileInfo *)malloc(sizeof(fileInfo) * MAX_FILE_ACTIVE);
//	BlockHandle = (blockInfo *)malloc(sizeof(blockInfo) * MAX_BLOCK);
//	if (FileHandle == NULL || BlockHandle == NULL) {
//		/************************************************************************/
//		/* throw                                                                */
//		/************************************************************************/
//		exit(1);
//	}
//	/* Initialization */
//	for (int i = 0; i < MAX_FILE_ACTIVE; i++) {
//		FileHandle[i].fileName = "";
//		FileHandle[i].firstBlock = NULL;
//		FileHandle[i].next = (i == MAX_FILE_ACTIVE) ? NULL : (FileHandle + i + 1);
//	}
//	for (int i = 0; i < MAX_BLOCK; i++) {
//		BlockHandle[i].file = NULL;
//		BlockHandle[i].prev = (i == 0) ? NULL : (BlockHandle + i - 1);
//		BlockHandle[i].next = (i == MAX_BLOCK - 1) ? NULL : (BlockHandle + i + 1);
//	}
//}
//void mem_quit() {
//	free(FileHandle);
//	free(BlockHandle);
//	exit(0);
//}
//void useDatabase(string DB_Name) {
//	database = string("E://MiniSQL//") + DB_Name;
//}
//
//blockInfo *findBlock() {
//	fileInfo *filenode = FileHandle;
//	blockInfo *blocknode;
//	int iTime_max = 0;
//	blockInfo *LRU_node = NULL;
//	blockInfo *IDLE_node = NULL;
//	/* Idle Block */
//	for (int i = 0; i < MAX_BLOCK; i++)
//		if (BlockHandle[i].file == NULL)
//			return (BlockHandle + i);
//	/* LRU Block */
//	while (filenode != NULL) {
//		blocknode = filenode->firstBlock;
//		while (blocknode != NULL) {
//			if (blocknode->lock == 0) {
//				int m_iTime = blocknode->iTime--;
//				if (iTime_max < m_iTime) {
//					iTime_max = m_iTime;
//					LRU_node = blocknode;
//				}
//			}
//			blocknode = blocknode->next;
//		}
//		filenode = filenode->next;
//	}
//	LRU_node->prev->next = LRU_node->next;
//	return LRU_node;
//}
//
//void replace(fileInfo *m_fileInfo, blockInfo *m_blockInfo) {
//	blockInfo *block_node = m_fileInfo->firstBlock;
//	while (block_node != NULL)
//		block_node = block_node->next;
//	block_node->next = m_blockInfo;
//	m_blockInfo->prev = block_node;
//	m_blockInfo->file = m_fileInfo;
//}
//
//blockInfo *get_file_block(string DBName, string Table_Name, int fileType, int blockNum) {
//	fileInfo *filenode = FileHandle, *new_file = NULL;
//	blockInfo *blocknode, *new_block = NULL;
//	FILE *file_read = NULL;
//	while (filenode != NULL) {
//		/* File in Buffer? */
//		if (filenode->fileName == Table_Name) {
//			file_read = file_in[(filenode - FileHandle) / sizeof(fileInfo)];
//			if ((blocknode = filenode->firstBlock) == NULL) {			// No Block after The File
//				filenode->firstBlock = new_block = findBlock();
//				new_block->next = NULL;
//				break;
//			}
//			while (blocknode->next != NULL)								// Find the Tail-Block Node
//				blocknode = blocknode->next;
//			new_block = findBlock();
//			new_block->next = NULL;
//			blocknode->next = new_block;
//			break;
//		}
//		filenode = filenode->next;
//	}
//	/* File Not in Buffer */
//	if (new_block == NULL) {
//		new_file = get_file_info(Table_Name, fileType);
//		file_read = file_in[(new_file - FileHandle) / sizeof(fileInfo)];
//		new_block = findBlock();
//		new_block->next = NULL;
//		new_file->firstBlock = new_block;
//	}
//	/************************************************************************/
//	/* Read Block Data From File                                            */
//	/************************************************************************/
//	if (!fseek(file_read, MAX_BLOCK * blockNum, SEEK_SET))
//		cout << "out" << endl;
//	if (fread((void *)new_block->cBlock, MAX_BLOCK, 1, file_read) == -1)
//		cout << "file read error" << endl;
//	new_block->blockNum = blockNum;
//	return new_block;
//}
//
//void closeDatabase() {
//	for (size_t i = 0; i < 5; i++)
//		closeFile(i);
//}
//
//void closeFile(size_t m_file_num) {
//	fileInfo *filenode = FileHandle + m_file_num;
//	//if (filenode->fileName != m_fileName || filenode->type != m_fileType)
//	//	/************************************************************************/
//	//	/* ERROR                                                                */
//	//	/************************************************************************/
//	//	exit(1);
//	blockInfo *blocknode = filenode->firstBlock;
//	while (blocknode) {
//		if (blocknode->dirtyBit)
//			/************************************************************************/
//			/* dirty bit                                                            */
//			/************************************************************************/
//			writeBlock(m_file_num, blocknode);
//		blocknode->file = NULL;
//	}
//	fclose(file_in[m_file_num]);
//}
//
//void writeBlock(size_t m_file_num, blockInfo *block) {
//	if (!fseek(file_in[m_file_num], MAX_BLOCK * block->blockNum, SEEK_SET)) {
//		cout << "write back, seek error" << endl;
//		exit(1);
//	}
//	if (fwrite((void *)block->cBlock, MAX_BLOCK, 1, file_in[m_file_num]) == -1) {
//		cout << "write back, write error" << endl;
//		exit(1);
//	}
//}
//
//fileInfo *get_file_info(string fileName, int m_fileType) {
//	fileInfo *node = FileHandle, *ret_node;
//	/* File numbers < 5 */
//	if (FILE_NUM < 5) {
//		while (node->fileName != string(""))
//			node = node->next;
//		return node;
//	}
//	/* File numbers = 5 */
//	for (size_t i = 0; i < file_out_num; i++)
//		node = node->next;
//	closeFile(file_out_num);
//	file_in[file_out_num] = fopen(get_file_path(fileName, m_fileType).data(), "wb+");
//	file_out_num = (file_out_num == 4) ? 0 : file_out_num++;
//	node->fileName = fileName;
//	node->type = m_fileType;
//	node->firstBlock = NULL;
//	return node;
//}
//
//blockInfo *readBlock(string DBName, string m_fileName, int m_blockNum, int m_fileType) {
//	blockInfo *ret_block = NULL;
//	fileInfo *filenode = FileHandle;
//	blockInfo *blocknode;
//	/* Block in Buffer? */
//	while (filenode != NULL) {
//		if (filenode->fileName == m_fileName && filenode->type == m_fileType) {
//			blocknode = filenode->firstBlock;
//			while (blocknode != NULL) {
//				if (blocknode->blockNum == m_blockNum)
//					return blocknode;
//				blocknode = blocknode->next;
//			}
//		}
//		filenode = filenode->next;
//	}
//	/* Block Not in Buffer */
//	ret_block = get_file_block("",m_fileName, m_fileType, m_blockNum);
//	return ret_block;
//}
//
//string get_file_path(string table_name, int file_type) {
//	string path = string("E://MiniSQL//") + database + string("//")
//		+ table_name + (file_type ? string(".dat") : string(".idx"));
//	return path;
//}


using namespace std;
static fileInfo FileHandle[MAX_FILE_ACTIVE];
static blockInfo BlockHandle[MAX_BLOCK];
//extern string DBName;
static size_t file_out_num = 0;
static FILE *file_in[MAX_FILE_ACTIVE];
static size_t FILE_NUM = 0;

//************************************
// Method:    main
// FullName:  main
// Access:    public 
// Returns:   int
// Qualifier:
//************************************
//int main()
//{
//	string tableName("hello");
//	fileInfo *a;
//	useDatabase(string("hac"));
//	a = get_file_info(tableName, 1);
//	getchar();
//	return 0;
//}

void mem_init() {
	if (FileHandle == NULL || BlockHandle == NULL) {
		/************************************************************************/
		/* throw                                                                */
		/************************************************************************/
		exit(1);
	}
	/* Initialization */
	for (int i = 0; i < MAX_FILE_ACTIVE; i++)
		file_in[i] = NULL;

	for (int i = 0; i < MAX_FILE_ACTIVE; i++) {
		FileHandle[i].fileName = "";
		FileHandle[i].firstBlock = NULL;
		FileHandle[i].next = (i == MAX_FILE_ACTIVE - 1) ? NULL : &FileHandle[i + 1];
	}
	for (int i = 0; i < MAX_BLOCK; i++) {
		BlockHandle[i].blockNum = i;
		BlockHandle[i].charNum = 0;
		BlockHandle[i].dirtyBit = 0;
		BlockHandle[i].iTime = 0;
		BlockHandle[i].lock = 0;
		BlockHandle[i].file = NULL;
		BlockHandle[i].prev = (i == 0) ? NULL : (BlockHandle + i - 1);
		BlockHandle[i].next = (i == MAX_BLOCK - 1) ? NULL : (BlockHandle + i + 1);
		BlockHandle[i].cBlock = (char *)malloc(BLOCK_LEN * sizeof(char));
	}
}
void mem_quit() {
	/* free cBlock */
	free(FileHandle);
	free(BlockHandle);
	exit(0);
}
string getDatabaseName()
{
	return DATABASE_NAME;
}
bool useDatabase(string DB_Name) {
	string path = string("E:\\MiniSQL\\") + DB_Name;
	if (_access(path.data(), 0) == 0)
	{
		DATABASE_NAME = DB_Name;
		return true;
	}
	else
		return false;
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
	if (LRU_node->dirtyBit) {
		size_t fileNum = LRU_node->file - FileHandle;
		writeBlock(fileNum, LRU_node);
	}
	LRU_node->dirtyBit = false;
	LRU_node->iTime = LRU_node->lock = 0;
	LRU_node->prev = LRU_node->next = NULL;
	return LRU_node;
}

void replace(fileInfo *m_fileInfo, blockInfo *m_blockInfo) {
	//show(m_fileInfo);
	if (m_fileInfo->firstBlock == NULL)
		m_fileInfo->firstBlock = m_blockInfo;
	else {
		blockInfo *block_node = m_fileInfo->firstBlock;
		while (block_node->next != NULL)
			block_node = block_node->next;
		block_node->next = m_blockInfo;
		m_blockInfo->prev = block_node;
	}
	m_blockInfo->next = NULL;
	m_blockInfo->file = m_fileInfo;
}

blockInfo *get_file_block(string fileName, int fileType, int blockNum) {
	fileInfo *filenode = FileHandle, *new_file = NULL;
	blockInfo *blocknode, *new_block = NULL;
	FILE *file_read = NULL;
	while (filenode != NULL) {
		/* File in Buffer? */
		if (filenode->fileName == fileName) {
			cout << filenode - FileHandle << endl;
			file_read = file_in[filenode - FileHandle];
			if ((blocknode = filenode->firstBlock) == NULL) {			// No Block after The File
				new_file = filenode;
				new_file->firstBlock = new_block = findBlock();
				new_block->next = NULL;
				break;
			}
			new_block = findBlock();									// Find the Tail-Block Node
			replace(filenode, new_block);
			//while (blocknode->next != NULL)								
			//	blocknode = blocknode->next;
			//new_block = findBlock();
			//new_block->next = NULL;
			//blocknode->next = new_block;
			break;
		}
		filenode = filenode->next;
	}
	/* File Not in Buffer */
	if (new_block == NULL) {
		new_file = get_file_info(fileName, fileType);
//show(new_file);
file_read = file_in[new_file - FileHandle];
new_block = findBlock();
replace(new_file, new_block);
	}
	/************************************************************************/
	/* Read Block Data From File                                            */
	//show(new_block);
	if (fseek(file_read, BLOCK_LEN * blockNum, SEEK_SET) == -1) {
		perror("Error in _FSEEK. Program terminated: ");

		exit(1);
	}

	if (fread((void *)new_block->cBlock, BLOCK_LEN, 1, file_read) != 1) {
		if (new_block->prev == NULL)
			new_file->firstBlock = NULL;
		else
			new_block->prev->next = NULL;
		new_block->file = NULL;
		return NULL;

		/*cout << "Error in _FREAD. Program terminated!" << endl;
		getchar();
		exit(1);*/
	}
	new_block->blockNum = blockNum;
	return new_block;
}
blockInfo *get_new_block(const string& file_name, int fileType, int blockNum) {
	blockInfo *new_block = findBlock();
	fileInfo *file_node = FileHandle;
	while (file_node)
		if (file_node->fileName != file_name || file_node->type != fileType)
			file_node = file_node->next;
		else
			break;
	replace(file_node, new_block);
	new_block->blockNum = blockNum;
	return new_block;
}
void closeDatabase() {
	for (size_t i = 0; i < 5; i++) {
		if (FILE_NUM == 0)
			return;
		closeFile(i);
	}
}
//void closeFile(string fileName, int fileType) {
//	fileInfo *filenode = FileHandle;
//	while (filenode) {
//		if (filenode->fileName == fileName && filenode->type == fileType)
//			break;
//		filenode = filenode->next;
//	}
//	if (!filenode)
//		return;
//	blockInfo *blocknode = filenode->firstBlock;
//	size_t m_file_num = filenode - FileHandle;
//	while (blocknode) {
//		if (blocknode->dirtyBit)
//			/************************************************************************/
//			/* dirty bit                                                            */
//			/************************************************************************/
//			writeBlock(m_file_num, blocknode);
//
//		blocknode->file = NULL;
//		blocknode = blocknode->next;
//
//	}
//	fclose(file_in[m_file_num]);
//	FILE_NUM--;
//	filenode->fileName = "";
//	filenode->firstBlock = NULL;
//}
void closeFile(size_t m_file_num) {
	fileInfo *filenode = FileHandle + m_file_num;
	if (filenode->fileName == "")
		return;
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
		blocknode = blocknode->next;

	}
	fclose(file_in[m_file_num]);
	file_in[m_file_num] = NULL;
	FILE_NUM--;
	filenode->fileName = "";
	filenode->firstBlock = NULL;
}

void writeBlock(size_t m_file_num, blockInfo *block) {
	if (fseek(file_in[m_file_num], BLOCK_LEN * block->blockNum, SEEK_SET) == -1) {
		cout << "write back, seek error" << endl;
		exit(1);
	}
	if (fwrite((void *)block->cBlock, BLOCK_LEN, 1, file_in[m_file_num]) <= 0) {
		cout << "write back, write error" << endl;
		exit(1);
	}
	block->dirtyBit = 0;
}

fileInfo *get_file_info(string fileName, int m_fileType) {
	fileInfo *node = FileHandle/*, *ret_node*/;
	/* File numbers < 5 */
	if (FILE_NUM < 5) {
		while (node->fileName != string(""))
			node = node->next;
		FILE_NUM++;
	}
	else {
		/* File numbers = 5 */
		for (size_t i = 0; i < file_out_num; i++)
			node = node->next;
		closeFile(file_out_num);
		file_out_num = (file_out_num == MAX_FILE_ACTIVE - 1) ? 0 : file_out_num++;
	}
	int file_num = node - FileHandle;
	if ((file_in[file_num] = fopen(get_file_path(fileName, m_fileType).data(), "rb+")) == NULL)
	{
		cout << "FILE NOT EXISTS, Program terminated!" << endl;
		getchar();
		exit(0);
	}
	node->fileName = string(fileName);
	node->type = m_fileType;
	node->firstBlock = NULL;
	//show(node);
	return node;
}

blockInfo *readBlock(string m_fileName, int m_blockNum, int m_fileType) {
	blockInfo *ret_block = NULL;
	fileInfo *filenode = FileHandle;
	blockInfo *blocknode;
	/* Block in Buffer? */
	while (filenode != NULL) {
		//	cout << "offset_origin = " << filenode - FileHandle << " * " << sizeof(fileInfo *) << " * " << sizeof(fileInfo) << endl;
		//cout << "offset = " << filenode - FileHandle << endl;
		if (filenode->fileName == m_fileName && filenode->type == m_fileType) {
			blocknode = filenode->firstBlock;
			while (blocknode != NULL) {
				if (blocknode->blockNum == m_blockNum)
					return blocknode;
				blocknode = blocknode->next;
			}
			break;
		}
		filenode = filenode->next;
	}
	/* Block Not in Buffer */
	ret_block = get_file_block(m_fileName, m_fileType, m_blockNum);
	return ret_block;
}

string get_file_path(string table_name, int file_type) {
	string path = string("E:\\MiniSQL\\") + DATABASE_NAME + string("\\")
		+ table_name + (file_type ? string(".idx") : string(".dat"));
	return path;
}
string get_directory_path(string databaseName) {
	string path = string("E:\\MiniSQL\\") + databaseName;
	return path;
}
size_t createDatabase(string databaseName) {
	string path = get_directory_path(databaseName);
	if (_access(path.data(), 0) == -1) {
		cout << _mkdir(path.data()) << endl;
		return 1;
	}
	cout << "Database '" << databaseName << "' Already Exists!" << endl;
	return 0;
}
size_t create_file(string fileName, int fileType) {
	string path = get_file_path(fileName, fileType);
	FILE *f;
	if (_access(path.data(), 0) == -1) {
		if (f = fopen(path.data(), "a")) {
			fclose(f);
			return 1;
		}
		cout << "Error in CREATE FILE!" << endl;
		return 0;
	}
	if (fileType)
		cout << "Index '";
	else
		cout << "Table '";
	cout << fileName << "' Already Exists!" << endl;
	return 0;
}
size_t delete_file(string fileName, int fileType) {
	string path = get_file_path(fileName, fileType);
	//string command = "rm " + path;
	int m_file_num = -1; /* Any number not between 0 and 4*/
	for (int i = 0; i < MAX_FILE_ACTIVE; i++) {
		if ((FileHandle + i)->fileName == fileName && (FileHandle + i)->type == fileType)
			if (file_in[i] != NULL) {
				m_file_num = i;
				break;
			}
	}

	if (_access(path.data(), 0) == 0) {
		if (m_file_num >= 0)
			closeFile(m_file_num);
		if (remove(path.data()) == 0)
			return 1;
		cout << "Error in DELETE FILE!" << endl;
		return 0;
	}
	else {
		if (fileType)
			cout << "Index '";
		else
			cout << "Table '";
		cout << fileName << "' Not Exists!" << endl;
		return 0;
	}
}
size_t deleteDatabase(string databaseName) {
	string path = get_directory_path(databaseName);
	string command = "rd/s/q " + path;
	if (_access(path.data(), 0) == 0) {
		if (system(command.data()) == 0)
			return 1;
		cout << "Error in DELETE DATABASE!" << endl;
		exit(1);
	}
	else {
		cout << "Database '" << databaseName << "' Not Exists!" << endl;
		return 0;
	}
}

//int main(int argc, char *argv[]) {
//	string name, path;
//	string command;
//	int type, num;
//	mem_init();
//	while (true) {
//		cout << "----->";
//		//cin >> command;
//
//		cin >> name;
//		if (name == "exit")
//			exit(0);
//		cin >> type >> num;
//		/*if (command == "cf")
//			create_file(name, type);
//		else if (command == "cd")
//			createDatabase(name);
//		else if (command == "df")
//			delete_file(name, type);
//		else if (command == "dd")
//			deleteDatabase(name);
//		else
//			cout << "Error Command" << endl;*/
//		
//		
//			//file_in[0] = fopen(get_file_path(name, 0).data(), "rb+");
//		blockInfo *node;
//		if ((node = readBlock(name, num, type)) == NULL)
//			node = get_new_block(name, type, num);
//		show(node);
//		getchar();
//		writeBlock(node->file - FileHandle, node);
//		
//		
////		cout << "create: " << create_file(name, type) << endl;
////		cout << "delete: " << delete_file(name, type) << endl;
////		cout << "delete: " << deleteDatabase(name) << endl;
//		cout << "==========================================" << endl;
//	}
//}
void show(blockInfo *node) {
	cout << ">========================================<" << endl;
	cout << left << setw(15) << "blockNum =" << left << setw(10) << node->blockNum << endl;
	cout << left << setw(15) << "dirtyBit =" << left << setw(10) << node->dirtyBit << endl;
	cout << left << setw(15) << "prev =" << left << setw(10) << (node->prev == NULL ? -1 : node->prev->blockNum) << endl;
	cout << left << setw(15) << "next =" << left << setw(10) << (node->next == NULL ? -1 : node->next->blockNum) << endl;
	cout << left << setw(15) << "file =" << left << setw(10) << (node->file == NULL ? "NULL" : node->file->fileName) << endl;
	cout << left << setw(15) << "charNum =" << left << setw(10) << node->charNum << endl;
	cout << left << setw(15) << "cBlock =" << left << setw(10) << (node->cBlock == NULL ? "NULL" : "NOT NULL") << endl;
	cout << left << setw(15) << "iTime =" << left << setw(10) << node->iTime << endl;
	cout << left << setw(15) << "lock =" << left << setw(10) << node->lock << endl;
	cout << ">========================================<" << endl << endl;
}
void show(fileInfo *node) {
	cout << ">========================================<" << endl;
	cout << left << setw(15) << "fileName =" << left << setw(10) << node->fileName << endl;
	cout << left << setw(15) << "type =" << left << setw(10) << node->type << endl;
	cout << left << setw(15) << "next =" << left << setw(10) << (node->next == NULL ? "NULL" : node->next->fileName) << endl;
	cout << left << setw(15) << "firstBlock =" << left << setw(10) << (node->firstBlock == NULL ? -1 : node->firstBlock->blockNum) << endl;
	cout << left << setw(15) << "recordAmount =" << left << setw(10) << node->recordAmount << endl;
	cout << left << setw(15) << "freeNum =" << left << setw(10) << node->freeNum << endl;
	cout << left << setw(15) << "recordLength =" << left << setw(10) << node->recordLength << endl;
	cout << ">========================================<" << endl << endl;
}
