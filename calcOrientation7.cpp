//-----------------------------------------------------------------
#include <iostream>
using namespace std;

//-----------------------------------------------------------------
#include <math.h>
#include "sift.h"
#include "protype.h"


extern int OCT;


//-----------------------------------------------------------------
//�K�E�V�A���t�B���^�[�����
//-----------------------------------------------------------------
PGM gaussian_filterOnly(double sig)
{
    int w=int(ceil(3.0*sig)*2+1);   //����
    int c=(w-1)/2;                  //�����a
    
    PGM mask(w,w);
    
    sig=2*sig*sig;
    
    for(int x=0;x<w;x++){
        int px=x-c;
        for(int y=0;y<w;y++){
            int    py=y-c;
            double dst=px*px+py*py;
            
            mask[x][y]=exp(-dst/sig)/(sig*M_PI);
        }
    }
    
    return mask;
}
//-----------------------------------------------------------------
//�I���G���e�[�V�����̎Z�o
//-----------------------------------------------------------------
void calc_orientation(PGM **L,PGM **Fpow,PGM **Farg,list<KEYPOINT*> &keys)
{
    cout<<"step3:"<<endl;
    
    //�e�������摜�̌��z�����v�Z
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
    
    //�K�E�V�A���t�B���^�[������
    PGM G[S];
    double k=pow(2,1/(double)S);
    for(int s=0;s<S;s++){
        double sig=pow(k,s+1)*SIG0;
        G[s]=gaussian_filterOnly(sig);
    }
    
    //���z��񂩂�e�L�[�|�C���g�̃q�X�g�O�������쐬
    for(list<KEYPOINT*>::iterator it=keys.begin();it!=keys.end();it++){
        //�L�[�|�C���g�̏��
        int o=(*it)->o;         //�I�N�^�[�u
        int s=(*it)->s-1;       //��
        int x=(int)((*it)->x);  //�ʒu��
        int y=(int)((*it)->y);  //�ʒu��
        
        if(s<0 || s>=S){
            //�͈̓`�F�b�N(�{���Ȃ炱���ɂ͗��Ȃ��͂�)
            cout<<"error\n\tovtave:"<<o<<" / "<<OCT<<"\n\tsigma no."<<s<<" / "<<S<<endl;
            continue;
        }
        
        //�q�X�g�O������������
        for(int bin=0;bin<KEYPOINT_BIN;bin++) (*it)->hst[bin]=0;
        
        //�K�E�V�A���t�B���^�̔��a
        int Rm=(G[s].width()-1)/2;
        
        //�q�X�g�O���~���O
        for(int i=x-Rm;i<=x+Rm;i++){
            //�}�X�N�̍��W�Ɖ摜�̍��W�Ƃ����킹���(x)
            int mx=i-x+Rm;
            
            for(int j=y-Rm;j<=y+Rm;j++){
                //�}�X�N�̍��W�Ɖ摜�̍��W�Ƃ����킹���(y)
                int my=j-y+Rm;
                
                //�p�x��񂩂�q�X�g�O������ bin �ԍ�������
                int bin=(int)floor(KEYPOINT_BIN*Farg[o][s](i,j))%KEYPOINT_BIN;
                
                //�d�ݕt���ŉ��Z
                (*it)->hst[bin]+=G[s][mx][my]*Fpow[o][s](i,j);
            }
        }
        
        //�q�X�g�O�����̃X���[�W���O
        for(/*int*/ i=0;i<6;i++){
            (*it)->hst[0]=((*it)->hst[KEYPOINT_BIN-1]+(*it)->hst[0]+(*it)->hst[1])/3.0;
            for(int bin=1;bin<KEYPOINT_BIN;bin++){
                (*it)->hst[bin]=((*it)->hst[bin-1]+(*it)->hst[bin]+(*it)->hst[(bin+1)%KEYPOINT_BIN])/3.0;
            }
        }        
    }
    
    cout<<"    number of keypoints = "<<keys.size()<<endl;
    cout<<endl;
}
