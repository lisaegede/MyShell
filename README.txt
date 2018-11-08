Directions on how to run your code:

****** WELCOME TO MY SHELL***************************

-R EMEMBER: Case matters

The following commands:

esc – quit the program

wipe – clear the screen

filez [target] – specify a target to print its contents. Omit [target] to print all the files in the current directory.

environ – list all the environment strings

ditto [comment] – any comment[s] after ditto will be printed onto output

help – [here!] type in this command to receive help/info regarding this program

mimic [source] [destination] – enter 2 files for both source and destination. If the file does not exist, an error will be thrown. Source will copied into destination, truncating any of its previous contents.

erase [myfile] – erases specified file

morph [source] [destination] – enter both a source and destination. Source must be a file and destination must be a directory. Source will move file to destination directory.

chdir [path] – changes present working directory

************************************************************

Building a Shell
Name: Lisa Egede
Email Address: lisaegede@ou.edu


My Approach: (Going off of my previous project1 implementation) When executing morph and mimic, I decided to combine both morph and mimic to increase efficiency. When I call the -r using either morph or mimic, I utilize the same function by checking see whether or not a mimic or morph is passed. Another tool that I implemented, is making use of system using execvp. I created a system function using fork to utilize this, and referenced it frequently, instead of copying and pasting code. I also made use of strcat, because it allowed me to create the directories, and made it easier for me to keep track of each directory I traversed through. 

***********************************************************
A declaration of references:

https://www.gnu.org/software/libc/manual/html_node/Testing-File-Type.html
The Linux Programming Interface
http://pubs.opengroup.org/onlinepubs/009604499/functions/stat.html
https://overiq.com/c-programming/101/fwrite-function-in-c/
https://www.quora.com/How-can-I-input-two-different-strings-separated-by-a-space-in-a-single-line-in-C
https://stackoverflow.com/questions/47838120/convert-char-to-unsigned-char-array-in-c
http://pubs.opengroup.org/onlinepubs/7908799/xsh/open.html
http://pubs.opengroup.org/onlinepubs/009695399/functions/getcwd.html
http://pubs.opengroup.org/onlinepubs/009604599/functions/putenv.html
https://www.tutorialspoint.com/c_standard_library/c_function_remove.htm
http://man7.org/linux/man-pages/man2/dup.2.html
https://stackoverflow.com/questions/14543443/in-c-how-do-you-redirect-stdin-stdout-stderr-to-files-when-making-an-execvp-or
http://man7.org/linux/man-pages/man7/environ.7.html
https://stackoverflow.com/questions/45375524/how-to-use-strcat-function
https://stackoverflow.com/questions/3554120/open-directory-using-c
