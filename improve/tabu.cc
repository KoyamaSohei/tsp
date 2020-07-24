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
extern int pos[MAX];

extern int bestlen;
extern vi bestlog;

extern void snapshot();

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

const int TL=20;

vi lifetime;

void tabu() {
  rep(i,n) {
    int a = tour[i];
    if(lifetime[a]) {
      lifetime[a]--;
      continue;
    }
    int b = tour[(i+1)%n];
    int score=-INF,pi=-INF;
    for(int c:neighbor[a]) {
      if(b==c) continue;
      if(lifetime[c]) continue;
      int ci = pos[c];
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

void run() {
  snapshot();
  lifetime.assign(n,0);
  int cnt = 10*log(n);
  rep(tryi,cnt) {
    tabu();
    snapshot();
  }
  length = bestlen;
  rep(i,n) {
    tour[i]=bestlog[i];
  }
}
