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
typedef vector<double> vd;
typedef vector<int> vi;
typedef vector<vi> vvi;
const int INF = 1001001001;

int xor64() {
  static uint64_t x = 88172645463325252ULL;
  x = x ^ (x << 13); x = x ^ (x >> 7);
  return x = x ^ (x << 17);
}

int main(int argc,char *argv[]) {
  if(argc!=2) {
    printf("Usage %s number of tours",argv[0]);
    exit(1);
  }
  int N = stoi(argv[1]);
  printf("%d\n",N);
  rep(i,N) {
    int x = abs(xor64())%1000;
    int y = abs(xor64())%1000;
    printf("%d %d\n",x,y);
  }
  return 0;
}