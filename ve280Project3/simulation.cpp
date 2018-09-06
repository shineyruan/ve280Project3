#include <string>
#include <iostream>
#include <fstream>
#include <sstream>
#include <cstdlib>
#include "world_type.h"
#include "simulation.h"
using namespace std;

bool isAtBoundary(const point_t pt, const grid_t &grid, direction_t dir) {
	//EFFECTS:	Checks if a point is at the boundary of the grid,
	//			according to the direction of the creature.
	//			Returns true if yes, false if no.
	//REQUIRES:	This function is only used locally.
	//
	//
	if (pt.r <= 0 && dir == NORTH) return true;
	else if (pt.r >= grid.height - 1 && dir == SOUTH) return true;
	else if (pt.c <= 0 && dir == WEST) return true;
	else if (pt.c >= grid.width - 1 && dir == EAST) return true;
	else return false;
}

bool checkdirectionname(const string dir) {
	//EFFECTS:  This function only uses locally. 
	//			Checks if the direction name is valid: true for yes, false for no.
	//REQUIRES: This function is only used locally.
	//
	//
	int flag = 0;
	for (int i = 0; i < DIRECT_SIZE; i++)
		if (dir == directName[i]) {
			flag = 1;
			break;
		}
	if (flag == 0) {
		cout << "Error: Direction " << dir << " is not recognized!" << endl;
		return false;
	}
	else return true;
}

bool checkspeciesname(const string name, const world_t &world) {
	//EFFECTS:  Check if the name belongs to one of the species.
	//			True for yes, false for no.
	//REQUIRES: This function is only used locally.
	//
	//
	int flag = 0;
	for (int i=0;i<world.numSpecies;i++)
		if (name == world.species[i].name) { flag = 1; break; }
	if (flag == 0) {
		cout << "Error: Species " << name << " not found!" << endl;
		return false;
	}
	else return true;
}

bool init_directions(direction_t &direction, string dir) {
	//EFFECTS:	Initializes direction_t and returns true if successfully, 
	//			false if unsuccessfully. 
	//REQUIRES:	Only used locally. 
	//
	//
	int index = -1;
	for (int i = 0; i < DIRECT_SIZE; i++) 
		if (directName[i] == dir) {
			index = i;
			break;
		}

	if (index < 0) return false;
	switch (index)
	{
	case 0:
		direction = EAST;
		break;
	case 1:
		direction = SOUTH;
		break;
	case 2:
		direction = WEST;
		break;
	case 3:
		direction = NORTH;
		break;
	default:
		break;
	}
	return true;
}

bool init_grid(grid_t &grid, const char data[MAXHEIGHT][MAXWIDTH], int rowNum, int colNum) {
	//EFFECTS:  This is a local function. It cannot be used in main().
	//			Returns if a grid is initialized successfully. 
	//
	//
	for (int i = 0; i < rowNum; i++)
		for (int j = 0; j < colNum; j++) {
			int index = -1; 
			for (int k = 0; k < TERRAIN_SIZE; k++) {
				if (terrainShortName[k].c_str()[0] == data[i][j]) {
					index = k;
					break;
				}
			}

			if (index < 0) {
				cout << "Error: Terrain square (" << data[i][j] << " " << i << " " << j << ") is invalid!" << endl;
				return false;
			}
			switch (index)
			{
			case 0:
				grid.terrain[i][j] = PLAIN;
				break;
			case 1:
				grid.terrain[i][j] = LAKE;
				break;
			case 2:
				grid.terrain[i][j] = FOREST;
				break;
			case 3:
				grid.terrain[i][j] = HILL;
				break;
			default:
				break;
			}
		}
	return true;
}

