//-----------------------------------------------------------------
#include <iostream>
using namespace std;

//-----------------------------------------------------------------
#include <math.h>
#include "sift.h"

#define M_PI 3.1415926  //m.nakai
//-----------------------------------------------------------------
//SIFT �p�����[�^
//-----------------------------------------------------------------
const double SIG0   = 1.6;
const int    S      = 3;
const int    MINW   = 20;

int OCT;

//-----------------------------------------------------------------
//�L�[�|�C���g���\����
//-----------------------------------------------------------------
struct KEYPOINT
{
    double x;           //�ʒux
    double y;           //�ʒuy
    
    int o;              //�I�N�^�[�u��
    int s;              //�X�P�[��
    
    
    //�R���X�g���N�^�Ƃ�
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
//2.1 �X�P�[���ƃL�[�|�C���g���o
//-----------------------------------------------------------------
void get_keypoints(PGM &src,PGM **L,PGM **DoG,list<KEYPOINT*> &keys)
{
    cout<<"step1:keypoint detection"<<endl;
    
    //������
    double k=pow(2,1/(double)S);
    
    //�������摜���쐬
    for(int o=0;o<OCT;o++){
        cout<<"    octave "<<o<<" : "<<flush;
        
        double sig=SIG0;
        for(int s=0;s<S+3;s++){
            //���摜�ɃK�E�V�A���t�B���^
            L[o][s]=gaussian_filter(src,sig);
            
            sig*=k;
            
            cout<<s<<flush;
        }
        //�_�E���T���v�����O
        down_sampling(src);
        
        cout<<endl;
    }
    
    //DoG�摜���쐬
    for(/*int*/ o=0;o<OCT;o++){
        for(int i=0;i<S+2;i++){
            //DoG[o][i]=L[o][i+1]-L[o][i];
            diff_pgm(DoG[o][i],L[o][i+1],L[o][i]);
        }
    }
    
    //�����ʒu�Ɋ��ɃL�[�|�C���g�����邩�ǂ����̃t���O
    bool **flg=new bool*[DoG[0][0].width()];
    flg[0]=new bool[DoG[0][0].width()*DoG[0][0].height()];
    for(int x=1;x<DoG[0][0].width();x++) flg[x]=flg[x-1]+DoG[0][0].height();
    
    //DoG�摜����ɒl���o
    for(/*int*/ o=0;o<OCT;o++){
        int W=DoG[o][0].width();
        int H=DoG[o][0].height();
        
        //�t���O������
        for(int x=1;x<W-1;x++){
            for(int y=1;y<H-1;y++) flg[x][y]=false;
        }
        
        for(int s=1;s<S+1;s++){
            for(int x=1;x<W-1;x++){
                for(int y=1;y<H-1;y++){
                    //���o�̃s�N�Z���̓L�[�|�C���g�ɂ��Ȃ�
                    if(flg[x][y]) continue;
                    
                    //26�ߖT��T��
                    bool is_max=true;     //�ɑ�l���ǂ���
                    bool is_mim=true;     //�ɏ��l���ǂ���
                    for(int ds=s-1;ds<=s+1;ds++){
                        for(int dx=x-1;dx<=x+1;dx++){
                            for(int dy=y-1;dy<=y+1;dy++){
                                //���S�͖���
                                if(ds==s && dx==x && dy==y) continue;
                                
                                //�ɑ�C�ɏ�����
                                if(is_max && DoG[o][s][x][y]<=DoG[o][ds][dx][dy]){
                                    is_max=false;
                                }
                                if(is_mim && DoG[o][s][x][y]>=DoG[o][ds][dx][dy]){
                                    is_mim=false;
                                }
                                
                                //�ɒl�łȂ������玟
                                if(!is_max && !is_mim) goto next;
                            }
                        }
                    }
                    
                    //�L�[�|�C���g���Ƃ��ēo�^
                    KEYPOINT *tmp=new KEYPOINT((double)x,(double)y,o,s);
                    keys.push_back(tmp);    //�L�[�|�C���g���X�g�ɒǉ�
                    flg[x][y]=true;         //�t���O��ݒ�
                    
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

