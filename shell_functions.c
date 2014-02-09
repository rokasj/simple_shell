#include "shell_functions.h"

void initShell() {
    /* Tikrinama ar shell paleistas interaktyviai */
    shellTerminal = STDIN_FILENO;
    shellIsInteractive = isatty(shellTerminal);

    if (shellIsInteractive) {
        /* Jei taip vykdomas ciklas kol shell nera "foreground" rezime */
        while (tcgetpgrp(shellTerminal) != (shellPgid = getpgrp()))
            kill (- shellPgid, SIGTTIN);

        /* Ingoruojami interaktyvus ir job-control signalai */
        signal(SIGINT, SIG_IGN);
        signal(SIGQUIT, SIG_IGN);
        signal(SIGTSTP, SIG_IGN);
        signal(SIGTTIN, SIG_IGN);
        signal(SIGTTOU, SIG_IGN);
        signal(SIGCHLD, SIG_IGN);

        /* Shell (apvalkalas) priskiriamas savo paties procesu grupei */
        shellPgid = getpid();
        if (setpgid(shellPgid, shellPgid) < 0) {
            perror("Couldn't put the shell in its own process group");
            exit(EXIT_FAILURE);
        }

        /* Apvalkalui perduodamas terminalo valdymas */
        tcsetpgrp(shellTerminal, shellPgid);

        /* Apvalkalui issaugojami numatytieji terminalo atributai */
        tcgetattr (shellTerminal, &shellTmodes);
    }
}

int isBuiltInCommand(char **cmd) {
    int rvalue = 0;
    if (strcmp("exit", cmd[0]) == 0) {
        exit(EXIT_SUCCESS);
        rvalue = 1;
    }
    if (strcmp("cd", cmd[0]) == 0) {
        changeDirectory(cmd);
        rvalue = 1;
    }
    return rvalue;
}

void cmdArgumentsCheck(char **cmd) {
    int i = 0;
    int j = 0;

    while (cmd[i] != NULL) {
        if ('$' == cmd[i][0]) {
            while (cmd[i][j] != '\0') {
                cmd[i][j] = cmd[i][j+1];
                j++;
            }
            if (atoi(cmd[i]) != 0) 
                cmd[i] = cmd[atoi(cmd[i])];
            else 
                cmd[i] = " ";
        }
        i++;
        j = 0;
    }
}

