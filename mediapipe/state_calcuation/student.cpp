#include <iostream>
#include <vector>
#include <string>
#include "student.h"

using namespace std; 

bool Student::isFixed(pair<int, int> pupil){
    int x = pupil.first - ex_pupil.first;
    int y = pupil.second - ex_pupil.second;

    int dis = x*x + y*y;

    if(dis < upper_bound*upper_bound){
        return true;
    }
    else return false;
}

State Student::change_state(){
    if(FACE_UNDETECTED_THES<=cnt_face.first)   state = ABSCENT;
    else if(EYE_FIXED_THES<=cnt_eye.first)   state = ATTEND;
    else if(state !=ABSCENT && EYE_UNFIXED_THES <= cnt_eye.first)   state = DISTRACT;
    else if(state !=ABSCENT && EYE_FIXED_THES <= cnt_eye.second)    state = CONCENTRATE;
		else return state;

   	cnt_face={0,0};
    cnt_eye={0,0};

    cout<<"change state"<<endl;
    return state;
}
State Student::chk_state(pair<int,int> pupil) {
    if(pupil.first<0 && pupil.second<0) {
        cnt_face.first++;
        cnt_eye.first++;
    }
    else if(isFixed(pupil)){ //state = const, distract, attend
        cnt_eye.second++;
				cnt_face.second++;
    }
    else{
        cnt_eye.first++;
				cnt_face.second++;
    }

    cout<<"check state"<<endl;
    return state;
}
