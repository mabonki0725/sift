//-----------------------------------------------------------------
//SIFT パラメータ
//-----------------------------------------------------------------
const double SIG0   = 1.6;
const int    S      = 3;
const int    MINW   = 20;

int OCT;

const double Rth    = 10.0;
const double TH_EDG = (Rth+1)*(Rth+1)/Rth;
//-----------------------------------------------------------------
//2.2 キーポイントのローカライズ
//-----------------------------------------------------------------
void localize_keypoints(PGM **DoG,list<KEYPOINT*> &keys)
{
    cout<<"step2:localize"<<endl;
    
    
    for(list<KEYPOINT*>::iterator it=keys.begin();it!=keys.end();){
        //<2.2.1 主曲率による絞り込み>
        int o=(*it)->o;
        int s=(*it)->s;
        int x=(int)((*it)->x);
        int y=(int)((*it)->y);
        
        //ヘッセ行列を求める
        double Dxx=( DoG[o][s](x-2,y)+DoG[o][s](x+2,y)-2*DoG[o][s](x,y) );
        double Dyy=( DoG[o][s](x,y-2)+DoG[o][s](x,y+2)-2*DoG[o][s](x,y) );

        //09/10/15 訂正．間違ってました，ごめんなさい．
        double Dxy=( DoG[o][s](x-1,y-1)-DoG[o][s](x-1,y+1)
                    -DoG[o][s](x+1,y-1)+DoG[o][s](x+1,y+1) );
        //訂正前
        //double Dxy=( DoG[o][s](x-1,y-1)-DoG[o][s](x-1,y+1)
        //            -DoG[o][s](x+1,y-1)-DoG[o][s](x+1,y+1) );
        
        //トレースと行列式
        double trc=Dxx+Dyy;
        double det=Dxx*Dyy-Dxy*Dxy;
        
        //削除判定
        if(trc*trc/det >= TH_EDG){
            //キーポイントの削除
            delete (*it);
            it=keys.erase(it);
            
            //削除したら以下の処理はスキップ
            continue;
        }
        else it++;       //削除しないとき次のキーポイント
    }
    
    cout<<"    number of keypoints = "<<keys.size()<<endl;
    cout<<endl;    
}