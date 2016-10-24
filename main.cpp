//-----------------------------------------------------------------
#include <iostream>
#include <math.h>
#include <time.h>
using namespace std;

#include "sift.h"
#include "protype.h"

int OCT;



#define PLOT
//-----------------------------------------------------------------
#ifdef PLOT
void plot_image(PGM &src,list<DESCRIPTOR*> &keys)
{
    FILE *gp=_popen("gnuplot","w");
    
    int W=src.width();
    int H=src.height();
    
    fprintf(gp,"set yrange [] reverse\n");
    fprintf(gp,"set format x ''\n");
    fprintf(gp,"set format y ''\n");
    fprintf(gp,"set size ratio %lf\n",H/(double)W);
    fprintf(gp,"set palette gray\n");
    fprintf(gp,"set xrange [0:%d]\n",W-1);
    fprintf(gp,"set yrange [0:%d]\n",H-1);
    fprintf(gp,"unset key\n");
    fprintf(gp,"unset colorbox\n");
    fprintf(gp,"plot '-' matrix with image,'-' w l\n");
    
    //�摜�̕\��
    for(int y=0;y<H;y++){
        for(int x=0;x<W;x++){
            fprintf(gp,"%f ",src[x][y]);
        }
        fprintf(gp,"\n");
    }
    fprintf(gp,"e\n");
    fprintf(gp,"e\n");
    
    for(list<DESCRIPTOR*>::iterator it=keys.begin();it!=keys.end();it++){
        double size=(*it)->sig;
        
        //�~��`��
        for(int i=0;i<20;i++){
            
            double x=(*it)->x + size*cos((i/19.0)*2*M_PI);
            double y=(*it)->y + size*sin((i/19.0)*2*M_PI);
            
            fprintf(gp,"%f %f\n",x,y);
        }
        fprintf(gp,"\n");
        
        //���z������`��
        fprintf(gp,"%f %f\n",(*it)->x,(*it)->y);
        fprintf(gp,"%f %f\n",(*it)->x+size*cos((*it)->arg),(*it)->y+size*sin((*it)->arg));
        fprintf(gp,"\n");
    }
    fprintf(gp,"e\n");
    
    fflush(gp);
    cout<<endl<<"enter>>";
    getchar();
    _pclose(gp);
}
#endif
//-----------------------------------------------------------------
//The SIFT
//-----------------------------------------------------------------
void SIFT(PGM &_src,list<DESCRIPTOR*> des)
//void SIFT(PGM &_src)
{
    PGM src(_src);
    
    //--�I�N�^�[�u��������(log2�Ƃ��g���������J�b�R�ǂ����ǂ�)
    int W=(src.width()<src.height()) ? src.width():src.height();
    for(OCT=0;W>MINW;OCT++,W/=2);
    
    //--�o�b�t�@�m��
    PGM **L=new PGM*[OCT];                      //�������摜�p�o�b�t�@
    for(int o=0;o<OCT;o++) L[o]=new PGM[S+3];
    
    PGM **DoG=new PGM*[OCT];                    //DoG�摜�p�o�b�t�@
    for(/*int*/ o=0;o<OCT;o++) DoG[o]=new PGM[S+2];
    
    list<KEYPOINT*> keys;                       //�L�[�|�C���g��⃊�X�g
    
    
    //detection
    get_keypoints(src,L,DoG,keys);             //�L�[�|�C���g���̌��o
    //localize_keypoints(src);                 //�L�[�|�C���g�̃��[�J���C�Y
    localize_keypoints(DoG,keys);
    //plot_image(_src,keys);

    PGM **Fpow=new PGM*[OCT];
    PGM **Farg=new PGM*[OCT];
    
    calc_orientation(L,Fpow,Farg,keys);

    calc_descriptor(L,Fpow,Farg,keys,des);    

    //--�o�b�t�@���
    for(/*int*/ o=0;o<OCT;o++){
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
//���C��
//-----------------------------------------------------------------
int main()
{
    PGM src1("b.ppm");
    
    list<DESCRIPTOR*> key1;
    
    cout<<"SIFT start::"<<endl<<endl;
    
    //���Ԍv���X�^�[�g
    clock_t str=clock();
    
    SIFT(src1,key1);
    //SIFT(src1);
    
    //���Ԍv���I���
    clock_t end=clock();
    cout<<"time = "<<(end-str)/(double)CLOCKS_PER_SEC<<"[sec]"<<endl;
    
#ifdef PLOT
    plot_image(src1,key1);
#endif    
    return 0;
}
//-----------------------------------------------------------------