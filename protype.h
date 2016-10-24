//-------------------------------------------------------------------
//�v���g�^�C�v
//-------------------------------------------------------------------
void get_keypoints(PGM &src,PGM **L,PGM **DoG,list<KEYPOINT*> &keys);
void localize_keypoints(PGM **DoG,list<KEYPOINT*> &keys);
void calc_orientation(PGM **L,PGM **Fpow,PGM **Farg,list<KEYPOINT*> &keys);
void calc_descriptor(PGM **L,PGM **Fpow,PGM **Farg,list<KEYPOINT*> &keys,list<DESCRIPTOR*> &des);
//-------------------------------------------------------------------
//�v���g�^�C�v localizekeypoint5.cpp
//-------------------------------------------------------------------
void calc_inv(double[3][3] ,double[3][3]);  //m.nakai
//-----------------------------------------------------------------
//�v���g�^�C�v gaussKernel.cpp
//-----------------------------------------------------------------
void down_sampling(PGM  &src);
//-------------------------------------------------------------------
//�v���g�^�C�v calcDiscriptor.cpp
//-------------------------------------------------------------------
void down_sampling(PGM  &src);
void make_descriptor(DESCRIPTOR *des,PGM &Fpow,PGM &Farg);