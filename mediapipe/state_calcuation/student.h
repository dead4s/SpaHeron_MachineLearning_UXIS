#ifndef STUDENT_H
#define STUDENT_H
#include <iostream>
using namespace std;
enum State{
    ATTEND,
    ABSCENT,
    CONCENTRATE,
    DISTRACT,
};


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
    State state;
    pair<int,int> cnt_face,cnt_eye;//first = there is'nt, second = there is
    pair<int,int> ex_pupil;
    long long id=-1;

    Student(){
        State state = ATTEND;
        cnt_face = {0,0};
        cnt_eye = {0,0};
    }
    bool isFixed();
    State change_state();
    State chk_state(pair<int,int> pupil);
};
#endif
