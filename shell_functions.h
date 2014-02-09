#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <unistd.h>
#include <fcntl.h>
#include <termios.h>
#include <errno.h>

#define MAX_CMDLINE_LENGTH 80                                        //Maksimalus eilutes ilgis
#define MAX_CMD_ATTRIBUTES 10                                        //Maksimalus komandos argumentu skaicius

void printPrompt();                                                  //Prompt (pakvietimo) rodymo funkcija
void getLine(char *line);                                            //Ivestos eilutes nuskaitymo funkcija
void clearCmd(char **cmd);                                           //Komandos "isvalymo" funkcija
void splitLine(char *line, char **splitLine);                        //Fnkcija komandai ir jos arg. gauti
int isBuiltInCommand(char **cmd);                                    //"Built-in" komandu tikrinimo funkcija
void executeCommand(char **cmd);                                     //Komandu vykdymo funkcija
void changeDirectory(char **cmd);                                    //Komandos "cd" funkcija
void redirectionCheck(char **cmd);                                   //Srautu nukreipimo funkcija
void initShell();                                                    //Shell inicializavimo funkcija
void deleteCmdArgument(int n);                                       //Komandos "n" argumento istrynimo funkcija
void cmdArgumentsCheck(char **cmd);                                  //Komandos argumentu "$n" tikrinimo funkcija

char *currentDirectory;                                              //buferis, kuriame saugojama esama direktorija
static int shellPid, shellPgid, shellTerminal, shellIsInteractive;   //kintamieji nusakantys shell (apvalkalo) duomenis
struct termios shellTmodes;                                          //cia saugojami numatytieji apvalkalo atributai

int status;
int childPid;                                                        //Sveikas skaicius ("vaiko" pid saugojimui)
char cmdLine[MAX_CMDLINE_LENGTH];                                    //Simboliu masyvas (ivestos eilutes saugojimui)
char *cmd[MAX_CMD_ATTRIBUTES];                                       //Rodykle i simboliu masyva
                                                                     //(komandos ir jos argumentu saugojimui)
