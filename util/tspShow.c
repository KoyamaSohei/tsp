

//: Quick help
/// Ver. 2009.09.24

/// ### Option
/// # make problem
/// [fileName] : set cityFile
/// -r [number] : make random cities list
/// -s [number] : rundom seed
///
/// # show
/// -n : ignore graphic
/// -a : ignore aspect ratio
/// -w [number] : wait percentage
/// 
/// # output
/// -c : output cities list
/// -t : output tour list
///
/// # for debug or test
/// -z [number] : rotate cities

/// ### Compile
/// gcc -O2 -I/usr/include/X11/ -L/usr/lib/ -lX11 -lm
//$     tspShow.c [inputFileName.c] -o [outputFileName]
///

/// ### InputFile
//;

//: Head
#include<stdio.h>
#include<stdlib.h>
#include <unistd.h>

/// tspr
#include<string.h>

/// tspg
#include<math.h>
#include<float.h>

/// tspr

/// max of number of cities 
#define MAX 20000
/// string buffer size
#define BUFSIZE 1024

//;

//: extern
extern int tspSolver(void); 
//;

//: Global

int n, length, tour[MAX];
float city[MAX][2];

int rndSeed = 127;
int rndRange = 1000;

/// tspg
int winSize = 800;
int winMarg = 50;

int bgColor = 0xFFFFFF;
int ctColor = 0x000072;
int toColor[] = {0x000072, 0xBB0000, 0x004000, 0x7f7f7f};
int toCNum = 4;

int *icity;
int waitPer = 100;
int gActive = 1;

