#include <math.h>
#include <string.h>
#include "mystr.h"
#include "IndexManager.h"

const int Int(0), Float(1), Char_n(2);
const int DataFile(0), IndexFile(1);

#include "datatype.h"

using namespace std;

IndexManager::IndexManager() :StrLen(10), N(3), LeafLeast(ceil((N - 1) / 2.0)), InnodeLeast(ceil(N / 2.0) - 1) {};

void IndexManager::setN(int n) {
	N = n;
	LeafLeast = ceil((N - 1) / 2.0);
	InnodeLeast = ceil(N / 2.0) - 1;
}

int AnyCmp(const string& s, const string& t, int type) {
	if (type == Int)
		return IntCmp(s, t);
	else if (type == Float)
		return FloatCmp(s, t);
	else if (type == Char_n)
		return StrCmp(s, t);
}

int IndexManager::search_leaf(const string& database, const string& table_name, const index_info& inform) {
	blockInfo *root = get_my_file_block(database, table_name, IndexFile, 0);
	blockInfo *node = get_my_file_block(database, table_name, IndexFile, root->blockNum);
	if (node->charNum == 0)
		return -3;
	string info = node->cBlock;
	int count(StrToI(info.substr(1, ValueLen))), start, length, blocknum(0);
	if (count == 0)
		return -7;
	if (inform.type == Int)
		length = IntLen;
	else if (inform.type == Float)
		length = FloatLen;
	else if (inform.type == Char_n)
		length = StrLen;
	else
		return -1;
	while (info[0] != '!') {
		count = StrToI(info.substr(1, ValueLen));
		//info = node->cBlock;
		int end;
		for (int i = 0; i < count; i++) {
			start = (length + ChildLen)*i + (1 + ValueLen + ChildLen);
			end = start + length - 1;
			int compare = AnyCmp(inform.value, info.substr(start, length), inform.type);
			if (compare >= 0) {
				if (end + ChildLen == node->charNum - 1) {
					blocknum = StrToI(info.substr(end + 1, ChildLen));
					break; // hit last pointer;
				}
				else
					continue;
			}
			else {
				blocknum = StrToI(info.substr(start - ChildLen, ChildLen));
				break;
			}
		}
		node = get_my_file_block(database, table_name, IndexFile, blocknum);
		if (node->charNum == 0)
			return -3;
		info = node->cBlock;
	}
	//info = node->cBlock;
	return node->blockNum;
}

int IndexManager::search_one(const string& database, const string& table_name, struct index_info& inform) {
	int count, start, length;
	int blocknum = search_leaf(database, table_name, inform);
	if (blocknum == -7) {
		inform.offset = 0;
		return blocknum;
	}
	if (blocknum == -1) {
		inform.offset = 0;
		return blocknum;
	}
	if (blocknum == -3) {
		inform.offset = 0;
		return blocknum;
	}
	if (inform.type == Int)
		length = IntLen;
	else if (inform.type == Float)
		length = FloatLen;
	else if (inform.type == Char_n)
		length = StrLen;
	blockInfo *node = get_my_file_block(database, table_name, IndexFile, blocknum);
	if (node->charNum == 0) {
		inform.offset = 0;
		return -3;
	}
	string info = node->cBlock;
	count = StrToI(info.substr(1, ValueLen));
	for (int i = 0; i < count; i++) {
		start = (length + LeafLen)*i + (1 + ValueLen + LeafLen);
		int compare = AnyCmp(inform.value, info.substr(start, length), inform.type);
		if (compare == 0) {
			string Linfo = info.substr(start - LeafLen, LeafLen);
			blocknum = StrToI(Linfo.substr(0, ChildLen));
			inform.offset = StrToI(Linfo.substr(ChildLen, ValueLen));
			return blocknum;
		}
	}
	inform.offset = 0;
	return -8;
}

int IndexManager::findNextLeafSibling(const string& database, const string& table_name, int blocknum) {
	blockInfo *node = get_my_file_block(database, table_name, IndexFile, blocknum);
	if (node->charNum == 0)
		return -3;
	string info = node->cBlock;
	if (info[node->charNum - 1] == '#')
		return 0;
	return StrToI(info.substr(node->charNum - 3, 3));
}

