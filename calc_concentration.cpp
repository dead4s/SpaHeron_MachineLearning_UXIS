#include <iostream>
#include <ctime>//timestamp
#include "student.cpp"
using namespace std;

int main(){
    Student st;
    pair<float,float> pupil;
    clock_t start = clock();
    while(1){
        //pupil 전송 받음
        if(((float)(clock()-start)/CLOCKS_PER_SEC)<1) continue;
        
        //cin>>pupil.first>>pupil.second;
        cout<<"get pupil"<<endl;

        st.chk_state(pupil);
        st.change_state();
        
        //web으로 state 전송, 유사도 계산을 위해 file 저장
        cout<<st.state<<endl;
        start = clock();
    }
    return 0;
}