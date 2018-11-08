#include <errno.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <dirent.h>

#define MAX_BUFFER 1024 // max line buffer
#define MAX_ARGS 64     // max # args
#define SEPARATORS " \t\n" // token separators

extern char **environ;
char* curDir;
pid_t pid;

int ioRedirectCreate = 0; //false = 0
int ioRedirectAppend = 0; //false = 0
int ioRedirectCheck = 0; // false = 0

int ioRedirectOutput = 0;
int ioRedirectInputFile = 0;

const char* ioRedirectOutputFile;

/***** FUNCTION DEFINITIONS ****/
void funcFilez(const char *target, char** args);
void funcMimicAndMorph(const char* mimicOrMorph,const char *srcM, const char *dstM, const char *arg3);
void batchFileCheck(const char* argCheck);
void funcChdir(const char *path, const char *path2);
void funcErase(const char *file, const char *path2);
void funcHelp(const char* argCheck);
void funcEnviron(const char *arg1);
int directoryCheck( const char* path);
int fileCheck(const char* path);
void funcRmdirz(const char* path);
void funcMkdirz(const char* path);
const char* parentCheck( const char* path);
int directoryEmptyCheck(const char* path);
const char* getEndOfPath(const char* path);
void copyFileToDestination(const char* filePath, const char* givenDirectory);
int systemUsingFork(char* command, char** args);
void recursiveMorphMimic(const char* morphOrMimic, const char* directorySource, const char* directoryDestination);
void FuncIORedirectCheck(char **args);




/********* FUNCTIONS *********/
/*funcFilez:
 *
 * For this function, the argument args[1] is passed in as *target.
 * It first checks to see if the target is not null by passing the path
 * into the stat function.
 * Success: If stat (file check) is a success, it returns 0.
 * fopen reads the target file in and prints out its contents
 * byte by byte until it reaches EOF. It closes the file after it is finished.
 *
 * Failure: Is stat (file check) is a failure, it returns -1.
 * an error message is routed routed to stderrr.
 *
 * If no other targets are specified, the program
 * ptints out the files int he current working directory.
 * */



void funcFilez(const char *target, char **args ) {
    struct stat check;
    int errnum;

    if (target != NULL) {
        if(directoryCheck(target) != 0 ){
            chdir(target);
            int status;
            pid = getpid();
            int errnum = 0;

            switch (pid = fork()) {
                case -1: /* Error */
                    errnum = errno;
                    fprintf(stderr, "ERROR: %s\n", strerror(errnum));
                    //return -1;

                case 0: /* Child */
                    execlp("ls", "-1", (const char*) NULL);
                    _exit(127); /* Failed exec */
                default: /* Parent */
                    waitpid(pid, &status, WUNTRACED);
            }
            chdir(curDir);

        }
        else if (fileCheck(target) == 0  /*|| directoryCheck(target) != 0*/) { //checks if target is a file

            printf("%s\n",target);
        }

        else { //if stat returns 0
            errnum = errno;
            fprintf(stderr, "ERROR: %s\n", strerror(errnum));
        }
    }

    else { //if args[1] is null, no target specified
        int status;
        pid = getpid();
        int errnum = 0;

        switch (pid = fork()) {
            case -1: /* Error */
                errnum = errno;
                fprintf(stderr, "ERROR: %s\n", strerror(errnum));
                //return -1;

            case 0: /* Child */

                args[0] = "ls";
                args[1] = "-1";
                args[2] = NULL;
                execvp(args[0], args);
                _exit(127); /* Failed exec */
            default: /* Parent */
                waitpid(pid, &status, WUNTRACED);
        }


    }
}

/*funcMimic
 * The user passes 3 arguments into the function (discluding the
 * word "mimic" . the first if function makes sure
 * that the user is passing in two orguments. (For two files)
 * Fopen is used to open the first file for reading, and the second file for writing.
 * mimicFile is a char array that is created to store the bytes that are
 * to be written to the file that needs to be copied. After the while loop
 * is finished reading the bytes of the first file. fWrite is called
 * to pass the bytes of the first file from mimicArray into the second file.
 * Both files are closed after the process is finished
 *
 * ERROR CHECK: If the file cannot be opened, ERROR is routed to stderr.
 * ERROR CHECK: If the wrong number of arguments are passed in, ERROR is routed to stderr.
 * */
