#ifndef STUDENT_H
#define STUDENT_H

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
    long long id=-1;

    Student(){
        state = 1;
        cnt_face = {0,0};
        cnt_eye = {0,0};
    }
    bool isFixed();
    int change_state();
    int chk_state(pair<int,int> pupil);

};
#endif