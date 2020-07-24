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
typedef pair<int,int> edge;
typedef vector<edge> ve;
const int INF = 1001001001;

extern int n, length, tour[MAX];

extern int dist(int i, int j);
extern int xor64();

extern vi neighbor[MAX];
extern int pos[MAX];

extern int bestlen;
extern vi bestlog;

extern void snapshot();

// before: a -> b,c -> d
//  after: a -> c,b -> d
// => reverse b ... c
void flip(int ai,int bi,int ci,int di) {
  stack<int> st;
  for(int p=bi;p!=di;p=(p+1)%n) {
    st.push(tour[p]);
  }
  for(int p=bi;p!=di;p=(p+1)%n) {
    tour[p]=st.top();
    pos[tour[p]]=p;
    st.pop();
  }
}


bool moveNext(int tmp,double t) {
  double x = double(xor64()%10000)/10000;
  double y = exp(double(tmp)/t);
  return x<y;
}

double tmax = 500.0;
double tmin = 1e-5;
double delta = 0.1;
const double maxrate = 0.98;
const int steps = 100000;
const int tsteps = 60;


int init_tour[MAX];
int init_pos[MAX];
int init_length;

// 温度tで何回2-optでswapされる回数/step数を返す
double exec(double t) {
  memcpy(tour,init_tour, sizeof(init_tour));
  memcpy(pos,init_pos, sizeof(init_pos));
  length=init_length;
  int imp=0;
  for(int i=0,cnt=0;cnt<steps;i=(i+1)%n) {
    int a = tour[i];
    int b = tour[(i+1)%n];
    for(int c:neighbor[a]) {
      if(b==c) continue;
      if(cnt>=steps) break;
      int k = pos[c];
      int d = tour[(k+1)%n];
      if(b==d||a==d) continue;
      cnt++;
      int tmp = dist(a,b)+dist(c,d)-dist(a,c)-dist(b,d);
      if(tmp>0) {
        flip(i,(i+1)%n,k,(k+1)%n);
        length-=tmp;
        imp++;
        break;
      }
      if(!moveNext(tmp,t)) {
        continue;
      }
      flip(i,(i+1)%n,k,(k+1)%n);
      length-=tmp;
      imp++;
      break;
    }
  }
  return double(imp)/steps;
}

void setParam() {
  // 最適なTmax,Tminを調べる
  memcpy(init_tour,tour, sizeof(tour));
  memcpy(init_pos,pos, sizeof(pos));
  init_length=length;
  {
    // find Tmax
    double lt=0,rt=INF;
    rep(tryi,30) {
      double m = (lt+rt)/2;
      double rate = exec(m);
      if(rate<maxrate) {
        lt=m;
      } else {
        rt=m;
      }
    }
    cerr << lt << endl;
    tmax=lt;
  }
  {
    // find Tmin
    double lt=1e-5,rt=tmax;
    rep(tryi,10) {
      double m = (lt+rt)/2;
      double rate = exec(m);
      if(rate<0) {
        lt=m;
      } else {
        rt=m;
      }
    }
    cerr << lt << endl;
    tmin=lt;
  }
  delta = exp(log(tmin/tmax)/tsteps);
  cerr << delta << endl;
  memcpy(init_tour,tour, sizeof(tour));
  memcpy(init_pos,pos, sizeof(pos));
  init_length=length;
}

void sa() {
  for(double t=tmax;t>tmin;t*=delta) {
    for(int i=0,step=0;step<steps;i=(i+1)%n) {
      int a = tour[i];
      int b = tour[(i+1)%n];
      for(int c:neighbor[a]) {
        if(b==c) continue;
        if(step>=steps) break;
        int k = pos[c];
        int d = tour[(k+1)%n];
        if(b==d||a==d) continue;
        step++;
        int tmp = dist(a,b)+dist(c,d)-dist(a,c)-dist(b,d);
        if(tmp>0) {
          flip(i,(i+1)%n,k,(k+1)%n);
          length-=tmp;
          snapshot();
          break;
        }
        if(!moveNext(tmp,t)) {
          continue;
        }
        flip(i,(i+1)%n,k,(k+1)%n);
        length-=tmp;
        break;
      }
    }
  }
}

void run() {
  snapshot();
  setParam();
  sa();
  length = bestlen;
  rep(i,n) {
    tour[i]=bestlog[i];
  }
}