void funcMimicAndMorph(const char* mimicOrMorph, const char *srcM, const char *dstM, const char *arg3){

    int morphCheck = -1;

    if(mimicOrMorph == "morph") {
        morphCheck = 1;
        //printf("morph passed in");
    }

    if (arg3 == NULL && dstM != NULL) { //Case: 2 arguments passed in

        char mimicFile[MAX_BUFFER] = {0};
        int c = 0;

        /*********** ERRORS ***********/

         //src is existing directory, dst existing file **ERROR& cannot write directory to a file
        if(directoryCheck(srcM) != 0 && fileCheck(srcM) == 0 && directoryCheck(parentCheck(dstM)) == 0) {
            //printf("%s is an existing directory and %s is a file\n", srcM, dstM);

            fprintf(stderr, "ERROR: cannot write directory to a file\n");
        }


        //src missing file, dst existing file **ERROR: nothing to be copied
        else if(fileCheck(srcM) != 0 && fileCheck(dstM) == 0) {
            //printf("%s file is missing and %s is an existing file \n", srcM, dstM);

            fprintf(stderr, "ERROR: Nothing to be copied\n");
        }

        //src missing file, src existing file dst existing directory *** ERROR
        else if( (fileCheck(srcM) != 0 && directoryCheck(srcM) == 0) && directoryCheck(dstM) != 0) {
            //printf("%s is A MISSING file and %s is a directory\n", srcM, dstM);

            fprintf(stderr, "ERROR: Nothing to be copied\n");
        }

        //src existing directory, dst missing file : **ERROR
        else if(directoryCheck(srcM) != 0 && fileCheck(dstM) != 0 && directoryCheck(parentCheck(dstM)) == 0) {
            //printf("%s is directory and %s is not a file\n", srcM, dstM);

            fprintf(stderr, "ERROR: Nothing to be copied\n");
        }

        //missing file , missing file : **ERROR
        else if(fileCheck(srcM) != 0 && fileCheck(dstM) != 0 && directoryCheck(parentCheck(dstM)) == 0) {
            //printf("%s is NOT a file and %s is NOT a file\n", srcM, dstM);

            fprintf(stderr, "ERROR: Nothing to be copied\n");
        }

        //missing file, missing directory error: **ERROR you cannot write a missing file
        else if((fileCheck(srcM) != 0  && directoryCheck(srcM) == 0) && directoryCheck(dstM) != 0) {
            //printf("%s is not a file or directory and %s is a directory\n", srcM, dstM);

            fprintf(stderr, "ERROR: Nothing to be copied\n");
        }


        /************** SUCCESSES *************/
        //existing file , missing file : SUCCESS
        if(fileCheck(srcM) == 0 &&  directoryCheck(dstM) == 0 ) {
            //printf("%s is IS a file and %s is NOT a file. File will be created now!\n", srcM, dstM);

            FILE *toR = fopen(srcM, "r");
            FILE *toW = fopen(dstM, "w+");
            int i = 0;

            while ((c = fgetc(toR)) != EOF) {
                mimicFile[i] =(char)c;
                i++;
            }

            fwrite(mimicFile, sizeof(mimicFile), 1, toW); //writes byte by byte to toW (2nd file)

            /** IF MORPH PASSED IN, DELETE SOURCE FILE **/
            if(morphCheck == 1) {
                funcErase(srcM, NULL);
            }
            fclose(toR);
            fclose(toW);
        }

        //src existing file , dst existing directory **SUCCESS*
        else if (fileCheck(srcM) == 0 && directoryCheck(srcM) == 0 && directoryCheck(dstM) != 0){

            //printf("%s is a file and %s is a directory\n", srcM, dstM);


                /*This function creates a concatnates
                 * the source file to the end of
                 * the directory path. The result is the full
                 * path, which is then used in fopen
                 * below. The file will be written to this path.
                 * if this file does not already exist in the directory,
                 * it will be created as noted by 'w'.*/


                char givenFile[50] = " ";
                char fullPath[50] = " ";
                strcpy(givenFile, srcM); //source copied to begin array
                strcpy(fullPath, dstM); //destination copied to fullPath array

                /*This function gets the part of the path containg
                 * the file, by checking to see if a '/' is present
                 * and copy after byte after that. After getting
                 * the file extension, it is concatanated
                 * to the end of the full path given in the destination.*/

                int b = sizeof(givenFile);
                for(b; b <= sizeof(givenFile); b--){
                    if(givenFile[b] == '/') {
                        int fC = 0;
                        while(b < sizeof(givenFile)){
                            givenFile[fC] = givenFile[b];
                            fC++;
                            b++;
                        }
                        givenFile[fC+1]= '\0';
                        break;
                    }
                    //else: do nothing
                }

                //adds trailing slash if trailing slash is not present
                if(fullPath[sizeof(fullPath)] != '/'){
                    strcat(fullPath, "/");
                }


                strcat(fullPath,givenFile); //adds file to end of path

            //printf("This is the final path: %s\n", fullPath);

                FILE *toR = fopen(srcM, "r"); //opens source file to to READ
                FILE *srcdstM = fopen(fullPath,"w"); //opens path to write

                int i = 0;
                int c = 0;

                /*This loop copies byte by byte from the file specified,
                 * to the char aray 'mimicFile'. when the while reaches EOF,
                 * the mimicFile array is written to the source destination
                 * (directory with new file path)
                 * */


                while ((c = fgetc(toR)) != EOF) {
                    mimicFile[i] =(char)c;
                    i++;
                }
                fwrite(mimicFile, sizeof(mimicFile), 1, srcdstM);

                fclose(toR);
                fclose(srcdstM);

                /** IF MORPH PASSED IN, DELETE SOURCE FILE **/
                if(morphCheck == 1) {
                    funcErase(srcM,NULL);
                }

        }


        //src is existing file, dst is existing file
        else if ( (fileCheck(srcM) == 0 && directoryCheck(srcM) == 0)
                  && (fileCheck(dstM) == 0 && directoryCheck(dstM) ==0) ) {

            //printf("%s is a file and %s is a file\n", srcM, dstM);

            FILE *toR = fopen(srcM, "r");
            FILE *toW = fopen(dstM, "w+");
            int i = 0;

            while ((c = fgetc(toR)) != EOF) {
                mimicFile[i] =(char)c;
                i++;
            }

            fwrite(mimicFile, sizeof(mimicFile), 1, toW); //writes byte by byte to toW (2nd file)

            fclose(toR);
            fclose(toW);

            /** IF MORPH PASSED IN, DELETE SOURCE FILE **/
            if(morphCheck == 1) {
                funcErase(srcM,NULL);
            }
        }


        //(-r not supplied) src existing directory, dst is an existing directory: SUCCESS
        else if(directoryCheck(srcM) != 0 && directoryCheck(dstM) != 0) {

            /* SUCCESS */
            if(directoryEmptyCheck(srcM) != -1){
                // printf("Directory is empty! the contents of %s can be copied into %s\n", srcM, dstM);

                /*
                 * This following code segement gets the name of the directory from the
                 * end of srcM by calling the getEndOfPath function. it changes the
                 * directory to the dstM to create the directory and then changes
                 * back to the current directory (the defined global variable
                 * that keeps track of the current directory)
                 */
                funcChdir(dstM, NULL);

                const char* fileSrc= " ";
                fileSrc = getEndOfPath(srcM);

                if (mkdir(fileSrc, S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH) == -1) {
                    fprintf(stderr, "ERROR: Directory failed to create.\n");
                }
                funcChdir((const char*)curDir, NULL);

                /** IF MORPH PASSED IN, DELETE SOURCE FILE **/
                if(morphCheck == 1) {
                    funcRmdirz(srcM);
                }
            }

            /* ERROR */
            else{
                fprintf(stderr, "Directory must be empty. Must use -r argument for non empty directory.\n");
            }
        }

        //src existing directory, dst is not an existing directory
        else if(directoryCheck(srcM) != 0 && ( directoryCheck(dstM) == 0 && fileCheck(dstM) != 0 && directoryCheck(parentCheck(dstM)) != 0)) {
            //printf("%s is a directory and %s is not a directory.. but it might have a parent!\n", srcM, dstM);


            /* SUCCESS */
            //if srcM directory is empty, the directory is copied to the parent of dstM (if the parent exists)
            if (directoryEmptyCheck(srcM) != -1) {
                /*
                     * This following code segement gets the name of the directory from the
                     * end of srcM by calling the getEndOfPath function. it changes the
                     * directory to the dstM to create the directory and then changes
                     * back to the current directory (the defined global variable
                     * that keeps track of the current directory)
                     */

                /*This code makes creates the folder inside the parent directory.
                 * */
                const char *parentdstM = " ";
                parentdstM = parentCheck(dstM);

                funcChdir(parentdstM, NULL);

                const char *fileSrc = " ";
                fileSrc = getEndOfPath(srcM);

                if (mkdir(fileSrc, S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH) == -1) {
                    fprintf(stderr, "ERROR: Directory failed to create.\n");
                }
                funcChdir((const char *) curDir, NULL);

                /** IF MORPH PASSED IN, DELETE SOURCE FILE **/
                if(morphCheck == 1) {
                    funcRmdirz(srcM);
                }
            }
        }

            /* ERROR: Directory is not empty!*/
        else{
            fprintf(stderr, "Directory must be empty. Must use -r argument for non empty directory.\n");
        }


    }

    else {
        fprintf(stderr, "ERROR: Wrong number of arguments passed in.\n");
    }
}


