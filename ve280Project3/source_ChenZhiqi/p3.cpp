#include "simulation.h"

int main(int argc, char *argv[]) {
  // check validity of command line input
  checkArguments(argc, argv);
  world_t world;
  // intialize world and check error
  if (initWorld(world, argv[1], argv[2])) {
    // get round
    int rounds=atoi(argv[3]);
    // decide print type
    bool outType=false;
    if (argc>=5) {
      string type=argv[4];
      if (type=="v" || type=="verbose") outType=true;
    }
    // print initial state
    cout << "Initial state" << endl;
    printGrid(world.grid);
    // start simulation
    for (int i=1;i<=rounds;i++) {
      cout << "Round " << i << endl;
      for (int j=0;j<world.numCreatures;j++) simulateCreature(world.creatures[j], world.grid, outType);
      if (!outType) printGrid(world.grid);
    }
  }
  return 0;
}
