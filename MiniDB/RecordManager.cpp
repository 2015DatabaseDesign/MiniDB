#include "RecordManager.h"

const Tuple & Selector::next() const
{
	return nextTuple;
}

bool Selector::hasNext()
{
	int TupleNumInPage = Page::getTupleNum(tableDesc);

	if (LineNums.size() == 0) //no given line numbers, simply iterate tuples
	{
		int blocknum = CurrentIndex / TupleNumInPage;
		int LineNumInPage = CurrentIndex % TupleNumInPage;

		while (true)
		{
			blockInfo *info = readBlock(DBName, TableName, blocknum, DataFile);
			if (info == NULL)
			{
				//no next tuple matches conditions
				return false;
			}

			Page page(info->cBlock, tableDesc);
			for (int i = LineNumInPage; i < page.TupleNum; i++)
			{
				if (page.isTupleValid(i))
				{
					const Tuple& t = page.ReadTuple(i);
					bool allMatch = true;
					//for (Condition c : conds)
					//{
					//	if (!c.match(t))
					//	{
					//		allMatch = false;
					//		break;
					//	}
					//}

					//if this tuple match all the conditions
					if (MatchMultiCond(conds,t,lop))
					{
						nextTuple = t;
						return true;
					}
				}
				//record current index
				CurrentIndex++;
			}
			blocknum++;
		}
	}
	else //use given line numbers
	{
		for (int LineNum : LineNums)
		{
			int blocknum = LineNum / TupleNumInPage;
			int LineNumInPage = LineNum % TupleNumInPage;
			blockInfo *info = readBlock(DBName, TableName, blocknum, DataFile);
			if (info == NULL)
			{
				//!!!error block shoudn't be null
				throw new logic_error("Read block using line number given by IndexManager but fail");
			}

			Page page(info->cBlock, tableDesc);
			const Tuple& t = page.ReadTuple(LineNumInPage);

			/*bool allMatch = true;
			for (Condition c : conds)
			{
				if (!c.match(t))
				{
					allMatch = false;
					break;
				}
			}*/

			//if this tuple match all the conditions
			if (MatchMultiCond(conds, t, lop))
			{
				nextTuple = t;
				CurrentIndex = LineNum;
				return true;
			}
		}
		//run out of given line numbers
		return false;
	}
}

const Tuple & Deleter::next() const
{
	return nextTuple;
}

bool Deleter::hasNext()
{
	int TupleNumInPage = Page::getTupleNum(tableDesc);

	if (LineNums.size() == 0) //no given line numbers, simply iterate tuples
	{
		int blocknum = CurrentIndex / TupleNumInPage;
		int LineNumInPage = CurrentIndex % TupleNumInPage;

		while (true)
		{
			blockInfo *info = readBlock(DBName, TableName, blocknum, DataFile);
			if (info == NULL)
			{
				//no next tuple matches conditions
				return false;
			}

			Page page(info->cBlock, tableDesc);
			for (int i = LineNumInPage; i < page.TupleNum; i++)
			{
				if (page.isTupleValid(i))
				{
					const Tuple t = page.ReadTuple(i);
					//bool allMatch = true;
					//for (Condition c : conds)
					//{
					//	if (!c.match(t))
					//	{
					//		allMatch = false;
					//		break;
					//	}
					//}
					//if this tuple match all the conditions
					if (MatchMultiCond(conds, t, lop))
					{
						nextTuple = t;
						page.SetHeader(i, false);	//mark it deleted
						info->dirtyBit = true;		//set it dirty
						return true;
					}
				}
				//record current index
				CurrentIndex++;
			}
			blocknum++;
		}
	}
	else //use given line numbers
	{
		for (int LineNum : LineNums)
		{
			int blocknum = LineNum / TupleNumInPage;
			int LineNumInPage = LineNum % TupleNumInPage;
			blockInfo *info = readBlock(DBName, TableName, blocknum, DataFile);
			if (info == NULL)
			{
				//!!!error block shoudn't be null
				throw new logic_error("Read block using line number given by IndexManager but fail");
			}

			Page page(info->cBlock, tableDesc);
			const Tuple& t = page.ReadTuple(LineNumInPage);

			bool allMatch = true;
			//for (Condition c : conds)
			//{
			//	if (!c.match(t))
			//	{
			//		allMatch = false;
			//		break;
			//	}
			//}
			//if this tuple match all the conditions
			if (MatchMultiCond(conds, t, lop))
			{
				nextTuple = t;
				page.SetHeader(LineNumInPage, false);	//mark it deleted
				info->dirtyBit = true;		//set it dirty
				CurrentIndex = LineNum;
				return true;
			}
		}
		//run out of given line numbers
		return false;
	}
}

