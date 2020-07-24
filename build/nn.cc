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

extern bgi::rtree<pair<point,unsigned>,bgi::quadratic<MAX>> rtree;
typedef vector<pair<point,unsigned>> vp;

struct UnionFind {
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

extern int dist(int i, int j);


UnionFind uf(MAX);
int pos[MAX];

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

// Nearest Neighbor法で初期解を構築
void build() {
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
  length=0;
  rep(i,n) {
    length += dist(tour[i],tour[(i+1)%n]);
  }
  // build pos
  rep(i,n) {
    pos[tour[i]]=i;
  }
}