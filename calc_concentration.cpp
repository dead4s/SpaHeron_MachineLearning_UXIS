#include <iostream>
#include "student.h"
using namespace std;

int main(){
    Student st;
    pair<float,float> pupil;
    while(1){
        //pupil 전송 받음
        cin>>pupil.first>>pupil.second;

        st.chk_state(pupil);
        st.change_state();
        
        //web으로 state 전송, 유사도 계산을 위해 file 저장
        cout<<st.state<<endl;
    }
    return 0;
}