int IndexManager::findLeftMostSibling(const string& database, const string& table_name) {
	blockInfo *node = get_my_file_block(database, table_name, IndexFile, 0);
	if (node->charNum == 0)
		return -3;
	string left;
	string info = node->cBlock;
	while (info[0] == '?') {
		left = info.substr(1 + ValueLen, ChildLen);
		node = get_my_file_block(database, table_name, IndexFile, StrToI(left));
		info = node->cBlock;
	}
	return StrToI(left);
}

int IndexManager::findPrevLeafSibling(const string& database, const string& table_name, int blocknum) {
	int left = findLeftMostSibling(database, table_name);
	if (left == blocknum)
		return -1;
	blockInfo* Left = get_my_file_block(database, table_name, IndexFile, left);
	if (Left->charNum == 0)
		return -3;
	string info = Left->cBlock;
	while (info.substr(Left->charNum - 3, 3) != IToStr(blocknum, 3)) {
		left = StrToI(info.substr(Left->charNum - 3, 3));
		Left = get_my_file_block(database, table_name, IndexFile, left);
		if (Left->charNum == 0)
			return -3;
		info = Left->cBlock;
	}
	return left;
}

int IndexManager::findParent(const std::string& database, const std::string& table_name, const index_info& inform, int blocknum) {
	blockInfo *node = get_my_file_block(database, table_name, IndexFile, blocknum);
	if (node->charNum == 0)
		return -3;
	int length, parent;
	if (inform.type == Int)
		length = IntLen;
	else if (inform.type == Float)
		length = FloatLen;
	else if (inform.type == Char_n)
		length = StrLen;
	string info = node->cBlock;
	string value;
	if (info[0] == '?')
		value = info.substr(1 + ValueLen + ChildLen, length);
	else if (info[0] == '!')
		value = info.substr(1 + ValueLen + LeafLen, length);
	blockInfo *root = get_my_file_block(database, table_name, IndexFile, 0);
	if (root->blockNum == blocknum)
		return -1; // already root
	int start;
	info = root->cBlock;
	while (root->blockNum != blocknum) {
		int count = StrToI(info.substr(1, ValueLen));
		int end;
		int blockNum;
		for (int i = 0; i <count; i++) {
			start = (length + ChildLen)*i + (1 + ValueLen + ChildLen);
			end = start + length - 1;
			int compare = AnyCmp(inform.value, info.substr(start, length), inform.type);
			if (compare >= 0) {
				if (end + ChildLen == node->charNum - 1) {
					blockNum = StrToI(info.substr(end + 1, ChildLen));
					break; // hit last pointer;
				}
				else
					continue;
			}
			else {
				blockNum = StrToI(info.substr(start - ChildLen, ChildLen));
				break;
			}
		}
		parent = root->blockNum;
		root = get_my_file_block(database, table_name, IndexFile, blockNum);
		info = root->cBlock;
	}
	return parent;
}

