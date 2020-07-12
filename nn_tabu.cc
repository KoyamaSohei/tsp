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

int getNextPoint(int k) {
  point p(city[k][0],city[k][1]);
  int num=2;
  while(1) {
    vp dst;
    rtree.query(bgi::nearest(p,num),back_inserter(dst));
    for(auto nea:dst) {
      int id = nea.second;
      if(id==k) continue;
      if(uf.root(id)==uf.root(k)) continue;
      uf.connect(id,k);
      return id;
    }
    num <<= 1;
  }
  return -1;
}

const double LIMIT=2.0;
vi neighbor[MAX];
int bestlen=INF;
vi bestlog;
clock_t startt,endt;
const int TL=50;

void snapshot() {
  if(bestlen>length) {
    chmin(bestlen,length);
    bestlog.resize(n);
    rep(i,n) {
      bestlog[i]=tour[i];
    }
  }
}

vi lifetime;

// 初期解をNearest Neighbor法で構築
// 未訪問の近傍の都市を求めるために、Rtreeを用いることで、計算量を落とした
void build() {
  {
    // init log
    bestlog.resize(n);
  }
  {
    // set time
    startt = clock();
    endt = startt + CLOCKS_PER_SEC*LIMIT;
  }
  {
    // set tabu
    lifetime.assign(n,TL);
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
    vi next(n,-1);
    int k=n-1;
    rep(i,n-1) {
      int nex = getNextPoint(k);
      next[k]=nex;
      k=nex;
    }
    next[k]=n-1;
    k=n-1;
    rep(i,n) {
      tour[i]=k;
      k=next[k];
    }
    rep(i,n) {
      length += dist(tour[i],tour[(i+1)%n]);
    }
    snapshot();
  }
  {
    // build neighbor
    rep(i,n) {
      point p(city[i][0],city[i][1]);
      vp dst;
      rtree.query(bgi::nearest(p,min(n,MAX/10)),back_inserter(dst));
      for(auto nea:dst) {
        int id = nea.second;
        if(id==i) continue;
        neighbor[i].push_back(id);
      }
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
    st.pop();
  }
}

int cnt = 0;

void tabu() {
  rep(i,n) {
    int a = tour[i];
    if(lifetime[a]) continue;
    int b = tour[(i+1)%n];
    int score=-INF,pi=-INF;
    for(int c:neighbor[a]) {
      if(b==c) continue;
      if(lifetime[c]) continue;
      int ci;
      rep(k,n) {
        if(tour[k]==c) {
          ci=k;
          break;
        }
      }
      int d = tour[(ci+1)%n];
      int s = dist(a,b)+dist(c,d)-dist(a,c)-dist(b,d);
      if(s>score) {
        score=s;
        pi=ci;
      }
    }
    if(pi<0) continue;
    lifetime[i]=TL;
    lifetime[(i+1)%n]=TL;
    lifetime[pi]=TL;
    lifetime[(pi+1)%n]=TL;
    flip(i,(i+1)%n,pi,(pi+1)%n);
  }
}

int tspSolver() {
  build();
  while(clock() < endt) {
    tabu();
    snapshot();
    rep(i,n) {
      if(lifetime[i]) {
        lifetime[i]--;
      }
    }
  }
  length = bestlen;
  rep(i,n) {
    tour[i]=bestlog[i];
  }
  return 1;
}