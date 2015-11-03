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
