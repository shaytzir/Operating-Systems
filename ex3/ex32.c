/*
 * Shay Tzirin
 * 315314930
 */



#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <dirent.h>
#include <memory.h>
#include <wait.h>

//define
#define FAIL -1
#define TRUE 1
#define FALSE 0
#define STUDENT_NUMBER 100
#define BUFF_SIZE 1024
#define ERROR "Error in system call\n"
#define STDERR 2
#define ERROR_SIZE 21
#define INPUT_SIZE 160
#define STUDENT_PROG "compiledStudProg.out"
#define STUDENT_OUTPUT "studOutput.txt"
#define ROW_LENGTH 1024
#define SPACE 32
#define NO_C_FILE "0"
#define NO_C_FILE_MESSAGE "NO_C_FILE"
#define COMPILATION_ERROR "0"
#define COMPILATION_ERROR_MESSAGE "COMPILATION_ERROR"
#define TIMEOUT_GRADE "0"
#define TIMEOUT_MESSAGE "TIMEOUT"
#define BAD_OUTPUT_GRADE "60"
#define BAD_OUTPUT_MESSAGE "BAD_OUTPUT"
#define SIMILAR_OUTPUT_GRADE "80"
#define SIMILAR_OUTPUT_MESSAGE "SIMILAR_OUTPUT"
#define GREAT_JOB_GRADE "100"
#define GREAT_JOB_MESSAGE "GREAT_JOB"



typedef struct student {
    char name[INPUT_SIZE];
    char cfileDirPath[INPUT_SIZE];
    char cFilePath[INPUT_SIZE];
    char grade[INPUT_SIZE];
    char reason[INPUT_SIZE];
} student;

/**
 * write error and exits the program
 */
void fail() {
    write(STDERR, ERROR, ERROR_SIZE);
    exit(FAIL);
}

/**
 * writes the students name grade and reasons to csv file
 * @param students list of all students
 * @param numOfStudents number of the students filled in the array
 */
void csvResults(student *students, int numOfStudents){
    //open/create a csv file called "results.csv"
    int csvId = open("results.csv",O_CREAT | O_TRUNC | O_WRONLY, 0644);
    //if the opening failed - call the appropriate function
    if(csvId== FAIL)
    {
        fail();
    }
    for (int i=0; i<numOfStudents; i++) {
        char row[ROW_LENGTH] = "";
        strcat(row, students[i].name);
        strcat(row, ",");
        strcat(row, students[i].grade);
        strcat(row, ",");
        strcat(row, students[i].reason);
        strcat(row, "\n");
        //if the writing failed
        if (write(csvId, row, ROW_LENGTH) < 0) {
            fail();
        }
    }
    //if closing failed
    if (close(csvId)==FAIL) {
        fail();
    }
}

/**
 * splits the configuration file by it's rows into the matching arrays
 * @param directoryPath the diectory path
 * @param inputFilePath the path of the input file
 * @param outputFilePath the path of the output file
 * @param congFile the path of the configuration file
 */
void splitToRows(char directoryPath[INPUT_SIZE], char inputFilePath[INPUT_SIZE],char outputFilePath [INPUT_SIZE],char* congFile) {
    int numRead;
    //open files
    int file = open(congFile, O_RDONLY);
    if (file < 0) {
        fail();
    }
    char buffer[BUFF_SIZE];
    //read bytes into the buffer
    while((numRead=read(file,buffer,sizeof(buffer)))>0)
    {
        //starting to read from the first row
        int row=1;
        int i,j;
        i=0;
        j=0;
        //writing by char into the relavant input arryas
        while(i<numRead) {
            if (buffer[i]!='\n'&&buffer[i]!=SPACE&&buffer[i]!='\0') {
                //choosing relavant array by the row
                if (row==1) {
                    directoryPath[j] = buffer[i];
                } else if(row==2) {
                    inputFilePath[j] = buffer[i];
                } else if(row==3) {
                    outputFilePath[j] = buffer[i];
                }
                j++;
            } else{
                row++;
                j=0;
            }
            i++;
        }
    }
    if (numRead < 0) {
        fail();
    }
}

