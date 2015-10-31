#include "stdafx.h"
#include "CppUnitTest.h"
#include "..\MiniDB\MiniDB.h"
using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace DBTest
{

	TEST_CLASS(ConditionTest)
	{
	private:
		Tuple t;
		Tuple t2;
	public:
		ConditionTest()
		{
			vector<shared_ptr<Data>> datas;
			datas.push_back(make_shared<Int>(233));
			datas.push_back(make_shared<Float>(5.2));
			datas.push_back(make_shared<Char>("Hey"));
			t = Tuple(datas);

			vector<shared_ptr<Data>> datas2;
			datas2.push_back(make_shared<Int>(123));
			datas2.push_back(make_shared<Int>(456));
			datas2.push_back(make_shared<Float>(78.9));
			datas2.push_back(make_shared<Char>("10"));
			t2 = Tuple(datas2);

		}
		~ConditionTest()
		{

		}

		TEST_METHOD(MatchTestEqual)
		{
			Condition cond1(Op::EQUALS, make_shared<Int>(233), 0);
			Assert::IsTrue(cond1.match(t));
			Condition cond2(Op::EQUALS, make_shared<Float>(5.2), 1);
			Assert::IsTrue(cond2.match(t));
			Condition cond3(Op::EQUALS, make_shared<Char>("Hey"), 2);
			Assert::IsTrue(cond3.match(t),L"Char comparation",LINE_INFO());
		}

		TEST_METHOD(MatchTestComp)
		{
			Condition cond1(Op::LESS_THAN, make_shared<Int>(2333), 0);
			Assert::IsTrue(cond1.match(t));
			Condition cond2(Op::GREATER_THAN, make_shared<Float>(2.2), 1);
			Assert::IsTrue(cond2.match(t));
			Condition cond3(Op::NOT_EQUAL, make_shared<Char>("Hey?"), 2);
			Assert::IsTrue(cond3.match(t));


			Condition cond4(Op::LESS_THAN_OR_EQUAL, make_shared<Int>(233), 0);
			Assert::IsTrue(cond4.match(t));
			Condition cond5(Op::GREATE_THAN_OR_EQUAL, make_shared<Float>(5.2), 1);
			Assert::IsTrue(cond5.match(t));
		}

		TEST_METHOD(MatchMultiCondTest)
		{
			//t:  233 | 5.2 | "Hey"
			//t2: 123 | 456 | 78.9 | "10"
			vector<Condition> conds;
			conds.push_back(Condition(Op::LESS_THAN, make_shared<Int>(2333), 0));
			conds.push_back(Condition(Op::GREATER_THAN, make_shared<Float>(2.2), 1));
			conds.push_back(Condition(Op::EQUALS, make_shared<Char>("Hey"), 2));
			Assert::IsTrue(MatchMultiCond(conds, t, AND));

			vector<Condition> conds2;
			conds2.push_back(Condition(Op::LESS_THAN_OR_EQUAL, make_shared<Int>(23), 0));
			conds2.push_back(Condition(Op::GREATER_THAN, make_shared<Float>(5.2), 1));
			Assert::IsFalse(MatchMultiCond(conds2, t, OR));
			Assert::IsFalse(MatchMultiCond(conds2, t2, AND));
			Assert::IsTrue(MatchMultiCond(conds2, t2, OR));

		}

	};

	TEST_CLASS(TupleTest)
	{
	private:
		Tuple t0;
		Tuple t1;
		Tuple t2;
		const Table* table;
	public:
		TupleTest()
		{
			//construct tuples
			vector<shared_ptr<Data>> datas;
			datas.push_back(make_shared<Char>("hedejin"));
			datas.push_back(make_shared<Int>(20));
			datas.push_back(make_shared<Char>("3130100676"));
			datas.push_back(make_shared<Float>(179.0));
			t0 = Tuple(datas);
			t2 = Tuple(datas);
			vector<shared_ptr<Data>> datas2;
			datas2.push_back(make_shared<Char>("Justin"));
			datas2.push_back(make_shared<Int>(20));
			datas2.push_back(make_shared<Char>("3130100645"));
			datas2.push_back(make_shared<Float>(167.2));
			t1 = Tuple(datas2);

			vector<Field> fields;
			fields.push_back(Field("StuName", DataType::CHAR, NORMAL, 10, false));
			fields.push_back(Field("StuAge", DataType::INT, NORMAL));
			fields.push_back(Field("ID", DataType::CHAR, PRIMARY, 10, true, "StuDefalutIndex"));
			fields.push_back(Field("Height", DataType::FLOAT, NORMAL));
			table = new Table("TestTable", fields, 28, 0, 0);
		}
		~TupleTest() 
		{
			delete table;
		}
		TEST_METHOD(EqualTest)
		{
			Assert::IsFalse(t0 == t1);
			Assert::IsTrue(t0 == t2);
		}

		TEST_METHOD(TupleWriteDataTest)
		{
			char* data = new char[100];
			t0.writeData(data, table);
//			char *expect = new char[100];
			//Logger::WriteMessage(data);
		}
	};

	TEST_CLASS(PageTest)
	{
	private:
		Page page;
		Table* table;
		Tuple t0;
		Tuple t1;
	public:
		PageTest()
		{
			vector<Field> fields;
			fields.push_back(Field("StuName", DataType::CHAR, NORMAL, 10, false));
			fields.push_back(Field("StuAge", DataType::INT, NORMAL));
			fields.push_back(Field("ID", DataType::CHAR, PRIMARY, 10, true, "StuDefalutIndex"));
			table = new Table("TestTable", fields, 24, 0, 0);
			
			char* data = new char[Page::PageSize];
			//sprintf(data, "hedejin%d3130a", 20);
			page = Page(data, table);

			//construct tuples
			vector<shared_ptr<Data>> datas;
			datas.push_back(make_shared<Char>("hedejin"));
			datas.push_back(make_shared<Int>(20));
			datas.push_back(make_shared<Char>("313010067"));
			t0 = Tuple(datas);

			vector<shared_ptr<Data>> datas2;
			datas2.push_back(make_shared<Char>("Justin"));
			datas2.push_back(make_shared<Int>(20));
			datas2.push_back(make_shared<Char>("313010064"));
			t1 = Tuple(datas2);

		}
		~PageTest()
		{
			delete table;
		}

		TEST_METHOD(TupleNumInPage)
		{
			int result = Page::calcTupleNum(table);
			int expect = 163;
			Assert::AreEqual(result, expect);
		}

		TEST_METHOD(HeaderTest)
		{

			Assert::IsFalse(page.isTupleValid(0));
			Assert::IsFalse(page.isTupleValid(10));

			page.SetHeader(0, true);
			page.SetHeader(10, true);

			Assert::IsTrue(page.isTupleValid(0));
			Assert::IsTrue(page.isTupleValid(10));

			page.SetHeader(0, false);
			page.SetHeader(10, false);

			Assert::IsFalse(page.isTupleValid(0));
			Assert::IsFalse(page.isTupleValid(10));

		}

		TEST_METHOD(InsertableTest)
		{
			page.SetHeader(0, true);
			page.SetHeader(1, true);
			page.SetHeader(3, true);

			int ins = page.InsertableIndex();
			Assert::AreEqual(2, ins);
			//Logger::WriteMessage("pass1");
			page.SetHeader(2, true);
			ins = page.InsertableIndex();

			Assert::AreEqual(4, ins);
			//Logger::WriteMessage("pass2");

			for (int i = 0; i < page.getTupleNum(); i++)
			{
				if (!page.isTupleValid(i))
				{
					page.SetHeader(i, true);
				}
			}

			ins = page.InsertableIndex();
			Assert::AreEqual(-1, ins);
			//Logger::WriteMessage("pass3");

		}

		TEST_METHOD(PageWriteReadTest)
		{

			page.WriteTuple(t0, 0);
			page.SetHeader(0, true);
			
			page.WriteTuple(t1, 1);
			page.SetHeader(1, true);
			//Logger::WriteMessage("Finsih writing.");
			const Tuple& tt0 = page.ReadTuple(0);
			const Tuple& tt1 = page.ReadTuple(1);
			//Logger::WriteMessage(tt0.toString().data());
			//Logger::WriteMessage(tt1.toString().data());
			Assert::IsTrue(tt0 == t0);
			Assert::IsTrue(tt1 == t1);
		}
	};

}