/*funcChdir
 * If the current path is equal to null (just Chdir was passed in) the
 * program will print the current path at that current directory by
 * calling the system() function.
 *
 * If an argument is passed in (path2 == NULL), THe program
 * will create a new array (acting as a buffer). It will
 * call getcwd to get the pathname of the current working direocy
 * by passing in the buffer array currDirr. After returning the
 * buf argument, the program calls chdir to change the current directory.
 * the New directory is passed in the putenv to set th enviroment variables
 * based on the the current directory.
 *
 * ERRORS: If putenv fails, Error printed to stderr
 *
 * ERRORS: If wrong number of arguments are passed in, Error is printed to stderr
 * */
void funcChdir(const char *path,const char *path2) {
    struct stat check;

    if (path == NULL) {
        systemUsingFork("pwd", NULL);
    }

    else if ( path2 == NULL) {
        char currDir[100];
        char *newDir = getcwd(currDir, sizeof(currDir));
        if(directoryCheck(path)) {
            chdir(path);
        }
        if(putenv(newDir) != 0) {
            fprintf(stderr, "ERROR\n");
        }
    }

    else{
        fprintf(stderr, "ERROR: Wrong number of arguments passed in.\n");
    }
}

/*funcErase
 *This function removes the file if one argument is passed in.
 * ERRORS: If more than 1 argument is passed in OR the the file
 * is unable to be removed, error is routed to stderr
 * */
