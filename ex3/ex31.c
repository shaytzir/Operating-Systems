/*
 * Shay Tzirn
 * 315314930
 */

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <fcntl.h>
#include <ctype.h>

#define ERROR "Error in system call\n"
#define ERRORLEN 21
#define TRUE 1
#define FALSE 0
#define FAIL -1
#define SIZE 1
#define IDENTICAL  3
#define SIMILAR 2


/**
 * the function write error into 2(STDERR) and exits with failure sign.
 */
void failure() {
    write(2, ERROR, ERRORLEN);
    exit(FAIL);

}

/**
 * the funcion gets the file descriptors of the files,
 * calls failure function and closes files f anything goes wrong.
 * @param file1desc first file - file descriptor
 * @param file1 first file's name
 * @param file2desc second file - file descriptor
 * @param file2 second file's name
 */
void getFileDescriptors(int *file1desc, char* file1,int *file2desc, char* file2) {
    *file1desc = open(file1, O_RDONLY);
    if(file1desc < 0) {
        failure();
    }
    *file2desc = open(file2, O_RDONLY);
    if (file2desc < 0) {
        close(*file1desc);
        failure();
    }
}

/**
 * closes both files when being given two files descriptors.
 * @param file1 first file descriptor
 * @param file2 second file descriptor
 */
void closeFiles(int file1, int file2) {
    close(file1);
    close(file2);
}

/**
 * the function checks if both given files are identical.
 * @param file1 first file's name
 * @param file2 second file's name
 * @return TRUE if identical, otherwise FALSE
 */
int areIdentical(char *file1, char *file2) {
    int file1desc, file2desc,read1, read2;
    int loop = TRUE;
    char buf1[SIZE];
    char buf2[SIZE];
    int flag = TRUE;
    getFileDescriptors(&file1desc, file1, &file2desc, file2);
    //loop comparing char by char between the two files
    while(loop == TRUE) {
        read1 = read(file1desc, buf1, SIZE);
        read2 = read(file2desc, buf2, SIZE);
        //if any failure occured
        if (read1==FAIL || read2==FAIL) {
            closeFiles(file1desc, file2desc);
            failure();
            //if one file finished before the other or they files differ in this specific char
        } else if ((*buf1 != *buf2) || (read1 == 0 && read2 != 0)
                   || (read1 != 0 && read2 == 0)) {
            flag = FALSE;
            break;
            //if both of them are done in the same time
        } else if (read1==0 && read2==0) {
            flag=TRUE;
            break;
        }

    }
    //close the files properly and return
    closeFiles(file1desc, file2desc);
    return flag;
}

/**
 * by given a file descriptor - keep reading untill the
 * current char isnt space or new line and the file isnt over
 * @param desc file descriptor to read from
 * @param reader "status" representation of the reading
 * @return returns the last readed char
 */
char ignoreSpecial(int desc, int* reader) {
    char buf[SIZE];
    do {
        *reader = read(desc, buf, SIZE);
        if (*reader==FAIL) {
            return FAIL;
        }
    } while ((*reader!=0)&&((*buf ==' ') || (*buf == '\n')));
    return *buf;

}

/**
 * checks if the files are similar - when lowercasing all text and
 * ignoring spaces and new lines.
 * @param file1 first file's name
 * @param file2 second file's name
 * @return TRUE if similar, otherwise FALSE
 */
int areSimilar(char* file1, char* file2) {
    int file1desc, file2desc,read1, read2;
    int loop = TRUE;
    char buf1[SIZE];
    char buf2[SIZE];
    int flag = TRUE;
    getFileDescriptors(&file1desc, file1, &file2desc, file2);
    while(loop == TRUE) {
        read1 = read(file1desc, buf1, SIZE);
        *buf1 = tolower(*buf1);
        read2 = read(file2desc, buf2, SIZE);
        *buf2 = tolower(*buf2);
        //if both of them are done at the same time
        if (read1==0 && read2==0) {
            flag=TRUE;
            break;
            //if any failure occured
        } else if (read1==FAIL || read2==FAIL) {
            closeFiles(file1desc, file2desc);
            failure();
            //if first file is done before second
        } else if (read1 == 0 && read2 != 0) {
            //ignore the spaces and new lines
            //of the second file
          *buf2 = ignoreSpecial(file2desc, &read2);
          if (read2 == FAIL) {
              closeFiles(file1desc, file2desc);
              failure();
          }
          //and check that there are no more files to the second file
            if (read2 != 0) {
                flag = FALSE;
            }
            break;
            //if the second file is done before first
        } else if (read1 != 0 && read2 == 0) {
            //ignore spaces and new lines in first file
           *buf1 =ignoreSpecial(file1desc, &read1);
           //check there are no more chars to the first one
            if (read2 == FAIL) {
                closeFiles(file1desc, file2desc);
                failure();
            }
            if (read1 != 0) {
                return FALSE;
            }
            break;
            //if after lowercasing them they differ
        } else if (*buf1 != *buf2) {
            //ignore both file spaces - a check will be made in the next iteration
            *buf1 = ignoreSpecial(file1desc, &read1);
            *buf2 = ignoreSpecial(file2desc, &read2);
        }

    }
    //close the files and return answer
    closeFiles(file1desc, file2desc);
    return flag;

}

int main(int argc, char **argv) {
    //if there are not enough arguments
    if (argc != 3) {
        failure();
    }
    //if the files are identical
    if (areIdentical(argv[1], argv[2]) == TRUE) {
        return IDENTICAL;
        //else if they are similar
    } else if (areSimilar(argv[1], argv[2]) == TRUE) {
        return SIMILAR;
        //else they are different
    } else {
        return 1;
    }
}