void IndexManager::search_many(const std::string& database, const std::string& table_name, int& start, int& end, int type, struct index_info& inform, vector<int>& container) {
	int length, next;
	if (inform.type == Int)
		length = IntLen;
	else if (inform.type == Float)
		length = FloatLen;
	else if (inform.type == Char_n)
		length = StrLen;
	int blocknum = search_leaf(database, table_name, inform);
	blockInfo *leaf = get_my_file_block(database, table_name, IndexFile, blocknum);
	string info = leaf->cBlock;
	int compare;
	if (type == Greater || type == NotLess) {
		if (info[info.size() - 1] == '#')
			compare = AnyCmp(inform.value, info.substr(info.size() - length - 1, length), inform.type);
		else
			compare = AnyCmp(inform.value, info.substr(info.size() - length - ChildLen, length), inform.type);
	}
	else if (type == Less || NotGreater)
		compare = AnyCmp(inform.value, info.substr(1 + ValueLen + LeafLen, length), inform.type);
	string Linfo;
	if (type == Greater) {
		if (info[info.size() - 1] == '#') { // last node
			if (compare >= 0) {
				start = 0;
				return;
			}
			else {
				start = leaf->blockNum;
				// inform.offset = getoffset(database, table_name, inform, type, start);
			}
		}
		else {
			if (compare >= 0) {
				start = findNextLeafSibling(database, table_name, leaf->blockNum);
				// string tmp = get_my_file_block(database, table_name, IndexFile, start)->cBlock;
				// inform.offset = StrToI(tmp.substr(1+ValueLen+ChildLen, OffsetLen));
			}
			else {
				start = leaf->blockNum;
				// inform.offset = getoffset(database, table_name, inform, type, start);
			}
		}
		while ((next = findNextLeafSibling(database, table_name, blocknum)) != 0)
			blocknum = next;
		end = blocknum;
		get_index(database, table_name, start, end, type, inform, container);
	}
	else if (type == NotLess) {
		if (info[info.size() - 1] == '#') { // last node
			if (compare > 0) {
				start = 0;
				return;
			}
			else {
				start = leaf->blockNum;
				// inform.offset = getoffset(database, table_name, inform, type, start);
			}
		}
		else {
			if (compare > 0) {
				start = findNextLeafSibling(database, table_name, leaf->blockNum);
				// string tmp = get_my_file_block(database, table_name, IndexFile, start)->cBlock;
				// inform.offset = StrToI(tmp.substr(1+ValueLen+ChildLen, OffsetLen));
			}
		}
		while ((next = findNextLeafSibling(database, table_name, blocknum)) != 0)
			blocknum = next;
		end = blocknum;
		get_index(database, table_name, start, end, type, inform, container);
	}
	else if (type == Less) {
		start = findLeftMostSibling(database, table_name);
		if (leaf->blockNum == start) {
			if (compare <= 0) {
				start = 0;
				return;
			}
			else {
				end = leaf->blockNum;
				// inform.offset = getoffset(database, table_name, inform, type, end);
				get_index(database, table_name, start, end, type, inform, container);
			}
		}
		else {
			if (compare <= 0) {
				end = findPrevLeafSibling(database, table_name, leaf->blockNum);
				// string tmp = get_my_file_block(database, table_name, IndexFile, end)->cBlock;
				// inform.offset = getoffset(database, table_name, inform, type, end);
				get_index(database, table_name, start, end, type, inform, container);
			}
			else {
				end = leaf->blockNum;
				// inform.offset = getoffset(database, table_name, inform, type, end);
				get_index(database, table_name, start, end, type, inform, container);
			}
		}
	}
	else if (type == NotGreater) {
		start = findLeftMostSibling(database, table_name);
		if (leaf->blockNum == start) {
			if (compare < 0) {
				start = 0;
				return;
			}
			else {
				end = leaf->blockNum;
				// inform.offset = getoffset(database, table_name, inform, type, end);
				get_index(database, table_name, start, end, type, inform, container);
			}
		}
		else {
			if (compare < 0) {
				end = findPrevLeafSibling(database, table_name, leaf->blockNum);
				// string tmp = get_my_file_block(database, table_name, IndexFile, end)->cBlock;
				// inform.offset = getoffset(database, table_name, inform, type, end);
				get_index(database, table_name, start, end, type, inform, container);
			}
			else {
				end = leaf->blockNum;
				// inform.offset = getoffset(database, table_name, inform, type, end);
				get_index(database, table_name, start, end, type, inform, container);
			}
		}
	}
}

/*
int IndexManager::getoffset(const std::string& database, const std::string& table_name, const index_info& inform, int type, int blocknum) {
blockInfo *leaf = get_my_file_block(database, table_name, IndexFile, blocknum);
string info = leaf->cBlock;
int length, count, start, compare;
if (inform.type == Int)
length = IntLen;
else if (inform.type == Float)
length = FloatLen;
else if (inform.type == Char_n)
length = StrLen;
count = StrToI(info.substr(1, ValueLen));
for (int i = 0; i < count; i++) {
start = (length+LeafLen)*i + (1+ValueLen+LeafLen);
compare = AnyCmp(inform.value, info.substr(start, length), inform.type);
switch(type) {
case Greater:
if (compare < 0)
return StrToI(info.substr(start-OffsetLen, OffsetLen));
break;
case NotLess:
if (compare <= 0)
return StrToI(info.substr(start-OffsetLen, OffsetLen));
break;
case Less:
if (compare > 0) {
if (i == count - 1)
return StrToI(info.substr(start-OffsetLen, OffsetLen));
}
else
return StrToI(info.substr(start-OffsetLen, OffsetLen));
break;
case NotGreater:
if (compare >= 0) {
if (i == count - 1)
return StrToI(info.substr(start-OffsetLen, OffsetLen));
}
else
return StrToI(info.substr(start-OffsetLen, OffsetLen));
break;
}
}
return -1;
}
*/

