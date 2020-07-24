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
extern vi neighbor[MAX];

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

int pos[MAX];
UnionFind uf(MAX); 

// 貪欲法で初期解を構築
void build() {
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
  // build pos
  rep(i,n) {
    pos[tour[i]]=i;
  }
}