//-----------------------------------------------------------------
//The SIFT
//-----------------------------------------------------------------
void SIFT(PGM &_src)
{
    PGM src(_src);
    
    //--オクターブ数を決定(log2とか使った方がカッコ良いけどね)
    int W=(src.width()<src.height()) ? src.width():src.height();
    for(OCT=0;W>MINW;OCT++,W/=2);
    
    //--バッファ確保
    PGM **L=new PGM*[OCT];                      //平滑化画像用バッファ
    for(int o=0;o<OCT;o++) L[o]=new PGM[S+3];
    
    PGM **DoG=new PGM*[OCT];                    //DoG画像用バッファ
    for(int o=0;o<OCT;o++) DoG[o]=new PGM[S+2];
    
    PGM **Fpow=new PGM*[OCT];                   //(勾配強度)
    PGM **Farg=new PGM*[OCT];                   //(勾配方向)
    for(int o=0;o<OCT;o++){
        Fpow[o]=new PGM[S];
        Farg[o]=new PGM[S];
    }
    
    list<KEYPOINT*> keys;                       //キーポイント候補リスト
    
    
    //detection
    get_keypoints(src,L,DoG,keys);             //キーポイント候補の検出
    localize_keypoints(DoG,keys);              //キーポイントのローカライズ
    
    //  -description
    calc_orientation(L,Fpow,Farg,keys);             //特徴量の記述
    
    plot_image(_src,keys);
    
    //--バッファ解放
    for(int o=0;o<OCT;o++){
        delete[] L[o];
        delete[] DoG[o];
        delete[] Fpow[o];
        delete[] Farg[o];
    }
    delete[] L;
    delete[] DoG;
    delete[] Fpow;
    delete[] Farg;
}
//-----------------------------------------------------------------
//-----------------------------------------------------------------
//オリエンテーションの算出
//-----------------------------------------------------------------
void calc_orientation(PGM **L,PGM **Fpow,PGM **Farg,list<KEYPOINT*> &keys)
{
    cout<<"step3:"<<endl;
    
    //各平滑化画像の勾配情報を計算
    for(int o=0;o<OCT;o++){
        for(int s=0;s<S;s++){
            int W=L[o][s+1].width();
            int H=L[o][s+1].height();
            
            Fpow[o][s].reset(W,H);
            Farg[o][s].reset(W,H);
            
            for(int u=0;u<W;u++){
                for(int v=0;v<H;v++){
                    double fu=L[o][s+1](u+1,v)-L[o][s+1](u-1,v);
                    double fv=L[o][s+1](u,v+1)-L[o][s+1](u,v-1);
                    
                    Fpow[o][s][u][v]=sqrt(fu*fu+fv*fv);
                    Farg[o][s][u][v]=(atan2(fv,fu)/M_PI+1)/2;
                }
            }
        }
    }
    
    cout<<endl;
}