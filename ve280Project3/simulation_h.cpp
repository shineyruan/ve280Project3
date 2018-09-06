#include<iostream>
#include<fstream>
#include<sstream>
#include<iomanip>
#include<string>
#include<cstdlib>
#include<cassert>

#include"simulation_h.h"
#include"world_type.h"

using namespace std;

/*******************************
********Error Check Part********
*******************************/

void error_check::arg_num()
{
	cout << "Error: Missing arguments!" << endl;
	cout << "Usage: ./p3 <species-summary> <world-file> <rounds> [v|verbose]" << endl;
	system("PAUSE");
	exit(0);
}

void error_check::neg_rnd()
{
	cout << "Error: Number of simulation rounds is negative!" << endl;
	system("PAUSE");
	exit(0);
}

void error_check::wrg_file(string file)
{
	cout << "Error: Cannot open file " << file << '!' << endl;
	spc.close();
	wld.close();
	system("PAUSE");
	exit(0);
}

void error_check::error_dicider()
{
	all_arg << arg_str;
	all_arg >> argv[0];
	if (argc < 4) arg_num();
	for (int i = 1; i < 4; i++) all_arg >> argv[i];
	if(stoi(argv[3])<0) neg_rnd();
	spc.open(argv[1]);
	wld.open(argv[2]);
	if (!spc) wrg_file(argv[1]);
	if (!wld) wrg_file(argv[2]);
	spc.close();
	wld.close();
}

/*******************************
********Simulation Part*********
*******************************/

/*Initial Part*/

void Sim::end_sim()
{
	p_sum.close();
	wld.close();
	system("PAUSE");
	exit(0);
}

void Sim::add_spc()
{
	string spc;
	while (getline(p_sum, spc))
	{
		world.numSpecies += 1;
		if (world.numSpecies > MAXSPECIES)
		{
			cout << "Error: Too many species!" << endl;
			cout << "Maximal number of species is " 
				<< MAXSPECIES << '.' << endl;
			end_sim();
		}
		world.species[world.numSpecies - 1].name = spc;
	}
}

void Sim::add_pro()
{
	for (unsigned int i = 0; i < world.numSpecies; i++)
	{
		ifstream a_spc;
		string loc_file;
		string pro_info;
		loc_file = ps_folder + '/' + world.species[i].name;
		a_spc.open(loc_file);
		if (!a_spc)
		{
			cout << "Error: Cannot open file " << loc_file << '!' << endl;
			a_spc.close();
			end_sim();
		}
		world.species[i].programSize = 0;
		while (getline(a_spc, pro_info))
		{
			if (pro_info == "") break;
			world.species[i].programSize += 1;
			if (world.species[i].programSize > MAXPROGRAM)
			{
				cout << "Error: Too many instructions for species " 
					<< world.species[i].name << '!' << endl;
				cout << "Maximal number of instructions is " 
					<< MAXPROGRAM << '.' << endl;
				end_sim();
			}
			stringstream pro_str;
			pro_str << pro_info;
			string pro;
			int go_to = 0;
			pro_str >> pro >> go_to;
			int j;
			for (j = 0; j < OP_SIZE; j++)
				if (pro == opName[j]) break;
			if (j == OP_SIZE)
			{
				cout << "Error: Instruction " << pro 
					<< " is not recognized!" << endl;
				a_spc.close();
				end_sim();
			}
			world.species[i].program[world.species[i].programSize - 1]
				.op = (opcode_t)j;
			world.species[i].program[world.species[i].programSize - 1]
				.address = go_to;
		}
		a_spc.close();
	}
}

void Sim::add_grd()
{
	//initial height, width
	wld >> world.grid.height >> world.grid.width;
	if (world.grid.height > MAXHEIGHT|| world.grid.height < 1)
	{
		cout << "Error: The grid height is illegal!" << endl;
		end_sim();
	}
	if (world.grid.width > MAXWIDTH|| world.grid.width < 1)
	{
		cout << "Error: The grid width is illegal!" << endl;
		end_sim();
	}
	//initial terrain
	//string *grd_row = new string[world.grid.height];
	char grd_c_r[MAXHEIGHT][MAXWIDTH];
	//for (unsigned int i = 0; i < world.grid.height; i++) wld >> grd_row[i];
	for (unsigned int j = 0; j < world.grid.height; j++)
		for (unsigned int k = 0; k < world.grid.width; k++)
		{
			string crt_trn = "";
			//crt_trn += grd_row[j][k];
			wld >> grd_c_r[j][k];
			crt_trn += grd_c_r[j][k];
			int m;
			for (m = 0; m < TERRAIN_SIZE; m++)
				if (crt_trn == terrainShortName[m]) break;
			if (m == TERRAIN_SIZE)
			{
				cout << "Error: Terrain square (" << crt_trn 
					<< ' ' << j << ' ' << k << ") is invalid!" << endl;
				end_sim();
			}
			world.grid.terrain[j][k] = (terrain_t)m;
		}
	//char a;
	//a=getchar(wld);
	//delete[] grd_row;
}

