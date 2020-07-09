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

struct RMQ {
  int N;
  vi segtree;
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
  void inc(int idx) {
    int s = sz(segtree) - (N-idx);
    segtree[s]++;
    while(1) {
      if(s == 0) break;
      s = (s-1)/2;
      segtree[s] = min(segtree[s*2+1],segtree[s*2+2]);
    }
  }
  void dec(int idx) {
    int s = sz(segtree) - (N-idx);
    segtree[s]--;
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
  // min(A[i] | 0 <= i < x)
  int query() {
    return _query(0,n,0,0,N);
  }
};

struct UnionFindUndo {
  vi data;
  stack<edge> history;
  UnionFindUndo() {
    data.assign(n, -1);
  }
  bool unite(int x, int y) {
    x = find(x), y = find(y);
    history.emplace(x, data[x]);
    history.emplace(y, data[y]);
    if(x == y) return false;
    if(data[x] > data[y]) swap(x, y);
    data[x] += data[y];
    data[y] = x;
    return true;
  }
  int find(int k) {
    if(data[k] < 0) return (k);
    return find(data[k]);
  }
  int size(int k) {
    return -data[find(k)];
  }
  void undo() {
    data[history.top().first] = history.top().second;
    history.pop();
    data[history.top().first] = history.top().second;
    history.pop();
  }
};

// TrackNeighborsの変更ログ
// ロールバックと遷移に使う
struct TrackNeighborsLog {
  int idx;
  int left_idx ,left_left ,left_right;
  int right_idx,right_left,right_right;
  bool istoggle=false;
  TrackNeighborsLog(int i) : idx(i) {};
};

// 現在requiredにもforbiddenにも含まれない辺のみ連結されている
// 辺はコストの昇順にソートされている
// また最小一木に使用されているかのフラグも含まれている
// 最小一木に含まれていない辺のうち一番最後にあるものを次の遷移で用いる
struct TrackNeighbors {
  vi left;
  vi right;
  const ve val;
  int len;
  // true -> 最小一木に使用,false->最小一木に使用されない
  vector<bool> isused;
  // 現在のrequired
  set<int> required;
  // 現在のused(required以外)
  set<int> used;
  TrackNeighbors(ve nei): val(nei),len(sz(nei)) {
    left.assign(len+2,-1);
    right.assign(len+2,-1);
    isused.assign(len+2,false);
    rep(i,len+1) {
      left[i+1]=i;
      right[i]=i+1;
    }
  };
  void rollback(stack<TrackNeighborsLog> log) {
    while(!log.empty()) {
      auto x(log.top());
      log.pop();
      if(x.istoggle) {
        if(isused[x.idx+1]) {
          used.erase(x.idx);
        } else {
          used.insert(x.idx);
        }
        isused[x.idx+1]=!isused[x.idx+1];
      } else {
        left[x.left_idx+1]=x.left_left;
        right[x.left_idx+1]=x.left_right;
        left[x.right_idx+1]=x.right_left;
        right[x.right_idx+1]=x.right_right;
      }
    }
  }
  void disable(TrackNeighborsLog &log) {
    // prev left 
    log.left_idx=left[log.idx+1]-1;
    log.left_left=left[log.left_idx+1];
    log.left_right=log.idx+1;
    // prev right
    log.right_idx=right[log.idx+1]-1;
    log.right_left=log.idx+1;
    log.right_right=right[log.right_idx+1];
    // now left 
    right[log.left_idx+1]=log.right_idx+1;
    // now right 
    left[log.right_idx+1]=log.left_idx+1;
  }
  void toggleFlag(TrackNeighborsLog &log) {
    log.istoggle=true;
    isused[log.idx+1]=!isused[log.idx+1];
  } 
};

int dist(int i, int j) {
  float xd = city[i][0] - city[j][0];
  float yd = city[i][1] - city[j][1];
  return (int)(sqrt(xd * xd + yd * yd) + .5);
}

int nowlogid=0;

// 「その状態でこれから何をするか」を表している
struct DfsLog {
  int id;
  int idx;
  bool isforbidden;
  int lb;
  DfsLog(int i,bool isf,int _lb): idx(i),isforbidden(isf),lb(_lb) {
    id=nowlogid++;
  }
};

// 「その状態でどのような操作を行ったか」を表している
// 帰りがけ順の処理(rollback)をする
struct StateLog {
  int id;
  int idx;
  stack<TrackNeighborsLog> tnlog;
  bool isforbidden;
  StateLog(DfsLog l):idx(l.idx),isforbidden(l.isforbidden),id(l.id) {};
};

struct State {
  // 各頂点に隣接している辺のうち、禁止されていない本数(入出次数)
  // 最小値が2未満のとき、巡回路を構築するのは不可能
  RMQ availabledims;
  // 状態
  UnionFindUndo uf;
  TrackNeighbors tn;
  // dfsに使うログ
  stack<DfsLog> log;
  // rollbackに使うログ
  stack<StateLog> rlog;
  // 現在のrequiredを結合したときの辺のコスト
  int rq=0;
  // 現在の下界 (これはdfsが深いほど増加する)
  int lb=0;

