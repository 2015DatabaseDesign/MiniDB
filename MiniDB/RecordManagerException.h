#pragma once
#include <exception>
using namespace std;
class IndexManagerIncompatiblewithRecordManager : public exception {
public:
	const char* what() const throw()
	{
		return "RecordManager can't find tuples according to line numbers given by IndexManager";
	}
};