void IndexManager::get_index(const std::string& database, const std::string& table_name, int begin, int end, int type, struct index_info& inform, vector<int>& container) {
	int length, count, start, compare;
	if (inform.type == Int)
		length = IntLen;
	else if (inform.type == Float)
		length = FloatLen;
	else if (inform.type == Char_n)
		length = StrLen;

	int between = begin, prev = -1;
	//bool flag = false;
	while (prev != end) {
		blockInfo *leaf = get_my_file_block(database, table_name, IndexFile, between);
		string info = leaf->cBlock;
		count = StrToI(info.substr(1, ValueLen));
		for (int i = 0; i < count; i++) {
			start = (length + LeafLen)*i + (1 + ValueLen + LeafLen);
			compare = AnyCmp(inform.value, info.substr(start, length), inform.type);
			switch (type) {
			case Greater:
				if (compare < 0)
					/*if (!flag)
						flag = true;
					else*/
						container.push_back(StrToI(info.substr(start - OffsetLen, OffsetLen)));
				break;
			case NotLess:
				if (compare <= 0)
					/*if (!flag)
						flag = true;
					else*/
						container.push_back(StrToI(info.substr(start - OffsetLen, OffsetLen)));
				break;
			case Less:
				if (compare > 0)
					container.push_back(StrToI(info.substr(start - OffsetLen, OffsetLen)));
				/*else
					if (!flag) {
						flag = true;
						container.push_back(StrToI(info.substr(start - OffsetLen, OffsetLen)));
					}*/
				break;
			case NotGreater:
				if (compare >= 0)
					container.push_back(StrToI(info.substr(start - OffsetLen, OffsetLen)));
				/*else
					if (!flag) {
						flag = true;
						container.push_back(StrToI(info.substr(start - OffsetLen, OffsetLen)));
					}*/
				break;
			}
		}
		prev = between;
		between = StrToI(info.substr(leaf->charNum - 3, 3));
	}
}


void IndexManager::delete_one(const string& database, const string& table_name, struct index_info& inform) {
	int L = search_leaf(database, table_name, inform);
	delete_entry(database, table_name, inform, L, inform.value, L);
}

