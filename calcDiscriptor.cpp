//-----------------------------------------------------------------
#include <iostream>
using namespace std;

//-----------------------------------------------------------------
#include <math.h>
#include "sift.h"
#include "protype.h"

extern int OCT;


//add m.nakai
double max_element(double *array,int size) {
    int i;
    double dmax;

    dmax=array[0];
    for(i=1;i<size;i++) {
        if(dmax < array[i]) dmax = array[i];
    }
    return(dmax);
}
//m.nakai
//-----------------------------------------------------------------
//特徴量の記述
//-----------------------------------------------------------------
void calc_descriptor(PGM **L,PGM **Fpow,PGM **Farg,list<KEYPOINT*> &keys,list<DESCRIPTOR*> &des)
{
    cout<<"step4:"<<endl;
    
    for(list<KEYPOINT*>::iterator it=keys.begin();it!=keys.end();it++){
        //キーポイントの情報
        int o=(*it)->o;         //オクターブ
        int s=(*it)->s-1;       //σ．．．ではないけど便宜上
        int x=(int)((*it)->x);  //位置ｘ
        int y=(int)((*it)->y);  //位置ｙ
        
        //ヒストグラムの最大値
        //double max_h=*max_element((*it)->hst,(*it)->hst+KEYPOINT_BIN);
        double max_h = max_element((*it)->hst,KEYPOINT_BIN);

        //の80%
        max_h*=0.8;
        
        //キーポイントへの割り当て判定
        for(int bin=0;bin<KEYPOINT_BIN;bin++){
            //最大値の80%以上か？
            if((*it)->hst[bin] >= max_h){
                //両隣のインデックス
                int bm=bin-1;
                int bp=bin+1;
                
                //範囲チェック
                if(bm<0)             bm=KEYPOINT_BIN-1;
                if(bp>=KEYPOINT_BIN) bp=0;
                
                //ピークか？
                if( (*it)->hst[bin]>(*it)->hst[bm] && (*it)->hst[bin]>(*it)->hst[bp] ){
                    double sig=pow(2,(s+1)/(double)S)*SIG0;
                    
                    //ピークなら記述子に採用
                    DESCRIPTOR *tmp=new DESCRIPTOR(x,y,sig,bin/(double)KEYPOINT_BIN);
                    
                    //特徴量を生成
                    make_descriptor(tmp,Fpow[o][s],Farg[o][s]);
                    
                    //キーポイントのスケールに合わせて座標等を変更（表示用）
                    double dlt=pow(2,o);
                    tmp->x  =dlt*x;
                    tmp->y  =dlt*y;
                    tmp->sig=dlt*pow(2,(s+1)/(double)S)*SIG0;
                    tmp->arg=(bin/(double)KEYPOINT_BIN-0.5)*2*M_PI;
                    
                    //記述子リストに追加
                    des.push_back(tmp);
                }
            }
        }
    }
    
    cout<<"    number of keypoints = "<<des.size()<<endl;
    cout<<endl;
}
//-----------------------------------------------------------------
//特徴量の生成
//-----------------------------------------------------------------
void make_descriptor(DESCRIPTOR *des,PGM &Fpow,PGM &Farg)
{
    int    x0  =int(des->x);
    int    y0  =int(des->y);
    double sig0=des->sig;
    double arg0=des->arg;
    
    //回転行列のあれを事前に計算
    double cos0=cos((arg0-0.5)*2*M_PI);
    double sin0=sin((arg0-0.5)*2*M_PI);
    
    int    Wm=int(ceil(3.0*sig0)*2+1);    //窓の幅
    int    Rm=(Wm -1)/2;                  //窓の半径
    int    W =Fpow.width();               //画像の幅
    int    H =Fpow.height();              //画像の高さ
    
    //ヒストグラムを初期化
    for(int bin=0;bin<128;bin++) des->v[bin]=0;
    
    for(int i=x0-Rm;i<=x0+Rm;i++){
        int mx=i-x0+Rm;         //マスクの座標と画像の座標とをあわせるよ(x)
        int bx=4*mx/Wm;         //マスク座標をビンの座標に変換
        
        for(int j=y0-Rm;j<=y0+Rm;j++){
            int my=j-y0+Rm;     //マスクの座標と画像の座標とをあわせるよ(y)
            int by=4*my/Wm;     //マスク座標をビンの座標に変換
            
            //座標を回転
            int px=x0+int(cos0*(i-x0)-sin0*(j-y0));
            int py=y0+int(sin0*(i-x0)+cos0*(j-y0));
            
            //勾配方向を回転
            double arg=Farg(px,py)-arg0;
            if(arg<0)       arg+=1;
            else if(arg>=1) arg-=1;
            
            //ビンの番号
            int bin=(int)(floor(8*arg)) + bx*8 + by*4*8;
            
            //ヒストグラム
            des->v[bin]+=Fpow(px,py);               //ホントはガウシアン掛けないといけないはず
            //des->v[bin]+=G[mx][my]*Fpow(px,py);   //こんな感じ？よ～わからんので要調査
        }
    }
    
    //ヒストグラムを正規化
    double sum=0;
    for(/*int*/ bin=0;bin<128;bin++) sum+=(des->v[bin]*des->v[bin]);
    sum=sqrt(sum);
    for(/*int*/ bin=0;bin<128;bin++) des->v[bin]/=sum;
}