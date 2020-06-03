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
const int INF = 1001001001;

typedef pair<double,double>  pd;

int main() {
  int N;
  cin >> N;
  vector<pd> pos(N);
  double xmin=INF,xmax=-INF,ymin=INF,ymax=-INF;
  rep(i,N) {
    cin >> pos[i].first >> pos[i].second;
    chmin(xmin,pos[i].first);
    chmax(xmax,pos[i].first);
    chmin(ymin,pos[i].second);
    chmax(ymax,pos[i].second);
  }
  int score = 0;
  double pad = max(xmax-xmin,ymax-ymin)*0.1;
  printf("<svg viewBox=\"%f %f %f %f\" xmlns=\"http://www.w3.org/2000/svg\">\n",xmin-pad,ymin-pad,(xmax-xmin)+2*pad,(ymax-ymin)+2*pad);
  rep(i,N) {
    double x1=pos[i].first;
    double y1=pos[i].second;
    double x2=pos[(i+1)%N].first;
    double y2=pos[(i+1)%N].second;
    double xd = x2-x1;
    double yd = y2-y1;
    score += (int)(sqrt(xd * xd + yd * yd) + .5);
    printf("  <line x1=\"%f\" y1=\"%f\" x2=\"%f\" y2=\"%f\" stroke=\"black\" />\n",x1,y1,x2,y2);
  }
  double r = min(xmax-xmin,ymax-ymin)/100;
  rep(i,N) {
    double x1=pos[i].first;
    double y1=pos[i].second;
    printf("  <circle cx=\"%f\" cy=\"%f\" r=\"%f\"/>\n",x1,y1,r);
  }
  printf("  <text x=\"0\" y=\"0\" font-size=\"%f\">%d</text>\n",pad*0.5,score);
  printf("</svg>\n");
}