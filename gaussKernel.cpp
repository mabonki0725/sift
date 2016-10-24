//-----------------------------------------------------------------
#include <iostream>
using namespace std;

//-----------------------------------------------------------------
#include <math.h>
#include "sift.h"
#include "protype.h"


extern int OCT;





//-----------------------------------------------------------------
//�摜�̍���
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
//�K�E�V�A���t�B���^��������
//-----------------------------------------------------------------
PGM gaussian_filter(PGM &src,double sig)
{
    int W=src.width();
    int H=src.height();
    
    //���ʉ摜
    PGM tmp(W,H);
    PGM dst(W,H);
    
    int Wm=int(ceil(3.0*sig)*2+1);  //����
    int Rm=(Wm-1)/2;                //�����a
    
    //�t�B���^
    double *msk=new double[Wm];
    
    sig=2*sig*sig;
    double div=sqrt(sig*M_PI);
    
    //�t�B���^�̍쐬
    for(int x=0;x<Wm;x++){
        int p=(x-Rm)*(x-Rm);
        msk[x]=exp(-p/sig)/div;
    }
    
    //��������
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
    //��������
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
//���ƍ��������ꂼ��1/2�Ƀ_�E���T���v�����O
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
                      //�L�[�|�C���g���Ƃ��ēo�^
                      KEYPOINT *tmp=new KEYPOINT((double)x,(double)y,o,s);
                      keys.push_back(tmp);    //�L�[�|�C���g���X�g�ɒǉ�
                      flg[x][y]=true;         //�t���O��ݒ�
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
