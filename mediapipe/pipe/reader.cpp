#include <iostream>
#include <fcntl.h>
#include <sys/stat.h>
#include <unistd.h>
#include <stdlib.h>
#include <string>
#include <signal.h>

#define PIPENAME "/mediapipe/pipe/pipe_name"

typedef struct data {
    int look;
    int student_id;
    std::string date;
    std::string time;
} DATA;

void OnSignal(int sig) {
	signal(sig, SIG_DFL);
	unlink(PIPENAME);
	exit(0);
}

int main(void) {
	signal(SIGINT, OnSignal);

	DATA data;
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

  while(1) {
    if(read(fd, &data, sizeof(data)) > 0) {
      // std::printf("%s\n", data.date.c_str());
      // std::cout << data.date.c_str() << std::endl;
      // std::cout << data.time << std::endl;
      std::cout << data.look << std::endl;
      std::cout << data.student_id << std::endl;
		}
  }

  return 0;
}
