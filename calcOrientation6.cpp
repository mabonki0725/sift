//-----------------------------------------------------------------
//The SIFT
//-----------------------------------------------------------------
void SIFT(PGM &_src)
{
    PGM src(_src);
    
    //--�I�N�^�[�u��������(log2�Ƃ��g���������J�b�R�ǂ����ǂ�)
    int W=(src.width()<src.height()) ? src.width():src.height();
    for(OCT=0;W>MINW;OCT++,W/=2);
    
    //--�o�b�t�@�m��
    PGM **L=new PGM*[OCT];                      //�������摜�p�o�b�t�@
    for(int o=0;o<OCT;o++) L[o]=new PGM[S+3];
    
    PGM **DoG=new PGM*[OCT];                    //DoG�摜�p�o�b�t�@
    for(int o=0;o<OCT;o++) DoG[o]=new PGM[S+2];
    
    PGM **Fpow=new PGM*[OCT];                   //(���z���x)
    PGM **Farg=new PGM*[OCT];                   //(���z����)
    for(int o=0;o<OCT;o++){
        Fpow[o]=new PGM[S];
        Farg[o]=new PGM[S];
    }
    
    list<KEYPOINT*> keys;                       //�L�[�|�C���g��⃊�X�g
    
    
    //detection
    get_keypoints(src,L,DoG,keys);             //�L�[�|�C���g���̌��o
    localize_keypoints(DoG,keys);              //�L�[�|�C���g�̃��[�J���C�Y
    
    //  -description
    calc_orientation(L,Fpow,Farg,keys);             //�����ʂ̋L�q
    
    plot_image(_src,keys);
    
    //--�o�b�t�@���
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
    
    cout<<endl;
}