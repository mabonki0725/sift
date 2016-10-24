//-----------------------------------------------------------------
#include "pgm.h"

#include <fstream>
using namespace std;

//-----------------------------------------------------------------
//�R���X�g���N�^
//-----------------------------------------------------------------
PGM::PGM(int _w,int _h)
{
    pixels=NULL;
    reset(_w,_h);
}
//-----------------------------------------------------------------
PGM::PGM(const PGM &src)
{
    pixels=NULL;
    reset(src.W,src.H);
    
    /*std::*/memcpy(pixels[0],src.pixels[0],W*H*sizeof(double));
}
//-----------------------------------------------------------------
PGM::PGM(const string &file_name)
{
    pixels=NULL;
    load_from_file(file_name);
}
//-----------------------------------------------------------------
//�f�X�g���N�^
//-----------------------------------------------------------------
PGM::~PGM()
{
    free();
}
//-----------------------------------------------------------------
//������
//-----------------------------------------------------------------
void PGM::reset(int _w,int _h)
{
    free();
    
    W=_w;
    H=_h;
    
    pixels   =new double*[W];
    pixels[0]=new double[W*H];
    for(int x=1;x<W;x++) pixels[x]=pixels[x-1]+H;
}
//-----------------------------------------------------------------
//���
//-----------------------------------------------------------------
void PGM::free()
{
    if(pixels!=NULL){
        if(pixels[0]!=NULL) delete[] pixels[0];
        delete[] pixels;
        pixels=NULL;
    }
    
    W=0;
    H=0;
}
//-----------------------------------------------------------------
//�I�y���[�^=(PGM �̑��)
//-----------------------------------------------------------------
PGM& PGM::operator=(const PGM &a)
{
    reset(a.width(),a.height());
    /*std::*/memcpy(pixels[0],a.pixels[0],W*H*sizeof(double));
    
    return *this;
}
//-----------------------------------------------------------------
//�I�y���[�^(��f�l�ւ̃A�N�Z�X�F���S��)
//-----------------------------------------------------------------
double& PGM::operator()(int i,int j)
{
    if(i<0)       i=0;
    else if(i>=W) i=W-1;
    if(j<0)       j=0;
    else if(j>=H) j=H-1;
    
    return pixels[i][j];
}
//-----------------------------------------------------------------
//�I�y���[�^*(double �Ƃ̏�Z)
//-----------------------------------------------------------------
PGM PGM::operator*(double a)
{
    PGM tmp(W,H);
    
    for(int x=0;x<W;x++){
        for(int y=0;y<H;y++){
            tmp.pixels[x][y]=a*pixels[x][y];
        }
    }    
    
    return tmp;
}
//-----------------------------------------------------------------
//�I�y���[�^+(PGM �Ɖ��Z)
//-----------------------------------------------------------------
PGM PGM::operator+(const PGM &a)
{
    int min_w = (a.width()  < W ) ? a.width()  : W;
    int min_h = (a.height() < H ) ? a.height() : H;
    
    PGM tmp(min_w,min_h);
    
    for(int x=0;x<min_w;x++){
        for(int y=0;y<min_h;y++){
            tmp.pixels[x][y]=a.pixels[x][y]+pixels[x][y];
        }
    }
    
    return tmp;
}
//-----------------------------------------------------------------
//�I�y���[�^-(PGM �ƌ��Z)
//-----------------------------------------------------------------
PGM PGM::operator-(const PGM &a)
{
    int min_w = (a.width()  < W ) ? a.width()  : W;
    int min_h = (a.height() < H ) ? a.height() : H;
    
    PGM tmp(min_w,min_h);
    
    for(int x=0;x<min_w;x++){
        for(int y=0;y<min_h;y++){
            tmp.pixels[x][y]=a.pixels[x][y]-pixels[x][y];
        }
    }
    
    return tmp;
}
//-----------------------------------------------------------------
//�t�@�C������ǂݍ���
//-----------------------------------------------------------------
bool PGM::load_from_file(const string &file_name)
{
    ifstream file(file_name.c_str());
    
    if(!file.is_open()) return false;
    
    bool grayscale=true;
    string str;
    
    file>>str;
    if(str=="P3") grayscale=false;      //PPM�t�@�C���H
    else if(str!="P2") return false;    //PGM�t�@�C���H
    
    //�摜�T�C�Y�ǂݍ���
    for(int i=0;i<3;i++){
        file>>str;
        while(str[0]=='#'){
            getline(file,str);
            file>>str;
        }
        switch(i){
            case 0: W=/*std::*/atoi(str.c_str()); break;
            case 1: H=/*std::*/atoi(str.c_str()); break;
        }
    }
    
    //�z��m��
    reset(W,H);
    
    //��f�l�ǂݍ���
    for(int y=0;y<H;y++){
        for(int x=0;x<W;x++){
            file>>str;
            while(str[0]=='#'){
                getline(file,str);
                file>>str;
            }
            
            if(grayscale) pixels[x][y]=/*std::*/atoi(str.c_str())/255.0;
            else{
                double r=/*std::*/atoi(str.c_str())/255.0;
                double g,b;
                for(int j=0;j<2;j++){
                    file>>str;
                    while(str[0]=='#'){
                        getline(file,str);
                        file>>str;
                    }
                    switch(j){
                        case 0: g=/*std::*/atoi(str.c_str())/255.0; break;
                        case 1: b=/*std::*/atoi(str.c_str())/255.0; break;
                    }
                }
                
                pixels[x][y]=r*0.299+g*0.587+b*0.114;
            }
        }
    }
    file.close();
    
    return true;
}
//-----------------------------------------------------------------
//�t�@�C���ɕۑ�
//-----------------------------------------------------------------
bool PGM::save_to_file(const string &file_name)
{
    ofstream file(file_name.c_str());
    
    if(!file.is_open()) return false;
    
    file<<"P2"<<std::endl;
    
    file<<"# CREATOR: no name"<<std::endl;
    file<<W<<" "<<H<<std::endl;
    file<<255<<std::endl;
    
    for(int y=0;y<H;y++){
        for(int x=0;x<W;x++){
            double tmp=pixels[x][y];
            if(tmp<0) tmp=0;
            else if(tmp>1) tmp=1;
            
            file<<(int)(255*tmp)<<endl;
        }
    }
    file.close();
    
    return true;
}
//-----------------------------------------------------------------