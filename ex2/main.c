//Shay Tzirin
//315314930


#include <stdio.h>
#include <memory.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <wait.h>

#define INPUT_LENGTH 1000
#define JOB_NUM 100
#define MAX_COMMAND 50
#define WAIT 1
#define DONTWAIT 0
#define PATH_SIZE 100
#define HOME "HOME"
#define TRUE 1
#define FALSE 0
#define EXIT 1
#define NO_EXIT 0


typedef struct job {
    pid_t pid;
    char command[INPUT_LENGTH];
} job;



/**
* split the string command to several arguements. if the last arg is '&' set the waiting flag to DONTWAIT and
 * make it null to point no more args, else the flag is already set to WAIT, then just set the next place to NULL
* @param arguments array to set with all arguments after splitting
* @param commandInput the string command - all arguments not seperated
* @param wait the waiting flag - might be change here to DONTWAIT
*/
void splitToArgs(char *arguments[INPUT_LENGTH], char *commandInput,char *cpyCommand, int* wait) {
    int i = 0;
    int last = strlen(commandInput) - 1;
    commandInput[last] = '\0';
    cpyCommand[last] = '\0';

    int quoteString = (*commandInput == '\"');
    char* currentQuo = strtok(commandInput, "\"");
    char* next = NULL;

    while(currentQuo)
    {
        if(quoteString)
        {
            arguments[i] = currentQuo;
            quoteString = FALSE;
            i++;
        }
        else
        {
            // saving the last strtok of the outer loop
            next = strtok(next, "\0");
            //splitting by " "
            char* currWord = strtok(currentQuo, " ");
            while(currWord)
            {
                arguments[i] = currWord;
                currWord = strtok(NULL, " ");
                i++;
            }
            quoteString = TRUE;
        }
        currentQuo = strtok(next, "\"");
        next = NULL;
    }
    //set the flag according to the last argument
    if (strcmp(arguments[i-1], "&") == 0) {
        *wait = DONTWAIT;
        arguments[i-1] = NULL;
    } else {
        arguments[i] = NULL;
    }
}


/**
 * initializing the jobs array
 * @param jobs a pointer to the jobs array
 */
void setJobs(job* jobs) {
    int i;
    for (i=0; i<JOB_NUM; i++) {
        jobs[i].pid = -1;
        strcpy(jobs[i].command, "");
    }
}

/**
 * prints all of the running jobs - those which still runs + ther pid isnt -1
 * @param jobs pointer to the jobs array
 */
void printRunningJobs (job *jobs) {
    int i;
    for (i = 0; i < JOB_NUM; i++) {
        if ((jobs[i].pid != -1) && ((waitpid(jobs[i].pid, NULL, WNOHANG)) == 0)) {
            printf("%d %s\n", jobs[i].pid, jobs[i].command);
        }
    }
}


/**
 * cd implemtation - deals with regular or complex cd ("~"/"-" flags)
 * @param args the original command splitted into char* array
 */
void cd(char **args) {
    static char prevPath[PATH_SIZE] = "";
    int cdSuccess;
    char temp[PATH_SIZE];

    // cd without arguments or only ~ - go back to home directory
    if ((args[1] == NULL) || (strcmp("~", args[1]) == 0)) {
        getcwd(prevPath, PATH_SIZE);
        chdir(getenv(HOME));
        //cd with "-" argument - jump back to the last directory and save
        //this directory for the next jump
    } else if ((strcmp(args[1], "-") == 0) && (args[2] == NULL)) {
        //if "cd -" is the first command of the prog - no prev path
        if (strcmp("", prevPath) == 0) {
            printf("%s\n", "cd: OLDPWD not set");
            return;
        }
        strcpy(temp, prevPath);
        getcwd(prevPath, PATH_SIZE);
        cdSuccess = chdir(temp);
        //checking success or failure
        if (cdSuccess == -1) {
            fprintf(stderr, "cannot set cd");
            printf("\n");
            strcpy(prevPath, temp);
        } else {
            printf("%s\n", temp);
        }
        //normal cd w specific directory argument
    } else {
        strcpy(temp, prevPath);
        getcwd(prevPath, PATH_SIZE);
        cdSuccess = chdir(args[1]);

        if (cdSuccess == -1) {
            fprintf(stderr, "cannot set cd");
            printf("\n");
            strcpy(prevPath, temp);
        }
    }
}

/**
 * executes the normal commands which should run  in a son process
 * @param args  commands seperated to char* array
 * @param cpyCommand saving the command without the "&" at the end
 * @param jobs jobs array
 * @param wait waiting flag
 */
void executeFork(char **args,char *cpyCommand, job *jobs, int wait) {
    pid_t pid;
    // create son process to execute the command
    pid = fork();
    //if the fork failed
    if (pid < 0) {
        fprintf(stderr, "Error in system call");
        return;
    }
    // son process - executes the command
    if (pid == 0) {
        if (strcmp(args[0], "cd") == 0) {
            printf("%d\n", getpid());
            cd(args);
        } else {
            execvp(args[0], args);
            //write to stderr beacuse the execution has failed - we know that
            // cause execvp doesnt supposed to get back to the program
            fprintf(stderr, "Error in system call");
            return;
        }
        //main process
    } else {
        //printing the child's pid
        printf("%d\n", pid);
        if (wait == WAIT) {
            waitpid(pid, NULL, 0);
        } else {
            for (int i = 0; i < JOB_NUM; i++) {
                //this job wasnt ever set or it finished it's running and get replaced
                if ((jobs[i].pid == -1) || ((waitpid(jobs[i].pid, NULL, WNOHANG)) == jobs[i].pid)) {
                    jobs[i].pid = pid;
                    strcpy(jobs[i].command, strtok(cpyCommand, "&"));
                    break;
                }
            }
        }
    }
}


/**
 * gets a command and checks whether it should be ruuning in a son procees or not
 * @param args command splitted
 * @param cpyCommand a coppy of the command
 * @param jobs jobs array
 * @param wait wait flag
 * @return letting the main know if it should exit the loop and exit the program
 */
int executeCommand(char **args, char *cpyCommand, job *jobs, int wait) {
    if (strcmp(args[0], "jobs") == 0) {
        printRunningJobs(jobs);
        return NO_EXIT;
    }
    if (strcmp(args[0], "exit") == 0) {
        printf("%d\n", getpid());
        for (int i=0; i < JOB_NUM; i++) {
            kill(jobs[i].pid, 0);
        }
        return EXIT;
    } else if (strcmp(args[0], "cd") == 0) {
        printf("%d\n", getpid());
        cd(args);
        return NO_EXIT;
    }
    executeFork(args, cpyCommand, jobs, wait);
    return  NO_EXIT;
}


int main() {
    int exit;
    job jobs[JOB_NUM];
    setJobs(jobs);
    int waiting;

    while (exit != EXIT) {
        printf("prompt> ");
        waiting = WAIT;
        char input[INPUT_LENGTH];
        fgets(input, INPUT_LENGTH, stdin);

        char cpyInput[INPUT_LENGTH];
        strcpy(cpyInput, input);
        char *args[MAX_COMMAND];
        splitToArgs(args, input, cpyInput, &waiting);

        exit = executeCommand(args, cpyInput, jobs, waiting);
    }
    return 0;
}