void RecordManager::Insert(const string& DBName, const string& TableName, Table* tableDesc, const Tuple& t)
{
	int blocknum = 0;
	blockInfo *info;
	while (true)
	{
		info = readBlock(DBName, TableName, blocknum, DataFile);
		if (info == NULL)
		{
			info = getNewBlock(DBName, TableName, blocknum, DataFile);	//!!!should be provided by buffer manager
		}

		Page page(info->cBlock, tableDesc);
		int index = page.InsertableIndex();
		if (index != -1)
		{
			page.WriteTuple(t, index);
			page.SetHeader(index, true);
			//set dirty
			info->dirtyBit = true;
			//insert_one index in IndexManager if needed
			break;
		}
		blocknum++;
	}
}
//
//
//void RecordManager::Select(const string & DBName, const string & TableName, Table * tableDesc)
//{
//	int blocknum = 0;
//	while (true)
//	{
//		blockInfo *info = readBlock(DBName, TableName, blocknum, DataFile);
//		if (info == NULL)
//			break;
//
//		Page page(info->cBlock, tableDesc);
//		bool first = true;
//		for (int i = 0; i < page.TupleNum; i++)
//		{
//			if (page.isTupleValid(i))
//			{
//				const Tuple& t = page.ReadTuple(i);
//				//first print the attribute names
//				if (first)
//				{
//					cout << *tableDesc << endl;
//					first = false;
//				}
//				//print tuples
//				cout << t << endl;
//			}
//		}
//
//		blocknum++;
//	}
//}
//
////Select with condition
//void RecordManager::Select(const string & DBName, const string & TableName, Table * tableDesc, const Condition & condition)
//{
//	int blocknum = 0;
//	while (true)
//	{
//		blockInfo *info = readBlock(DBName, TableName, blocknum, DataFile);
//		if (info == NULL)
//			break;
//
//		Page page(info->cBlock, tableDesc);
//		bool first = true;
//		for (int i = 0; i < page.TupleNum; i++)
//		{
//			if (page.isTupleValid(i))
//			{
//				const Tuple& t = page.ReadTuple(i);
//				if (condition.match(t))
//				{
//					//first print the attribute names
//					if (first)
//					{
//						cout << *tableDesc << endl;
//						first = false;
//					}
//
//					//print tuples
//					cout << t << endl;
//				}
//			}
//		}
//
//		blocknum++;
//	}
//}
//
//void RecordManager::Select(const string & DBName, const string & TableName, Table * tableDesc, const vector<Condition>& conditions, LinkOp lop)
//{
//
//}
//
bool RecordManager::CheckUnique(const string & DBName, const string & TableName, Table * tableDesc, const vector<Condition>& conditions)
{
	int blocknum = 0;
	while (true)
	{
		blockInfo *info = readBlock(DBName, TableName, blocknum, DataFile);
		if (info == NULL)
			break;

		Page page(info->cBlock, tableDesc);
		bool first = true;
		for (int i = 0; i < page.TupleNum; i++)
		{
			if (page.isTupleValid(i))
			{
				const Tuple& t = page.ReadTuple(i);
				//bool isMutiple = false;
				for (auto condition : conditions)
				{
					if (condition.match(t))
					{
						//isMutiple = true;    
						return false;
					}
				}
			}
		}

		blocknum++;
	}

	return true;// no tuple has the same value on the same attribute as the given tuple
}
//
////Select with condition and line number given by IndexManager
//void RecordManager::Select(const string& DBName, const string& TableName, Table* tableDesc, int LineNum)
//{
//	int TupleNumPerPage = Page::getTupleNum(tableDesc);
//
//	int blocknum = LineNum / TupleNumPerPage;
//	blockInfo *block = readBlock(DBName,TableName, blocknum, FileType::DataFile);
//
//	Page page(block->cBlock, tableDesc);
//	 
//	int LineNumInPage = LineNum % TupleNumPerPage;
//	if (page.isTupleValid(LineNumInPage))
//	{
//		const Tuple& t = page.ReadTuple(LineNumInPage);
//
//		//first print the attribute names
//		cout << *tableDesc << endl;
//
//		//print tuples
//		cout << t << endl;
//	}
//}
//
////Select with condition and line numbers given by IndexManager
//void RecordManager::Select(const string& DBName, const string& TableName, Table* tableDesc, deque<int> LineNums)
//{
//	int TupleNumPerPage = Page::getTupleNum(tableDesc);
//	for (int line : LineNums)
//	{
//		int blocknum = line / TupleNumPerPage;
//		blockInfo *block = readBlock(DBName,TableName, blocknum, FileType::DataFile);
//
//		Page page(block->cBlock, tableDesc);
//
//		int LineNumInPage = line % TupleNumPerPage;
//		if (page.isTupleValid(LineNumInPage))
//		{
//			const Tuple& t = page.ReadTuple(LineNumInPage);
//
//			//first print the attribute names
//			cout << *tableDesc << endl;
//
//			//print tuples
//			cout << t << endl;
//
//		}
//	}
//}
//
////Delete without given line number
//Tuple RecordManager::Delete(const string & DBName, const string & TableName, Table * tableDesc, const Condition & condition)
//{
//	int blocknum = 0;
//	while (true)
//	{
//		blockInfo *info = readBlock(DBName, TableName, blocknum, DataFile);	//!!!should be done in API
//		if (info == NULL)
//		{
//			//error :no such tuple
//		}
//
//		Page page(info->cBlock, tableDesc);
//		for (int i = 0; i < page.TupleNum; i++)
//		{
//			if (!page.isTupleValid(i))
//			{
//				continue;
//			}
//
//			const Tuple& t = page.ReadTuple(i);
//			if (condition.match(t))
//			{
//				page.SetHeader(i, false);
//				//set dirty
//				info->dirtyBit = true;
//				//return a Tuple contain datas in the deleted tuple 
//				//in order to apply deletion in index_manger
//				return t;
//			}
//		}
//	}
//}
//
//Tuple RecordManager::Delete(const string & DBName, const string & TableName, Table * tableDesc, int LineNum)
//{
//	int TupleNumPerPage = Page::getTupleNum(tableDesc);
//
//	int blocknum = LineNum / TupleNumPerPage;
//	blockInfo *info= readBlock(DBName, TableName, blocknum, FileType::DataFile);
//	if (info == NULL)
//	{
//		//error:page is not in this file
//	}
//
//	Page page(info->cBlock, tableDesc);
//
//	int LineNumInPage = LineNum % TupleNumPerPage;
//	if (page.isTupleValid(LineNumInPage))
//	{
//		page.SetHeader(LineNumInPage, false);
//		//delete indexs in IndexManager
//		info->dirtyBit = true;
//		//return a Tuple contain datas in the deleted tuple 
//		//in order to apply deletion in index_manger
//		return page.ReadTuple(LineNumInPage);
//	}
//	else
//	{
//		//tuple is not valid
//	}
//}
//
//vector<Tuple> RecordManager::Delete(const string & DBName, const string & TableName, Table * tableDesc, vector<int> LineNums)
//{
//	vector<Tuple> DeletdTuples;
//	int TupleNumPerPage = Page::getTupleNum(tableDesc);
//	for (int LineNum : LineNums)
//	{
//		int blocknum = LineNum / TupleNumPerPage;
//		blockInfo *info = readBlock(DBName, TableName, blocknum, FileType::DataFile);
//		if (info == NULL)
//		{
//			//error:page is not in this file
//		}
//
//		Page page(info->cBlock, tableDesc);
//
//		int LineNumInPage = LineNum % TupleNumPerPage;
//		if (page.isTupleValid(LineNumInPage))
//		{
//			page.SetHeader(LineNumInPage, false);
//			//delete indexs in IndexManager
//			info->dirtyBit = true;
//			//return a Tuple contain datas in the deleted tuple 
//			//in order to apply deletion in index_manger
//			DeletdTuples.push_back(page.ReadTuple(LineNumInPage));
//		}
//		else
//		{
//			//tuple is not valid
//		}
//	}
//
//	return DeletdTuples;
//}

const Tuple & Operation::next() const
{
	throw MethodNotImplementException();
}

bool Operation::hasNext()
{
	throw MethodNotImplementException();
}
