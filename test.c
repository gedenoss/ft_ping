// #include <unistd.h>
// #include <stdio.h>
// #include <signal.h>

// volatile sig_atomic_t stopp;

// void inthand(int signum) {
//     stopp = 1;
// }

// int main(int argc, char **argv) {

//     signal(SIGINT, inthand);

//     while (!stopp)
//     {
//         printf("loop\n");
//         printf("2\n");
//         sleep(1);

//     }

//     printf("exiting safely\n");

//     return 0;
// }