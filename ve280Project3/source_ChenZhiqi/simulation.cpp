#include "simulation.h"

// check validity of command line input
void checkArguments(int argc, char *argv[]) {
  // check number of input commands
  if (argc<4) {
    cout << "Error: Missing arguments!" << endl;
    cout << "Usage: ./p3 <species-summary> <world-file> <rounds> [v|verbose]" << endl;
    exit(0);
  }
  // check round number
  if (atoi(argv[3])<0) {
    cout << "Error: Number of simulation rounds is negative!" << endl;
    exit(0);
  }
}

// intialize world and check error
bool initWorld(world_t &world, const string &speciesFile, const string &worldFile) {
  // input speciesFile
  ifstream iSpecy;
  iSpecy.open(speciesFile);
  if (!iSpecy) {
    cout << "Error: Cannot open file " << speciesFile << '!' << endl;
    iSpecy.close();
    exit(0);
  }
  // get filePath
  string specyName="";
  getline(iSpecy, specyName);
  string filePath="";
  filePath=specyName+"/";
  // create species
  int specyNum=0;
  while (getline(iSpecy, specyName)) {
    if (specyName=="") break;
    if (specyNum>=MAXSPECIES) {
      cout << "Error: Too many species!" << endl;
      cout << "Maximal number of species is " << MAXSPECIES << '.' << endl;
      iSpecy.close();
      exit(0);
    }
    world.species[specyNum].name=specyName;
    specyNum++;
  }
  world.numSpecies=specyNum;
  iSpecy.close();
  // input creatures
  for (int i=0;i<world.numSpecies;i++) {
    iSpecy.open(filePath+world.species[i].name);
    if (!iSpecy) {
      cout << "Error: Cannot open file " << filePath << world.species[i].name << '!' << endl;
      iSpecy.close();
      exit(0);
    }
    string instruction="";
    // create creatures
    int programNum=0;
    while (getline(iSpecy, instruction)) {
      if (instruction=="") break;
      if (programNum>=MAXPROGRAM) {
        cout << "Error: Too many instructions for species " << world.species[i].name << '!' << endl;
        cout << "Maximal number of instructions is " << MAXPROGRAM << '.' << endl;
        iSpecy.close();
        exit(0);
      }
      // input instructions
      istringstream insStream;
      insStream.str(instruction);
      string insName="";
      insStream >> insName;
      if (insName=="hop") world.species[i].program[programNum].op=HOP;
      else if (insName=="left") world.species[i].program[programNum].op=LEFT;
      else if (insName=="right") world.species[i].program[programNum].op=RIGHT;
      else if (insName=="infect") world.species[i].program[programNum].op=INFECT;
      else if (insName=="ifempty") world.species[i].program[programNum].op=IFEMPTY;
      else if (insName=="ifenemy") world.species[i].program[programNum].op=IFENEMY;
      else if (insName=="ifsame") world.species[i].program[programNum].op=IFSAME;
      else if (insName=="ifwall") world.species[i].program[programNum].op=IFWALL;
      else if (insName=="go") world.species[i].program[programNum].op=GO;
      else {
        cout << "Error: Instruction " << insName << " is not recognized!" << endl;
        iSpecy.close();
        exit(0);
      }
      if (world.species[i].program[programNum].op>3) insStream >> world.species[i].program[programNum].address;
      programNum++;
    }
    world.species[i].programSize=programNum;
    iSpecy.close();
  }
  // input world
  ifstream iWorld;
  iWorld.open(worldFile);
  if (!iWorld) {
    cout << "Error: Cannot open file " << worldFile << '!' << endl;
    iWorld.close();
    exit(0);
  }
  // get woorld size
  iWorld >> world.grid.height >> world.grid.width;
  if (world.grid.height<1 || world.grid.height>MAXHEIGHT) {
    cout << "Error: The grid height is illegal!" << endl;
    iWorld.close();
    exit(0);
  }
  if (world.grid.width<1 || world.grid.width>MAXWIDTH) {
    cout << "Error: The grid width is illegal!" << endl;
    iWorld.close();
    exit(0);
  }
  // get terrain type
  string terrainType="";
  getline(iWorld, terrainType);
  for (int i=0;i<world.grid.height;i++) {
    getline(iWorld, terrainType);
    for (int j=0;j<world.grid.width;j++) {
      // initial creature link
      world.grid.squares[i][j]=NULL;
      if (terrainType[j]=='P') world.grid.terrain[i][j]=PLAIN;
      else if (terrainType[j]=='L') world.grid.terrain[i][j]=LAKE;
      else if (terrainType[j]=='F') world.grid.terrain[i][j]=FOREST;
      else if (terrainType[j]=='H') world.grid.terrain[i][j]=HILL;
      else {
        cout << "Error: Terrain square (" << terrainType[j] << ' ' << i << ' ' << j << ") is invalid!" << endl;
        iWorld.close();
        exit(0);
      }
    }
  }
  // get creature infomation
  int creatureNum=0;
  string creatureInfo="";
  while (getline(iWorld, creatureInfo)) {
    if (creatureInfo=="") break;
    if (creatureNum>=MAXCREATURES) {
      cout << "Error: Too many creatures!" << endl;
      cout << "Maximal number of creatures is " << MAXCREATURES << '.' << endl;
      iWorld.close();
      exit(0);
    }
    istringstream creInf;
    creInf.str(creatureInfo);
    string creatureDetail="";
    // set programID
    world.creatures[creatureNum].programID=0;
    // get specy
    creInf >> creatureDetail;
    int findCategory=0;
    for (int i=0;i<world.numSpecies;i++) {
      if (creatureDetail==world.species[i].name) {
        world.creatures[creatureNum].species=&world.species[i];
        findCategory=1;
      }
    }
    if (findCategory==0) {
      cout << "Error: Species " << creatureDetail << " not found!" << endl;
      iWorld.close();
      exit(0);
    }
    // get direction
    creInf >> creatureDetail;
    if (creatureDetail=="east") world.creatures[creatureNum].direction=EAST;
    else if (creatureDetail=="west") world.creatures[creatureNum].direction=WEST;
    else if (creatureDetail=="north") world.creatures[creatureNum].direction=NORTH;
    else if (creatureDetail=="south") world.creatures[creatureNum].direction=SOUTH;
    else {
      cout << "Error: Direction " << creatureDetail << " is not recognized!" << endl;
      iWorld.close();
      exit(0);
    }
    // get location
    creInf >> world.creatures[creatureNum].location.r >> world.creatures[creatureNum].location.c;
    if (world.creatures[creatureNum].location.r>=world.grid.height || world.creatures[creatureNum].location.c>=world.grid.width) {
      cout << "Error: Creature (" << world.creatures[creatureNum].species->name << ' ' << directName[world.creatures[creatureNum].direction] << ' ' << world.creatures[creatureNum].location.r << ' ' << world.creatures[creatureNum].location.c << ") is out of bound!" << endl;
      cout << "The grid size is " << world.grid.height << "-by-" << world.grid.width << '.' << endl;
      iWorld.close();
      exit(0);
    }
    // link creature to grid
    world.grid.squares[world.creatures[creatureNum].location.r][world.creatures[creatureNum].location.c]=&world.creatures[creatureNum];
    // set hillActive
    if (world.grid.terrain[world.creatures[creatureNum].location.r][world.creatures[creatureNum].location.c]==HILL) world.creatures[creatureNum].hillActive=1;
    else world.creatures[creatureNum].hillActive=0;
    // get ability
    world.creatures[creatureNum].ability[FLY]=0;
    world.creatures[creatureNum].ability[ARCH]=0;
    while (!creInf.eof()) {
      string ability;
      creInf >> ability;
      if (ability=="") continue;
      if (ability=="f") world.creatures[creatureNum].ability[FLY]=1;
      else if (ability=="a") world.creatures[creatureNum].ability[ARCH]=1;
      else {
        cout << "Error: Creature (" << world.creatures[creatureNum].species->name << ' ' << directName[world.creatures[creatureNum].direction] << ' ' << world.creatures[creatureNum].location.r << ' ' << world.creatures[creatureNum].location.c << ") has an invalid ability " << ability << '!' << endl;
        iWorld.close();
        exit(0);
      }
    }
    creatureNum++;
  }
  world.numCreatures=creatureNum;
  iWorld.close();
  for (int i=0;i<world.numCreatures-1;i++) {
    for (int j=i+1;j<world.numCreatures;j++) {
      if (world.creatures[i].location.r==world.creatures[j].location.r && world.creatures[i].location.c==world.creatures[j].location.c) {
        cout << "Error: Creature (" << world.creatures[j].species->name << ' ' << directName[world.creatures[j].direction] << ' ' << world.creatures[j].location.r << ' ' << world.creatures[j].location.c << ") overlaps with creature (";
        cout << world.creatures[i].species->name << ' ' << directName[world.creatures[i].direction] << ' ' << world.creatures[i].location.r << ' ' << world.creatures[i].location.c << ")!" << endl;
        exit(0);
      }
    }
  }
  for (int i=0;i<world.numCreatures;i++) {
    if (world.creatures[i].ability[FLY]==0 && world.grid.terrain[world.creatures[i].location.r][world.creatures[i].location.c]==LAKE) {
      cout << "Error: Creature (" << world.creatures[i].species->name << ' ' << directName[world.creatures[i].direction] << ' ' << world.creatures[i].location.r << ' ' << world.creatures[i].location.c << ") is in a lake square!" << endl;
      cout << "The creature cannot fly!" << endl;
      exit(0);
    }
  }
  return true;
}

