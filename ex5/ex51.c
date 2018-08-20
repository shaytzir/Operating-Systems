#include <stdio.h>
#include <unistd.h>
#include <termios.h>
#include <signal.h>
#include <stdlib.h>


#define LEFT 'a'
#define RIGHT 'd'
#define DOWN 's'
#define ROTATE 'w'
#define QUIT 'q'
#define ARGSNUM 2
#define STDERR 2
#define TETRISOUT "./draw.out"

char getch() {
    char buf = 0;
    struct termios old = {0};
    if (tcgetattr(0, &old) < 0)
        perror("tcsetattr()");
    old.c_lflag &= ~ICANON;
    old.c_lflag &= ~ECHO;
    old.c_cc[VMIN] = 1;
    old.c_cc[VTIME] = 0;
    if (tcsetattr(0, TCSANOW, &old) < 0)
        perror("tcsetattr ICANON");
    if (read(0, &buf, 1) < 0)
        perror ("read()");
    old.c_lflag |= ICANON;
    old.c_lflag |= ECHO;
    if (tcsetattr(0, TCSADRAIN, &old) < 0)
        perror ("tcsetattr ~ICANON");
    return (buf);
}

/**
 * prints error in sys call to stderr.
 */
void fail() {
    char* errorMsg = "Error in system call\n";
    int errorLength = sizeof(errorMsg);
    write(STDERR, errorMsg, errorLength);
    exit(-1);
}

int validPlayKey(char key) {
    if (key == LEFT || key == RIGHT || key == DOWN || key == ROTATE) {
        return 1;
    }
    return 0;
}

int main() {
    int Pipe[2];
    pipe(Pipe);
    int pid;

    if ((pid = fork()) < 0) {
        fail();
    }

    // Child process
    if (pid == 0) {
        // Force our stdin to be the read size of the pipe we made
        dup2(Pipe[0], 0);
        char* args[ARGSNUM]={TETRISOUT, NULL};
        execlp(args[0], &args[0]);
        //if the child return - there was an error
        fail();
    }

    //Father Process
    char readChar;
    while ((readChar = getch()) != QUIT) {
        if (validPlayKey(readChar)){
            if (write(Pipe[1], &readChar, 1) < 0) {
                fail();
            }

            if (kill(pid, SIGUSR2) < 0) {
                fail();
            }
        }

    }

    return 0;
}