int init_Species(species_t species[], const string species_summary) {
	//EFFECTS:	This is a local function. It cannot be used in main function.
	//			Returns the number of species.
	//			If not initialized properly, returns -1.
	//
	//
	string species_dir;
	ifstream iFile;
	int num = 0;	//This serves as world.numSpecies.
					//
	iFile.open(species_summary);
	if (!iFile) {
		cout << "Error: Cannot open file " << species_summary << "!" << endl;
		iFile.close();
		return -1;
	}
	else {
		getline(iFile, species_dir);
		string speciesFileLine;	//This serves as a buffer for reading species name.
								//
		while (getline(iFile, speciesFileLine)) {
			if (speciesFileLine == "") continue;	//Eliminate blank lines.
			num++;	//MUST first add 1,
					//and judge whether the size exceeds 
					//the maximum value IMMEDIATELY.
					//Consider boundary conditions.
					//
			if (num > MAXSPECIES) {
				cout << "Error: Too many species!" << endl
					<< "Maximal number of species is " << MAXSPECIES << "." << endl;
				iFile.close();
				return -1;
			}
			species[num - 1].name = speciesFileLine;
			//If does not exceed the maximum species value, 
			//begins to read the intructions.
			//
			ifstream iFile_program;
			string temp;
			iFile_program.open(species_dir + "/" + species[num - 1].name);
			if (!iFile_program) {
				cout << "Error: Cannot open file " << species_dir + "/" + species[num - 1].name
					<< "!" << endl;
				iFile_program.close();
				iFile.close();
				return -1;
			}
			else {
				int count = 0;	//Indicates the position of the program (instruction)
								//in the array.
								//Alternative for species[i].programSize.
								//
				species[num - 1].programSize = 0;
				while (getline(iFile_program, temp)) {
					int numPrograms = species[num - 1].programSize;
					numPrograms++;									//MUST first add 1,
																	//and judge whether the size exceeds 
																	//the maximum value IMMEDIATELY.
																	//Consider boundary conditions.
																	//
					if (numPrograms > MAXPROGRAM) {
						cout << "Error: Too many instructions for species " + species[num - 1].name + "!" << endl
							<< "Maximal number of instructions is " << MAXPROGRAM << '.' << endl;
						iFile_program.close();
						iFile.close();
						return -1;
					}

					if (temp.empty()) break;
					else {
						stringstream buffer(temp);
						int ad = 0; string op_code;
						buffer >> op_code >> ad;
						int index = -1;
						//Judges if the instruction is correct.
						//
						for (int p = HOP; p < OP_SIZE; p++)
							if (op_code == opName[p]) {
								index = p;
								break;
							}

						if (index < 0) {
							cout << "Error: Instruction " + op_code + " is not recognized!" << endl;
							iFile_program.close();
							iFile.close();
							return -1;
						}
						//Correct --- begins to add it to the SPECIES class.
						//
						switch (index)
						{
						case HOP:
							species[num - 1].program[count].op = HOP;
							species[num - 1].program[count].address = 0;
							species[num - 1].programSize++;
							count++;
							break;
						case LEFT:
							species[num - 1].program[count].op = LEFT;
							species[num - 1].program[count].address = 0;
							species[num - 1].programSize++;
							count++;
							break;
						case RIGHT:
							species[num - 1].program[count].op = RIGHT;
							species[num - 1].program[count].address = 0;
							species[num - 1].programSize++;
							count++;
							break;
						case INFECT:
							species[num - 1].program[count].op = INFECT;
							species[num - 1].program[count].address = 0;
							species[num - 1].programSize++;
							count++;
							break;
						case IFEMPTY:
							species[num - 1].program[count].op = IFEMPTY;
							species[num - 1].program[count].address = ad;
							species[num - 1].programSize++;
							count++;
							break;
						case IFENEMY:
							species[num - 1].program[count].op = IFENEMY;
							species[num - 1].program[count].address = ad;
							species[num - 1].programSize++;
							count++;
							break;
						case IFSAME:
							species[num - 1].program[count].op = IFSAME;
							species[num - 1].program[count].address = ad;
							species[num - 1].programSize++;
							count++;
							break;
						case IFWALL:
							species[num - 1].program[count].op = IFWALL;
							species[num - 1].program[count].address = ad;
							species[num - 1].programSize++;
							count++;
							break;
						case GO:
							species[num - 1].program[count].op = GO;
							species[num - 1].program[count].address = ad;
							species[num - 1].programSize++;
							count++;
							break;
						default:
							break;
						}
					}

				}
			}

			iFile_program.close();
		}
	}
	iFile.close();
	return num;
}