// print world grid
void printGrid(const grid_t &grid) {
  for (int i=0;i<grid.height;i++) {
    for (int j=0;j<grid.width;j++) {
      if (grid.squares[i][j]==NULL) cout << "____ ";
      else cout << grid.squares[i][j]->species->name[0] << grid.squares[i][j]->species->name[1] << '_' << directShortName[grid.squares[i][j]->direction] << ' ';
    }
    cout << endl;
  }
}

// print world grid
point_t adjacentPoint(point_t pt, direction_t dir) {
  point_t newPt=pt;
  if (dir==NORTH) newPt.r-=1;
  else if (dir==WEST) newPt.c-=1;
  else if (dir==SOUTH) newPt.r+=1;
  else newPt.c+=1;
  return newPt;
}

// command hop
void hopFrom(grid_t &grid, point_t adjacentPoint, creature_t &creature) {
  if (!ifwallFrom(grid, adjacentPoint, creature) && ifemptyFrom(grid, adjacentPoint)) {
    grid.squares[adjacentPoint.r][adjacentPoint.c]=&creature;
    grid.squares[creature.location.r][creature.location.c]=NULL;
    creature.location=adjacentPoint;
  }
}

// command left
void leftFrom(creature_t &creature) {
  if (creature.direction==NORTH) creature.direction=WEST;
  else if (creature.direction==WEST) creature.direction=SOUTH;
  else if (creature.direction==SOUTH) creature.direction=EAST;
  else creature.direction=NORTH;
}

