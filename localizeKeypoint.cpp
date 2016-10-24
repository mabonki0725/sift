//-----------------------------------------------------------------
//SIFT �p�����[�^
//-----------------------------------------------------------------
const double SIG0   = 1.6;
const int    S      = 3;
const int    MINW   = 20;

int OCT;

const double Rth    = 10.0;
const double TH_EDG = (Rth+1)*(Rth+1)/Rth;
//-----------------------------------------------------------------
//2.2 �L�[�|�C���g�̃��[�J���C�Y
//-----------------------------------------------------------------
void localize_keypoints(PGM **DoG,list<KEYPOINT*> &keys)
{
    cout<<"step2:localize"<<endl;
    
    
    for(list<KEYPOINT*>::iterator it=keys.begin();it!=keys.end();){
        //<2.2.1 ��ȗ��ɂ��i�荞��>
        int o=(*it)->o;
        int s=(*it)->s;
        int x=(int)((*it)->x);
        int y=(int)((*it)->y);
        
        //�w�b�Z�s������߂�
        double Dxx=( DoG[o][s](x-2,y)+DoG[o][s](x+2,y)-2*DoG[o][s](x,y) );
        double Dyy=( DoG[o][s](x,y-2)+DoG[o][s](x,y+2)-2*DoG[o][s](x,y) );

        //09/10/15 �����D�Ԉ���Ă܂����C���߂�Ȃ����D
        double Dxy=( DoG[o][s](x-1,y-1)-DoG[o][s](x-1,y+1)
                    -DoG[o][s](x+1,y-1)+DoG[o][s](x+1,y+1) );
        //�����O
        //double Dxy=( DoG[o][s](x-1,y-1)-DoG[o][s](x-1,y+1)
        //            -DoG[o][s](x+1,y-1)-DoG[o][s](x+1,y+1) );
        
        //�g���[�X�ƍs��
        double trc=Dxx+Dyy;
        double det=Dxx*Dyy-Dxy*Dxy;
        
        //�폜����
        if(trc*trc/det >= TH_EDG){
            //�L�[�|�C���g�̍폜
            delete (*it);
            it=keys.erase(it);
            
            //�폜������ȉ��̏����̓X�L�b�v
            continue;
        }
        else it++;       //�폜���Ȃ��Ƃ����̃L�[�|�C���g
    }
    
    cout<<"    number of keypoints = "<<keys.size()<<endl;
    cout<<endl;    
}