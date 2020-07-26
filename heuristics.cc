#include <time.h>
#include <iostream>
using namespace std;

extern void bootstrap();
extern void build();
extern void run();

int tspSolver() {
  bootstrap();
  auto a = clock();
  build();
  auto b = clock();
  run();
  auto c = clock();
  cerr << "build time "   << (b-a) << endl;
  cerr << "improve time " << (c-b) << endl;
  return 1;
}