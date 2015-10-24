#ifndef __DATATYPE_H_
#define __DATATYPE_H_
#include <string>

const int ValueLen(4), ChildLen(3), OffsetLen(4), LeafLen(7), IntLen(5), FloatLen(10);
const int Int(0), Float(1), Char_n(2);
const int DataFile(0), IndexFile(1);
const int Greater(1), NotLess(2), Less(3), NotGreater(4);

struct blockInfo;
struct fileInfo {
	int type;                   // 0-> data file
								// 1 -> index file
	std::string fileName;            // the name of the file
	int recordAmount;           // the number of record in the file
	int freeNum;                // the free block number which could be used for the file
	int recordLength;           // the length of the record in the file
	fileInfo *next;             // the pointer points to the next file
	blockInfo *firstBlock;      // point to the first block within the file
};

struct blockInfo {
	int blockNum;               // the block number of the block, which indicate it when it be newed
	bool dirtyBit;              // 0 -> flase
								// 1 -> indicate dirty, write back
	blockInfo *prev;			// the pointer point to previous block
	blockInfo *next;            // the pointer point to next block
	fileInfo *file;             // the pointer point to the file, which the block belongs to
	int charNum;                // the number of chars in the block
	char *cBlock;               // the array space for storing the records in the block in buffer
	int iTime;                  // it indicate the age of the block in use
	int lock;                   // prevent the block from replacing
};

struct index_info {
	std::string index_name;     //the name of the index file
	int length;                 //the length of the value
	char type;                  //the type of the value
								//0---int,1---float,2----char(n)    
	long offset;                //the record offset in the table file
	std::string value;          //the value
};


#endif
