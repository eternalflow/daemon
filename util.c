#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <signal.h>

int main(int argc, char** argv) {
    if (argc == 1) {
        printf("Usage: util [N], where N represents number of threads in daemon\n");
        return -1;
    }
    else {
        int info = creat("/home/sheldon/cnt", 0666);
        write(info, argv[1], strlen(argv[1]));
        close(info);
        
        int pid;
        char* cpid[6];
        int fpid = open("/home/sheldon/daemon.pid", O_RDONLY);
        read(fpid, cpid, 6);
        close(fpid);
        
        pid = atoi(cpid);
        kill(pid, SIGHUP);
        return 0;
    }
}