  State(ve nei): tn(nei) {}
  // 遷移適用
  void apply(DfsLog &k,StateLog &diff) {
    lb=k.lb;
    if(k.isforbidden) {
      availabledims.dec(tn.val[k.idx].first);
      availabledims.dec(tn.val[k.idx].second);
      TrackNeighborsLog x(k.idx);
      tn.disable(x);
      diff.tnlog.push(x);
    } else {
      if(k.idx<0) return;
      TrackNeighborsLog x(k.idx);
      tn.disable(x);
      diff.tnlog.push(x);
      uf.unite(tn.val[k.idx].first,tn.val[k.idx].second);
      tn.required.insert(k.idx);
      rq += dist(tn.val[k.idx].first,tn.val[k.idx].second);
    }
  }
  // 遷移のロールバック
  void rollback(StateLog &k) {
    if(k.isforbidden) {
      availabledims.inc(tn.val[k.idx].first);
      availabledims.inc(tn.val[k.idx].second);
      tn.rollback(k.tnlog);
    } else {
      uf.undo();
      tn.rollback(k.tnlog);
      tn.required.erase(k.idx);
      rq -= dist(tn.val[k.idx].first,tn.val[k.idx].second);
    }
  }
  // 下界を設定
  bool lowerbound(StateLog &diff) {
    int now = sz(uf.history);
    // 最小一木の構築
    int res=rq;
    // 連結に必要ない辺
    int min_unused=INF;
    // 全て連結されるまで
    int rpos=sz(tn.right)-1;
    for(int pos=tn.right[0];pos<rpos;pos=tn.right[pos]) {
      if(uf.find(tn.val[pos-1].first)==uf.find(tn.val[pos-1].second)) {
        if(min_unused<INF) {
          if(tn.isused[pos]) {
            TrackNeighborsLog x(pos-1);
            tn.toggleFlag(x);
            diff.tnlog.push(x);
            tn.used.erase(pos-1);
          }
          continue;
        }
        chmin(min_unused,pos);
        res += dist(tn.val[min_unused-1].first,tn.val[min_unused-1].second);
        if(!tn.isused[pos]) {
          TrackNeighborsLog x(pos-1);
          tn.toggleFlag(x);
          diff.tnlog.push(x);
          tn.used.insert(pos-1);
        }
      } else {
        uf.unite(tn.val[pos-1].first,tn.val[pos-1].second);
        res += dist(tn.val[pos-1].first,tn.val[pos-1].second);
        if(!tn.isused[pos]) {
          TrackNeighborsLog x(pos-1);
          tn.toggleFlag(x);
          diff.tnlog.push(x);
          tn.used.insert(pos-1);
        }
      }
    }
    if(min_unused==INF) {
      // 復元
      while(sz(uf.history)>now) {
        uf.undo();
      }
      lb=INF;
      return false;
    }
    // 構築不可能
    if(uf.size(0)!=n||sz(tn.used)+sz(tn.required)!=n) {
      lb=INF;
      while(sz(uf.history)>now) {
        uf.undo();
      }
      return false;
    }
    bool isvalid=isvalidroad();
    lb=res;
    // 復元
    while(sz(uf.history)>now) {
      uf.undo();
    }
    return isvalid;
  }
  