bool init_World(world_t &world, const string speciesfile, const string worldfile) {
	//Initializes species.
	//
	int species_num = init_Species(world.species, speciesfile);
	if (species_num < 0) return false;
	else {
		world.numSpecies = species_num;
		ifstream iFile;
		iFile.open(worldfile);
		if (!iFile) {
			cout << "Error: Cannot open file " + worldfile + "!" << endl;
			return false;
		}
		else {
			//Initializes grid.
			//
			iFile >> world.grid.height >> world.grid.width;
			if (world.grid.height<1 || world.grid.height>MAXHEIGHT) {
				cout << "Error: The grid height is illegal!" << endl;
				return false;
			}
			if (world.grid.width<1 || world.grid.width>MAXWIDTH) {
				cout << "Error: The grid width is illegal!" << endl;
				return false;
			}

			char terrbuffer[MAXHEIGHT][MAXWIDTH];	//terrbuffer[][] is used to store input values.
													//
			for (int i = 0; i < world.grid.height; i++)
				for (int j = 0; j < world.grid.width; j++)
					iFile >> terrbuffer[i][j];
			if (!init_grid(world.grid, terrbuffer, world.grid.height, world.grid.width))
				return false;
			else {
				for (int i = 0; i < world.grid.height; i++)
					for (int j = 0; j < world.grid.width; j++) 
						world.grid.squares[i][j] = NULL;
				//Initializes creatures.
				//
				world.numCreatures = 0;
				string line; getline(iFile, line);	//This getline() aims to get the '\n' char 
													//at the end of the terrain input.
													//
				while (getline(iFile, line)) {
					if (line == "") continue;
					int total = world.numCreatures;
					total++;						//PATCH: uses "total" to judge if the numCreatures exceeds the maximum.
													//Refer to the method in judging numSpecies.
													//
					if (total > MAXCREATURES) {
						cout << "Error: Too many creatures!" << endl
							<< "Maximal number of creatures is " << MAXCREATURES << "." << endl;
						return false;
					}
					stringstream iString(line);
					string name, direction;
					int row = 0, col = 0;
					iString >> name >> direction >> row >> col;
					if (!checkspeciesname(name, world) || !checkdirectionname(direction))
						return false;
					if (row >= world.grid.height || row < 0 || col >= world.grid.width || col < 0) {
						cout << "Error: Creature (" << name << " " << direction << " " << row << " " << col
							<< ") is out of bound!" << endl
							<< "The grid size is " << world.grid.height << "-by-" << world.grid.width << "." << endl;
						return false;
					}
					//Deals with ability (consider multiple input).
					//
					string ability[100];
					for (int i = 0; i < 100; i++) {
						ability[i] = "";
						iString >> ability[i];
					}
					int flag = 1; int f = 0, a = 0;
					string invalidability = "";
					for (int i = 0; i < 100; i++) {
						if (ability[i] == "f") 
							f = 1;
						else if (ability[i] == "a") 
							a = 1;
						else if (ability[i] != "") {
							flag = 0; 
							invalidability = ability[i]; 
							break;
						}
					}
					if (flag == 0) {
						cout << "Error: Creature (" << name << " " << direction << " " << row << " " << col
							<< ") has an invalid ability " << invalidability << "!" << endl;
						return false;
					}

					//Initializes creature info.
					//
					for (int i = 0; i < world.numSpecies; i++)
						if (name == world.species[i].name) {
							world.creatures[world.numCreatures].species = &world.species[i];
							break;
						}
					init_directions(world.creatures[world.numCreatures].direction, direction);
					world.creatures[world.numCreatures].location.r = row;
					world.creatures[world.numCreatures].location.c = col;
					world.creatures[world.numCreatures].programID = 1;
					world.creatures[world.numCreatures].ability[FLY] = (f == 1) ? true : false;
					world.creatures[world.numCreatures].ability[ARCH] = (a == 1) ? true : false;
					world.creatures[world.numCreatures].hillActive = false;
					//Checks overlapping info and cannotfly-LAKE info.
					//
					if (world.grid.squares[row][col] != NULL) {
						cout << "Error: Creature (" << name << " " << direction << " " << row << " " << col <<
							") overlaps with creature (" << world.grid.squares[row][col]->species->name << " "
							<< directName[world.grid.squares[row][col]->direction] << " "
							<< world.grid.squares[row][col]->location.r << " "
							<< world.grid.squares[row][col]->location.c << ")!" << endl;
						return false;
					}
					else
						world.grid.squares[row][col] = &world.creatures[world.numCreatures];
					if (world.grid.terrain[row][col] == LAKE) 
						if (f != 1) {
							cout << "Error: Creature (" << name << " " << direction << " " << row << " " << col
								<< ") is in a lake square!" << endl
								<< "The creature cannot fly!" << endl;
							return false;
						}
					world.numCreatures++;
				}
			}
		}
		iFile.close();
	}
	return true;
}

