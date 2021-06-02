#include <iostream>
#include <vector>
#include <string>
using namespace std; 
const int STATE = 0;
const int ATTEND = 1;
const int ABSCENT = 0;
const int CONCENTRATE = 2;
const int DISTRACT = 3;
string STATES_STR[] = {"ATTEND","ABSCENT","CONCENTRATE","DISTRACT"};

const int upper_bound = 10;

const int FACE_UNDETECTED_THES = 10;
const int EYE_FIXED_THES = 10;
const int EYE_UNFIXED_THES = 10;

const int FACE_UNDETECTED_CNT = 0;
const int EYE_FIXED_CNT = 0;
const int EYE_UNFIXED_CNT = 0;

pair<int,int> pupil;

class Student{
    public:
    int state;
    pair<int,int> cnt_face,cnt_eye;//first = there is'nt, second = there is
    pair<int,int> ex_pupil;

    Student(){
        state = 1;
        cnt_face = {0,0};
        cnt_eye = {0,0};
    }
    bool isFixed(){
        int x = pupil.first-ex_pupil.first;
        int y = pupil.second-ex_pupil.second;
        int dis = x*x+y*y;

        if(dis < upper_bound*upper_bound){
            return true;
        }
        else return false;
    }
    
    int change_state(){
        if(FACE_UNDETECTED_THES<=cnt_face.first)   state = 0;
        else if(EYE_UNFIXED_THES<=cnt_eye.first)   state = 3;
        else if(EYE_FIXED_THES<=cnt_eye.second)    state = 2;
        cnt_face={0,0};
        cnt_eye={0,0};

        return state;
    }
    int chk_state(pair<int,int> pupil){
        if(pupil.first<0&&pupil.second<0) {
            cnt_face.first++;
            cnt_eye.first++;
        }
        else if(state == 0){
            cnt_face.second++;
        }
        else if(isFixed()){//state = const, distract, attend
            cnt_eye.second++;
        }
        else{
            cnt_eye.first++;
        }
        return state;
    }

};