// n: the node to be checked, K: key, nod: the child of n
void IndexManager::delete_entry(const string& database, const string& table_name, struct index_info& inform, int n, const string& K, int nod) {
	int length;
	if (inform.type == Int) {
		length = IntLen;
		//setN(340);
	}
	else if (inform.type == Float) {
		length = FloatLen;
		//setN(240);
	}
	else if (inform.type == Char_n) {
		StrLen = inform.length;
		length = StrLen;
		setN(4088 / (length + LeafLen));
	}
	blockInfo *Node = get_my_file_block(database, table_name, IndexFile, n);
	string info = Node->cBlock;
	string originN = info;
	int num = StrToI(info.substr(1, ValueLen));
	string tmpN = Node->cBlock;
	tmpN.replace(1, ValueLen, IToStr(num - 1, ValueLen));
	int start;
	for (int i = 0; i < num; i++) {
		int compare;
		if (tmpN[0] == '?')
			start = (length + ChildLen)*i + ValueLen + ChildLen +  1;
		else if (tmpN[0] == '!')
			start = (length + LeafLen)*i + ValueLen + LeafLen + 1;
		compare = AnyCmp(K, info.substr(start, length), inform.type);
		if (compare == 0) {
			if (tmpN[0] == '?') {
				if (tmpN.substr(start - 3, 3) == IToStr(nod, 3)) { // parent
					tmpN.replace(start - 3, length + 3, "");
					break;
				}
				else if (tmpN.substr(start, length + 3) == IToStr(nod, 3)) { // parent
					tmpN.replace(start, length + 3, "");
					break;
				}
			}
			else if (tmpN[0] == '!') { // leaf
				tmpN.replace(start - LeafLen, length + LeafLen, "");
				break;
			}
		}
	}
	int parent = findParent(database, table_name, inform, Node->blockNum);
	write(Node, tmpN);
	if (Node->blockNum == 0 && originN.substr(1, 4) == "0001") { // root has only one child 
		if (tmpN[0] == '?') // it is already leaf node, no children at all
			return;
		// make the child of Node as the root node
		blockInfo *child = get_my_file_block(database, table_name, IndexFile, StrToI(originN.substr(5, 3)));
		Node->blockNum = child->blockNum;
		child->blockNum = 0;
		writeRootBlock(database, table_name, child);
		deleteBlock(database, table_name, Node);
	}
	else {
		info = Node->cBlock;
		tmpN = info;
		if (info[0] == '!') { // leafnode
			if (StrToI(info.substr(1, 4)) < LeafLeast) {
				int n1 = findPrevLeafSibling(database, table_name, n);
				blockInfo *N1 = get_my_file_block(database, table_name, IndexFile, n1);
				int parent1 = findParent(database, table_name, inform, N1->blockNum);
				if (parent == parent1) {
					string K1 = originN.substr(1 + ValueLen + LeafLen, length);
					string tmpN1 = N1->cBlock;
					if (StrToI(info.substr(1, 4)) + StrToI(tmpN1.substr(1, 4)) <= N - 1) { // merge
						tmpN1.replace(1, 4, IToStr(StrToI(info.substr(1, 4)) + StrToI(tmpN1.substr(1, 4)), 4));
						tmpN1.replace(tmpN1.size() - 3, 3, "");
						tmpN1 += info.substr(5, strlen(Node->cBlock) - 5);
						write(N1, tmpN1);
						delete_entry(database, table_name, inform, parent, K1, n);
						deleteBlock(database, table_name, Node);
					}
					else { // move last elem. in prev. as first in node
						int numN1 = StrToI(tmpN1.substr(1, 4));
						string last = tmpN1.substr(5 + (length + LeafLen)*(numN1 - 1), length);
						tmpN1.replace(1, 4, IToStr(StrToI(tmpN1.substr(1, 4)) - 1, 4));
						tmpN1.replace(5 + (length + LeafLen)*(numN1 - 1), length, "");
						write(N1, tmpN1);
						tmpN.replace(1, 4, IToStr(StrToI(tmpN.substr(1, 4)) + 1, 4));
						tmpN.insert(5, last);
						write(Node, tmpN);
						string Km = last.substr(LeafLen, length);
						blockInfo* Parent = get_my_file_block(database, table_name, IndexFile, parent);
						string tmpP = Parent->cBlock;
						num = StrToI(tmpP.substr(1, 4));
						for (int i = 0; i < num; i++) {
							start = (3 + length)*i + 8;
							if (tmpP.substr(start, length) == K1) {
								tmpP.replace(start, K1.size(), Km);
								write(Parent, tmpP);
								break;
							}
						}
					}
				}
				else { // same as before, mutual parent;
					n1 = findNextLeafSibling(database, table_name, n);
					blockInfo *N1 = get_my_file_block(database, table_name, IndexFile, n1);
					string tmpN1 = N1->cBlock;
					string K1 = tmpN1.substr(1 + ValueLen + LeafLen, length);
					if (StrToI(tmpN.substr(1, 4)) + StrToI(tmpN1.substr(1, 4)) <= N - 1) { // merge
						tmpN1.replace(1, 4, IToStr(StrToI(info.substr(1, 4)) + StrToI(tmpN1.substr(1, 4)), 4));
						tmpN.replace(tmpN.size() - 3, 3, "");
						tmpN1.insert(5, tmpN.substr(5, Node->charNum - 5));
						write(N1, tmpN1);
						delete_entry(database, table_name, inform, parent, K1, n);
						deleteBlock(database, table_name, Node);
					}
					else { // move first elem. in next. as last in node
						string first = tmpN1.substr(1 + ValueLen + LeafLen, length);
						tmpN1.replace(1, 4, IToStr(StrToI(tmpN1.substr(1, 4)) - 1, 4));
						tmpN1.replace(5, length + LeafLen, "");
						write(N1, tmpN1);
						tmpN.replace(1, 4, IToStr(StrToI(tmpN.substr(1, 4)) + 1, 4));
						tmpN.insert(tmpN.size() - 3, first);
						write(Node, tmpN);
						string N1K1 = tmpN1.substr(1 + ValueLen + LeafLen, length);
						blockInfo* Parent = get_my_file_block(database, table_name, IndexFile, parent);
						string tmpP = Parent->cBlock;
						num = StrToI(tmpP.substr(1, 4));
						for (int i = 0; i < num; i++) {
							start = (3 + length)*i + 8;
							if (tmpP.substr(start, length) == K1) {
								tmpP.replace(start, length, N1K1);
								write(Parent, tmpP);
								break;
							}
						}
					}
				}
			}

		}
		else if (info[0] == '?') { // innode
			if (StrToI(info.substr(1, 4)) < InnodeLeast) { // merge
				blockInfo *Parent = get_my_file_block(database, table_name, IndexFile, parent);
				string tmpP = Parent->cBlock;
				int num = StrToI(tmpP.substr(1, 4));
				int start, end, n1, pre = 1; // bool previous
				string K1;
				for (int i = 0; i <= num; i++) {
					start = (3 + length)*i + 5;
					end = start + 3;
					if (tmpP.substr(start, 3) == IToStr(Node->blockNum, 3)) {
						if (end != 8) { // previous node
							n1 = StrToI(tmpP.substr(start - length - 3, 3));
							K1 = tmpP.substr(start - length, length);
							pre = 1;
						}
						else { // since no previous node, get next node
							n1 = StrToI(tmpP.substr(end + length, 3));
							K1 = tmpP.substr(end, length);
							pre = 0;
						}
						break;
					}

				}
				blockInfo *N1 = get_my_file_block(database, table_name, IndexFile, n1);
				string tmpN1 = N1->cBlock;
				if (pre == 1) {
					if (StrToI(tmpN.substr(1, 4)) + StrToI(tmpN1.substr(1, 4)) < N - 1) { // fit in one block;
						tmpN1.replace(1, 4, IToStr(StrToI(tmpN.substr(1, 4)) + StrToI(tmpN1.substr(1, 4)) + 1, 4));
						string tail = tmpN.substr(5, Node->charNum - 5);
						string head = tmpN1.substr(0, 5);
						tmpN1 = head + K1 + tail;
						write(N1, tmpN1);
						delete_entry(database, table_name, inform, parent, K1, n);
						deleteBlock(database, table_name, Node);
					}
					else { // redistribution
						string N1Pm = tmpN1.substr(N1->charNum - 3, 3); // last pointer in N1
						string N1K = tmpN1.substr(N1->charNum - 3 - length, length);
						tmpN1.replace(1, 4, IToStr(StrToI(tmpN1.substr(1, 4)) - 1, 4));
						tmpN1 = tmpN1.substr(0, N1->charNum - 3 - length);
						write(N1, tmpN1);
						// insert last pointer in N1 as the first in N
						tmpN.replace(1, 4, IToStr(StrToI(tmpN.substr(1, 4)) + 1, 4));
						string head = tmpN.substr(0, 5);
						string tail = tmpN.substr(5, Node->charNum - 5);
						tmpN = head + N1Pm + K1 + tail;
						write(Node, tmpN);
						tmpP.replace(start - length, length, N1K);
						write(Parent, tmpP);
					}
				}
				else if (pre == 0) {
					if (StrToI(tmpN.substr(1, 4)) + StrToI(tmpN1.substr(1, 4)) < N - 1) {
						tmpN1.replace(1, 4, IToStr(StrToI(tmpN.substr(1, 4)) + StrToI(tmpN1.substr(1, 4)) + 1, 4));
						string head = tmpN1.substr(0, 5);
						string tail = tmpN1.substr(5, N1->charNum - 5);
						string part = tmpN.substr(5, tmpN.size() - 5);
						tmpN1 = head + part + K1 + tail;
						write(N1, tmpN1);
						delete_entry(database, table_name, inform, parent, K1, n);
						deleteBlock(database, table_name, Node);
					}
					else { // move the first pointer in N1 to N
						string N1P1 = tmpN1.substr(5, 3);
						string N1K1 = tmpN1.substr(8, length);
						tmpN1.substr(1, 4) = IToStr(StrToI(tmpN1.substr(1, 4)) - 1, 4);
						string headN1 = tmpN1.substr(0, 5);
						string tailN1 = tmpN1.substr(8 + length, N1->charNum - 8 - length);
						tmpN1 = headN1 + tailN1;
						write(N1, tmpN1);
						tmpN.replace(1, 4, IToStr(StrToI(tmpN.substr(1, 4)) + 1, 4));
						tmpN = tmpN + K1 + N1P1;
						write(Node, tmpN);
						tmpP.replace(end, length, N1K1);
						write(Parent, tmpP);
					}
				}
			}
		}
	}
}

