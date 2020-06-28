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
#define uni(x) x.erase(unique(rng(x)),x.end())
#define show(x) cout<<#x<<" = "<<x<<endl;
#define chmin(x,y) x=min(x,y)
#define chmax(x,y) x=max(x,y)
using namespace std;
typedef vector<int> vi;
typedef vector<vi> vvi;
typedef pair<int,int> edge;
typedef vector<set<int>> vs;
typedef vector<edge> ve;
const int INF = 1001001001;

namespace bg = boost::geometry;
namespace bgi = boost::geometry::index;
typedef bg::model::point<float, 2, bg::cs::cartesian> point;

bgi::rtree<pair<point,unsigned>,bgi::quadratic<MAX>> rtree;
typedef vector<pair<point,unsigned>> vp;

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

int dist(int i, int j) {
  float xd = city[i][0] - city[j][0];
  float yd = city[i][1] - city[j][1];
  return (int)(sqrt(xd * xd + yd * yd) + .5);
}

ve neighbor;

struct State {
  vs required;
  vs forbidden;
  State(): required(n),forbidden(n) {};
  State(vs r,vs f): required(r),forbidden(f) {};
  int lower_bound() {
    UnionFind uf(n);
    int res = 0;
    int hasplus1=0;
    int num=0;
    rep(a,n) {
      for(int b:required[a]) {
        if(a>b) continue;
        if(uf.root(a)!=uf.root(b)) {
          uf.connect(a,b);
          res += dist(a,b);
          num++;
        } else if(!hasplus1) {
          hasplus1++;
          res += dist(a,b);
          num++;
        } else {
          return INF;
        }
      }
    }
    rep(i,sz(neighbor)) {
      if(num==n) break;
      edge e = neighbor[i];
      if(required[e.first].count(e.second)) continue;
      if(forbidden[e.first].count(e.second)) continue;
      if(uf.root(e.first)!=uf.root(e.second)) {
        uf.connect(e.first,e.second);
        res += dist(e.first,e.second);
        num++;
      } else if(!hasplus1) {
        hasplus1++;
        res += dist(e.first,e.second);
        num++;
      }
    }
    return res;
  }
  bool is_valid() {
    // All dimention of point must be 2
    rep(i,n) {
      if(sz(required[i])!=2) {
        return false;
      }
    }
    // Length of cycle must be n 
    // Num of cycle must be 1
    //printf("all dim is 2\n");
    int par=-1;
    int now=0;
    int len=1;
    while(!(required[now].count(0)&&par!=0)&&len<2*n) {
      len++;
      int a = *required[now].begin();
      int b = *required[now].rbegin();
      //show(now);
      //show(a);
      //show(b);
      int c = a==par?b:a;
      par=now;
      now=c;
    }
    //show(len);
    return len==n;
  }
  void snapshot() {
    int par=-1;
    int now=0;
    vi route{0};
    while(!(required[now].count(0)&&par!=0)) {
      int a = *required[now].begin();
      int b = *required[now].rbegin();
      int c = a==par?b:a;
      par=now;
      now=c;
      route.push_back(now);
    }
    length=0;
    rep(i,n) {
      length += dist(route[i],route[(i+1)%n]);
      tour[i]=route[i];
    }
  }
};

vi seq;
vs feasible;

stack<State> st;
int bound=INF;
State best;

void build() {
  // build rtree
  rep(i,n) {
    point p(city[i][0],city[i][1]);
    rtree.insert(make_pair(p,i));
  }
  // build neighbor max(3,N/100)
  rep(i,n) {
    point p(city[i][0],city[i][1]);
    vp dst;
    rtree.query(bgi::nearest(p,max(30,n/100)),back_inserter(dst));
    for(auto nea:dst) {
      int id = nea.second;
      if(id==i) continue;
      neighbor.emplace_back(minmax(id,i));
    }
  }
  sort(rng(neighbor),[&](edge a,edge b) {
    int ad = dist(a.first,a.second);
    int bd = dist(b.first,b.second);
    if(ad!=bd) {
      return ad<bd;
    }
    if(a.first!=b.first) {
      return a.first<b.first;
    }
    return a.second<b.second;
  });
  uni(neighbor);
  // build seq(0..N-1)
  seq.resize(n);
  rep(i,n) {
    seq[i]=i;
  }
  // build feasible
  feasible.assign(n,set<int>());
  for(edge e:neighbor) {
    feasible[e.first].insert(e.second);
    feasible[e.second].insert(e.first);
  }
}

