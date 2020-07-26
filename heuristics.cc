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
  cerr << "build duration "   << (b-a)/CLOCKS_PER_SEC << endl;
  cerr << "improve time " << (c-b)/CLOCKS_PER_SEC << endl;
  return 1;
}