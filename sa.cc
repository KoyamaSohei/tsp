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

unsigned int randxor() {
    static unsigned int x=123456789,y=362436069,z=521288629,w=88675123;
    unsigned int t;
    t=(x^(x<<11));x=y;y=z;z=w; return( w=(w^(w>>19))^(t^(t>>8)) );
}

int dist(int i, int j) {
  float xd = city[i][0] - city[j][0];
  float yd = city[i][1] - city[j][1];
  return (int)(sqrt(xd * xd + yd * yd) + .5);
}

UnionFind uf(MAX);

bgi::rtree<pair<point,unsigned>,bgi::quadratic<MAX>> rtree;
typedef vector<pair<point,unsigned>> vp;

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
vi neighbor10[MAX];
int bestlen = INF;
vi bestlog;
clock_t startt,endt;

// TODO: そこそこ良い構築をする
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
  }
  {
    // build neighbor10
    rep(i,n) {
      point p(city[i][0],city[i][1]);
      vp dst;
      rtree.query(bgi::nearest(p,max(n,11)),back_inserter(dst));
      for(auto nea:dst) {
        int id = nea.second;
        if(id==i) continue;
        neighbor10[i].push_back(id);
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


void snapshot() {
  if(bestlen>length) {
    chmin(bestlen,length);
    bestlog.resize(n);
    rep(i,n) {
      bestlog[i]=tour[i];
    }
  }
}

bool moveNext(int tmp) {
  clock_t now = clock();
  double x = 0.0001*exp(10.0*(endt - now)/(endt-startt));
  int f = randxor()%200000;
  int res = 200000+tmp*x-f;
  // show(res);
  return res<0;
}

void sa() {
  int x = randxor()%n;
  int a = tour[x];
  int b = tour[(x+1)%n];
  int k = neighbor10[a][randxor()%sz(neighbor10[a])];
  int c = tour[k];
  if(b==c) return;
  int d = tour[(k+1)%n];
  if(b==d||a==d) return;
  int tmp = dist(a,b)+dist(c,d)-dist(a,c)-dist(b,d);
  if(tmp>0) {
    flip(x,(x+1)%n,k,(k+1)%n);
    length-=tmp;
    return;
  }
  if(!moveNext(tmp)) return;
  flip(x,(x+1)%n,k,(k+1)%n);
  length-=tmp;
}

int tspSolver() {
  build();
  while(clock() < endt) {
    sa();
    snapshot();
  }
  length = bestlen;
  rep(i,n) {
    tour[i]=bestlog[i];
  }
  return 1;
}