void funcErase(const char *file, const char *path2){
    if( path2 == NULL) {

        int e = remove(file);
        int errnum;
        if (e != 0) {
            errnum = errno;
            fprintf(stderr, "ERROR: Unable to remove file: %s\n", strerror(errnum));
        }
    }

    else{
        fprintf(stderr, "ERROR: Wrong number of arguments passed in.\n");

    }
}

/*batchFileCheck
 * This function checks to see whether a batchfile has been passed in or not.
 * The function reads the file passed into batchFileCheck. If the file is not able
 * to be opened, an error is routed to stderr. If it is opened, the argCheck
 * file descriptor is routed to STDIN.
 *
 * ERRORS: Errors are routed to STDERR if the argCheck file is not opened
 * and if there is an error accessing the file descriptor for dup2.
 * */
void batchFileCheck(const char* argCheck){
    int fd;
    setbuf(stdout, NULL);
    fd = open(argCheck, O_RDONLY);

    if (fd < 0) { //returns -1 if file cant be opened
        int errnum;
        errnum = errno;
        fprintf(stderr, "ERROR: %s\n", strerror(errnum));
        close(fd);
    }

    else {
        int dupC = dup2(fd, STDIN_FILENO);

        if (dupC == -1) {
            int errnum;
            errnum = errno;
            fprintf(stderr, "ERROR: %s\n", strerror(errnum));
            close(dupC);
        }
    }

    close(fd);
}

/*funcHelp
 * This functions prints out the directions on using the program
 * by printing out the README.txt file (that contains the directions).
 * fopen is used to open the file specified by the README.txt file path.
 * Its file path is specified to make sure it can be opened in any directory.
 * It prints char by char until it reaches EOF.
 *
 * ERRORS: if any arguments are passed in after the user types "help"
 * the program will throw an error.
 * */
void funcHelp(const char* argCheck){

    if(argCheck == NULL) {
        FILE *fo = fopen("/home/lisaegede96/projects/1/README.txt", "r");
        int c = 0;
        while ((c = fgetc(fo)) != EOF) {
            printf("%c", c);
        }
        fclose(fo);
    }
    else{
        fprintf(stderr, "ERROR: Wrong number of arguments passed in.\n");
    }
}

/*funcEnviron
 *This function lists all the enviroment strings.
 *
 * ERROR: if the user types in another argument after the 'environ' argument,
 * the program will route an error to stderr
 * */

void funcEnviron(const char *arg1){
    char **env = environ;

    if (arg1 == NULL ) {
        while (*env) printf("%s\n", *env++);
    }

    else {
        fprintf(stderr, "ERROR: Too many arguments passed in.\n");
    }
}

/*
 * This function checks to see whether a specified path is a directory or not.
 * The stat function obtains info about the path and passes into the buf specified as eheck.
 * if it is successful the function will return a non zero value
 * if the file is a directory.
 * */
int directoryCheck( const char* path){
    struct stat check;
    if(stat(path,&check) != 0) {
        return 0;
    }
    else{
        return S_ISDIR(check.st_mode); //returns non zero if path is a directory
    }
}

int fileCheck(const char* path){
    FILE *fCheck;
    fCheck = fopen(path, "r");
    if(fCheck != NULL) {
        fclose(fCheck);
        return 0; //return 0 if file exists
    }
    else {
        fprintf(stderr, "%s File does not exist\n", path);
    }
    return -1;
}


/*
 * This function performs a removal of a directory
 * if the path is valid and not empty.
 * */
void funcRmdirz(const char* path){

    struct stat check = { 0 };
    if (stat(path, &check) == 0 ) { //path is not empty
        int stat = rmdir(path);
        if (stat== -1){
            fprintf(stderr, "ERROR: Directory failed to remove.");
        }
    }
}

/*
 * This function checks to see if the parent is present.
 * It searches by finding slash and setting the
 * rest of the array to NULL if it cannot be found. It also accounts
 * for
 * */
const char* parentCheck( const char* path){

    char fullPath[75] = { 0 };
    strcat(fullPath, path);

    int origSize = sizeof(fullPath);
    int i = origSize;

    while (i <= sizeof(fullPath)) {
        if(fullPath[origSize] == '/') { //accounts for trailing slash
            fullPath[origSize+1] = '\0';
        }

        if(fullPath[i] == '/'){
            fullPath[i+1] = '\0';
            break;
        }
        else {
            i--;
        }
    }

    path = (const char*)fullPath;
    return path;
}

/*
 * This functions purpose it to serve
 * the morph/mimic recursive function. It
 * makes sure the path passed in is not already a
 * directory before creating it.
 */

