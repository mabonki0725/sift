//-----------------------------------------------------------------
#ifndef			__SIFT_H
#define			__SIFT_H

#include <list>
#include "pgm.h"

#define M_PI 3.1415926  //m.nakai
//-----------------------------------------------------------------
//SIFT パラメータ
//-----------------------------------------------------------------
const double SIG0   = 1.6;
const int    S      = 3;
const int    MINW   = 20;

//-----------------------------------------------------------------
//特徴量記述子構造体
//-----------------------------------------------------------------
struct DESCRIPTOR
{
    double x;       //位置ｘ
    double y;       //位置ｙ
    double sig;     //分散σ
    double arg;     //オリエンテーション
    
    double v[128];  //SIFT特徴量
    
    DESCRIPTOR(){x=y=0.0; sig=arg=0.0;}
    DESCRIPTOR(double _x,double _y,double _sig,double _arg)
    {
        x=_x; y=_y; sig=_sig; arg=_arg;
    }
    ~DESCRIPTOR(){}
};

//m.nakai
#define KEYPOINT_BIN 36

//-----------------------------------------------------------------
//キーポイント候補構造体
//-----------------------------------------------------------------
struct KEYPOINT
{
    double x;           //位置x
    double y;           //位置y
    
    int o;              //オクターブ数
    int s;              //スケール
    
    double hst[KEYPOINT_BIN];   //ヒストグラム
    
    //コンストラクタとか
    KEYPOINT(){x=y=0.0; o=s=0;}
    KEYPOINT(double _x,double _y,int _o,int _s)
    {
        x=_x; y=_y; o=_o; s=_s;
    }
    ~KEYPOINT(){}
};
//-----------------------------------------------------------------
//SIFT関数のプロトタイプ宣言
//-----------------------------------------------------------------
void SIFT(PGM &src,/*std::*/list<DESCRIPTOR*> keys);

//-----------------------------------------------------------------
#endif