int remain_edges(State &s,int p) {
  return sz(feasible[p])-sz(s.required[p])-sz(s.forbidden[p]);
}

bool is_unfeasible(State &s) {
  rep(i,n) {
    if(sz(feasible[i])-sz(s.forbidden[i])<2) {
      return true;
    }
  }
  return false;
}



void pushS1(State &s,int p,vi &es) {
  State s1(s);
  if(sz(es)<2) {
    if(sz(es)==0) return;
    // -> S4(R \cup e1,F) (this)
    // -> S3(R, F \cup e1) 
    int e1 = es[0];
    if(sz(s1.required[p])>1) return;
    if(sz(s1.required[e1])>1) return;
    s1.required[p].insert(e1);
    s1.required[e1].insert(p);
    st.push(s1);
    //printf("push s4\n");
    //show(e1);
    return;
  }
  int e1 = es[0];
  int e2 = es[1];
  if(sz(s1.required[p])>0) return;
  if(sz(s1.required[e1])>1) return;
  if(sz(s1.required[e2])>1) return;
  s1.required[p].insert(e1);
  s1.required[e1].insert(p);
  s1.required[p].insert(e2);
  s1.required[e2].insert(p);
  //printf("push s1\n");
  //show(e1);
  //show(e2);
  st.push(s1);
}

void pushS2(State &s,int p,vi &es) {
  State s2(s);
  if(sz(es)<2) return;
  int e1 = es[0];
  int e2 = es[1];
  if(sz(s2.required[p])>1) return;
  if(sz(s2.required[e1])>1) return;
  s2.required[p].insert(e1);
  s2.required[e1].insert(p);
  s2.forbidden[p].insert(e2);
  s2.forbidden[e2].insert(p);
  //printf("push s2\n");
  //show(e1);
  //show(e2);
  st.push(s2);
}

void pushS3(State &s,int p,vi &es) {
  State s3(s);
  if(sz(es)<1) return;
  int e1 = es[0];
  if(sz(s3.required[p])>2) return;
  s3.forbidden[p].insert(e1);
  s3.forbidden[e1].insert(p);
  //printf("push s3\n");
  //show(e1);
  st.push(s3);
}



int tspSolver() {
  build();
  st.emplace();
  //show(sz(neighbor));
  while(!st.empty()) {
    State s = st.top();
    st.pop();
    int c = s.lower_bound();
    //cout << endl << endl;
    //show(c);
    //printf("---begin required ---\n");
    /*
    rep(i,n) {
      show(i);
      for(int j:s.required[i]) {
        show(j);
      }
      cout << endl;
    }*/
    //printf("---end required ---\n");
    //printf("---begin forbidden ---\n");
    /*
    rep(i,n) {
      show(i);
      for(int j:s.forbidden[i]) {
        show(j);
      }
      cout << endl;
    }*/
    //printf("---end forbidden ---\n");
    if(c>=bound) continue;
    if(is_unfeasible(s)) continue;
    if(s.is_valid()) {
      bound=c;
      best=s;
      continue;
    }
    // choice p
    sort(rng(seq),[&](int a,int b) {
      int as = sz(s.required[a]);
      int bs = sz(s.required[b]);
      if(as!=bs) {
        return as>bs; 
      }
      int ar = remain_edges(s,a);
      int br = remain_edges(s,b);
      if(ar!=br) {
        return ar<br;
      }
      return sz(s.forbidden[a])>sz(s.forbidden[b]);
    });
    int pi=0;
    while(pi<n&&sz(s.required[seq[pi]])==2) pi++;
    if(pi==n) continue;
    int p = seq[pi];
    //show(p);
    vi es;
    for(int b:feasible[p]) {
      if(s.required[p].count(b)) continue;
      if(s.forbidden[p].count(b)) continue;
      es.push_back(b);
    }
    pushS1(s,p,es);
    pushS2(s,p,es);
    pushS3(s,p,es);
  }
  //show(bound);
  best.snapshot();
  return 1;
}