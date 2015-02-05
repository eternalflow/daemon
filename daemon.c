#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <pthread.h>
#include <signal.h>

int daemonize() {
    umask(0);
    if (setsid() < 0) return -1;
    if (chdir("/") < 0) return -1;
    int fpid = creat("/home/sheldon/daemon.pid", 0666);
    char* cpid[6];
    sprintf(cpid, "%d", getpid());
    write(fpid, cpid, strlen(cpid));
    
    int info = creat("/home/sheldon/cnt", 0666);
    int cnt = 3;
    char* buf[5];
    sprintf(buf, "%d", cnt);
    write(info, buf, strlen(buf));
    close(info);
    
    int log = creat("/home/sheldon/daemonlog", 0666);
    close(STDIN_FILENO);
    close(STDOUT_FILENO);
    close(STDERR_FILENO);
    return log;
}

static pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
static int fd;
int cnt;

void* thread(void* arg) {
    int n = (int) arg;
    const char* buf[20];
    sprintf(buf, "Thread %d\n", n);
    int i;
    pthread_mutex_lock(&mutex);
    for (i=0; i<5; i++)
        write(fd, buf, strlen(buf));
    pthread_mutex_unlock(&mutex);
}

int readcnt() {
    char* ret[5];
    int info = open("/home/sheldon/cnt", O_RDONLY);
    read(info, &ret, strlen(ret));
    close(info);
    int count = atoi(ret);
    return count;
}

void sig(int signo) {
    cnt = readcnt();
}

int main(int argc, char** argv) {
    pid_t pid;
    pid = fork();
    if (pid < 0) {
        printf("Failed fork()\n");
        return -1;
    }
    if (pid > 0) {
        printf("Fork OK. Daemon started with pid %d\n", pid);
        return 0;
    }
    
    fd = daemonize();
    if (fd < 0) return -1;
    if (fd > 0) {
        
        const char* buf = "Daemon goes:\n";
        write(fd, buf, strlen(buf));
        
        cnt = readcnt();
        if (cnt == 0) {
            const char* err = "Error with reading count\n";
            write(fd, err, strlen(err));
            return -1;
        }
        
        signal(SIGHUP, sig);
        
        while (1) {
            int i;
            pthread_t* t[cnt];
            for (i=0; i<cnt; i++)
                pthread_create(&t[i], NULL, thread, (void*) i+1);         
            for (i=0; i<cnt; i++)
                pthread_join(t[i], NULL);
            sleep(5);
        }
    }
    else return -1;
}

