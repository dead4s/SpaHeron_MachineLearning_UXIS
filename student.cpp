#include <iostream>
#include <vector>
#include <string>
#include "student.h"
using namespace std; 
bool Student::isFixed(){
    int x = pupil.first-ex_pupil.first;
    int y = pupil.second-ex_pupil.second;
    int dis = x*x+y*y;

    if(dis < upper_bound*upper_bound){
        return true;
    }
    else return false;
}

int Student::change_state(){
    if(FACE_UNDETECTED_THES<=cnt_face.first)   state = 0;
    else if(EYE_UNFIXED_THES<=cnt_eye.first)   state = 3;
    else if(EYE_FIXED_THES<=cnt_eye.second)    state = 2;
    cnt_face={0,0};
    cnt_eye={0,0};

    return state;
}
int Student::chk_state(pair<int,int> pupil){
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
