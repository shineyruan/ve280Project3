#pragma once
#ifndef SIMULATION_H
#define SIMULATION_H

bool init_World(world_t &world, const string speciesfile, const string worldfile);
void printGrid(const grid_t &grid);
point_t adjacentPoint(point_t pt, direction_t dir);
direction_t leftFrom(direction_t dir);
direction_t rightFrom(direction_t dir);
instruction_t getInstruction(const creature_t &creature);
creature_t *getCreature(const grid_t &grid, point_t location);
void simulateCreature(creature_t &creature, grid_t &grid, bool verbose);
#endif
