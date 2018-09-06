#define _CRTDBG_MAP_ALLOC
#include<iostream>
#include<fstream>
#include<sstream>
#include<iomanip>
#include<string>
#include<cstdlib>
#include<cassert>
#include <crtdbg.h>

#include"simulation_h.h"
#include"world_type.h"

using namespace std;

int main(int argc, char *argv[])
{
	ostringstream all_arg;
	for (int i = 0; i < argc; i++) all_arg << argv[i] << ' ';
	string arg = all_arg.str();

	{error_check error(argc, arg);
	error.error_dicider();
	}
	int round = atoi(argv[3]);

	Sim sim;
	sim.rnd = round;
	if (argc >= 5 && 
		(string(argv[4]) == "v" || string(argv[4]) == "verbose"))
		sim.is_v = true;
	sim.p_sum.open(argv[1]);
	sim.wld.open(argv[2]);
	sim.bld_wld();
	sim.p_sum.close();
	sim.wld.close();

	sim.tk_act();
	_CrtDumpMemoryLeaks();
	system("PAUSE");
	return 0;
}