void printGrid(const grid_t &grid) {
	for (int i = 0; i < grid.height; i++) {
		for (int j = 0; j < grid.width; j++) {
			if (grid.squares[i][j] == NULL) cout << "____ ";
			else
				cout << grid.squares[i][j]->species->name[0] << grid.squares[i][j]->species->name[1]
				<< "_" + directShortName[grid.squares[i][j]->direction] << " ";
		}
		cout << endl;
	}
}

point_t adjacentPoint(point_t pt, direction_t dir) {
	point_t adjacent;
	switch (dir)
	{
	case EAST:
		adjacent.c = pt.c + 1;
		adjacent.r = pt.r;
		break;
	case SOUTH:
		adjacent.c = pt.c;
		adjacent.r = pt.r + 1;
		break;
	case WEST:
		adjacent.c = pt.c - 1;
		adjacent.r = pt.r;
		break;
	case NORTH:
		adjacent.c = pt.c;
		adjacent.r = pt.r - 1;
		break;
	default:
		break;
	}
	return adjacent;
}

direction_t leftFrom(direction_t dir) {
	switch (dir)
	{
	case EAST:
		return NORTH;
		break;
	case SOUTH:
		return EAST;
		break;
	case WEST:
		return SOUTH;
		break;
	case NORTH:
		return WEST;
		break;
	default:
		return DIRECT_SIZE;
		break;
	}
}

direction_t rightFrom(direction_t dir) {
	switch (dir)
	{
	case EAST:
		return SOUTH;
		break;
	case SOUTH:
		return WEST;
		break;
	case WEST:
		return NORTH;
		break;
	case NORTH:
		return EAST;
		break;
	default:
		return DIRECT_SIZE;
		break;
	}
}

instruction_t getInstruction(const creature_t &creature) {
	return creature.species->program[creature.programID - 1];
}

creature_t *getCreature(const grid_t &grid, point_t location) {
	return grid.squares[location.r][location.c];
}

