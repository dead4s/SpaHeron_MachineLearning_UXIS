#include <iostream>
#include <ctime>//timestamp
#include "student.cpp"
using namespace std;

int main(){
    Student st;
    pair<float,float> pupil={0,0};
    clock_t start = clock();
    while(1){
        //get_pupil from opencv process and save to pupil 
        if(((float)(clock()-start)/CLOCKS_PER_SEC)<1) continue;
        
        //cin>>pupil.first>>pupil.second;
        cout<<"get pupil"<<endl;

        st.chk_state(pupil);
        st.change_state();
        st.ex_pupil = pupil;
        
        cout<<st.state<<endl;
        // send st.state to model process
        start = clock();
        
    }
    return 0;
}
