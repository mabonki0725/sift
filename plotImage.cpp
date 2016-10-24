//-----------------------------------------------------------------
#include <iostream>
using namespace std;

//-----------------------------------------------------------------
#include <math.h>
#include "sift.h"

#define M_PI 3.1415926  //m.nakai
//-----------------------------------------------------------------
//SIFT パラメータ
//-----------------------------------------------------------------
const double SIG0   = 1.6;
const int    S      = 3;
const int    MINW   = 20;

int OCT;

//-----------------------------------------------------------------
//キーポイント候補構造体
//-----------------------------------------------------------------
struct KEYPOINT
{
    double x;           //位置x
    double y;           //位置y
    
    int o;              //オクターブ数
    int s;              //スケール
    
    
    //コンストラクタとか
    KEYPOINT(){x=y=0.0; o=s=0;}
    KEYPOINT(double _x,double _y,int _o,int _s)
    {
        x=_x; y=_y; o=_o; s=_s;
    }
    ~KEYPOINT(){}
};
#include <list>

PGM gaussian_filter(PGM &src,double sig);
void down_sampling(PGM  &src);
void diff_pgm(PGM &dst,PGM &a,PGM &b);

//-----------------------------------------------------------------
//2.1 スケールとキーポイント検出
//-----------------------------------------------------------------
void get_keypoints(PGM &src,PGM **L,PGM **DoG,list<KEYPOINT*> &keys)
{
    cout<<"step1:keypoint detection"<<endl;
    
    //増加率
    double k=pow(2,1/(double)S);
    
    //平滑化画像を作成
    for(int o=0;o<OCT;o++){
        cout<<"    octave "<<o<<" : "<<flush;
        
        double sig=SIG0;
        for(int s=0;s<S+3;s++){
            //元画像にガウシアンフィルタ
            L[o][s]=gaussian_filter(src,sig);
            
            sig*=k;
            
            cout<<s<<flush;
        }
        //ダウンサンプリング
        down_sampling(src);
        
        cout<<endl;
    }
    
    //DoG画像を作成
    for(/*int*/ o=0;o<OCT;o++){
        for(int i=0;i<S+2;i++){
            //DoG[o][i]=L[o][i+1]-L[o][i];
            diff_pgm(DoG[o][i],L[o][i+1],L[o][i]);
        }
    }
    
    //同じ位置に既にキーポイントがあるかどうかのフラグ
    bool **flg=new bool*[DoG[0][0].width()];
    flg[0]=new bool[DoG[0][0].width()*DoG[0][0].height()];
    for(int x=1;x<DoG[0][0].width();x++) flg[x]=flg[x-1]+DoG[0][0].height();
    
    //DoG画像から極値検出
    for(/*int*/ o=0;o<OCT;o++){
        int W=DoG[o][0].width();
        int H=DoG[o][0].height();
        
        //フラグ初期化
        for(int x=1;x<W-1;x++){
            for(int y=1;y<H-1;y++) flg[x][y]=false;
        }
        
        for(int s=1;s<S+1;s++){
            for(int x=1;x<W-1;x++){
                for(int y=1;y<H-1;y++){
                    //既出のピクセルはキーポイントにしない
                    if(flg[x][y]) continue;
                    
                    //26近傍を探索
                    bool is_max=true;     //極大値かどうか
                    bool is_mim=true;     //極小値かどうか
                    for(int ds=s-1;ds<=s+1;ds++){
                        for(int dx=x-1;dx<=x+1;dx++){
                            for(int dy=y-1;dy<=y+1;dy++){
                                //中心は無視
                                if(ds==s && dx==x && dy==y) continue;
                                
                                //極大，極小判定
                                if(is_max && DoG[o][s][x][y]<=DoG[o][ds][dx][dy]){
                                    is_max=false;
                                }
                                if(is_mim && DoG[o][s][x][y]>=DoG[o][ds][dx][dy]){
                                    is_mim=false;
                                }
                                
                                //極値でなかったら次
                                if(!is_max && !is_mim) goto next;
                            }
                        }
                    }
                    
                    //キーポイント候補として登録
                    KEYPOINT *tmp=new KEYPOINT((double)x,(double)y,o,s);
                    keys.push_back(tmp);    //キーポイントリストに追加
                    flg[x][y]=true;         //フラグを設定
                    
                    next:;
                }
            }
        }
    }
    
    cout<<"    number of keypoints = "<<keys.size()<<endl;
    cout<<endl;
    
    delete[] flg[0];
    delete[] flg;
}

