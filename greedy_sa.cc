#include <bits/stdc++.h>
#include <boost/geometry.hpp>
#include <boost/geometry/geometries/point.hpp>
#include <boost/geometry/geometries/box.hpp>
#include <boost/geometry/index/rtree.hpp>
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

namespace bg = boost::geometry;
namespace bgi = boost::geometry::index;
typedef bg::model::point<float, 2, bg::cs::cartesian> point;

extern int n, length, tour[MAX];
extern float city[MAX][2];
extern void showTour(int *tou, int wai, int color);
extern void showCTour(int *tou, int wai, int *color);
extern void showString(char *str);
extern void showLength(int leng);

int xor64() {
  static uint64_t x = 88172645463325252ULL;
  x = x ^ (x << 13); x = x ^ (x >> 7);
  x = x ^ (x << 17);
  return abs(int(x));
}

int dist(int i, int j) {
  float xd = city[i][0] - city[j][0];
  float yd = city[i][1] - city[j][1];
  return (int)(sqrt(xd * xd + yd * yd) + .5);
}

class UnionFind {
  public:
    vi Parent;
    UnionFind(int N) {
      Parent = vi(N,-1);
    }
  
    int root(int A) {
      if(Parent[A] < 0) return A;
      return Parent[A] = root(Parent[A]);
    }
  
    int size(int A) {
      return -Parent[root(A)];
    }
  
    bool connect(int A,int B) {
      A = root(A);
      B = root(B);
      if(A == B) return false;
      if(size(A) < size(B)) swap(A,B);
      Parent[A] += Parent[B];
      Parent[B] = A;
      return true;
    }
};


bgi::rtree<pair<point,unsigned>,bgi::quadratic<MAX>> rtree;
typedef vector<pair<point,unsigned>> vp;

UnionFind uf(MAX);

vi neighbor[MAX];
int pos[MAX];
int bestlen=INF;
vi bestlog;

void snapshot() {
  if(bestlen>length) {
    chmin(bestlen,length);
    bestlog.resize(n);
    rep(i,n) {
      bestlog[i]=tour[i];
    }
  }
}

// 初期解をGreedy法で構築
void build() {
  {
    // init log
    bestlog.resize(n);
  }
  {
    // build rtree
    rep(i,n) {
      point p(city[i][0],city[i][1]);
      rtree.insert(make_pair(p,i));
    }
  }
  {
    // build initial path
    ve e;
    rep(i,n) {
      srep(j,i+1,n) {
        e.emplace_back(i,j);
      }
    }
    sort(rng(e),[&](edge a,edge b) {
      return dist(a.first,a.second)<dist(b.first,b.second);
    });
    vvi G(n);
    for(auto itr = e.begin();uf.size(0)<n;itr++) {
      if(uf.root(itr->first)==uf.root(itr->second)) continue;
      if(sz(G[itr->first])==2) continue;
      if(sz(G[itr->second])==2) continue;
      uf.connect(itr->first,itr->second);
      G[itr->first].push_back(itr->second);
      G[itr->second].push_back(itr->first);
    }
    vi rem;
    rep(i,n) {
      if(sz(G[i])<2) {
        rem.push_back(i);
      }
    }
    G[rem[0]].push_back(rem.back());
    G[rem.back()].push_back(rem[0]);
    int par=-1;
    int now=0;
    vi route{0};
    while(!(par>0&&now==0)) {
      int a = G[now][0];
      int b = G[now].back();
      int c = a==par?b:a;
      par=now;
      now=c;
      route.push_back(now);
    }
    length=0;
    rep(i,n) {
      tour[i]=route[i];
      length += dist(route[i],route[(i+1)%n]);
    }
    snapshot();
  }
  {
    // build neighbor
    rep(i,n) {
      point p(city[i][0],city[i][1]);
      vp dst;
      rtree.query(bgi::nearest(p,int(10*log2(n))),back_inserter(dst));
      for(auto nea:dst) {
        int id = nea.second;
        if(id==i) continue;
        neighbor[i].push_back(id);
      }
    }
  }
  {
    // build pos
    rep(i,n) {
      pos[tour[i]]=i;
    }
  }
}

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
  double x = double(abs(xor64())%10000)/10000;
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

int tspSolver() {
  build();
  setParam();
  sa();
  length = bestlen;
  rep(i,n) {
    tour[i]=bestlog[i];
  }
  return 1;
}