void redirectionCheck(char **cmd) {
    int i = 0;
    int j = 0;

    /*int inDescriptor;
    int outDescriptor;
    int errDescriptor;*/

    FILE *inDescriptor;
    FILE *outDescriptor;
    FILE *errDescriptor;

    while (cmd[i] != NULL) {
        if (strcmp("<", cmd[i]) == 0) {
            if (cmd[i+1] != NULL) {
                /*inDescriptor = open(cmd[i+1], O_RDONLY, 0600);
                dup2(inDescriptor, STDIN_FILENO);
                close(inDescriptor);*/
                if (strcmp("0", cmd[i-1]) == 0) {
                    inDescriptor = fopen(cmd[i+1], "r");
                    dup2(fileno(inDescriptor), STDIN_FILENO);
                    close(fileno(inDescriptor));
                    deleteCmdArgument(i-1);
                    deleteCmdArgument(i-1);
                } else {
                    inDescriptor = fopen(cmd[i+1], "r");
                    dup2(fileno(inDescriptor), STDIN_FILENO);
                    close(fileno(inDescriptor));
                    deleteCmdArgument(i);
                }
                
            }
        } else if (strcmp(">", cmd[i]) == 0) {
            if (cmd[i+1] != NULL) {
                /*outDescriptor = open(cmd[i+1], O_CREAT | O_TRUNC | O_WRONLY, 0600);
                dup2(outDescriptor, STDOUT_FILENO);
                close(outDescriptor);*/
                if (strcmp("0", cmd[i-1]) == 0) {
                    inDescriptor = fopen(cmd[i+1], "w");
                    dup2(fileno(inDescriptor), STDIN_FILENO);
                    close(fileno(inDescriptor));
                    deleteCmdArgument(i-1);
                    deleteCmdArgument(i-1);
                    deleteCmdArgument(i-1);
                } else if (strcmp("1", cmd[i-1]) == 0) {
                    outDescriptor = fopen(cmd[i+1], "w");
                    dup2(fileno(outDescriptor), STDOUT_FILENO);
                    close(fileno(outDescriptor));
                    deleteCmdArgument(i-1);
                    i -= 2;
                } else if (strcmp("2", cmd[i-1]) == 0) {
                    errDescriptor = fopen(cmd[i+1], "w");
                    dup2(fileno(errDescriptor), STDERR_FILENO);
                    close(fileno(errDescriptor));
                    deleteCmdArgument(i-1);
                    deleteCmdArgument(i-1);
                    deleteCmdArgument(i-1);
                } else if (atoi(cmd[i-1]) != 0) {
                    outDescriptor = fopen(cmd[i+1], "w");
                    dup2(fileno(outDescriptor), atoi(cmd[i-1]));
                    close(fileno(outDescriptor));
                    deleteCmdArgument(i-1);
                    deleteCmdArgument(i-1);
                    deleteCmdArgument(i-1);
                } else {
                    outDescriptor = fopen(cmd[i+1], "w");
                    dup2(fileno(outDescriptor), STDOUT_FILENO);
                    close(fileno(outDescriptor));
                    deleteCmdArgument(i);
                    deleteCmdArgument(i);
                }
            }
        } else if (strcmp(">>", cmd[i]) == 0) {
            if (cmd[i+1] != NULL) {
                /*inDescriptor = open(cmd[i+1], O_WRONLY | O_APPEND);
                dup2(inDescriptor, STDIN_FILENO);
                close(inDescriptor);*/
                if (strcmp("0", cmd[i-1]) == 0) {
                    inDescriptor = fopen(cmd[i+1], "a");
                    dup2(fileno(inDescriptor), STDIN_FILENO);
                    close(fileno(inDescriptor));
                    deleteCmdArgument(i-1);
                    deleteCmdArgument(i-1);
                    deleteCmdArgument(i-1);
                } else if (strcmp("1", cmd[i-1]) == 0) {
                    outDescriptor = fopen(cmd[i+1], "a");
                    dup2(fileno(outDescriptor), STDOUT_FILENO);
                    close(fileno(outDescriptor));
                    deleteCmdArgument(i-1);
                    i--;
                } else if (strcmp("2", cmd[i-1]) == 0) {
                    errDescriptor = fopen(cmd[i+1], "a");
                    dup2(fileno(errDescriptor), STDERR_FILENO);
                    close(fileno(errDescriptor));
                    deleteCmdArgument(i-1);
                    i -= 2;
                } else if (atoi(cmd[i-1]) != 0) {
                    outDescriptor = fopen(cmd[i+1], "a");
                    dup2(fileno(outDescriptor), atoi(cmd[i-1]));
                    close(fileno(outDescriptor));
                    deleteCmdArgument(i-1);
                    deleteCmdArgument(i-1);
                    deleteCmdArgument(i-1);
                } else {
                    outDescriptor = fopen(cmd[i+1], "a");
                    dup2(fileno(outDescriptor), STDOUT_FILENO);
                    close(fileno(outDescriptor));
                    deleteCmdArgument(i);
                    deleteCmdArgument(i);
                }
            }
        }
        i++;
    }
}

void deleteCmdArgument(int n) {
    while (cmd[n+1] != NULL) {
        cmd[n] = cmd[n+1];
        n++;
    }
    cmd[n] = NULL;
}

void executeCommand(char **cmd) {
    cmdArgumentsCheck(cmd);
    redirectionCheck(cmd);
    if ((execvp(cmd[0], cmd)) == -1) {
        printf ("%s %s\n", cmd[0], ": command not found");
        exit(EXIT_FAILURE);
    }
}

void getLine(char *line) {
    char character;
    int i = 0;
    while ((line[i] = getc(stdin)) != '\n') i++;
    line[i] = '\0';
}

void splitLine(char *line, char **splitLine) {
    int i = 1;
    splitLine[0] = strtok(line, " ");
    while ((splitLine[i] = strtok(NULL, " ")) != NULL) i++;
    
}

void clearCmd(char **cmd) {
    int i;
    for (i = 0; i < MAX_CMD_ATTRIBUTES; i++) cmd[i] = NULL;
}

void printPrompt() {
    printf("%s:%s> ", getenv("LOGNAME"), getcwd(currentDirectory, 1024));
}

void changeDirectory(char **cmd) {
    if (cmd[1] == NULL) {
        chdir(getenv("HOME"));
    } else {
        if (chdir(cmd[1]) == -1)
            printf("%s: no such directory\n", cmd[1]);
    }
}
