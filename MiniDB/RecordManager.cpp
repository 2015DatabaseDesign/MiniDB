#include "RecordManager.h"
#include <fstream>
bool MatchMultiCond(vector<Condition> conds, const Tuple&t, LinkOp lop)
{
	if (lop == LinkOp::AND)
	{
		bool allMatch = true;
		for (Condition c : conds)
		{
			if (!c.match(t))
			{
				allMatch = false;
				break;
			}
		}
		return allMatch;
	}
	else //lop == LinkOp :: OR
	{
		//add by hedejin 11/4
		if (conds.size() == 0)
			return true;
		bool someMatch = false;
		for (Condition c : conds)
		{
			if (c.match(t))
			{
				someMatch = true;
				break;
			}
		}
		return someMatch;
	}
}
const Tuple & Selector::next() const
{
	return nextTuple;
}

bool Selector::hasNext()
{
	int TupleNumInPage = Page::calcTupleNum(tableDesc);
	int nextIndex = CurrentIndex+1;
	if (LineNums.size() == 0) 
	{
		//if (useIndex)
		//	return false;//Index has no reclt
		
		//no given line numbers, simply iterate tuples
		while (true)
		{
			int blocknum = nextIndex / TupleNumInPage;
			int LineNumInPage = nextIndex % TupleNumInPage;

			//read a new block
			if (LineNumInPage == 0)
			{
				blockInfo *info = readBlock(TableName, blocknum, DataFile);
				if (info == NULL)
				{
					//no next tuple matches conditions
					return false;
				}
				else
					CurrentBlock = info;
			}

			Page page(CurrentBlock->cBlock, tableDesc);
			for (int i = LineNumInPage; i < page.getTupleNum(); i++)
			{
				//CurrentIndex++;
				if (page.isTupleValid(i))
				{
					const Tuple& t = page.ReadTuple(i);
					bool allMatch = true;
					
					if (MatchMultiCond(conds, t, lop))
					{
						nextTuple = t;
						CurrentIndex = nextIndex;
						return true;
					}
				}
				nextIndex++;
			}
			blocknum++;
		}
	}
	else //use given line numbers
	{
		//for (int i = nextIndex; i < LineNums.size(); i++)
		for (unsigned nextIndex = givenLineNumIndex + 1; nextIndex < LineNums.size();nextIndex++)
		{
			int LineNum = LineNums[nextIndex];//###

			int blocknum = LineNum / TupleNumInPage;
			int LineNumInPage = LineNum % TupleNumInPage;
			if (CurrentBlock == NULL || blocknum != CurrentBlock->blockNum)	//Read next block
			{
				blockInfo *info = readBlock(TableName, blocknum, DataFile);
				if (info == NULL)//!!!error block shoudn't be null
					throw IndexManagerIncompatiblewithRecordManager();
				else
					CurrentBlock = info;
			}

			Page page(CurrentBlock->cBlock, tableDesc);
			if (!page.isTupleValid(LineNumInPage))
				throw IndexManagerIncompatiblewithRecordManager();

			const Tuple& t = page.ReadTuple(LineNumInPage);

			//if this tuple match all the conditions
			if (MatchMultiCond(conds, t, lop))
			{
				nextTuple = t;
				CurrentIndex = LineNum;
				givenLineNumIndex = nextIndex;
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
	int TupleNumInPage = Page::calcTupleNum(tableDesc);
	int nextIndex = CurrentIndex + 1;
	if (LineNums.size() == 0) 
	{
		//if (useIndex)
		//	return false;//Index has no result
		
		//no given line numbers, simply iterate tuples
		while (true)
		{
			int blocknum = nextIndex / TupleNumInPage;
			int LineNumInPage = nextIndex % TupleNumInPage;

			//read a new block
			if (LineNumInPage == 0)
			{
				blockInfo *info = readBlock(TableName, blocknum, DataFile);
				if (info == NULL)
				{
					//no next tuple matches conditions
					return false;
				}
				else
					CurrentBlock = info;
			}

			Page page(CurrentBlock->cBlock, tableDesc);
			for (int i = LineNumInPage; i < page.getTupleNum(); i++)
			{
				//CurrentIndex++;
				if (page.isTupleValid(i))
				{
					const Tuple& t = page.ReadTuple(i);
					bool allMatch = true;

					if (MatchMultiCond(conds, t, lop))
					{
						nextTuple = t;
						CurrentIndex = nextIndex;
						page.SetHeader(i, false); //lazy deletion
						page.SetHeader(i - (i % JUMPLENGTH), 2);//mark
						CurrentBlock->dirtyBit = true;//set it dirty
						return true;
					}
				}
				nextIndex++;
			}
			blocknum++;
		}
	}
	else //use given line numbers
	{
		//for (int i = nextIndex; i < LineNums.size(); i++)
		for (unsigned nextIndex = givenLineNumIndex + 1; nextIndex < LineNums.size(); nextIndex++)
		{
			int LineNum = LineNums[nextIndex];//###

			int blocknum = LineNum / TupleNumInPage;
			int LineNumInPage = LineNum % TupleNumInPage;
			if (CurrentBlock == NULL || blocknum != CurrentBlock->blockNum)	//Read next block
			{
				blockInfo *info = readBlock(TableName, blocknum, DataFile);
				if (info == NULL)//!!!error block shoudn't be null
					throw IndexManagerIncompatiblewithRecordManager();
				else
					CurrentBlock = info;
			}

			Page page(CurrentBlock->cBlock, tableDesc);
			if (!page.isTupleValid(LineNumInPage))
				throw IndexManagerIncompatiblewithRecordManager();

			const Tuple& t = page.ReadTuple(LineNumInPage);

			//if this tuple match all the conditions
			if (MatchMultiCond(conds, t, lop))
			{
				nextTuple = t;
				CurrentIndex = LineNum;
				givenLineNumIndex = nextIndex;
				page.SetHeader(LineNumInPage, false); //Lazy deletion
				page.SetHeader(LineNumInPage - LineNumInPage % JUMPLENGTH, 2);
				CurrentBlock->dirtyBit = true;
				return true;
			}
		}
		//run out of given line numbers
		return false;
	}
	//int TupleNumInPage = Page::calcTupleNum(tableDesc);

	//if (LineNums.size() == 0) //no given line numbers, simply iterate tuples
	//{
	//	/*int blocknum = CurrentIndex / TupleNumInPage;
	//	int LineNumInPage = CurrentIndex % TupleNumInPage;*/
	//	while (true)
	//	{
	//		int blocknum = CurrentIndex / TupleNumInPage;
	//		int LineNumInPage = CurrentIndex % TupleNumInPage;
	//		blockInfo *info = readBlock( TableName, blocknum, DataFile);
	//		if (info == NULL)
	//		{
	//			//no next tuple matches conditions
	//			return false;
	//		}

	//		Page page(info->cBlock, tableDesc);
	//		for (int i = LineNumInPage; i < page.getTupleNum(); i++)
	//		{
	//			if (page.isTupleValid(i))
	//			{
	//				const Tuple t = page.ReadTuple(i);

	//				if (MatchMultiCond(conds, t, lop))
	//				{
	//					nextTuple = t;
	//					page.SetHeader(i, false);	//mark it deleted
	//					info->dirtyBit = true;		//set it dirty
	//					CurrentIndex++;
	//					return true;
	//				}
	//			}
	//			//record current index
	//			CurrentIndex++;
	//		}
	//		blocknum++;
	//	}
	//}
	//else //use given line numbers
	//{
	//	for (int i = givenLineNumIndex; i < LineNums.size(); i++)
	//	{
	//		int LineNum = LineNums[i];//###
	//		int blocknum = LineNum / TupleNumInPage;
	//		int LineNumInPage = LineNum % TupleNumInPage;
	//		blockInfo *info = readBlock(TableName, blocknum, DataFile);
	//		if (info == NULL)
	//		{
	//			//!!!error block shoudn't be null
	//			throw IndexManagerIncompatiblewithRecordManager();
	//		}

	//		Page page(info->cBlock, tableDesc);
	//		if (!page.isTupleValid(LineNumInPage))
	//			throw IndexManagerIncompatiblewithRecordManager();

	//		const Tuple& t = page.ReadTuple(LineNumInPage);

	//		//if this tuple match all the conditions
	//		if (MatchMultiCond(conds, t, lop))
	//		{
	//			nextTuple = t;
	//			CurrentIndex = LineNum;
	//			givenLineNumIndex++;
	//			page.SetHeader(LineNumInPage, false);	//mark it deleted
	//			info->dirtyBit = true;		//set it dirty
	//			return true;
	//		}
	//	}
	//	//run out of given line numbers
	//	return false;
	//}
}

int RecordManager::Insert(const string& DBName, const string& TableName, Table* tableDesc, const Tuple& t)
{
	int blocknum = 0;
	blockInfo *info;
	while (true)
	{
		bool newblockflag = false;
		info = readBlock( TableName, blocknum, DataFile);
		if (info == NULL)
		{
			info = get_new_block(TableName, DataFile, blocknum);	//!!!should be provided by buffer manager
			newblockflag = true;
		}

		Page page(info->cBlock, tableDesc);
		if (newblockflag)
		{
			for (int i = 0; i < page.getTupleNum(); i++)
			{
				page.SetHeader(i, 4);
			}
		}
		
		int index = page.InsertableIndex();
		if (index != -1)
		{
			page.WriteTuple(t, index);

			/*page.SetHeader(index, true);*/
			page.SetHeader(index, 1);
			page.SetHeader(index - index % JUMPLENGTH, 3); // valid tuple --
			//set dirty
			info->dirtyBit = true;
			//insert_one index in IndexManager if needed
			int lineNum = blocknum*page.getTupleNum() + index;
			return lineNum;
		}
		blocknum++;
	}
}

bool RecordManager::CheckUnique(const string & DBName, const string & TableName, Table * tableDesc, const vector<Condition>& conditions)
{
	int blocknum = 0;
	while (true)
	{
		blockInfo *info = readBlock( TableName, blocknum, DataFile);
		if (info == NULL)
			break;

		Page page(info->cBlock, tableDesc);
		bool first = true;
		for (int i = 0; i < page.getTupleNum(); i++)
		{
			if (page.isTupleValid(i))
			{
				const Tuple& t = page.ReadTuple(i);
				for (auto condition : conditions)
				{
					if (condition.match(t))
					{
						std::cout << "ERROR: Violation of UNIQUE KEY constraint '" +
							TableName << "'." << endl;
						std::cout << "The duplicate key value occurs in: "
							<< t << ". The statement has been terminated." << endl;

						return false;
					}
				}
			}
		}
		blocknum++;
	}

	return true;// no tuple has the same value on the same attribute as the given tuple
}

const Tuple & Operation::next() const
{
	throw MethodNotImplementException();
}

bool Operation::hasNext()
{
	throw MethodNotImplementException();
}