// command right
void rightFrom(creature_t &creature) {
  if (creature.direction==NORTH) creature.direction=EAST;
  else if (creature.direction==EAST) creature.direction=SOUTH;
  else if (creature.direction==SOUTH) creature.direction=WEST;
  else creature.direction=NORTH;
}

// command infect
void infectFrom(grid_t &grid, point_t adjacentPoint, const creature_t &creature) {
  if (creature.ability[ARCH]==0) {
    if (ifenemyFrom(grid, adjacentPoint, creature)) {
      grid.squares[adjacentPoint.r][adjacentPoint.c]->species=creature.species;
      grid.squares[adjacentPoint.r][adjacentPoint.c]->programID=0;
    }
  }
  else {
    while (inGrid(adjacentPoint, grid)) {
      if (!ifemptyFrom(grid, adjacentPoint) && grid.squares[adjacentPoint.r][adjacentPoint.c]->species->name!=creature.species->name) {
        grid.squares[adjacentPoint.r][adjacentPoint.c]->species=creature.species;
        grid.squares[adjacentPoint.r][adjacentPoint.c]->programID=0;
        break;
      }
      // get new adjacent point
      if (creature.direction==NORTH) adjacentPoint.r-=1;
      else if (creature.direction==WEST) adjacentPoint.c-=1;
      else if (creature.direction==SOUTH) adjacentPoint.r+=1;
      else adjacentPoint.c+=1;
    }
  }
}

// whether adjacent point is empty
bool ifemptyFrom(const grid_t &grid, point_t adjacentPoint) {
  if (inGrid(adjacentPoint, grid) && grid.squares[adjacentPoint.r][adjacentPoint.c]==NULL) return true;
  return false;
}

// whether adjacent point is enemy
bool ifenemyFrom(const grid_t &grid, point_t adjacentPoint, const creature_t &creature) {
  if (inGrid(adjacentPoint, grid) && grid.terrain[adjacentPoint.r][adjacentPoint.c]!=FOREST && !ifemptyFrom(grid, adjacentPoint) && grid.squares[adjacentPoint.r][adjacentPoint.c]->species->name!=creature.species->name) return true;
  return false;
}

// whether adjacent point is same
bool ifsameFrom(const grid_t &grid, point_t adjacentPoint, const creature_t &creature) {
  if (inGrid(adjacentPoint, grid) && grid.terrain[adjacentPoint.r][adjacentPoint.c]!=FOREST && !ifemptyFrom(grid, adjacentPoint) && grid.squares[adjacentPoint.r][adjacentPoint.c]->species->name==creature.species->name) return true;
  return false;
}