void Sim::add_crt()
{
	string crt_info;
	while (getline(wld, crt_info))
	{
		//if (crt_info == "") break;
		//initial size and ID
		world.numCreatures += 1;
		if (world.numCreatures > MAXCREATURES)
		{
			cout << "Error: Too many creatures!" << endl;
			cout << "Maximal number of creatures is " << MAXCREATURES 
				<< "." << endl;
			end_sim();
		}
		world.creatures[world.numCreatures - 1].programID = 0;
		//initial species pointer
		stringstream all_info(crt_info);
		string crt, dir, s_ablt;
		int rd_r, rd_c;
		all_info >> crt >> dir >> rd_r >> rd_c;
		getline(all_info, s_ablt);
		unsigned int i;
		for (i = 0; i < world.numSpecies; i++)
			if (crt == world.species[i].name) break;
		if (i == world.numSpecies)
		{
			cout << "Error: Species "<< crt <<" not found!" << endl;
			end_sim();
		}
		world.creatures[world.numCreatures - 1].species 
			= &world.species[i];
		//initial direction
		int j;
		for (j = 0; j < DIRECT_SIZE; j++)
			if (dir == directName[j]) break;
		if (j == DIRECT_SIZE)
		{
			cout << "Error: Direction "<< dir 
				<<" is not recognized!" << endl;
			end_sim();
		}
		world.creatures[world.numCreatures - 1].direction 
			= (direction_t)j;
		//initial location
		world.creatures[world.numCreatures - 1].hillActive = true;
		if (rd_r >= world.grid.height || rd_c >= world.grid.width)
		{
			cout << "Error: Creature (" << crt << " " << dir 
				<< " " << rd_r << " " << rd_c << ") is out of bound!" << endl
			 << "The grid size is " << world.grid.height 
				<< "-by-" << world.grid.width << "." << endl;
			end_sim();
		}
		world.creatures[world.numCreatures - 1].location.r = rd_r;
		world.creatures[world.numCreatures - 1].location.c = rd_c;
		//initial abilities
		world.creatures[world.numCreatures - 1].ability[FLY] = false;
		world.creatures[world.numCreatures - 1].ability[ARCH] = false;
		string crt_ablt;
		stringstream nw_ablt;
		nw_ablt << s_ablt;
		while (nw_ablt>>crt_ablt)
		{
			int k;
			for (k = 0; k < ABILITY_SIZE; k++)
				if (crt_ablt == abilityShortName[k]) break;
			if (k == ABILITY_SIZE)
			{
				cout << "Error: Creature (" << crt << ' ' << dir << ' ' 
					<< rd_r << ' ' << rd_c << ") has an invalid ability " 
					<< crt_ablt << "!" << endl;
				end_sim();
			}
			world.creatures[world.numCreatures - 1].ability[k] = true;
		}
		if (world.grid.terrain[rd_r][rd_c] == HILL &&
			world.creatures[world.numCreatures - 1].ability[FLY] == false)
			world.creatures[world.numCreatures - 1].hillActive = false;
		//put in grid and check
		if (world.grid.squares[rd_r][rd_c] != NULL)
		{
			cout << "Error: Creature (" << crt << ' ' << dir << ' ' 
				<< rd_r << ' ' << rd_c << ") overlaps with creature (" 
				<< world.grid.squares[rd_r][rd_c]->species->name << ' ' 
				<< directName[world.grid.squares[rd_r][rd_c]->direction]
				<< ' ' << rd_r << ' ' << rd_c << ")!" << endl;
			end_sim();
		}
		world.grid.squares[rd_r][rd_c] 
			= &world.creatures[world.numCreatures - 1];
		if (world.grid.terrain[rd_r][rd_c] == LAKE &&
			world.creatures[world.numCreatures - 1].ability[FLY] == false)
		{
			cout << "Error: Creature (" << crt << ' ' << dir << ' '
				<< rd_r << ' ' << rd_c << ") is in a lake square!" << endl;
			cout << "The creature cannot fly!" << endl;
			end_sim();
		}
	}
}

