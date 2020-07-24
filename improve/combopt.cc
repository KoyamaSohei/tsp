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

// before: a -> b, c -> t -> d 
// after: a -> t > b, c -> d
void shift(int a,int b,int t) {
  queue<int> que;
  rep(i,n) {
    if(tour[i]==t) continue;
    que.push(tour[i]);
    if(tour[i]==a) {
      que.push(t);
    }
  }
  rep(i,n) {
    tour[i]=que.front();
    pos[tour[i]]=i;
    que.pop();
  }
}

bool combopt() {
  bool updated=false;
  rep(i,n) {
    int a = tour[i];
    int b = tour[(i+1)%n];
    for(int c:neighbor[a]) {
      if(b==c) continue;
      // 2opt
      int k = pos[c];
      int d = tour[(k+1)%n];
      if(b==d||a==d) continue;
      int tmp = dist(a,b)+dist(c,d)-dist(a,c)-dist(b,d);
      if(tmp>0) {
        flip(i,(i+1)%n,k,(k+1)%n);
        length-=tmp;
        updated=true;
        break;
      }
      // 1.5opt
      int e = tour[(i+2)%n];
      if(e==c||e==d) continue;
      tmp = dist(a,b)+dist(b,e)+dist(c,d)-(dist(a,e)+dist(b,c)+dist(b,d));
      if(tmp>0) {
        shift(c,d,b);
        length-=tmp;
        updated=true;
        break;
      }
    }
  }
  return updated;
}

void run() {
  while(combopt());
}