/**
 * generating path by the directory path given and combining it from extracting the name of the dirent
 * @param subDir array to save the path generated
 * @param directoryPath main path
 * @param pDirent extracting name to combine
 */
void createSubDirPath(char subDir[INPUT_SIZE],char directoryPath[INPUT_SIZE],struct dirent* pDirent) {
    int length = strlen(directoryPath);
    const char* lastchar = &directoryPath[length-1];
    strcpy(subDir, directoryPath);
    //if its not ending w "/" already - add
    if (strcmp(lastchar,"/") != 0) {
        strcat(subDir, "/");
    }
    //combine
    strcat(subDir, pDirent->d_name);
}

/**
 * generating path by combining char arrays
 * @param result array to save the new path
 * @param directoryPath main path
 * @param sub sub path to combine
 */
void createSubDirPathChars(char result[INPUT_SIZE], char directoryPath[INPUT_SIZE], char sub[INPUT_SIZE]) {
    int length = strlen(directoryPath);
    const char* lastchar = &directoryPath[length-1];
    strcpy(result, directoryPath);
    if (strcmp(lastchar,"/") != 0) {
        strcat(result, "/");
    }
    strcat(result, sub);

}

/**
 * looking for the c file.
 * @param dirToSearchC path to look for c file
 * @param students all students array
 */
void searchCFile(char *dirToSearchC, student *students){
    DIR* dip;
    struct dirent* dit;
    //if there failure with opening the directory
    if((dip=opendir(dirToSearchC))==NULL){
        fail();
    }
    //read from the directory - may read another dir or normal file
    while ((dit=readdir(dip))!=NULL) {
        //if its a regular file - checks if its a c file
        if (dit->d_type == DT_REG) {
            char* extension;
            extension = strrchr(dit->d_name,'.');
            //if it is - update the relavant fields in the student struct
            if (strcmp(extension, ".c") == 0) {
                strcpy(students->cfileDirPath, dirToSearchC);
                strcpy(students->cFilePath, dit->d_name);
                break;
            }
            //if its  a directory (other than "." or "..")
        } else if (dit->d_type == DT_DIR && strcmp(dit->d_name, ".") != 0 && strcmp(dit->d_name, "..") != 0) {
            //create the new directory to look in
            char subDirectory[INPUT_SIZE]={};
            createSubDirPath(subDirectory,dirToSearchC,dit);
            //and re-call the function
            searchCFile(subDirectory, students);
        }
    }
}


/**
 * look for information for each student in the students list
 * @param directoryPath directory of all students
 * @param myStudents the students list
 * @param studentIndex number of students-may be updated
 */
void getInfoFromStudentsDir(char *directoryPath, student *myStudents, int *studentIndex) {
    DIR* pDir;
    struct dirent* pDirent;
    if((pDir=opendir(directoryPath))==NULL){
        fail();
    }
    int sizeOfStudentsArr = STUDENT_NUMBER;
    //reading from the directory
    while ((pDirent=readdir(pDir))!=NULL) {
        //if its a directory other than "." and ".."
        if (strcmp(pDirent->d_name, ".") != 0 && strcmp(pDirent->d_name, "..") != 0 && pDirent->d_type == DT_DIR) {
            //fill the struct w the name of the student
            strcpy(myStudents[*studentIndex].name, pDirent->d_name);
            //creates the path "studentDIR/studentsname
            char subDir[INPUT_SIZE] = {};
            createSubDirPath(subDir, directoryPath,pDirent);
            //fills the cfile path and the Dir the cfile is in
            searchCFile(subDir, &myStudents[*studentIndex]);
            //indrease index to the other student
            (*studentIndex)++;
            //realloc if the first generated list wasnt enough
            if ((*studentIndex)>=sizeOfStudentsArr) {
                sizeOfStudentsArr+=STUDENT_NUMBER;
                myStudents = (student *)realloc(myStudents,sizeof(struct student)*sizeOfStudentsArr);
                if (myStudents==NULL){
                    fail();
                }
            }
        }
    }
    if(closedir(pDir)==FAIL){
        fail();
    }
}

