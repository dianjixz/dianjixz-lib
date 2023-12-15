// file : fork_process.c
#include <stdio.h>
#include <unistd.h>
int main(int argc, char argv[])
{
    pid_t id = fork(); // 创建子进程

    if (id == 0)
    {
        printf("i am process...child---pid:%d,ppid:%d\n", getpid(), getppid());
        sleep(1);
    }
    else if (id > 0)
    {
        printf("i am process..father---pid:%d,ppid:%d\n", getpid(), getppid());
        sleep(1);
    }
    else
    {
        perror("fork error!");
    }
    return 0;
}