void simulateCreature(creature_t &creature, grid_t &grid, bool verbose) {
	//Checks if the creature is in a hill square
	//as well as whether the creature can simulate in the hill square.
	//
	if (!creature.ability[FLY])
		if (grid.terrain[creature.location.r][creature.location.c] == HILL)
			if (!creature.hillActive) {
				creature.hillActive = !creature.hillActive;
				return;
			}
			else creature.hillActive = !creature.hillActive;
	//The output part.
	//
	cout << "Creature (" << creature.species->name << " " << directName[creature.direction] << " "
		<< creature.location.r << " " << creature.location.c << ") "
		<< "takes action:";
	if (!verbose) cout << " ";

	//The instruction (program) part.
	//
	while (1) {
		instruction_t instr = getInstruction(creature);
		switch (instr.op)
		{
		case HOP:
			//The output infos in the loop.
			//
			if (verbose)
				cout << endl << "Instruction " << creature.programID << ": " << opName[instr.op];
			//The operation part.
			//
			if (isAtBoundary(creature.location, grid, creature.direction)) {
				creature.programID++;
				if (!verbose) cout << opName[instr.op] << endl;
				else cout << endl;
				if (verbose) printGrid(grid);
				return;
			}
			else {
				point_t ahead = adjacentPoint(creature.location, creature.direction);
				if (grid.terrain[ahead.r][ahead.c] == LAKE && !creature.ability[FLY]) {	
					creature.programID++;
					if (!verbose) cout << opName[instr.op] << endl;
					else cout << endl;
					if (verbose) printGrid(grid);
					return;
				}
				else if (grid.squares[ahead.r][ahead.c] != NULL) {
					creature.programID++;
					if (!verbose) cout << opName[instr.op] << endl;
					else cout << endl;
					if (verbose) printGrid(grid);
					return;
				}
				else {
					grid.squares[creature.location.r][creature.location.c] = NULL;
					grid.squares[ahead.r][ahead.c] = &creature;
					creature.location = ahead;
					creature.programID++;
					if (!verbose) cout << opName[instr.op] << endl;
					else cout << endl;
					if (verbose) printGrid(grid);
					return;
				}
			}
			break;
		case LEFT:
			//The output infos in the loop.
			//
			if (verbose)
				cout << endl << "Instruction " << creature.programID << ": " << opName[instr.op];
			//The operation part.
			//
			creature.direction = leftFrom(creature.direction);
			creature.programID++;
			if (!verbose) cout << opName[instr.op] << endl;
			else cout << endl;
			if (verbose) printGrid(grid);
			return;
			break;
		case RIGHT:
			//The output infos in the loop.
			//
			if (verbose)
				cout << endl << "Instruction " << creature.programID << ": " << opName[instr.op];
			//The operation part.
			//
			creature.direction = rightFrom(creature.direction);
			creature.programID++;
			if (!verbose) cout << opName[instr.op] << endl;
			else cout << endl;
			if (verbose) printGrid(grid);
			return;
			break;
		case INFECT:
			//The output infos in the loop.
			//
			if (verbose)
				cout << endl << "Instruction " << creature.programID << ": " << opName[instr.op];
			//The operation part.
			//
			if (creature.ability[ARCH]) {
				if (isAtBoundary(creature.location, grid, creature.direction)) {
					creature.programID++;
					if (!verbose) cout << opName[instr.op] << endl;
					else cout << endl;
					if (verbose) printGrid(grid);
					return;
				}
				else {
					point_t ahead[MAXHEIGHT];	//This is the same with ahead[MAXWIDTH].
												//
					int length = 0;
					ahead[length++] = adjacentPoint(creature.location, creature.direction);
						//Add the first adjacent point to the array,
						//just for convenience of writing the condition of WHILE block.
						//
					while (!isAtBoundary(adjacentPoint(ahead[length - 1], creature.direction), grid, creature.direction)) {
							ahead[length] = adjacentPoint(ahead[length - 1], creature.direction);
							length++;
					}
					ahead[length] = adjacentPoint(ahead[length - 1], creature.direction);
					length++;
						//Put the boundary adjacent point into the array.
						//
					for (int i = 0; i < length; i++)
						if (grid.squares[ahead[i].r][ahead[i].c] != NULL && grid.squares[ahead[i].r][ahead[i].c]->species != creature.species) {
							grid.squares[ahead[i].r][ahead[i].c]->species = creature.species;
							grid.squares[ahead[i].r][ahead[i].c]->programID = 1;
							break;
						}
					creature.programID++;
					if (!verbose) cout << opName[instr.op] << endl;
					else cout << endl;
					if (verbose) printGrid(grid);
					return;
				}
			}
			else {
				if (isAtBoundary(creature.location, grid, creature.direction)) {
					creature.programID++;
					if (!verbose) cout << opName[instr.op] << endl;
					else cout << endl;
					if (verbose) printGrid(grid);
					return;
				}
				else {
					point_t ahead = adjacentPoint(creature.location, creature.direction);
					if (grid.terrain[ahead.r][ahead.c] == FOREST) {
						creature.programID++;
						if (!verbose) cout << opName[instr.op] << endl;
						else cout << endl;
						if (verbose) printGrid(grid);
						return;
					}
					else if (grid.squares[ahead.r][ahead.c] == NULL) {
						creature.programID++;
						if (!verbose) cout << opName[instr.op] << endl;
						else cout << endl;
						if (verbose) printGrid(grid);
						return;
					}
					else if (grid.squares[ahead.r][ahead.c]->species == creature.species) {
						creature.programID++;
						if (!verbose) cout << opName[instr.op] << endl;
						else cout << endl;
						if (verbose) printGrid(grid);
						return;
					}
					else {
						grid.squares[ahead.r][ahead.c]->species = creature.species;
						grid.squares[ahead.r][ahead.c]->programID = 1;
						creature.programID++;
						if (!verbose) cout << opName[instr.op] << endl;
						else cout << endl;
						if (verbose) printGrid(grid);
						return;
					}
				}
			}
			break;
		case IFEMPTY:
			//The output infos in the loop.
			//
			if (verbose)
				cout << endl << "Instruction " << creature.programID << ": " << opName[instr.op] << " " << instr.address;
			//The operation part.
			//
			if (isAtBoundary(creature.location, grid, creature.direction)) creature.programID++;
			else {
				point_t ahead = adjacentPoint(creature.location, creature.direction);
				if (grid.squares[ahead.r][ahead.c] == NULL) creature.programID = instr.address;
				else creature.programID++;
			}
			break;
		case IFENEMY:
			//The output infos in the loop.
			//
			if (verbose)
				cout << endl << "Instruction " << creature.programID << ": " << opName[instr.op] << " " << instr.address;
			//The operation part.
			//
			if (isAtBoundary(creature.location, grid, creature.direction)) creature.programID++;
			else {
				point_t ahead = adjacentPoint(creature.location, creature.direction);
				if (grid.squares[ahead.r][ahead.c] == NULL) creature.programID++;
				else if (grid.terrain[ahead.r][ahead.c] != FOREST && grid.squares[ahead.r][ahead.c]->species != creature.species)
					creature.programID = instr.address;
				else creature.programID++;
			}
			break;
		case IFSAME:
			//The output infos in the loop.
			//
			if (verbose)
				cout << endl << "Instruction " << creature.programID << ": " << opName[instr.op] << " " << instr.address;
			//The operation part.
			//
			if (isAtBoundary(creature.location, grid, creature.direction)) creature.programID++;
			else {
				point_t ahead = adjacentPoint(creature.location, creature.direction);
				if (grid.squares[ahead.r][ahead.c] == NULL) creature.programID++;
				else if (grid.terrain[ahead.r][ahead.c] != FOREST && grid.squares[ahead.r][ahead.c]->species == creature.species)
					creature.programID = instr.address;
				else creature.programID++;
			}
			break;
		case IFWALL:
			//The output infos in the loop.
			//
			if (verbose)
				cout << endl << "Instruction " << creature.programID << ": " << opName[instr.op] << " " << instr.address;
			//The operation part.
			//
			if (isAtBoundary(creature.location, grid, creature.direction)) creature.programID = instr.address;
			else {
				point_t ahead = adjacentPoint(creature.location, creature.direction);
				if (grid.terrain[ahead.r][ahead.c] == LAKE && !creature.ability[FLY])
					creature.programID = instr.address;
				else creature.programID++;
			}
			break;
		case GO:
			//The output infos in the loop.
			//
			if (verbose)
				cout << endl << "Instruction " << creature.programID << ": " << opName[instr.op] << " " << instr.address;
			//The operation part.
			//
			creature.programID = instr.address;
			break;
		default:
			break;
		}
	}
}