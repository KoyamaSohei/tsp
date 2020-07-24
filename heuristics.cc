extern void bootstrap();
extern void build();
extern void run();

int tspSolver() {
  bootstrap();
  build();
  run();
  return 1;
}