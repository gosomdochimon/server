#include "Accepter.h"

int main()
{

	Accepter acpt;
	
	if (acpt.Initialize()) {

		acpt.func();
	}

	return 8;
}