/**
 * filling the args for the compiling command
 * @param args args array to fill
 * @param path path of file to compile
 */
void fillArgs(char* args[INPUT_SIZE],char path[INPUT_SIZE]) {
    args[0] = "gcc";
    args[1] = "-o";
    args[2] = STUDENT_PROG;
    args[3] = path;
    args[4] = NULL;
}

/**
 * compiling the student's c file by given arguments
 * @param args arguments to run to execvp
 */
void runCompilation(char** args) {
    int indication;
    pid_t pid;
    pid = fork();
    if (pid == 0) {
        indication = execvp(args[0], args);
        if (indication == FAIL) {
            fail();
        }
    } else {
        waitpid(pid, NULL, WCONTINUED);
    }
}

/**
 * checks if the last compilation successed
 * @return true if succeded. otherwise false
 */
int checkCompliationSuccess() {
    DIR *pDir;
    struct dirent *pDirent;
    char cwd[BUFF_SIZE];
    getcwd(cwd, sizeof(cwd));
    if ((pDir = opendir(cwd)) == NULL) {
        fail();
    }
    //looking for a compilation reasult called STUDENT_PROG
    while ((pDirent = readdir(pDir)) != NULL) {
        if (strcmp(pDirent->d_name, STUDENT_PROG) == 0) {
            closedir(pDir);
            return TRUE;
        }
    }
    //otherwise return false
    closedir(pDir);
    return FALSE;
}

void openRelevantFiles(int *input, int *output, char inputPath[INPUT_SIZE]) {
    *input = open(inputPath, O_RDONLY);
    if (input < 0) {
        fail();
    }
    *output = open(STUDENT_OUTPUT,  O_CREAT | O_TRUNC | O_WRONLY, 0644);
    if (output < 0) {
        close(*input);
        fail();
    }
}

/**
 * changing the sream w dup
 * @param input input
 * @param output output
 */
void changeWithDup(int *input, int *output) {
    int stat = dup2(*input, 0);
    if (stat == FAIL) {
        close(*output);
        unlink(STUDENT_OUTPUT);
        close(*input);
        fail();
    }
    stat = dup2(*output, 1);
    if (stat == FAIL) {
        close(*output);
        unlink(STUDENT_OUTPUT);
        close(*input);
        fail();
    }

}


/**
 * runs the the comp.out program and returns its result
 * @param outputPath output path of the right reault to compare with
 * @return comparison output
 */
int compareOutputs(char outputPath[INPUT_SIZE]) {
    //make args for comp.out
    char *args[INPUT_SIZE];
    int stat, value;
    char command[INPUT_SIZE] = "./comp.out";
    args[0] = command;
    args[1] = STUDENT_OUTPUT;
    args[2] = outputPath;
    args[3] = NULL;
    pid_t pid;
    pid = fork();
    //son calls the running
    if (pid == 0) {
        stat = execvp(args[0], &args[0]);
        if (stat == FAIL) {
            fail();
        }
        //father waits and return the result of the comparison
    } else {
        waitpid(pid, &value, 0);
        int comparison = WEXITSTATUS(value);
        if (comparison == FAIL) {
            fail();
            return comparison;
        }
    }
}

/**
 * runs the student c file for getting his output file
 * @param stud student
 * @param inputPath input for his prog
 * @param outputPath output for his prog
 */
