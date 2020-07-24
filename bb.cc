#include <bits/stdc++.h>
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
typedef long long int ll;
typedef pair<int,int> edge;
typedef vector<set<int>> vs;
typedef vector<edge> ve;
const int INF = 1001001001;

#define DEBUG 0

extern int n, length, tour[MAX];
extern float city[MAX][2];
extern void showTour(int *tou, int wai, int color);
extern void showCTour(int *tou, int wai, int *color);
extern void showString(char *str);
extern void showLength(int leng);

struct RMQUndo {
  int N;
  vi segtree;
  stack<vi> history;
  RMQUndo() {
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
  int at(int idx) {
    int s = sz(segtree) - (N-idx);
    return segtree[s];
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
  // min(A[i] | 0 <= i < n)
  int query() {
    return _query(0,n,0,0,N);
  }
  void snapshot() {
    history.push(segtree);
  }
  // O(N)かかるのでなるべく使わない
  // snapshot()時の状態に戻る
  void undo() {
    segtree=history.top();
    history.pop();
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
  // true -> 最小一木に使用,false -> 最小一木に使用されない
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

ll nowlogid=0;

// 「その状態でこれから何をするか」を表している
struct DfsLog {
  ll id;
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
  ll id;
  int idx;
  stack<TrackNeighborsLog> tnlog;
  bool isforbidden;
  StateLog(DfsLog l):idx(l.idx),isforbidden(l.isforbidden),id(l.id) {};
};

// 最小TARGET木
// 頂点TARGETに接続してある辺を一つ取り除いても、最小全域木になる
// 近傍の点が離れているほど実行時間がかかる？
int TARGET=0;

struct State {
  // 各頂点に隣接している辺のうち、禁止されていない本数(入出次数)
  // 最小値が2未満のとき、巡回路を構築するのは不可能
  RMQUndo availabledims;
  // 各頂点に隣接している辺のうち、requiredな本数(入出次数)
  // 最大値が3以上のとき、巡回路を構築するのは不可能
  RMQUndo requireddims;
  // 状態
  UnionFindUndo uf;
  TrackNeighbors tn;
  // ufから頂点TARGETを端点とした辺を除いたもの
  // lowerboundv2で使う
  UnionFindUndo uf2;
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
      int a = tn.val[k.idx].first;
      int b = tn.val[k.idx].second;
      uf.unite(a,b);
      if(a!=TARGET&&b!=TARGET) {
        uf2.unite(a,b);
      }
      tn.required.insert(k.idx);
      rq += dist(a,b);
      requireddims.inc(a);
      requireddims.inc(b);
    }
  }
  // 遷移のロールバック
  void rollback(StateLog &k) {
    if(k.isforbidden) {
      availabledims.inc(tn.val[k.idx].first);
      availabledims.inc(tn.val[k.idx].second);
      tn.rollback(k.tnlog);
    } else {
      tn.rollback(k.tnlog);
      tn.required.erase(k.idx);
      int a = tn.val[k.idx].first;
      int b = tn.val[k.idx].second;
      uf.undo();
      if(a!=TARGET&&b!=TARGET) {
        uf2.undo();
      }
      rq -= dist(a,b);
      requireddims.dec(a);
      requireddims.dec(b);
    }
  }
  // どうにかして頂点TARGETにつながる辺を取り除くと最小全域木になるように構築
  // 頂点TARGETにつながる辺を2つとも取り除き、TARGET以外の頂点の最小全域木を構築するようにする
  bool lowerboundv2(StateLog &diff) {
    int res=rq;
    int now = sz(uf.history);
    int now2 = sz(uf2.history);
    int rpos=sz(tn.right)-1;
    for(int pos=tn.right[0];pos<rpos;pos=tn.right[pos]) {
      int a =tn.val[pos-1].first;
      int b =tn.val[pos-1].second;
      if(a==TARGET||b==TARGET) {
        if(tn.isused[pos]) {
          TrackNeighborsLog x(pos-1);
          tn.toggleFlag(x);
          diff.tnlog.push(x);
          tn.used.erase(pos-1);
        }
        continue;
      }
      if(uf2.find(a)==uf2.find(b)) {
        if(tn.isused[pos]) {
          TrackNeighborsLog x(pos-1);
          tn.toggleFlag(x);
          diff.tnlog.push(x);
          tn.used.erase(pos-1);
        }
        continue;
      }
      uf.unite(a,b);
      uf2.unite(a,b);
      res += dist(a,b);
      requireddims.inc(a);
      requireddims.inc(b);
      if(!tn.isused[pos]) {
        TrackNeighborsLog x(pos-1);
        tn.toggleFlag(x);
        diff.tnlog.push(x);
        tn.used.insert(pos-1);
      }
    }
    // 構築不可能
    if(uf2.size((TARGET+1)%n)!=n-1||sz(tn.used)+sz(tn.required)!=n) {
      lb=INF;
      // 復元
      while(sz(uf.history)>now) {
        uf.undo();
      }
      while(sz(uf2.history)>now2) {
        uf2.undo();
      }
      lb=INF;
      requireddims.undo();
      return false;
    }
    bool isvalid=isvalidroad();
    lb=res;
    // 復元
    while(sz(uf.history)>now) {
      uf.undo();
    }
    while(sz(uf2.history)>now2) {
      uf2.undo();
    }
    requireddims.undo();
    return isvalid;
  }
  // 下界を設定
  bool lowerbound(StateLog &diff) {
    int now = sz(uf.history);
    requireddims.snapshot();
    // 最小一木の構築
    int res=rq;
    // 頂点TARGETに連結している辺の数
    int dim=requireddims.at(TARGET);
    // 2のとき、0をのぞく頂点の最小全域木を構築
    if(dim==2) {
      return lowerboundv2(diff);
    }
    // 最小一木の最後につけたす辺
    int tpos=INF;
    // 0より大きい時、頂点TARGETを端点とする辺は最小全域木につかわない
    // 全て連結されるまで
    int rpos=sz(tn.right)-1;
    for(int pos=tn.right[0];pos<rpos;pos=tn.right[pos]) {
      int a = tn.val[pos-1].first;
      int b = tn.val[pos-1].second;
      if(uf.find(a)==uf.find(b)) {
        if(tn.isused[pos]) {
          TrackNeighborsLog x(pos-1);
          tn.toggleFlag(x);
          diff.tnlog.push(x);
          tn.used.erase(pos-1);
        }
        if(a==TARGET||b==TARGET) {
          chmin(tpos,pos);
        }
        continue;
      }
      if(a==TARGET||b==TARGET) {
        if(dim>=1) {
          if(tn.isused[pos]) {
            TrackNeighborsLog x(pos-1);
            tn.toggleFlag(x);
            diff.tnlog.push(x);
            tn.used.erase(pos-1);
          }
          chmin(tpos,pos);
          continue;
        }
        dim++;
      }
      uf.unite(a,b);
      res += dist(a,b);
      requireddims.inc(a);
      requireddims.inc(b);
      if(!tn.isused[pos]) {
        TrackNeighborsLog x(pos-1);
        tn.toggleFlag(x);
        diff.tnlog.push(x);
        tn.used.insert(pos-1);
      }
    }
    if(tpos<INF&&dim<2) {
      int a = tn.val[tpos-1].first;
      int b = tn.val[tpos-1].second;
      res += dist(a,b);
      requireddims.inc(a);
      requireddims.inc(b);
      dim++;
      TrackNeighborsLog x(tpos-1);
      tn.toggleFlag(x);
      diff.tnlog.push(x);
      tn.used.insert(tpos-1);
    }
    if(dim!=2) {
      // 復元
      while(sz(uf.history)>now) {
        uf.undo();
      }
      lb=INF;
      requireddims.undo();
      return false;
    }
    // 構築不可能
    if(uf.size(0)!=n||sz(tn.used)+sz(tn.required)!=n) {
      lb=INF;
      while(sz(uf.history)>now) {
        uf.undo();
      }
      requireddims.undo();
      return false;
    }
    bool isvalid=isvalidroad();
    lb=res;
    // 復元
    while(sz(uf.history)>now) {
      uf.undo();
    }
    requireddims.undo();
    return isvalid;
  }
  // この辺を禁止した場合構築できなくなるとき、禁止不可能
  bool canForbidden(int k) {
    int a = tn.val[k].first;
    int b = tn.val[k].second;
    if(availabledims.at(a)==2) return false;
    if(availabledims.at(b)==2) return false;
    return true;
  }
  // この辺が接続する頂点の次数が既に2だったとき、使用不可能
  // この辺によって巡回路にならない閉路が出来るとき、使用不可能
  bool canUse(int k) {
    int a = tn.val[k].first;
    int b = tn.val[k].second;
    if(requireddims.at(a)>=2) return false;
    if(requireddims.at(b)>=2) return false;
    if(uf.find(a)==uf.find(b)&&uf.size(a)!=n) return false;
    return true;
  }
  int selectedge() {
    // 禁止遷移のみ可能
    int fpos=-INF;
    // 使用遷移かつ禁止遷移が可能
    int avapos=-INF;
    // 1. 使用遷移のみ可能な辺を返す
    // 2. 禁止遷移のみ可能な辺を返す
    // 3. 遷移可能な辺を返す
    // 各優先順位で複数の辺があった場合最もコストが大きいものを優先する
    for(int pos=tn.left.back();pos>0;pos=tn.left[pos]) {
      if(tn.isused[pos]) continue;
      bool f = canForbidden(pos-1);
      bool u = canUse(pos-1);
      if(!f&&u) {
        return pos-1;
      }
      if(f&&!u) {
        chmax(fpos,pos-1);
      }
      chmax(avapos,pos-1);
    }
    if(fpos>=0) {
      return fpos;
    }
    if(avapos>=0) {
      return avapos;
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
    // 4. 全ての入出次数が2
    return requireddims.query()==2;
  }
  // 使用遷移
  // R' = R \cup k
  // F' = F
  void pushS1(int k) {
    if(!canUse(k)) return;
    log.emplace(k,false,lb);
  }
  // 禁止遷移
  // R' = R
  // F' = F \cup k
  void pushS2(int k) {
    if(!canForbidden(k)) return;
    log.emplace(k,true,lb);
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
    ll now = log.top().id;
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
          if(DEBUG) {
            printf("[%d] update upperbound: %d -> %d\n",clock(),length,lb);
          }
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
    pushS1(k);
    pushS2(k);
    // ロールバック用log
    rlog.push(diff);
  }
};

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
  length=init_length;
  int imp=0;
  for(int i=0,cnt=0;cnt<steps;i=(i+1)%n) {
    int a = tour[i];
    int b = tour[(i+1)%n];
    rep(k,n) {
      int c = tour[k];
      if(b==c) continue;
      if(cnt>=steps) break;
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
  init_length=length;
}

void sa() {
  for(double t=tmax;t>tmin;t*=delta) {
    for(int i=0,step=0;step<steps;i=(i+1)%n) {
      int a = tour[i];
      int b = tour[(i+1)%n];
      rep(k,n) {
        int c = tour[k];
        if(b==c) continue;
        if(step>=steps) break;
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

void buildFI() {
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
  vector<RMQUndo> rmq(n);
  rep(i,n) {
    rmq[i].build(0,dist(0,i));
  }
  while(sz(x)<n) {
    int k=-INF,kr=-INF;
    for(int i:y) {
      int r = rmq[i].query();
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
}

// なるべくいい上界をきめる
// FI / SA をそのまま使う
void setupperbound() {
  buildFI();
  snapshot();
  setParam();
  sa();
  length = bestlen;
  rep(i,n) {
    tour[i]=bestlog[i];
  }
}

State build() {
  ve neighbore;
  rep(i,n) {
    srep(j,i+1,n) {
      neighbore.emplace_back(i,j);
    }
  }
  sort(rng(neighbore),[&](edge a,edge b) {
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
  uni(neighbore);
  State state(neighbore);
  state.log.emplace(-INF,false,0);
  rep(i,n) {
    state.availabledims.build(i,n-1);
    state.requireddims.build(i,0);
  }
  return state;
}


int tspSolver() {
  State state = build();
  setupperbound(); 
  cerr << "init length = " << length << endl;
  while(!state.log.empty()) {
    state.exec();
  }
  return 1;
}