void funcMkdirz(const char* path){

    path = parentCheck(path);

    if ( directoryCheck(path) != 0) { //path does not exist && parent path does exist
        if (mkdir(path,S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH) == -1) {
            fprintf(stderr, "ERROR: Directory failed to create.\n");
        }
    }
    else {
        fprintf(stderr, "ERROR: Parent not a directory / Directory is not empty\n");
    }
}

int directoryEmptyCheck(const char* path){
    /** SUCCESS, CHECK IF DIRECTORY IS EMPTY! **/

    int emptyCount = 0;

    DIR *dstDIR;
    struct dirent *dD;
    if ((dstDIR = opendir(path)) == NULL) {
        fprintf(stderr, "ERROR");

    }
    else {

        while (readdir(dstDIR) != NULL) {

            emptyCount = emptyCount + 1;
        }
    }

    /** Is directory empty? **/
    if(emptyCount < 3) {
        //printf("This is  an empty directory");
        return 0;
    }

    else if(emptyCount > 2) {
        //printf("This directory is NOT empty.");
        return -1;
    }
}


/*This function gets the part of the path containg
             * the file, by checking to see if a '/' is present
             * and copy after byte after that. After getting
             * the file extension, it is concatanated
             * to the end of the full path given in the destination.*/

const char* getEndOfPath(const char* path){

    char givenFile[50] = " ";
    strcpy(givenFile, path);


    int b = 0;
    for(b; b < sizeof(givenFile); b++){
        if(givenFile[b] == '/') {
            int fC = 0;
            while(b < sizeof(givenFile)){
                givenFile[fC] = givenFile[b+1];
                fC++;
                b++;
            }
            givenFile[fC+1]= '\0';
            break;
        }
        //else: do nothing
    }

    path = (const char*)givenFile;

    //checks for trailing '/'
    int i = sizeof(path);
    for( i; i <= sizeof(path); i--){
        if(givenFile[i] == '/'){
            givenFile[i] = '\0';
            break;
        }
    }

    path = (const char*)givenFile;

    //printf("****~~~ path is now: %s", path);
    return path;

}

/*
 * This functions copy the file (as given by the file path) to the given
 * directory (as given by the directory path). This function is meant for the
 * morph -r mimic -r functions.
 * */
void copyFileToDestination(const char* filePath, const char* givenDirectory){


    if (fileCheck(filePath) == 0 && directoryCheck(filePath) == 0 && directoryCheck(givenDirectory) != 0){

        //printf("%s is a file and %s is a directory\n", filePath, givenDirectory);


        /*This function creates a concatnates
         * the source file to the end of
         * the directory path. The result is the full
         * path, which is then used in fopen
         * below. The file will be written to this path.
         * if this file does not already exist in the directory,
         * it will be created as noted by 'w'.*/

        char mimicFile[MAX_BUFFER] = {0};

        char givenFile[50] = " ";
        char fullPath[50] = " ";
        strcpy(givenFile, filePath); //source copied to begin array
        strcpy(fullPath, givenDirectory); //destination copied to fullPath array

        /*This function gets the part of the path containg
         * the file, by checking to see if a '/' is present
         * and copy after byte after that. After getting
         * the file extension, it is concatanated
         * to the end of the full path given in the destination.*/



        int b = 0;
        for(b; b < sizeof(givenFile); b++){
            if(givenFile[b] == '/') {
                int fC = 0;
                while(b < sizeof(givenFile)){
                    givenFile[fC] = givenFile[b];
                    fC++;
                    b++;
                }
                givenFile[fC+1]= '\0';
                break;
            }
            //else: do nothing
        }



        strcat(fullPath,givenFile); //adds file to end of path

        //printf("This is the final path: %s\n", fullPath);

        FILE *toR = fopen(filePath, "r"); //opens source file to to READ
        FILE *srcdstM = fopen(fullPath,"w"); //opens path to write

        int i = 0;
        int c = 0;

        /*This loop copies byte by byte from the file specified,
         * to the char aray 'mimicFile'. when the while reaches EOF,
         * the mimicFile array is written to the source destination
         * (directory with new file path)
         * */


        while ((c = fgetc(toR)) != EOF) {
            mimicFile[i] =(char)c;
            i++;
        }

        fwrite(mimicFile, sizeof(mimicFile), 1, srcdstM);

        fclose(toR);
        fclose(srcdstM);

    }
    else{
        fprintf(stderr, "There is an error copying the file to directory");
    }

}

int systemUsingFork(char* command, char** args){
        int status;
        //pid_t childPid;
        pid = getpid();
        int errnum = 0;
        int b = 0;

        switch (pid = fork()) {
            case -1: /* Error */
                errnum = errno;
                fprintf(stderr, "ERROR:  %s\n", strerror(errnum));
                return -1;

            case 0: /* Child */

                while(args[b] != NULL ) {

                    if(!strcmp(args[b], "<" )) //redirecting echo
                    {
                        freopen(args[b-1],"w",stdin);
                    }

                    if(!strcmp(args[b], ">" )) //redirecting echo
                    {
                        freopen(args[b+1],"w",stdout);
                        args[b] = '\0';
                        args[b + 1] = "\0";
                    }

                    else if(!strcmp(args[b], ">>" )) //redirecting
                    {
                        freopen(args[b+1],"a+",stdout);
                        args[b] = "\0";
                        args[b + 1] = "\0";
                    }
                    b++;
                }

                execvp( *args, args);
                _exit(127); /* Failed exec */
            default: /* Parent */
                if (waitpid(pid, &status, WUNTRACED) == -1)
                    return -1;
                else
                    return status;
        }


}

