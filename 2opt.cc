#include <bits/stdc++.h>
#define MAX 20000
#define rep(i,n) for(int i = 0; i < (n); ++i)
#define drep(i,n) for(int i = (n)-1; i >= 0; --i)
#define srep(i,s,t) for (int i = s; i < t; ++i)
#define rng(a) a.begin(),a.end()
#define sz(x) (int)(x).size()
#define show(x) cout<<#x<<" = "<<x<<endl;
#define chmin(x,y) x=min(x,y)
#define chmax(x,y) x=max(x,y)
using namespace std;
typedef vector<int> vi;
typedef vector<vi> vvi;
const int INF = 1001001001;

extern int n, length, tour[MAX];
extern float city[MAX][2];
extern void showTour(int *tou, int wai, int color);
extern void showCTour(int *tou, int wai, int *color);
extern void showString(char *str);
extern void showLength(int leng);

// http://xoshiro.di.unimi.it/xoshiro256starstar.c

static inline uint64_t rotl(const uint64_t x, int k) {
	return (x << k) | (x >> (64 - k));
}

static uint64_t s[4];

uint64_t next(void) {
	const uint64_t result = rotl(s[1] * 5, 7) * 9;

	const uint64_t t = s[1] << 17;

	s[2] ^= s[0];
	s[3] ^= s[1];
	s[1] ^= s[2];
	s[0] ^= s[3];

	s[2] ^= t;

	s[3] = rotl(s[3], 45);

	return result;
}

int dist(int i, int j) {
  float xd = city[i][0] - city[j][0];
  float yd = city[i][1] - city[j][1];
  return (int)(sqrt(xd * xd + yd * yd) + .5);
}


// TODO: そこそこ良い構築をする
void build() {
  length=0;
  rep(i,n) {
    tour[i]=i;
  }
  rep(i,n) {
    length+=dist(i,(i+1)%n);
  }
}

void flip(int ai,int bi,int ci,int di) {
  vi arr;
  int p=bi;
  while(p!=di) {
    arr.push_back(tour[p]);
    p++;
    p%=n;
  }
  reverse(rng(arr));
  p=bi;
  rep(i,sz(arr)) {
    tour[(bi+i)%n]=arr[i];
  }
}

void twoopt() {
  rep(i,n) {
    int a = tour[i];
    int b = tour[(i+1)%n];
    int k = (next())%n;// TODO: 近くを選びやすくする
    int c = tour[k];
    if(b==c) continue;
    int d = tour[(k+1)%n];
    if(b==d||a==d) continue;
    int tmp = dist(a,b)+dist(c,d)-dist(a,c)-dist(b,d);
    if(tmp>0) {
      flip(i,(i+1),k,(k+1)%n);
      length-=tmp;
      return;
    }
  }
}

const double LIMIT=2.0;

int tspSolver() {
  build();
  const auto until_ck = clock() + CLOCKS_PER_SEC*LIMIT;
  while(clock() < until_ck) {
    twoopt();
  }
  return 1;
}