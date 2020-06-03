#include <bits/stdc++.h>
#define fi first
#define se second
#define rep(i,n) for(int i = 0; i < (n); ++i)
#define drep(i,n) for(int i = (n)-1; i >= 0; --i)
#define srep(i,s,t) for (int i = s; i < t; ++i)
#define rng(a) a.begin(),a.end()
#define sz(x) (int)(x).size()
#define uni(x) x.erase(unique(rng(x)),x.end())
#define show(x) cout<<#x<<" = "<<x<<endl;
#define PQ(T) priority_queue<T,v(T),greater<T> >
#define newline puts("")
#define v(T) vector<T>
#define vv(T) v(v(T))
#define chmin(x,y) x=min(x,y)
#define chmax(x,y) x=max(x,y)
#define EPS (1e-10)
#define equals(a, b) (fabs((a)-(b)) < EPS)
using namespace std;
typedef long long int ll;
typedef pair<int,int> P;
typedef set<int> S;
typedef queue<int> Q;
typedef queue<P> QP;
typedef vector<int> vi;
typedef vector<vi> vvi;
typedef vector<ll> vl;
typedef vector<P> vp;
const ll LINF = 1001002003004005006ll;
const int INF = 1001001001;
const int MOD = 1000000007;
  
int main(int argc,char *argv[]) {
  if(argc!=2) {
    printf("Usage %s [tour name]",argv[0]);
    exit(1);
  }
  FILE *fp;
  // open .tsp 
  
  char tsp[100];
  sprintf(tsp,"./lib/%s.tsp",argv[1]);
  if((fp = fopen(tsp,"r")) == NULL) {
    fprintf(stderr, "cannot open file!\n");
    exit(1);
  }
  char buf[1024];
  int dim;
  while(fgets(buf,1024,fp) != NULL) {
    if(strncmp("DIMENSION", buf, 9)==0) {
      char *c1;
      for(c1 = buf + 9; *c1 != ':'; c1++); 
      dim = atoi(c1+1);
      continue;
    }
    if(strncmp("NODE_COORD_SECTION", buf, 18)==0) break;
  }

  vi xs(dim),ys(dim);
  while(fgets(buf,1024,fp) != NULL) {
    if(strncmp("EOF", buf, 3)==0) break;
    char *p = buf;
    int k = strtol(p,&p,10);
    int x = strtol(p,&p,10);
    int y = strtol(p,&p,10);
    xs[k-1]=x;
    ys[k-1]=y;
  }
  fclose(fp);
  
  
  char tour[100];
  sprintf(tour,"./lib/%s.opt.tour",argv[1]);
  if((fp = fopen(tour,"r")) == NULL) {
    fprintf(stderr, "cannot open file!\n");
    exit(1);
  }
  int dim2;
  while(fgets(buf,1024,fp) != NULL) {
    if(strncmp("DIMENSION", buf, 9)==0) {
      char *c1;
      for(c1 = buf + 9; *c1 != ':'; c1++); 
      dim2 = atoi(c1+1);
      continue;
    }
    if(strncmp("TOUR_SECTION", buf, 12)==0) break;
  }
  if(dim!=dim2) {
    printf("dimension(tsp: %d, tour: %d) is invalid!\n",dim,dim2);
    exit(1);
  }
  cout << dim << endl;
  while(fgets(buf,1024,fp) != NULL) {
    if(strncmp("-1", buf, 2)==0) break;
    char *p = buf;
    int k = strtol(p,&p,10);
    printf("%d %d\n",xs[k-1],ys[k-1]);
  }
  return 0;
}