/*
 * This function resursively traverses through directory to check
 * for both files and subdirectories. If morph is passed in,
 * file/directory is deleted after being copied over. If mimic is
 * passed in, file/directory is not deleted after being copied over*/
void recursiveMorphMimic(const char* morphOrMimic, const char* directorySource, const char* directoryDestination){
    //src directory and all of its contents are copied into dst

    int morphCheck = -1;

    if(morphOrMimic == "morph") {
        morphCheck = 1;
    }

    DIR *srcDIR;
    struct dirent *sD;
    const char* pCheck = directorySource;
    const char* destDir = directoryDestination;



    char fullDirP[75] ={ 0 };
    strcpy(fullDirP, pCheck);

    if((srcDIR = opendir(pCheck)) == NULL) {
        fprintf(stderr, "ERROR");
    }


    int d = sizeof(fullDirP);
    while(d <= sizeof(fullDirP)){
        if(fullDirP[d] == '\0') {
            d--;
        }
        else if(fullDirP[d] != '/') {
            strcat(fullDirP, "/");
            break;
        }
        else{
            d = -1;
        }
    }


    //LIST DIRECTORY FUNCTION
    while((sD = readdir(srcDIR)) != NULL){
        DIR *opDIR;

        char sdName[50] = { 0 };
        char sdPath[50] = { 0 };
        strcpy(sdName, sD->d_name);
        strcpy(sdPath, fullDirP);
        strcat(sdPath, sdName);

        if(directoryCheck(sdPath) == 0) { // CHECK FOR FILES
            char sdNameF[50] = { 0 };
            char sdPathF[50] = { 0 };
            strcpy(sdNameF, sD->d_name);
            strcpy(sdPathF, fullDirP);
            strcat(sdPathF, sdName);

            //printf("THIS IS A FILE****: %s\n", sdPathF);


            if(directoryCheck(parentCheck(sdPathF)) != 0){

                //printf("this is the parentCheck inside the file check: %s\n ", parentCheck(sdPathF));

                char pcPath[50] = { 0};
                strcpy(pcPath, parentCheck(sdPathF));

                funcChdir(destDir, NULL);

                const char* fileSrc= " ";
                fileSrc = getEndOfPath((const char*) pcPath);

                //printf("this is the directory being created aka fileSrc: %s\n", getEndOfPath((const char*) pcPath));

                if (mkdir(getEndOfPath((const char*) pcPath), S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH) == -1) {
                    fprintf(stderr, "ERROR: Directory failed to create.\n");
                }
                funcChdir(curDir,NULL);

            }

            copyFileToDestination(sdPathF, destDir);

            /** IF MORPH PASSED IN, DELETE SOURCE FILE **/
            if(morphCheck == 1) {
                printf("removing");
                funcErase(sdPathF,NULL);
            }
        }

        else if(directoryCheck(sdPath) != 0
                && (strcmp(sD->d_name, "..") != 0)
                && (strcmp(sD->d_name, ".") != 0)) {


            //printf("______This is a directory*** %s\n", sdPath);

            funcChdir(destDir, NULL); //changes directory to destination

            const char* fileSrc= " ";
            fileSrc = getEndOfPath(sdPath);

            //is a directory being created in destination path %s\n", fileSrc, destDir

            if (mkdir(fileSrc, S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH) == -1) {
                fprintf(stderr, "ERROR: Directory failed to create.\n");
            }

            funcChdir(curDir, NULL); //changes back to current directory

            char newFileSrc[50] = { 0 };
            char currDestPath[50] = { 0 };
            strcpy(newFileSrc, fileSrc);
            strcpy(currDestPath, destDir);
            strcat(currDestPath, "/");
            strcat(currDestPath, newFileSrc);

            if ((opDIR = opendir(sdPath)) == NULL) {
                fprintf(stderr, "ERROR opening directory\n");
            }



            while( (sD = readdir(opDIR)) != NULL) {
                char sdName1[50] = { 0 };
                char sdPath1[50] = { 0 };
                strcpy(sdName1, sD->d_name);
                strcpy(sdPath1, sdPath);
                strcat(sdPath1, "/");
                strcat(sdPath1, sdName1);

                if(directoryCheck(sdPath1) != 0
                   &&
                   (strcmp(sD->d_name, "..") != 0)
                   && (strcmp(sD->d_name, ".") != 0) ){


                    const char* directoryEnd= " ";
                    directoryEnd = getEndOfPath(sdPath1);

                    char destDirFinal[50] = { 0 };
                    char destFileSource[50] = { 0 };

                    strcpy(destDirFinal, currDestPath);
                    strcpy(destFileSource, fileSrc);
                    strcat(destDirFinal, "/");
                    strcat(destDirFinal, destFileSource);

                    funcChdir(destDirFinal, NULL); //changes directory to destination


                    if (mkdir((const char*)destDirFinal, S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH) == -1) {
                        fprintf(stderr, "ERROR: Directory failed to create.\n");
                    }
                    funcChdir(curDir, NULL); //changes back to current directory

                    char newFileSrc2nd[50] = { 0 };
                    char currDestPath2nd[50] = { 0 };
                    strcpy(newFileSrc2nd, directoryEnd);
                    strcpy(currDestPath2nd, currDestPath);
                    strcat(currDestPath2nd, "/");
                    strcat(currDestPath2nd, newFileSrc2nd);


                    if(morphCheck == 1) {
                        funcRmdirz((const char*)sdPath1);
                    }

                }

                else if(directoryCheck(sdPath1) == 0 &&
                        (strcmp(sD->d_name, "..") != 0)
                        && (strcmp(sD->d_name, ".") != 0) ) {

                    char sdpathEnd[50] = { 0 };
                    char finalDestDirec[50] = { 0 };
                    char beginningPathFinal[50] = { 0 };
                    strcpy(sdpathEnd, sD->d_name);
                    strcpy(finalDestDirec, currDestPath);
                    strcat(finalDestDirec, "/");
                    strcat(finalDestDirec, sdpathEnd);

                    //**not neccassary if full path is given**
                    strcat(beginningPathFinal, fullDirP);
                    strcat(beginningPathFinal,finalDestDirec);


                    ///*TO READ*/printf("````` Might be a file: %s\n", (const char*) sdPath1);
                    ///*TO WRITE */ printf("Will be copied to directory %s\n", (const char*)finalDestDirec);



                    mode_t mode = S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH;

                    int origPath = open((const char*) sdPath1, O_RDONLY, mode);
                    int srcHandle = open((const char*)finalDestDirec, O_CREAT | O_WRONLY | O_SYNC | O_APPEND, S_IRUSR | S_IWUSR | S_IRGRP);

                    int errnum;
                    if( srcHandle == -1) {
                        errnum = errno;
                        fprintf(stderr, "ERROR: creating to file: %s\n", strerror(errnum));
                    }

                    char bufRead[MAX_BUFFER];
                    size_t nbytes = sizeof(bufRead);

                    if(read(origPath, bufRead, nbytes) < 0){
                        errnum = errno;
                        fprintf(stderr, "ERROR: reading to file: %s\n", strerror(errnum));
                    }


                    if(write(srcHandle, bufRead, nbytes) != nbytes){
                        errnum = errno;
                        fprintf(stderr, "ERROR: writing to file: %s\n", strerror(errnum));
                    }

                    close(srcHandle);
                    close(origPath);

                    /** IF MORPH PASSED IN, DELETE SOURCE FILE AFTER FINISHING COPYING **/
                    if(morphCheck == 1) {
                        funcErase((const char*)sdPath1,NULL);
                    }
                }
            }

            /** DELETES DIRECTORY AFTER LOOP FINISHES**/
            if(morphCheck == 1) {
                // printf("REMOVING DIRECTORY %s\n", sdPath);
                funcRmdirz((const char*)sdPath);
            }

        }
    }

    if(morphCheck == 1) {
        //printf("REMOVING DIRECTORY %s\n", directorySource);
        funcRmdirz(directorySource);
    }
}

