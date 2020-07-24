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

struct RMQ {
  vi segtree;
  int N;
  RMQ() {
    N = 1;
    int len = 1;
    while(N < n) {
      N *= 2;
      len += N;
    }
    segtree = vi(len,INF);
  }
  void build(int idx,int val) {
    int s = sz(segtree) - (N-idx);
    segtree[s] = val;
    while(1) {
      if(s == 0) break;
      s = (s-1)/2;
      segtree[s] = min(segtree[s*2+1],segtree[s*2+2]);
    }
  }
  int _query(int a,int b,int idx,int l,int r) {
    if(a <= l && r <= b) return segtree[idx];
    if(r <= a || b <= l) return INF;
    int vl = _query(a,b,idx*2+1,l,(l+r)/2);
    int rl = _query(a,b,idx*2+2,(l+r)/2,r);
    return min(vl,rl);
  }
  // min(A[i] | l <= i < r)
  int query(int l,int r) {
    return _query(l,r,0,0,N);
  }
};

int pos[MAX];

// 最遠挿入法で初期解を構築
void build() {
  vector<set<int>> G(n);
  // 巡回路に含まれる頂点集合x,その補集合y
  set<int> x,y;
  x.insert(0);
  G[0].insert(0);
  srep(i,1,n) {
    y.insert(i);
  }
  // 各頂点間の距離を前計算
  // xにはいっているもののみ更新
  vector<RMQ> rmq(n);
  rep(i,n) {
    rmq[i].build(0,dist(0,i));
  }
  while(sz(x)<n) {
    int k=-INF,kr=-INF;
    for(int i:y) {
      int r = rmq[i].query(0,n);
      if(kr<r) {
        kr=r;
        k=i;
      }
    }
    int a=-INF,b=-INF,abr=INF;
    for(int i:x) {
      for(int j:G[i]) {
        int r = dist(i,k)+dist(k,j)-dist(i,j);
        if(r<abr) {
          abr=r;
          a=i;
          b=j;
        }
      }
    }
    if(a==b) {
      G[a].insert(k);
      G[k].insert(a);
      G[a].erase(b);
    } else if(sz(x)==2) {
      G[a].insert(k);
      G[k].insert(a);
      G[b].insert(k);
      G[k].insert(b);
    } else {
      G[a].erase(b);
      G[b].erase(a);
      G[a].insert(k);
      G[b].insert(k);
      G[k].insert(a);
      G[k].insert(b);
    }
    x.insert(k);
    y.erase(k);
    rep(i,n) {
      rmq[i].build(k,dist(k,i));
    }
  }
  int par=-1;
  int now=0;
  vi route{0};
  while(!(par>0&&now==0)) {
    int a = *G[now].begin();
    int b = *G[now].rbegin();
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
