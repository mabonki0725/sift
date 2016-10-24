//-----------------------------------------------------------------
#include <iostream>
#include <math.h>
using namespace std;

#include "sift.h"
#include "protype.h"

//-----------------------------------------------------------------


extern int OCT;

const double Rth    = 10;
const double TH_EDG = (Rth+1)*(Rth+1)/Rth;
const double TH_POW = 0.03;



//-----------------------------------------------------------------
//2.2 �L�[�|�C���g�̃��[�J���C�Y
//-----------------------------------------------------------------
void localize_keypoints(PGM **DoG,list<KEYPOINT*> &keys)
{
    cout<<"step2:localize"<<endl;
    
    double mD[3][3];    //���֐��s��
    double iD[3][3];    //�̋t�s��p
    double xD[3];       //�L�[�|�C���g�ʒu
    double X[3];        //�T�u�s�N�Z���ʒu
    
    for(list<KEYPOINT*>::iterator it=keys.begin();it!=keys.end();){
        //<�G�b�W��̃L�[�|�C���g���폜>
        int o=(*it)->o;
        int s=(*it)->s;
        int x=(int)((*it)->x);
        int y=(int)((*it)->y);
        
        //�w�b�Z�s������߂�
        double Dxx=( DoG[o][s](x-2,y)+DoG[o][s](x+2,y)-2*DoG[o][s](x,y) );
        double Dyy=( DoG[o][s](x,y-2)+DoG[o][s](x,y+2)-2*DoG[o][s](x,y) );
        double Dxy=( DoG[o][s](x-1,y-1)-DoG[o][s](x-1,y+1)
                    -DoG[o][s](x+1,y-1)+DoG[o][s](x+1,y+1) );
        
        //�g���[�X�ƍs��
        double trc=Dxx+Dyy;
        double det=Dxx*Dyy-Dxy*Dxy;
        
        //�폜����
        if(trc*trc/det >= TH_EDG){
            //�L�[�|�C���g�̍폜
            delete (*it);
            it=keys.erase(it);
            
            //�폜������ȉ��̏����̓X�L�b�v
            continue;
        }
        
        //<�R���g���X�g���Ⴂ�L�[�|�C���g���폜>
        int sm1=(s-1<0) ? 0:s-1;
        int sm2=(s-2<0) ? 0:s-2;
        int sp1=(s+1>=S+2) ? S+1:s+1;
        int sp2=(s+2>=S+2) ? S+1:s+2;
        
        //�T�u�s�N�Z������
        double Dx=(DoG[o][s][x-1][y]-DoG[o][s][x+1][y]);
        double Dy=(DoG[o][s][x][y-1]-DoG[o][s][x][y+1]);
        double Ds=(DoG[o][sm1][x][y]-DoG[o][sp1][x][y]);
        
        double Dss=(DoG[o][sm2][x][y]-DoG[o][sp2][x][y]+2*DoG[o][s][x][y]);
        double Dxs=(DoG[o][sm1][x-1][y]-DoG[o][sm1][x+1][y]
                   +DoG[o][sp1][x-1][y]-DoG[o][sp1][x+1][y]);
        double Dys=(DoG[o][sm1][x][y-1]-DoG[o][sm1][x][y+1]
                   +DoG[o][sp1][x][y-1]-DoG[o][sp1][x][y+1]);
        
        
        mD[0][0]=Dxx; mD[0][1]=Dxy; mD[0][2]=Dxs;
        mD[1][0]=Dxy; mD[1][1]=Dyy; mD[1][2]=Dys;
        mD[2][0]=Dxs; mD[2][1]=Dys; mD[2][2]=Dss;
        
        xD[0]=-Dx; xD[1]=-Dy; xD[2]=-Ds;
        
        //�t�s��v�Z(mD�̋t�s���iD��)
        calc_inv(mD,iD);
        
        //�T�u�s�N�Z���ʒu(�s��̐�)
        //09/10/19 �C��
        X[0]=iD[0][0]*xD[0]+iD[0][1]*xD[1]+iD[0][2]*xD[2];
        X[1]=iD[1][0]*xD[0]+iD[1][1]*xD[1]+iD[1][2]*xD[2];
        X[2]=iD[2][0]*xD[0]+iD[2][1]*xD[1]+iD[2][2]*xD[2];
        //�C���O
        //X[0]=mD[0][0]*xD[0]+mD[0][1]*xD[1]+mD[0][2]*xD[2];
        //X[1]=mD[1][0]*xD[0]+mD[1][1]*xD[1]+mD[1][2]*xD[2];
        //X[2]=mD[2][0]*xD[0]+mD[2][1]*xD[1]+mD[2][2]*xD[2];
        
        
        //�T�u�s�N�Z���ʒu�ł̏o��(��21)
        double Dpow=fabs(DoG[o][s][x][y]+(xD[0]*X[0]+xD[1]*X[1]+xD[2]*X[2])/2);
        
        //臒l����
        if(Dpow<TH_POW){
            //�L�[�|�C���g�̍폜
            delete (*it);
            it=keys.erase(it);
        }
        else it++;        //�폜���Ȃ��Ƃ��C�e���[�^��i�߂�
    }
    
    cout<<"    number of keypoints = "<<keys.size()<<endl;
    cout<<endl;    
}
//-----------------------------------------------------------------
//3x3�s��̋t�s��v�Z(3��N�Ƃ��ɂ������ʉ��ł����)
//-----------------------------------------------------------------
void calc_inv(double mat[3][3],double inv[3][3])
{
    int    i,j,k;
    double buf;
    
    //������
    for(i=0;i<3;i++){
        for(j=0;j<3;j++) inv[i][j]=0;
        inv[i][i]=1;
    }
    
    //�|���o���@
    for(i=0;i<3;i++){
        buf=1/mat[i][i];
        for(j=0;j<3;j++){
            mat[i][j]*=buf;
            inv[i][j]*=buf;
        }
        
        for(j=0;j<3;j++){
            if(i!=j){
                buf=mat[j][i];
                for(k=0;k<3;k++){
                    mat[j][k]-=mat[i][k]*buf;
                    inv[j][k]-=inv[i][k]*buf;
                }
            }
        }
    }
}