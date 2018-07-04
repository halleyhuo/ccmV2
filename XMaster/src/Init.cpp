
#include <unistd.h>

#include "Types.h"
#include "Init.h"
#include "XGenerator.h"
#include "XMaster.h"
#include "XTui.h"

int32 main(int32 argc, char* argv[])
{


	CXGenerator		*xGenerator;
	CXMaster		*xMaster;

	xMaster = new CXMaster();
	xMaster->Init();

	xGenerator = new CXGenerator();
	xGenerator->Init();

	while(true)
	{
		sleep(5);
	}

	return 0;
}