  int selectedge() {
    for(int pos=tn.left[sz(tn.left)-1];pos>0;pos=tn.left[pos]) {
      if(!tn.isused[pos]) return pos-1;
    }
    return -INF;
  }
  // 復元をする前に実行する
  bool isvalidroad() {
    // 1. 全ての頂点が連結
    if(uf.size(0)!=n) return false;
    // 2. 辺がn本
    if(sz(tn.required)+sz(tn.used)!=n) return false;
    // 3. 全ての頂点の入出次数が2以上残している
    if(availabledims.query()<2) return false;
    // 4. 全ての入出次数が2(O(NlogN)、遅いのでなるべく実行しないか書き換える)
    vi cnt(n,0);
    for(int p:tn.required) {
      cnt[tn.val[p].first]++;
      cnt[tn.val[p].second]++;
    }
    for(int p:tn.used) {
      cnt[tn.val[p].first]++;
      cnt[tn.val[p].second]++;
    }
    rep(i,n) {
      if(cnt[i]!=2) return false;
    }
    return true;
  }
  // tour更新
  void snapshot() {
    vvi G(n);
    for(int p:tn.required) {
      G[tn.val[p].first].push_back(tn.val[p].second);
      G[tn.val[p].second].push_back(tn.val[p].first);
    }
    for(int p:tn.used) {
      G[tn.val[p].first].push_back(tn.val[p].second);
      G[tn.val[p].second].push_back(tn.val[p].first);
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
    rep(i,n) {
      tour[i]=route[i];
    }
  }
  // 各状態で
  // 1. 巡回路を作成不可能/上界更新不可能なら探索を打ち切る
  // 2. validな巡回路ならば上界更新
  // 3. 次の状態をpush
  // をO(n)かけずにやりたい
  void exec() {
    // rollback state
    int now = log.top().id;
    while(!rlog.empty()&&rlog.top().id>now) {
      rollback(rlog.top());
      rlog.pop();
    }
    StateLog diff(log.top());
    // 状態遷移　
    apply(log.top(),diff);
    log.pop();
    // 巡回路を作成不可能
    if(availabledims.query()<2) {
      rlog.push(diff);
      return;
    }
    // 禁止遷移であれば(未使用辺を選んでいるので)、下界に変化はない
    if(!diff.isforbidden) {
      bool isvalid = lowerbound(diff);
      // 上界更新不可能
      if(lb>=length) {
        //printf("上界更新不可能\n");
        rlog.push(diff);
        return;
      }
      if(isvalid) {
        // 上界(ベストスコア)更新
        if(lb<length) {
          printf("[%d] update upperbound: %d -> %d\n",clock(),length,lb);
          length=lb;
          snapshot();
        }
        rlog.push(diff);
        return;
      }
    }
    int k = selectedge();
    if(k<0) {
      rlog.push(diff);
      return;
    }
    // 使用遷移
    // R' = R \cup k
    // F' = F
    log.emplace(k,false,lb);
    // 禁止遷移
    // R' = R
    // F' = F \cup k
    log.emplace(k,true,lb);
    // ロールバック用log
    rlog.push(diff);
  }
};

State build() {
  length = INF;
  rep(i,n) {
    point p(city[i][0],city[i][1]);
    rtree.insert(make_pair(p,i));
  }
  ve neighbor;
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
  State state(neighbor);
  state.log.emplace(-INF,false,0);
  rep(i,n) {
    state.availabledims.build(i,0);
  }
  for(edge e:neighbor) {
    state.availabledims.inc(e.first);
    state.availabledims.inc(e.second);
  }
  return state;
}

int tspSolver() {
  State state = build();
  while(!state.log.empty()) {
    state.exec();
  }
  return 1;
}