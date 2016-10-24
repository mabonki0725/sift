//-----------------------------------------------------------------
#ifndef			__SIFT_H
#define			__SIFT_H

#include <list>
#include "pgm.h"

#define M_PI 3.1415926  //m.nakai
//-----------------------------------------------------------------
//SIFT �p�����[�^
//-----------------------------------------------------------------
const double SIG0   = 1.6;
const int    S      = 3;
const int    MINW   = 20;

//-----------------------------------------------------------------
//�����ʋL�q�q�\����
//-----------------------------------------------------------------
struct DESCRIPTOR
{
    double x;       //�ʒu��
    double y;       //�ʒu��
    double sig;     //���U��
    double arg;     //�I���G���e�[�V����
    
    double v[128];  //SIFT������
    
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
//�L�[�|�C���g���\����
//-----------------------------------------------------------------
struct KEYPOINT
{
    double x;           //�ʒux
    double y;           //�ʒuy
    
    int o;              //�I�N�^�[�u��
    int s;              //�X�P�[��
    
    double hst[KEYPOINT_BIN];   //�q�X�g�O����
    
    //�R���X�g���N�^�Ƃ�
    KEYPOINT(){x=y=0.0; o=s=0;}
    KEYPOINT(double _x,double _y,int _o,int _s)
    {
        x=_x; y=_y; o=_o; s=_s;
    }
    ~KEYPOINT(){}
};
//-----------------------------------------------------------------
//SIFT�֐��̃v���g�^�C�v�錾
//-----------------------------------------------------------------
void SIFT(PGM &src,/*std::*/list<DESCRIPTOR*> keys);

//-----------------------------------------------------------------
#endif