void runStudentCompiled(student *stud, char inputPath[INPUT_SIZE], char outputPath[INPUT_SIZE]) {
    pid_t pid;
    pid = fork();
    int input, output, stat;
    //son
    if (pid == 0) {
        char command[INPUT_SIZE] = {};
        strcpy(command,"./");
        strcat(command,STUDENT_PROG);
        char *args[INPUT_SIZE];
        args[0] = command;
        args[1] = NULL;
        openRelevantFiles(&input, &output, inputPath);
        changeWithDup(&input, &output);
        //run the student compiled file
        stat = execvp(args[0], args);
        if (stat == FAIL) {
            close(input);
            unlink(STUDENT_PROG);
            unlink(STUDENT_OUTPUT);
            close(output);
            fail();
        }
        if ((close(input) == FAIL) || (close(output)) == FAIL) {
            fail();
        }
        //father
    } else {
        int value;
        sleep(5);
        pid_t returnPid = waitpid(pid, &value, WNOHANG);
        //if the student compiled project runs more than 5 seconds
        if (returnPid == 0) {
            strcpy(stud->grade, TIMEOUT_GRADE);
            strcpy(stud->reason, TIMEOUT_MESSAGE);
        } else {
            //if it was shorter than 5 seconds
            //get the comparison value
            int result = compareOutputs(outputPath);
            switch (result) {
                //if they are differnt
                case 1:
                    strcpy(stud->grade, BAD_OUTPUT_GRADE);
                    strcpy(stud->reason, BAD_OUTPUT_MESSAGE);
                    break;
                    //if they are simialr
                case 2:
                    strcpy(stud->grade, SIMILAR_OUTPUT_GRADE);
                    strcpy(stud->reason, SIMILAR_OUTPUT_MESSAGE);
                    break;
                    //if they are identical
                case 3:
                    strcpy(stud->grade, GREAT_JOB_GRADE);
                    strcpy(stud->reason, GREAT_JOB_MESSAGE);
                    break;
                default:
                    break;
            }
            if (unlink(STUDENT_OUTPUT) == FAIL || unlink(STUDENT_PROG)==FAIL) {
                fail();
            }
        }
    }
}

/**
 * grading the students
 * @param students students list
 * @param numOfStudents amount of students
 * @param input input path
 * @param output output path
 */
void gradesAndReasons(student *students, int numOfStudents, char input[INPUT_SIZE], char output[INPUT_SIZE]) {
    for (int i=0; i<numOfStudents; i++) {
        student *current = &students[i];

        if (strcmp(current->cFilePath, "\0") == 0 ) {
            strcpy(current->grade, NO_C_FILE);
            strcpy(current->reason, NO_C_FILE_MESSAGE);
        } else {
            char fullCPath[INPUT_SIZE] = {};
            createSubDirPathChars(fullCPath, current->cfileDirPath, current->cFilePath);
            char* args[INPUT_SIZE];
            fillArgs(args,fullCPath);
            runCompilation(args);
            if (!checkCompliationSuccess()) {
                strcpy(current->grade, COMPILATION_ERROR);
                strcpy(current->reason, COMPILATION_ERROR_MESSAGE);
            } else {
                runStudentCompiled(current,input,output);
            }

        }
    }
}
/**
 * main function.
 * runs the program.
 * @param argc - amount of arguments.
 * @param argv - the arguments.
 * @return
 */
int main(int argc, char **argv) {
    //the index keeps track of the amount of students
    int studentIndex = 0;
    //if the amount of inputs isnt 2 (name and configuration path)
    if (argc != 2) {
        fail();
    }

    char dirsPath[INPUT_SIZE] = {};
    char inputPath[INPUT_SIZE] = {};
    char outputPath[INPUT_SIZE] = {};
    //calls the function to split the configuration file into the matching arrays
    splitToRows(dirsPath, inputPath, outputPath,argv[1]);
    //creating a dynamic list of students
    student* students = (student *)malloc(STUDENT_NUMBER* sizeof(student));
    if (students == NULL) {
        fail();
    }
    //calls function to search informations in the students directories
    getInfoFromStudentsDir(dirsPath, students, &studentIndex);
    //give grades and matching reason for it for all students
    gradesAndReasons(students,studentIndex,inputPath,outputPath);
    //write the results into a file
    csvResults(students,studentIndex);
    //free the students list
    free(students);
    return 0;
}