#include <iostream>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <string>
#include <cstdlib>
#include "world_type.h"
#include "simulation.h"
using namespace std;

int main(int argc, char*argv[]) {
	//Deal with program arguments.
	//
	if (argc < 4) {
		cout << "Error: Missing arguments!" << endl
			<< "Usage: ./p3 <species-summary> <world-file> <rounds> [v|verbose]" << endl;
	}
	else {
		string species_summary = argv[1], world_file = argv[2];
		int rounds = atoi(argv[3]);
		//Judge if verbose.
		//
		bool verbose = true;
		if (argc == 4) verbose = false;
		else {
			string isverbose = argv[4];
			if (isverbose == "v" || isverbose == "verbose") verbose = true;
			else verbose = false;
		}
		//Judge if rounds<0.
		//
		if (rounds < 0) 
			cout << "Error: Number of simulation rounds is negative!" << endl;
		else {
			//Initialize world info.
			//
			world_t world;
			if (init_World(world, species_summary, world_file)) {
				cout << "Initial state" << endl;
				printGrid(world.grid);
				//Simulation begins.
				//
				int count = 1;
				while (count <= rounds) {
					cout << "Round " << count << endl;
					for (int i = 0; i < world.numCreatures; i++)
						simulateCreature(world.creatures[i], world.grid, verbose);
					if (!verbose) printGrid(world.grid);
					count++;
				}
			}
		}	
	}
	
//	system("PAUSE");
	return 0;
}