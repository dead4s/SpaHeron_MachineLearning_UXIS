#include <iostream>
#include <fcntl.h>
#include <sys/stat.h>
#include <unistd.h>
#include <ctime> //timestamp
#include <signal.h>
#include "student.cpp"

#define PIPENAME "/home/pipe/pipe_name"

using namespace std;

typedef struct Concentration {
    int look;
    int student_id;
    float left;
    float right;
} DATA;

void OnSignal(int sig) {
  signal(sig, SIG_DFL);
  unlink(PIPENAME);
  exit(0);
}

int main(){
    signal(SIGINT, OnSignal);

    Student st;
    DATA data;
    pair<float,float> pupil = {0,0};
    // clock_t start = clock();
    int fd;

    if(access(PIPENAME, F_OK) == 0)
        unlink(PIPENAME);
  
    if(mkfifo(PIPENAME, 0666) < 0) {
      std::cout << "Fail to make pipe\n";
      return 0;
    }

    if((fd = open(PIPENAME, O_RDWR)) < 0) {
      std::cout << "Fail to make pipe\n";
      return 0;
    }

    while(1){
        if(read(fd, &data, sizeof(data)) > 0) {
            cout<< "Get pupil " << data.left << ", " << data.right << endl;
            pupil.first = data.left / 100;
            pupil.second = data.right / 100;

            st.chk_state(pupil);
            st.change_state();
            st.ex_pupil = pupil;

            cout << "STATE:" << st.state << endl;
            cout << endl;
        }
    }
    return 0;
}
