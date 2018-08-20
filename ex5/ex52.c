/*
 * Lee Alima
 * 313467441
 */

#include <stdio.h>
#include <signal.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdbool.h>

#define SIZEOFBOARD 20
#define LEFT 'a'
#define RIGHT 'd'
#define DOWN 's'
#define ROTATE 'w'
#define QUIT 'q'
#define VERTICAL 0
#define HORIZONTAL 1

// saves date about the board game
typedef struct Board{
    char board[SIZEOFBOARD][SIZEOFBOARD];
    int yPos;
    int xPos;
    int direction;
} Board;

// global variable
Board board;

void startGame();
void initializeBoard();
void displayBoard();
void updateBoard();
void moveLeft();
void moveRight();
void moveDown();
void rotate();
void handleSignal();
void alarmMoveDown();

void moveByKey(char aChar);

bool noPlaceToGoDown();

int main() {
    startGame();
}

/**
 * This function creates a new lay player in the upper middle point
 * of the board, fills the board and handles signals - SIGUSR2 and SIGALRM
 */
void startGame(){
    // creates a new player
    board.yPos = 0;
    board.xPos = SIZEOFBOARD/2;
    board.direction = HORIZONTAL;

    initializeBoard(board);
    displayBoard();
    // listen to signal SIGALRM
    signal(SIGALRM, alarmMoveDown);
    alarm(1);
    // listen to signal SIGUSR2
    signal(SIGUSR2, handleSignal);
    while (1) { //keep running
        pause();
    }
}

/**
 * This funcion initialize the board an fill all the
 * cells as was asked in the Targil
 */
void initializeBoard(){
    int i,j;
    for (i = 0 ; i<SIZEOFBOARD ; i++){
        for(j = 0 ; j<SIZEOFBOARD ; j++){

            if (j==0||j==SIZEOFBOARD-1||i==SIZEOFBOARD-1) {
                board.board[i][j] = '*';
            } else {
                board.board[i][j] = ' ';
            }
        }
    }
}

/**
 * This function prints the board
 */
void displayBoard(){

    switch(board.direction) {
        case HORIZONTAL:
            board.board[board.yPos][board.xPos-1] = '-';
            board.board[board.yPos][board.xPos] = '-';
            board.board[board.yPos][board.xPos+1] = '-';
            break;
        case VERTICAL:
            board.board[board.yPos-1][board.xPos] = '-';
            board.board[board.yPos][board.xPos] = '-';
            board.board[board.yPos+1][board.xPos] = '-';
            break;

    }
    // clearing the screen (other boards)
    system("clear");
    int i,j;
    for (i = 0 ; i<SIZEOFBOARD ; i++){
        for(j = 0 ; j<SIZEOFBOARD ; j++){
            printf("%c", board.board[i][j]);
        }
        printf("\n"); // next line
    }
}


/**
 * This funcion moves the player left
 */
void moveLeft() {

    switch (board.direction) {
        case HORIZONTAL:
            if (board.xPos > 2){
                board.xPos-=1;
            }
            break;
        case VERTICAL:
            if (board.xPos > 1){
                board.xPos-=1;
            }
            break;
    }
    /*if (board.direction == VERTICAL){ // VERTICAL
        if (board.xPos > 1){
            board.xPos-=1;
        }
    }else { // HORIZONTAL
        if (board.xPos > 2){
            board.xPos-=1;
        }
    }*/
}

/**
 * This funcion moves the player right
 */
void moveRight() {

    switch (board.direction) {
        case HORIZONTAL:
            if (board.xPos < SIZEOFBOARD -3 ){
                board.xPos+=1;
            }
            break;
        case VERTICAL:
            if (board.xPos < SIZEOFBOARD-2){
                board.xPos+=1;
            }
            break;
    }

   /* if (board.direction == VERTICAL){ // VERTICAL
        if (board.xPos < SIZEOFBOARD-2){
            board.xPos+=1;
        }
    }else { // HORIZONTAL
        if (board.xPos < SIZEOFBOARD -3 ){
            board.xPos+=1;
        }
    }*/
}

/**
 * This funcion moves the player down
 */
void moveDown() {
    // if player should be cleared
    if (noPlaceToGoDown()) {
        board.direction = HORIZONTAL;
        board.yPos = 0;
        board.xPos = SIZEOFBOARD / 2;
    } else {
        switch (board.direction) {
            case HORIZONTAL:
                if (board.yPos < SIZEOFBOARD - 2) {
                    board.yPos++;
                }
                break;
            case VERTICAL:
                if (board.yPos < SIZEOFBOARD - 3) {
                    board.yPos++;
                }
                break;
        }

    }

    bool noPlaceToGoDown() {
        if (board.direction == HORIZONTAL && board.yPos >= SIZEOFBOARD - 2) {
            return 1;
        } else if (board.direction == VERTICAL && board.yPos >= SIZEOFBOARD - 3) {
            return 1;
        } else {
            return 0;
        }
    }

/**
 * This function flips the player shape
 */
    void rotate() {
        if (board.direction == VERTICAL) {
            if (board.xPos < SIZEOFBOARD - 2 && board.xPos > 1) { // can flip
                board.direction = HORIZONTAL;
            }
        } else {
            if (board.yPos < SIZEOFBOARD - 2 && board.yPos > 0) { // can flip
                board.direction = VERTICAL;
            }
        }
    }

/**
 * This function clear the player '-' cells from the board
 */
    void removeTheLine() {
        switch (board.direction) {
            case VERTICAL:
                board.board[board.yPos - 1][board.xPos] = ' ';
                board.board[board.yPos][board.xPos] = ' ';
                board.board[board.yPos + 1][board.xPos] = ' ';
                break;
            case HORIZONTAL:
                board.board[board.yPos][board.xPos - 1] = ' ';
                board.board[board.yPos][board.xPos] = ' ';
                board.board[board.yPos][board.xPos + 1] = ' ';
                break;
        }
    }

/**
 * This function handle signals from ex51.c -
 * reading chars from the pipe instead of stdin
 */
    void handleSignal() {
        char pipeChar;
        scanf("%c", &pipeChar); // get char from pipe
        removeTheLine();
        if (pipeChar == QUIT) {
            exit(0);
        }
        moveByKey(pipeChar);
        displayBoard();
        // get next signal
        signal(SIGUSR2, handleSignal);
    }

    void moveByKey(char key) {
        switch (key) {
            case DOWN:
                moveDown();
                break;
            case LEFT:
                moveLeft();
                break;
            case RIGHT:
                moveRight();
                break;
            case ROTATE:
                rotate();
                break;
        }

    }

/**
 * This function is called every 1 second and calls moveDown
 */
    void alarmMoveDown() {
        signal(SIGALRM, alarmMoveDown); // listen to SIGALRM
        alarm(1); // next moving down
        removeTheLine();
        moveDown();
        displayBoard();
    }
}