// whether adjacent point is wall
bool ifwallFrom(const grid_t &grid, point_t adjacentPoint, const creature_t &creature) {
  if (!inGrid(adjacentPoint, grid)) return true;
  if (grid.terrain[adjacentPoint.r][adjacentPoint.c]==LAKE && creature.ability[FLY]==false) return true;
  return false;
}

// whether adjacent point is in grid
bool inGrid(point_t pt, const grid_t &grid) {
  if (pt.r>=0 && pt.r<grid.height && pt.c>=0 && pt.c<grid.width) return true;
  return false;
}

// simulate world
void simulateCreature(creature_t &creature, grid_t &grid, bool outType) {
  if (creature.hillActive && !creature.ability[FLY]) creature.hillActive=0;
  else {
    if (outType) cout << "Creature (" << creature.species->name << ' ' << directName[creature.direction] << ' ' << creature.location.r << ' ' << creature.location.c << ") takes action:" << endl;
    int active=1;
    while (active) {
      if (creature.species->program[creature.programID].op==HOP) {
        printInstruction(creature, outType);
        hopFrom(grid, adjacentPoint(creature.location, creature.direction), creature);
        renewHillActive(creature, grid);
        nextProgramID(creature);
        active=0;
      }
      else if (creature.species->program[creature.programID].op==LEFT) {
        printInstruction(creature, outType);
        leftFrom(creature);
        renewHillActive(creature, grid);
        nextProgramID(creature);
        active=0;
      }
      else if (creature.species->program[creature.programID].op==RIGHT) {
        printInstruction(creature, outType);
        rightFrom(creature);
        renewHillActive(creature, grid);
        nextProgramID(creature);
        active=0;
      }
      else if (creature.species->program[creature.programID].op==INFECT) {
        printInstruction(creature, outType);
        infectFrom(grid, adjacentPoint(creature.location, creature.direction), creature);
        renewHillActive(creature, grid);
        nextProgramID(creature);
        active=0;
      }
      else if (creature.species->program[creature.programID].op==IFEMPTY) {
        printInstruction(creature, outType);
        if (ifemptyFrom(grid, adjacentPoint(creature.location, creature.direction))) creature.programID=creature.species->program[creature.programID].address-1;
        else nextProgramID(creature);
      }
      else if (creature.species->program[creature.programID].op==IFENEMY) {
        printInstruction(creature, outType);
        if (ifenemyFrom(grid, adjacentPoint(creature.location, creature.direction), creature)) creature.programID=creature.species->program[creature.programID].address-1;
        else nextProgramID(creature);
      }
      else if (creature.species->program[creature.programID].op==IFSAME) {
        printInstruction(creature, outType);
        if (ifsameFrom(grid, adjacentPoint(creature.location, creature.direction), creature)) creature.programID=creature.species->program[creature.programID].address-1;
        else nextProgramID(creature);
      }
      else if (creature.species->program[creature.programID].op==IFWALL) {
        printInstruction(creature, outType);
        if (ifwallFrom(grid, adjacentPoint(creature.location, creature.direction), creature)) creature.programID=creature.species->program[creature.programID].address-1;
        else nextProgramID(creature);
      }
      else {
        printInstruction(creature, outType);
        creature.programID=creature.species->program[creature.programID].address-1;
      }
    }
    if (outType) printGrid(grid);
  }
}

// next programID
void nextProgramID(creature_t &creature) {
  if (creature.programID<creature.species->programSize-1) creature.programID++;
  else creature.programID=0;
}

// renew hillActive
void renewHillActive(creature_t &creature, grid_t &grid) {
  if (grid.terrain[creature.location.r][creature.location.c]==HILL) creature.hillActive=1;
}

// print instruction
void printInstruction(creature_t &creature, bool outType) {
  if (creature.species->program[creature.programID].op>3) {
    if (outType) cout << "Instruction " << creature.programID+1 << ": " << opName[creature.species->program[creature.programID].op] << ' ' << creature.species->program[creature.programID].address << endl;
  }
  else {
    if (outType) cout << "Instruction " << creature.programID+1 << ": " << opName[creature.species->program[creature.programID].op] << endl;
    else cout << "Creature (" << creature.species->name << ' ' << directName[creature.direction] << ' ' << creature.location.r << ' ' << creature.location.c << ") takes action: " << opName[creature.species->program[creature.programID].op] << endl;
  }
}
