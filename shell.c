#include "shell_functions.h"

int main(int argc, char **argv) {

    initShell();                                                     //Shell (apvalkalo) inicializavimas

    while (1) {
        printPrompt();                                               //Prompt (pakvietimo) rodymas
        getLine(cmdLine);
        if ((cmdLine[0] != '\0') && (cmdLine[0] != ' ')) {
            clearCmd(cmd);
            splitLine(cmdLine, cmd);
            if (isBuiltInCommand(cmd)) {
            } else {		
                childPid = fork();
                if(childPid == -1) {
                    printf("can't fork, error : %d\n", errno);
                } else if (childPid == 0) {
                    signal(SIGINT, SIG_DFL);                         //Signalu apdorojimo veiksmams ("signal handlers")
                    signal(SIGQUIT, SIG_DFL);                        //priskiriamas numatytasis veiksmas
                    signal(SIGTSTP, SIG_DFL);
                    signal(SIGTTIN, SIG_DFL);
                    signal(SIGCHLD, SIG_DFL);
                    setpgid(childPid, childPid);                     //Nustatomas "vaiko" pgid
                    tcsetpgrp(shellTerminal, getpid());
                    //printf("Child process: pid=%d , parentpid=%d\n", getpid(), getppid());
                    executeCommand(cmd);                             //Kviecia execvp
                } else {
                    setpgid(childPid, childPid);                     //Cia taip pat turi buti nustatytas "vaiko" pgid
                    //printf("Father process: pid=%d , parentpid=%d\n", getpid(), getppid());
                    waitpid(childPid, &status, 0);                   //Sustabdomas kvieciancio proceso vykdymas ir laukiama,
                    tcsetpgrp(shellTerminal, getpid());

                }		                                     //kol "vaikas" (su pid = childPid) nepakeite busenos
            }
        }
    }

}