void Sim::bld_wld()
{
	world.numSpecies = 0;
	world.numCreatures = 0;
	getline(p_sum, ps_folder);
	add_spc();
	add_pro();
	add_grd();
	/*char trw;
	wld.get(trw);*/
	string trw;
	getline(wld, trw);
	add_crt();
}

/*Going on part*/

point_t Sim::frt_pnt(point_t pt, direction_t dir)
{
	switch (dir)
	{
	case EAST: pt.c += 1;
		break;
	case SOUTH: pt.r += 1;
		break;
	case WEST: pt.c -= 1;
		break;
	case NORTH: pt.r -= 1;
		break;
	default: break;
	}
	return pt;
}

direction_t Sim::lft_dir(direction_t dir)
{
	switch (dir)
	{
	case EAST: dir = NORTH;
		break;
	case SOUTH: dir = EAST;
		break;
	case WEST: dir = SOUTH;
		break;
	case NORTH: dir = WEST;
		break;
	default: break;
	}
	return dir;
}

direction_t Sim::rgt_dir(direction_t dir)
{
	switch (dir)
	{
	case EAST: dir = SOUTH;
		break;
	case SOUTH: dir = WEST;
		break;
	case WEST: dir = NORTH;
		break;
	case NORTH: dir = EAST;
		break;
	default: break;
	}
	return dir;
}

instruction_t Sim::gt_pro(const creature_t &crt)
{
	return crt.species->program[crt.programID];
}

creature_t* Sim::get_crt(point_t loc)
{
	return world.grid.squares[loc.r][loc.c];
}

void Sim::ift_line(point_t crt_pt, direction_t dir, const creature_t &crt)
{
	point_t frt_sqr = frt_pnt(crt_pt, dir);
	if (is_otsd(frt_sqr)) return;
	else if (get_crt(frt_sqr) != NULL)
	{
		if (get_crt(frt_sqr)->species->name != crt.species->name)
		{
			world.grid.squares[frt_sqr.r][frt_sqr.c]->programID = 0;
			get_crt(frt_sqr)->species = crt.species;
			return;
		}
		else ift_line(frt_sqr, dir, crt);
	}
	else ift_line(frt_sqr, dir, crt);
}

bool Sim::is_otsd(point_t pt)
{
	if (pt.r < 0 || pt.r >= world.grid.height 
		|| pt.c < 0 || pt.c >= world.grid.width) 
		return true;
	return false;
}

void Sim::prt_grd()
{
	for (unsigned int i = 0; i < world.grid.height; i++)
	{
		for (unsigned int j = 0; j < world.grid.width; j++)
		{
			if (world.grid.squares[i][j] == NULL) cout << setfill('_') 
				<< setw(5) << ' ';
			else cout << world.grid.squares[i][j]->species->name[0]
				<< world.grid.squares[i][j]->species->name[1] << '_'
				<< directShortName[world.grid.squares[i][j]->direction] 
				<< ' ';
		}
		cout << endl;
	}
}

void Sim::sim_ttl()
{
	for (unsigned int i = 0; i < world.numCreatures; i++)
	{
		if (world.creatures[i].hillActive == false)
		{
			world.creatures[i].hillActive = true;
			continue;
		}
		cout << "Creature (" << world.creatures[i].species->name 
			<< ' ' << directName[world.creatures[i].direction] << ' ' 
			<< world.creatures[i].location.r << ' ' 
			<< world.creatures[i].location.c << ") takes action:";
		if (is_v == true) cout << endl;
		sim_sgl(world.creatures[i]);
		if (world.grid.terrain[world.creatures[i].location.r]
			[world.creatures[i].location.c] == HILL
			&& world.creatures[i].ability[FLY] == false) 
			world.creatures[i].hillActive = false;
		if (is_v == true) prt_grd();
	}
}