void IndexManager::insert_one(const std::string& database, const std::string& table_name, struct index_info& inform) {
	blockInfo *root = get_my_file_block(database, table_name, IndexFile, 0);
	int length, count, start, compare, Node;
	if (inform.type == Int) {
		length = IntLen;
		setN(340);
	}
	else if (inform.type == Float) {
		length = FloatLen;
		setN(240);
	}
	else if (inform.type == Char_n) {
		StrLen = inform.length;
		length = StrLen;
		setN(4088 / (length + LeafLen));
	}
	if (root->charNum == 0) {
		string tmp = "!0001";
		tmp += IToStr(0, ChildLen);
		tmp += IToStr(inform.offset, OffsetLen);
		tmp += SpanStrToLen(inform.value, length);
		tmp += "#";
		write(root, tmp);
		return;
	}

	//count = StrToI(info.substr(1, ValueLen));
	Node = search_leaf(database, table_name, inform);
	blockInfo *node = get_my_file_block(database, table_name, IndexFile, Node);
	string info = node->cBlock;
	if (StrToI(info.substr(1, ValueLen)) < N - 1)
		insert_leaf(database, table_name, inform, Node);
	else {  // divide T - L1
		blockInfo *L1 = get_my_new_block(database, table_name, -1);
		int l1 = L1->blockNum;
		blockInfo *T = get_my_new_block(database, table_name, l1);////////
		int t = T->blockNum; // tmp backup of leaf node;///////
		T->cBlock = (char *)info.c_str();//////
		string pn;
		if (info[node->charNum - 1] == '#')
			pn = info.substr(node->charNum - 1, 1);
		else
			pn = info.substr(node->charNum - ChildLen, ChildLen);
		try {
		insert_leaf(database, table_name, inform, t);/////

		}
		catch (const exception &e) {
			cout << e.what() << endl;
		}
		string tmpT = T->cBlock;
		int half = ceil((StrToI(info.substr(1, ValueLen)) + 1) / 2.0);
		string tmpL = "!" + IToStr(half, ValueLen);
		tmpL += tmpT.substr(1 + ValueLen, (LeafLen + length)*half);
		tmpL += IToStr(l1, ChildLen);
		write(node, tmpL);
		int n = StrToI(tmpT.substr(1, ValueLen)); // total value num
		string tmpL1;
		tmpL1 = "!" + IToStr(n - half, ValueLen);
		tmpL1 += tmpT.substr(1 + ValueLen + (LeafLen + length)*half, (n - half)*(length + LeafLen));
		tmpL1 += pn;
		write(L1, tmpL1);
		string K1 = tmpL1.substr(1 + ValueLen + LeafLen, length); // smallest value in L1;
		insert_parent(database, table_name, inform, Node, K1, l1);
		deleteBlock(database, table_name, T);
	}
}

