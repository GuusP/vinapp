#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

int main(){
    mkdir("./aaa/bbb", 0777);
    return 0;
}