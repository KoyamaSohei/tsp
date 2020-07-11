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
#define uni(x) x.erase(unique(rng(x)),x.end())
using namespace std;
typedef vector<int> vi;
typedef vector<vi> vvi;
typedef pair<int,int> edge;
typedef vector<edge> ve;
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

int xor64() {
  static uint64_t x = 88172645463325252ULL;
  x = x ^ (x << 13); x = x ^ (x >> 7);
  return x = x ^ (x << 17);
}

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

void build(vvi &G) {
  UnionFind uf(n);
  // Kruskal法で最小全域木を求める
  ve e;
  rep(i,n) {
    srep(j,i+1,n) {
      e.emplace_back(i,j);
    }
  }
  sort(rng(e),[](edge a,edge b) {
    return dist(a.first,a.second)<dist(b.first,b.second);
  });
  for(int k=0;uf.size(0)<n;k++) {
    int a = e[k].first;
    int b = e[k].second;
    if(uf.root(a)==uf.root(b)) continue;
    uf.connect(a,b);
    G[a].push_back(b);
    G[b].push_back(a);
  }
  // 次数が奇数の頂点をあつめる
  vi isodd(n,0),oddv;
  rep(k,n) {
    if(sz(G[k])%2){
      isodd[k]++;
      oddv.push_back(k);
    }
  }
  // 重みが最小となる完全マッチングを求める
  // 最適解を求めるアルゴリズムは実装が大変で計算量も大きいので、近似解を求める
  // 適当に貪欲で初期解を求めてから2-optで更新する
  {
    // 次数が奇数の頂点同士をつなぐ辺(コストの昇順)
    ve odde;
    // 対となる頂点
    vi pairwise(n,-1);
    for(edge x:e) {
      if(!isodd[x.first]) continue;
      if(!isodd[x.second]) continue;
      odde.push_back(x);
      // 初期解構築
      if(pairwise[x.first]>=0) continue;
      if(pairwise[x.second]>=0) continue;
      pairwise[x.first]=x.second;
      pairwise[x.second]=x.first;
    }
    int len = sz(oddv);
    for(int cnt=0;cnt<len*len;cnt++) {
      int a1 = oddv[abs(xor64())%len];
      int a2 = pairwise[a1];
      int b1 = oddv[abs(xor64())%len];
      int b2 = pairwise[b1];
      if(a1==b1||a1==b2||a2==b1||a2==b2) continue;
      int plen = dist(a1,a2)+dist(b1,b2);
      int nlen = dist(a1,b1)+dist(a2,b2);
      if(plen<=nlen) continue;
      // カウンタをリセット
      cnt=0;
      pairwise[a1]=b1;
      pairwise[a2]=b2;
      pairwise[b1]=a1;
      pairwise[b2]=a2;
    }
    rep(i,n) {
      if(!isodd[i]) continue;
      G[i].push_back(pairwise[i]);
    }
  }
}

// オイラー路を求める
void buildEulerianTrail(vi &seq,vvi &G) {
  ve ord;
  vector<vector<pair<edge,int>>> g(n);
  rep(i,n) {
    for(int j:G[i]) {
      if(i>=j) continue;
      g[i].emplace_back(edge(i,j),sz(g[j]));
      g[j].emplace_back(edge(j,i),sz(g[i])-1);
    }
  }
  
  stack<pair<int,edge>> st;
  st.emplace(0,edge(-1,-1));
  while(!st.empty()) {
    int p = st.top().first;
    if(g[p].empty()) {
      ord.emplace_back(st.top().second);
      st.pop();
      continue;
    }
    auto x = g[p].back();
    g[p].pop_back();
    if(x.second<0) continue;
    g[x.first.second][x.second].second=-1;
    st.emplace(x.first.second,x.first);
  }

  ord.pop_back();
  reverse(rng(ord));
  for(edge p:ord) {
    seq.push_back(p.first);
  }
}

int tspSolver() {
  vvi G(n);
  vi seq;
  build(G);
  buildEulerianTrail(seq,G);
  // 既に訪れた頂点をとばす
  vi visited(n,0);
  length=0;
  int now=0;
  for(int p:seq) {
    if(visited[p]) continue;
    visited[p]++;
    tour[now++]=p;
  }
  rep(i,n) {
    length += dist(tour[i],tour[(i+1)%n]);
  }
  return 1;
}