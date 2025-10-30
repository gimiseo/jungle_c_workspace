#include <stdio.h>
#include <stdlib.h>

int main() {
    int x; // 스택에 정수 x를 위한 공간만 할당됨 (쓰레기 값)
    int y;

    if (x > 10) { // 🚨 Valgrind가 여기서 경고!
        y = 1;
    } else {
        y = 2;
    }

    printf("y = %d\n", y); // y의 값도 쓰레기 값에 의해 결정됨
    
    return 0;
}