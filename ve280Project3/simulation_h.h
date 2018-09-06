#pragma once
#ifndef SIMULATION_H
#define SIMULATION_H
	
#include<iostream>
#include<fstream>
#include<sstream>
#include<iomanip>
#include<string>
#include<cstdlib>
#include<cassert>

#include"world_type.h"

using namespace std;

/*Check if the arguments are illegal and print the error message.*/
class error_check
{
public:
	int argc;
	stringstream all_arg;
	string arg_str;
	string *argv;
	ifstream spc, wld;

	/*A function to test which kind of error occurs*/
	void error_dicider();

	error_check(int num, string arg): 
		argc(num), arg_str(arg), argv(new string [argc]) {}
	~error_check() { cout << "Dstrct!" << endl; delete[] argv; }
private:

	/*Deal with lack of some neccassary arguments*/
	void arg_num();

	/*Deal with the input round is negative*/
	void neg_rnd();

	/*Deal with the wrong files*/
	void wrg_file(string file);
};


/*Run the simulation and give infomation for runtime error*/
class Sim
{
public:
	ifstream p_sum, wld;
	world_t world;
	string ps_folder;
	int rnd;
	bool is_v = false;

	/*Build the world*/
	void bld_wld();

	/*Add all the species into the list*/
	void add_spc();

	/*Add all the instructions into each species*/
	void add_pro();

	/*Add the infomation about grid into the world*/
	void add_grd();

	/*Add all the creatures into the world*/
	void add_crt();

	/*Simulate the world and print the situation*/
	void tk_act();

	/*Simulate all the creactures in a round*/
	void sim_ttl();

	/*Simulate a single creature's action*/
	void sim_sgl(creature_t &crt);

	/*See whether the front square is outside the grid*/
	bool is_otsd(point_t pt);

	/*Infect the creature on the direction when the creature has ARCH*/
	void ift_line(point_t crt_pt, direction_t dir, const creature_t &crt);

private:
	/*Exit the simulation when error occurs*/
	void end_sim();

	/*Print the current grid if needed*/
	void prt_grd();

	// EFFECTS: Returns a point that results from moving one square
	// in the direction "dir" from the point "pt".
	point_t frt_pnt(point_t pt, direction_t dir);
	
	// EFFECTS: Returns the direction that results from turning
	// left from the given direction "dir".
	direction_t lft_dir(direction_t dir);
	
	// EFFECTS: Returns the direction that results from turning
	// right from the given direction "dir".
	direction_t rgt_dir(direction_t dir);
	
	// EFFECTS: Returns the current instruction of "creature".
	instruction_t gt_pro(const creature_t &crt);
	
	// REQUIRES: location is inside the grid.
	//
	// EFFECTS: Returns a pointer to the creature at "location" in "grid".
	creature_t *get_crt(point_t loc);
};
#endif // !SIMULATION_H