/*
 * This function
 */

void FuncIORedirectCheck(char **args){

    if (ioRedirectCheck == 1) {
        int d = 1;
        while (args[d] != NULL) {
            printf("redirection began.. entering.");
            if(!strcmp(args[d], "<")) {
                ioRedirectOutput = 1;
                ioRedirectInputFile = (const char*)args[d+1];
            }

            if (!strcmp(args[d], ">")) {
                ioRedirectCreate = 1; //redirect output to
                ioRedirectOutputFile = (const char*) args[d+1];
            }
            else if (!strcmp(args[d], ">>")) {
                ioRedirectAppend = 1;
                ioRedirectOutputFile = (const char*) args[d+1];
            }
            d++;
        }
    }
}

int main (int argc, char **argv) {
    setbuf(stdout, NULL);
    char buf[MAX_BUFFER]; // line buffer
    char *args[MAX_ARGS]; // pointers to arg
    char **arg;            // working pointer
    char *prompt = "==>"; // shell prompt

    char display[FILENAME_MAX];

    int boolPrompt = 0; //false = 0


    if( argc == 2 && fileCheck(argv[1]) == 0) { //checks to see if batchfile is passed in
        //printf("batchfile passed in.. entering.");
        batchFileCheck(argv[1]);
        boolPrompt = 1; //if batch file is passed in, prompt is changed to 1 (true)
    }


    else if( argc == 2) { //
        ioRedirectCheck = 1;
        freopen(argv[1],"r",stdin);
    }


    /***********************************/
    // keep reading input until "quit" command or eof redirected
    while (!feof(stdin)) {
        curDir = getcwd(display, FILENAME_MAX);


        if (boolPrompt == 0) { //if batchfile is not passed in, prompt from user
            fputs(curDir, stdout);
            fputs(prompt, stdout);         // write prompt
        }


        if (fgets(buf, MAX_BUFFER, stdin)) { // read a line
            // tokenize the input into args array

            if (boolPrompt == 1) { //if batch is passed in, print prompt
                printf("%s==>%s", curDir, buf); //prints prompt plus line from batchfile
            }

            arg = args;
            *arg++ = strtok(buf, SEPARATORS);   // tokenize input
            //tokenize the input into args array




            while ((*arg++ = strtok(NULL, SEPARATORS)));// last entry will be NULL

            /******** IO REDIRECTION ***********/
            //FuncIORedirectCheck(args);


            if (args[0]) {                           // if there's anything is there
                // check for internal/external command

                //check for IO REDIRECTION
                if (ioRedirectCheck == 1) {
                    FuncIORedirectCheck(args);
                    systemUsingFork(argv[1], args);

                }


                /**/if (!strcmp(args[0], "wipe")) { // "clear" command
                    systemUsingFork("clear", args);
                    continue;
                }

                    /**/else if (!strcmp(args[0], "esc")) {  // "quit" command
                    break;                     // break out of 'while' loop
                }

                    /**/else if (!strcmp(args[0], "filez")) {
                    funcFilez(args[1], args);
                }

                    /**/else if (!strcmp(args[0], "ditto")) {
                    int i = 1; //sets args to 1 to ignore first input of 0
                    while (args[i] != NULL) {
                        printf("%s ", args[i]); //prints array of user input
                        i++;
                    }
                    printf("\n");
                }

                    /**/else if (!strcmp(args[0], "environ")) {
                    funcEnviron(args[1]);
                }

                    /**/else if (!strcmp(args[0], "mimic")) {

                    /* Calls functon to perform mimic if -r is not supplied. */
                    if (strcmp(args[1], "-r") != 0) {
                        funcMimicAndMorph((const char *) args[0], args[1], args[2], args[3]);
                    }

                        //-r supplied existing directory srcM, existing directory - dstM
                    else if (!strcmp(args[1], "-r") && directoryCheck(args[2]) != 0 && directoryCheck(args[3]) != 0) {
                        recursiveMorphMimic(args[0], args[2], args[3]);
                    }
                }

                    /**/else if (!strcmp(args[0], "help")) {
                    funcHelp(args[1]);
                }

                    /**/else if (!strcmp(args[0], "erase")) {
                    funcErase(args[1], args[2]);
                }

                    /**/else if (!strcmp(args[0], "morph")) {
                    /* Calls function to perform mimic if -r is not supplied. */
                    if (strcmp(args[1], "-r") != 0) {
                        funcMimicAndMorph("morph", args[1], args[2], args[3]);
                    }

                        //-r supplied existing directory srcM, existing directory - dstM
                    else if (!strcmp(args[1], "-r") && directoryCheck(args[2]) != 0 && directoryCheck(args[3]) != 0) {
                        recursiveMorphMimic("morph", args[2], args[3]);
                    }
                }

                    /**/else if (!strcmp(args[0], "chdir")) {
                    funcChdir(args[1], args[2]);
                }
                    /**/else if (!strcmp(args[0], "rmdirz")) {
                    funcRmdirz(args[1]);

                }
                    /**/else if (!strcmp(args[0], "mkdirz")) {

                    if (mkdir(args[1], S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH) == -1) {
                        fprintf(stderr, "ERROR: Directory failed to create.\n");
                    }

                }
                    /**/else {
                    int b = 0;
                    int status;
                    pid = getpid();
                    int errnum = 0;


                    /****** Fork to Redirect: Echo ">"********/
                    switch (pid = fork()) {
                        case -1: /* Error */
                            errnum = errno;
                            fprintf(stderr, "ERROR:  %s\n", strerror(errnum));
                            break;

                        case 0: /* Child */
                            while(args[b] != NULL ) {

                                if(!strcmp(args[b], "<" )) //redirecting echo
                                {
                                    freopen(args[b-1],"w",stdin);
                                }

                                if(!strcmp(args[b], ">" )) //redirecting echo
                                {
                                    freopen(args[b+1],"w",stdout);
                                    args[b] = '\0';
                                    args[b + 1] = "\0";
                                 }

                                else if(!strcmp(args[b], ">>" )) //redirecting
                                {
                                    freopen(args[b+1],"a+",stdout);
                                    args[b] = "\0";
                                    args[b + 1] = "\0";
                                }
                                b++;
                            }

                            execvp(args[0], args);
                            _exit(127); /* Failed exec */
                        default: /* Parent */
                            if (waitpid(pid, &status, WUNTRACED) == -1)
                                break;
                    }

                }
            }
        }
    }

    return 0;
}
