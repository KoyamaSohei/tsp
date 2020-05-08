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

int dist(int i, int j) {
  float xd = city[i][0] - city[j][0];
  float yd = city[i][1] - city[j][1];
  return (int)(sqrt(xd * xd + yd * yd) + .5);
}

// 始点: n-1 (0-indexed)
// dp[i][s] := 始点から集合sを通りiまで移動した最短距離
// dp[j][s|(1<<j)] := min(dp[i][s]+dist(i,j) | i \in s)

int tspSolver() {
  vvi dp(n,vi(1<<n,INF));
  rep(i,n-1) {
    dp[i][1<<i] = dist(n-1,i);
  }
  rep(s,1<<n) {
    rep(i,n) {
      if(!((s>>i)&1)) continue;
      rep(j,n) {
        chmin(dp[j][s|(1<<j)],dp[i][s]+dist(i,j));
      }
    }
  }
  // 経路復元
  int s = (1<<n)-1;
  int k=n-1;
  vi next(n);
  while(__builtin_popcount(s)>1) {
    int a = dp[k][s];
    rep(i,n) {
      if(i==k) continue;
      int b = dp[i][s^(1<<k)];
      if(b+dist(i,k)!=a) continue;
      next[k]=i;
      s = s^(1<<k);
      k=i;
      break;
    }
  }
  next[k]=n-1;
  k=n-1;
  rep(i,n) {
    tour[i]=k;
    k=next[k];
  }
  length = dp[n-1][(1<<n)-1];
  return 1;
}