//: "main"
int main(int argc, char *argv[]) 
{ 
  //: local variable
  /// tspr
  FILE *fp;
  int fflag = 0;
  char buf[BUFSIZE];
  char *c1, *c2;
  char ct;
  float cBuf[3];
  
  int rndm = 0;
  int sed = rndSeed;
  int zur = 0;
  int bar = 0;
  int cis = 0;
  int tos = 0;

  /// tspg
  double maxx = DBL_MIN;
  double maxy = DBL_MIN;
  double minx = DBL_MAX;
  double miny = DBL_MAX;
  double cenx, ceny;
  double wid, hei;
  double d;
  
  int as = 0;
  
  float xd, yd;
  int t1;
  int i, j, fl;
  //;

  //: option
  for(i = 1; i < argc; i++) {
    if(argv[i][0] == '-') {
      for(j = 1; ; j++) {
        if(argv[i][j] == 'r') {
          rndm = 1;
          i++;
          if(i >= argc) {
            fprintf(stderr, "no number!\n");
            exit(1);
          }
          n = atoi(argv[i]);
          if(n < 3 || MAX < n) {
            fprintf(stderr, "bad number!\n");
            exit(1);
          }
          break;
        } else if(argv[i][j] == 's') {
          i++;
          if(i >= argc) {
            fprintf(stderr, "no seed!\n");
            exit(1);
          }
          sed = atoi(argv[i]);
          break;
        } else if(argv[i][j] == 'z') {
          i++;
          if(i >= argc) {
            fprintf(stderr, "no zurashi!\n");
            exit(1);
          }
          zur = atoi(argv[i]);
          if(zur < 0) {
            fprintf(stderr, "bad zurashi!\n");
            exit(1);
          }
          break;
        } else if(argv[i][j] == 'b') {
          i++;
          if(i >= argc) {
            fprintf(stderr, "no barashi!\n");
            exit(1);
          }
          bar = atoi(argv[i]);
          if(bar < 0) {
            fprintf(stderr, "bad barashi!\n");
            exit(1);
          }
          break;
        } else if(argv[i][j] == 'c') {
          cis = 1;
        } else if(argv[i][j] == 't') {
          tos = 1;
        } else if(argv[i][j] == 'a') {
          as = 1;
        } else if(argv[i][j] == 'n') {
          gActive = 0;
        } else if(argv[i][j] == 'w') {
          i++;
          if(i >= argc) {
            fprintf(stderr, "no wait!\n");
            exit(1);
          }
          waitPer = atoi(argv[i]);
          if(waitPer < 0) {
            waitPer = 0;
          }
          break;
        } else {
          break;
        }
      }
    } else if(fflag == 0) {
      if((fp = fopen(argv[i], "r")) == NULL) {
        fprintf(stderr, "cannot open file!\n");
        exit(1);
      }
      fflag = 1;
    }
  }
  //;
  
  //: make cities list
  if(rndm == 0 || fflag == 1) {
    
    if(fflag == 0) {
      fp = stdin;
    }

    //: read dimension
    n = 0;
    while(fgets(buf, BUFSIZE, fp) != NULL) {
      ct = buf[0];
      if(ct == 'N') {
        if(strncmp("NODE_CORD_SECTION", buf, 17) == 0) break;
        if(strncmp("NODE_COORD_SECTION", buf, 18) == 0) break;
        if(strncmp("NAME", buf, 4) == 0) { }
      } else if(ct == 'D') {
        if(strncmp("DIMENSION", buf, 9) == 0) {
          for(c1 = buf + 9; *c1 != ':'; c1++); 
          n = atoi(c1 + 1);
        }
      } else if(ct == 'T') {
        if(strncmp("TYPE:", buf, 5) == 0) { }
      } else if(ct == 'C') {
        if(strncmp("COMMENT:", buf, 5) == 0) { }
      } else if(ct == 'E') {
        if(strncmp("EDGE_WEIGHT_TYPE:", buf, 17) == 0) { }
      } else if('0' <= ct && ct <= '9' && !n) {
        n = atoi(buf);
        break;
      }
    }
    
    if(n < 3 || n > MAX) {

      fprintf(stderr, "bad DIMENSION!\n");
      exit(1);
    }
    //;
    //: read cities list
    for(i = 0; i < n; i++) {
      if(fgets(buf, BUFSIZE, fp) == NULL) {
        fprintf(stderr, "city data error!\n");
        exit(1);
      }
      
      c1 = buf;
      j = 0;
      fl = 0;
      for(c2 = buf; ; c2++) {
        if(*c2 == ' ' || *c2 == '\0') {
          if(*c2 == ' ') {
            *c2 = '\0';
            fl = 1;
          }
          
          if(c1 != c2) {
            cBuf[j] = (float)atof(c1);
            j++;
            if(j >= 3) break;
          }
          
          c1 = c2 + 1;
          if(fl == 1) {
            fl = 0;
          } else {
            break;
          }
        }
      }
      t1 = (i + zur) % n;
      if(j == 3) {
        if((int)(cBuf[0]) != i + 1) {
          fprintf(stderr, "city data error!\n");
          exit(1);
        }
        city[t1][0] = cBuf[1];
        city[t1][1] = cBuf[2];
      } else {
        city[t1][0] = cBuf[0];
        city[t1][1] = cBuf[1];
      }
    }
    //;
    
    if(fflag == 1) {
      fclose(fp);
    } else {
      fp = NULL;
    }
    
  //;

  //: make random cities list
  } else {
    
    srand(sed);  

    for(i = 0; i < n; i++) {
      for(j = 0; j < 2; j++) {
        city[(i + zur) % n][j] = (float)( (double)rand() / ((double)RAND_MAX / rndRange) );
      }
    }
  }
  //;

  //: tspg init
  icity = (int *)malloc(sizeof(int) * n * 2);
  
  //enu
  tspSolver();
  
  
  if(cis == 1) {
    printf("%d\n", n);
    for(i = 0; i < n; i++) {
      printf("%f %f\n", city[tour[i]][0], city[tour[i]][1]);
    }
  }
  if(tos == 1) {
    for(i = 0; i < n; i++) {
      printf("%d ", tour[i]);
    }
    printf("\n");
  }
  t1 = 0;
  for(i = 0; i < n; i++) {
    xd = city[tour[i]][0] - city[tour[(i + 1) % n]][0];
    yd = city[tour[i]][1] - city[tour[(i + 1) % n]][1];
    t1 += (int)(sqrt(xd * xd + yd * yd) + .5);
  }
  if(t1 != length) {
    fprintf(stderr, "length = %d (%d?)\n", t1, length); 
  } else {
    fprintf(stderr, "length = %d            \n", t1);
  }
  //;
  
  return 0;
}
//;


