#include "MiniDB.h"

int main(int argc, char *argv[])
{
	MiniDB db;
	while (true) {
		try {
			db.readinput();
		}
		catch (IllegalCommand e) {
			e.DBerror();
		}
		catch (const exception& e)
		{
			cout << e.what() << endl;
		}
	}
}
	//s
	//char* data = new char[Page::PageSize];
	////sprintf(data, "hedejin%d3130a", 20);
	//Page page(data, table);

	////construct tuples
	//vector<shared_ptr<Data>> datas;
	//datas.push_back(make_shared<Char>("hedejin"));
	//datas.push_back(make_shared<Int>(20));
	//datas.push_back(make_shared<Char>("313010067"));
	//Tuple t0(datas);

	//vector<shared_ptr<Data>> datas2;
	//datas2.push_back(make_shared<Char>("Justin"));
	//datas2.push_back(make_shared<Int>(20));
	//datas2.push_back(make_shared<Char>("313010064"));
	//Tuple t1(datas2);

	//if (db.checkTupleFormat(table, t0) && db.checkTupleFormat(table, t1))
	//{
	//	page.WriteTuple(t0, 0);
	//	page.SetHeader(0, true);

	//	page.WriteTuple(t1, 1);
	//	page.SetHeader(1, true);


	//	const Tuple &tt0 = page.ReadTuple(0);
	//	const Tuple &tt1 = page.ReadTuple(1);
	//	cout << tt0 << endl;
	//	cout << tt1 << endl;
	//	cout << t0 << endl;
	//	cout << t1 << endl;
	//	if(tt0 == t0)
	//		cout << "0true" << endl;
	//	if(tt1 == t1)
	//		cout << "1true" << endl;

	//}
	//else
	//{
	//	cout << "Fail" << endl;
	//}


	//char *dataToWrite = new char[100];
	////char name[] ="hedejin  ";
	//shared_ptr<Data> d = make_shared<Char>("hedejin");
	//string str = (char *)d->getValue();
	//while (str.size() != 10-1)
	//{
	//	str += " ";
	//}
	//memcpy(dataToWrite, str.data(), 10);

	//int intv = 20;
	//memcpy(dataToWrite + 10, &intv, 4);

	//int read;
	//memcpy(&read, dataToWrite + 10, 4);

	//cout << read << endl;

	//char* readname = new char[10];
	//memcpy(readname, dataToWrite, 10);
	//cout << strlen(readname) << endl;
	//puts(readname);