// insert, do not concern about the division
void IndexManager::insert_leaf(const string& database, const string& table_name, struct index_info& inform, int Node) {
	blockInfo *leaf = get_my_file_block(database, table_name, IndexFile, Node);
	string info = leaf->cBlock;
	int count = StrToI(info.substr(1, ValueLen));
	int start, length, compare;
	if (inform.type == Int)
		length = IntLen;
	else if (inform.type == Float)
		length = FloatLen;
	else if (inform.type == Char_n)
		length = StrLen;
	string tmp = leaf->cBlock;
	string bnum = IToStr(0, ChildLen); // change!!!
	string offset = IToStr(inform.offset, OffsetLen);
	string insert = bnum + offset + SpanStrToLen(inform.value, length);
	tmp.replace(1, ValueLen, IToStr(StrToI(info.substr(1, ValueLen)) + 1, ValueLen));
	for (int i = 0; i < count; i++) {
		start = (length + LeafLen)*i + 1 + ValueLen + LeafLen;
		compare = AnyCmp(inform.value, info.substr(start, length), inform.type);

		if (compare < 0) {
			tmp.insert(start - LeafLen, insert);
			write(leaf, tmp);
			return;
		}
	}
		// insert in the last
	if (tmp[tmp.size() - 1] == '#')
		tmp.insert(tmp.size() - 1, insert);
	else
		tmp.insert(tmp.size() - ChildLen, insert);
	write(leaf, tmp);
}

