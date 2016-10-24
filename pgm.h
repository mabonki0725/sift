//-----------------------------------------------------------------
#ifndef			__PGM_H
#define			__PGM_H

//-----------------------------------------------------------------
#include <string>

//-----------------------------------------------------------------
//PGMクラス
//-----------------------------------------------------------------
class PGM
    {
    private:
        int W;
        int H;
        
        void free();
        
    public:
        double **pixels;   //pixels[W][H]
        
        PGM(){ pixels=NULL; W=H=0; }
        PGM(int _w,int _h);
        PGM(const PGM &src);
        PGM(const std::string &file_name);
        ~PGM();
        
        void reset(int _w,int _h);
        int  width() const{ return W; }
        int  height()const{ return H; }
        
        bool load_from_file(const std::string &file_name);
        bool save_to_file(const std::string &file_name);
        
        //演算子の定義
        PGM& operator=(const PGM&);
        PGM  operator*(double);
        PGM  operator+(const PGM&);
        PGM  operator-(const PGM&);
        
        double* operator[](int i){ return pixels[i]; }  //画素値へのアクセス（高速）
        double& operator()(int i,int j);                //画素値へのアクセス（安全）
    };
//-----------------------------------------------------------------

#endif