void Sim::sim_sgl(creature_t &crt)
{
	if (is_v == true)
	{
		cout << "Instruction " << crt.programID + 1 << ": " 
			<< opName[crt.species->program[crt.programID].op];
		if (crt.species->program[crt.programID].address != 0)
			cout << ' ' << crt.species->program[crt.programID].address;
		cout << endl;
	}
	point_t frt_sqr = frt_pnt(crt.location, crt.direction);
	switch (crt.species->program[crt.programID].op)
	{
	case HOP:
		if (is_v == false) cout << ' '
			<< opName[crt.species -> program[crt.programID].op] << endl;
		if (is_otsd(frt_sqr) == true)
		{
			crt.programID += 1;
			break;
		}
		if (get_crt(frt_sqr) != NULL 
			|| (world.grid.terrain[frt_sqr.r][frt_sqr.c] == LAKE 
				&& crt.ability[FLY] == false))
			crt.programID += 1;
		else
		{
			world.grid.squares[frt_sqr.r][frt_sqr.c] = get_crt(crt.location);
			world.grid.squares[crt.location.r][crt.location.c] = NULL;
			crt.location = frt_sqr;
			crt.programID += 1;
		}
		break;
	case LEFT:
		if (is_v == false) cout << ' '
			<< opName[crt.species->program[crt.programID].op] << endl;
		crt.direction = lft_dir(crt.direction);
		crt.programID += 1;
		break;
	case RIGHT:
		if (is_v == false) cout << ' '
			<< opName[crt.species->program[crt.programID].op] << endl;
		crt.direction = rgt_dir(crt.direction);
		crt.programID += 1;
		break;
	case INFECT:
		if (is_v == false) cout << ' '
			<< opName[crt.species->program[crt.programID].op] << endl;
		if (is_otsd(frt_sqr) == true)
		{
			crt.programID += 1;
			break;
		}
		if (crt.ability[ARCH] == false)
		{
			if (get_crt(frt_sqr) != NULL)
			{
				if (get_crt(frt_sqr)->species->name != crt.species->name)
				{
					if (world.grid.terrain[frt_sqr.r][frt_sqr.c] == FOREST)
					{
						crt.programID += 1;
						break;
					}
					world.grid.squares[frt_sqr.r][frt_sqr.c]->programID = 0;
					get_crt(frt_sqr)->species = crt.species;
				}
			}
		}
		else ift_line(crt.location, crt.direction, crt);
		crt.programID += 1;
		break;
	case IFEMPTY:
		if (is_otsd(frt_sqr) == true)
		{
			crt.programID += 1;
			sim_sgl(crt);
			break;
		}
		if (get_crt(frt_sqr) == NULL)
			crt.programID = crt.species->program[crt.programID].address - 1;
		else crt.programID += 1;
		sim_sgl(crt);
		break;
	case IFENEMY:
		if (is_otsd(frt_sqr) == true)
		{
			crt.programID += 1;
			sim_sgl(crt);
			break;
		}
		if(get_crt(frt_sqr)==NULL) crt.programID += 1;
		else if ((get_crt(frt_sqr)->species->name != crt.species->name)
			&& world.grid.terrain[frt_sqr.r][frt_sqr.c] != FOREST)
			crt.programID = crt.species->program[crt.programID].address - 1;
		else crt.programID += 1;
		sim_sgl(crt);
		break;
	case IFSAME:
		if (is_otsd(frt_sqr) == true)
		{
			crt.programID += 1;
			sim_sgl(crt);
			break;
		}
		if (get_crt(frt_sqr) == NULL) crt.programID += 1;
		else if (get_crt(frt_sqr)->species->name == crt.species->name
			&& world.grid.terrain[frt_sqr.r][frt_sqr.c] != FOREST)
			crt.programID = crt.species->program[crt.programID].address - 1;
		else crt.programID += 1;
		sim_sgl(crt);
		break;
	case IFWALL:
		if (is_otsd(frt_sqr) == true)
		{
			crt.programID = crt.species->program[crt.programID].address - 1;
			sim_sgl(crt);
			break;
		}
		if (world.grid.terrain[frt_sqr.r][frt_sqr.c]
			== LAKE && crt.ability[FLY] == false)
			crt.programID = crt.species->program[crt.programID].address - 1;
		else crt.programID += 1;
		sim_sgl(crt);
		break;
	case GO: 
		crt.programID = crt.species->program[crt.programID].address - 1;
		sim_sgl(crt);
		break;
	default:
		break;
	}
}

void Sim::tk_act()
{
	cout << "Initial state" << endl;
	prt_grd();
	for (int i = 0; i < rnd; i++)
	{
		cout << "Round " << i + 1 << endl;
		sim_ttl();
		if (is_v == false) prt_grd();
	}
}
