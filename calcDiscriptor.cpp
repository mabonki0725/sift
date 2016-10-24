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
//�����ʂ̋L�q
//-----------------------------------------------------------------
void calc_descriptor(PGM **L,PGM **Fpow,PGM **Farg,list<KEYPOINT*> &keys,list<DESCRIPTOR*> &des)
{
    cout<<"step4:"<<endl;
    
    for(list<KEYPOINT*>::iterator it=keys.begin();it!=keys.end();it++){
        //�L�[�|�C���g�̏��
        int o=(*it)->o;         //�I�N�^�[�u
        int s=(*it)->s-1;       //�ЁD�D�D�ł͂Ȃ����Ǖ֋X��
        int x=(int)((*it)->x);  //�ʒu��
        int y=(int)((*it)->y);  //�ʒu��
        
        //�q�X�g�O�����̍ő�l
        //double max_h=*max_element((*it)->hst,(*it)->hst+KEYPOINT_BIN);
        double max_h = max_element((*it)->hst,KEYPOINT_BIN);

        //��80%
        max_h*=0.8;
        
        //�L�[�|�C���g�ւ̊��蓖�Ĕ���
        for(int bin=0;bin<KEYPOINT_BIN;bin++){
            //�ő�l��80%�ȏォ�H
            if((*it)->hst[bin] >= max_h){
                //���ׂ̃C���f�b�N�X
                int bm=bin-1;
                int bp=bin+1;
                
                //�͈̓`�F�b�N
                if(bm<0)             bm=KEYPOINT_BIN-1;
                if(bp>=KEYPOINT_BIN) bp=0;
                
                //�s�[�N���H
                if( (*it)->hst[bin]>(*it)->hst[bm] && (*it)->hst[bin]>(*it)->hst[bp] ){
                    double sig=pow(2,(s+1)/(double)S)*SIG0;
                    
                    //�s�[�N�Ȃ�L�q�q�ɍ̗p
                    DESCRIPTOR *tmp=new DESCRIPTOR(x,y,sig,bin/(double)KEYPOINT_BIN);
                    
                    //�����ʂ𐶐�
                    make_descriptor(tmp,Fpow[o][s],Farg[o][s]);
                    
                    //�L�[�|�C���g�̃X�P�[���ɍ��킹�č��W����ύX�i�\���p�j
                    double dlt=pow(2,o);
                    tmp->x  =dlt*x;
                    tmp->y  =dlt*y;
                    tmp->sig=dlt*pow(2,(s+1)/(double)S)*SIG0;
                    tmp->arg=(bin/(double)KEYPOINT_BIN-0.5)*2*M_PI;
                    
                    //�L�q�q���X�g�ɒǉ�
                    des.push_back(tmp);
                }
            }
        }
    }
    
    cout<<"    number of keypoints = "<<des.size()<<endl;
    cout<<endl;
}
//-----------------------------------------------------------------
//�����ʂ̐���
//-----------------------------------------------------------------
void make_descriptor(DESCRIPTOR *des,PGM &Fpow,PGM &Farg)
{
    int    x0  =int(des->x);
    int    y0  =int(des->y);
    double sig0=des->sig;
    double arg0=des->arg;
    
    //��]�s��̂�������O�Ɍv�Z
    double cos0=cos((arg0-0.5)*2*M_PI);
    double sin0=sin((arg0-0.5)*2*M_PI);
    
    int    Wm=int(ceil(3.0*sig0)*2+1);    //���̕�
    int    Rm=(Wm -1)/2;                  //���̔��a
    int    W =Fpow.width();               //�摜�̕�
    int    H =Fpow.height();              //�摜�̍���
    
    //�q�X�g�O������������
    for(int bin=0;bin<128;bin++) des->v[bin]=0;
    
    for(int i=x0-Rm;i<=x0+Rm;i++){
        int mx=i-x0+Rm;         //�}�X�N�̍��W�Ɖ摜�̍��W�Ƃ����킹���(x)
        int bx=4*mx/Wm;         //�}�X�N���W���r���̍��W�ɕϊ�
        
        for(int j=y0-Rm;j<=y0+Rm;j++){
            int my=j-y0+Rm;     //�}�X�N�̍��W�Ɖ摜�̍��W�Ƃ����킹���(y)
            int by=4*my/Wm;     //�}�X�N���W���r���̍��W�ɕϊ�
            
            //���W����]
            int px=x0+int(cos0*(i-x0)-sin0*(j-y0));
            int py=y0+int(sin0*(i-x0)+cos0*(j-y0));
            
            //���z��������]
            double arg=Farg(px,py)-arg0;
            if(arg<0)       arg+=1;
            else if(arg>=1) arg-=1;
            
            //�r���̔ԍ�
            int bin=(int)(floor(8*arg)) + bx*8 + by*4*8;
            
            //�q�X�g�O����
            des->v[bin]+=Fpow(px,py);               //�z���g�̓K�E�V�A���|���Ȃ��Ƃ����Ȃ��͂�
            //des->v[bin]+=G[mx][my]*Fpow(px,py);   //����Ȋ����H��`�킩���̂ŗv����
        }
    }
    
    //�q�X�g�O�����𐳋K��
    double sum=0;
    for(/*int*/ bin=0;bin<128;bin++) sum+=(des->v[bin]*des->v[bin]);
    sum=sqrt(sum);
    for(/*int*/ bin=0;bin<128;bin++) des->v[bin]/=sum;
}