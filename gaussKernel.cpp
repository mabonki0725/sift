//-----------------------------------------------------------------
#include <iostream>
using namespace std;

//-----------------------------------------------------------------
#include <math.h>
#include "sift.h"
#include "protype.h"


extern int OCT;





//-----------------------------------------------------------------
//画像の差分
//-----------------------------------------------------------------
void diff_pgm(PGM &dst,PGM &a,PGM &b)
{
    int W=(a.width()  < b.width() ) ? a.width()  : b.width();
    int H=(a.height() < b.height()) ? a.height() : b.height();

    dst.reset(W,H);
    for(int x=0;x<W;x++) {
        for(int y=0;y<H;y++) {
            dst[x][y] = a[x][y] - b[x][y];
        }
    }
}
//m.nakai
//-----------------------------------------------------------------
//ガウシアンフィルタをかける
//-----------------------------------------------------------------
PGM gaussian_filter(PGM &src,double sig)
{
    int W=src.width();
    int H=src.height();
    
    //結果画像
    PGM tmp(W,H);
    PGM dst(W,H);
    
    int Wm=int(ceil(3.0*sig)*2+1);  //窓幅
    int Rm=(Wm-1)/2;                //窓半径
    
    //フィルタ
    double *msk=new double[Wm];
    
    sig=2*sig*sig;
    double div=sqrt(sig*M_PI);
    
    //フィルタの作成
    for(int x=0;x<Wm;x++){
        int p=(x-Rm)*(x-Rm);
        msk[x]=exp(-p/sig)/div;
    }
    
    //垂直方向
    for(/*int*/ x=0;x<W;x++){
        for(int y=0;y<H;y++){
            double sum=0;
            for(int i=0;i<Wm;i++){
                int p=y+i-Rm;
                sum+=msk[i]*src(x,p);
            }
            tmp[x][y]=sum;
        }
    }
    //水平方向
    for(/*int*/ x=0;x<W;x++){
        for(int y=0;y<H;y++){
            double sum=0;
            for(int i=0;i<Wm;i++){
                int p=x+i-Rm;
                sum+=msk[i]*tmp(p,y);
            }
            dst[x][y]=sum;
        }
    }
    delete[] msk;
    
    return dst;
}
//-----------------------------------------------------------------
//幅と高さをそれぞれ1/2にダウンサンプリング
//-----------------------------------------------------------------
void down_sampling(PGM  &src)
{
    int W=src.width() /2;
    int H=src.height()/2;
    PGM tmp(src);
    
    src.reset(W,H);
    
    for(int x=0;x<W;x++){
        for(int y=0;y<H;y++){
            src.pixels[x][y]=tmp.pixels[2*x][2*y];
        }
    }
}
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
                                //m.nakai
                                //if(!is_max && !is_mim) goto next;
                                if(!is_max && !is_mim) break;
                            }
                            if(!is_max && !is_mim) break;
                        }
                        if(!is_max && !is_mim) break;
                        //m.nakai
                    }
                    //m.nakai
                    if(!is_max && !is_mim);
                    else {
                      //キーポイント候補として登録
                      KEYPOINT *tmp=new KEYPOINT((double)x,(double)y,o,s);
                      keys.push_back(tmp);    //キーポイントリストに追加
                      flg[x][y]=true;         //フラグを設定
                    }
//next:;
                }
            }
        }
    }
    
    cout<<"    number of keypoints = "<<keys.size()<<endl;
    cout<<endl;
    
    delete[] flg[0];
    delete[] flg;
}