// Node: the node to be inserted, K1: key, N1: sibling of Node
void IndexManager::insert_parent(const std::string& database, const std::string& table_name, struct index_info& inform, int Node, const string& K1, int N1) {
	blockInfo *root = get_my_file_block(database, table_name, IndexFile, 0);
	int length;
	if (inform.type == Int)
		length = IntLen;
	else if (inform.type == Float)
		length = FloatLen;
	else if (inform.type == Char_n)
		length = StrLen;
	if (root->blockNum == Node) { // Node == 0
		blockInfo *R = get_my_new_block(database, table_name, -1);
		int r = R->blockNum;
		string info = "?0001" + IToStr(r, ChildLen) + K1 + IToStr(N1, ChildLen);
		R->blockNum = Node;
		root->blockNum = r;
		write(R, info);
		writeRootBlock(database, table_name, R);
		return;
	}
	blockInfo *N0 = get_my_file_block(database, table_name, IndexFile, Node);
	int p = findParent(database, table_name, inform, N0->blockNum);
	blockInfo *P = get_my_file_block(database, table_name, IndexFile, p);
	string info = P->cBlock;
	if (StrToI(info.substr(1, 4)) < N - 1) {
		string tmp = P->cBlock;
		int count = StrToI(tmp.substr(1, ValueLen));
		int start;
		tmp.replace(1, ValueLen, IToStr(count + 1, ValueLen));
		for (int i = 0; i <= count; i++) {
			start = (length + ChildLen)*i + 1 + ValueLen;
			if (tmp.substr(start, ChildLen) == IToStr(Node, ChildLen)) {
				tmp.insert(start + ChildLen, K1 + IToStr(N1, ChildLen));
				return;
			}
		}
	}
	else { // divide P;
		blockInfo *P1 = get_my_new_block(database, table_name, -1);
		int p1 = P1->blockNum;
		blockInfo *T = get_my_new_block(database, table_name, p1);
		T->cBlock = P->cBlock;
		string tmpT = T->cBlock;
		int count = StrToI(tmpT.substr(1, ValueLen));
		int start;
		tmpT.replace(1, ValueLen, IToStr(count + 1, ValueLen));
		for (int i = 0; i < count; i++) {
			start = (length + ChildLen)*i + 1 + ValueLen;
			if (tmpT.substr(start, ChildLen) == IToStr(Node, ChildLen)) {
				tmpT.insert(start + ChildLen, K1 + IToStr(N1, ChildLen));
				break;
			}
		}
		// blockInfo *P1 = get_my_new_block(database, table_name);
		// int p1 = P1->blockNum;
		int half = ceil(count / 2.0);
		string tmpP = "?" + IToStr(half, ValueLen);
		tmpP += tmpT.substr(5, (length + ChildLen)*half + ChildLen);
		write(P, tmpP);
		// remaining half, move K11 up to parent
		string K11 = tmpT.substr(5 + (length + ChildLen)*half + ChildLen, length);
		string tmpP1 = "?" + IToStr(count - half, ValueLen);
		tmpP1 += tmpT.substr(5 + (length + ChildLen)*(half + 1), (length + ChildLen)*(count - half) + ChildLen);
		write(P1, tmpP1);
		insert_parent(database, table_name, inform, p, K11, p1);
		deleteBlock(database, table_name, T);
	}
}


void IndexManager::write(blockInfo *const node, const string& s) {
	// if (node->cBlock)
	//     delete[] node->cBlock;
	memset(node->cBlock, 0, 4096);
	// node->cBlock = new char[4000];

	strcpy(node->cBlock, s.c_str());
	node->charNum = strlen(node->cBlock);
	node->dirtyBit = true;
}




























