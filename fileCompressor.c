#include <stdio.h>
#include <sys/types.h>
#include <dirent.h> 
#include <libgen.h>
#include <fcntl.h>
#include <stdlib.h>
#include <string.h> 
//#include <unistd.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <dirent.h>
#include "build.h"

/*
    Check List:

    Need to fix input checking to opendir and check, if in the R case, the currentDir is a path
    Compress and Decompress recursive
    write to files



*/


void checkValid (int n, char* args[]) {

    if (n < 2) {
        
        fprintf(stderr, "Fatal Error: expected 2, 3, or 4 arguments, only had %d\n", n);
        exit(-1);
    }
    else if (n > 4) {

        fprintf(stderr, "Fatal Error: expected 2, 3, or 4 arguments, argument list too long\n");
        exit(-1);
    }

    if (strcmp(args[0], "-b") != 0 && strcmp(args[0], "-c") != 0 && strcmp(args[0], "-d") != 0 && strcmp(args[0], "-R") != 0) {

        fprintf(stderr, "Fatal Error: '%s' is an invalid flag, please use '-b', '-c', '-d', or '-R'\n", args[0]);
        exit(-1);
    }

    if (strcmp(args[0], "-R") == 0) {

        if (strcmp(args[1], "-b") != 0 && strcmp(args[1], "-c") != 0 && strcmp(args[1], "-d") != 0) {

            fprintf(stderr, "Fatal Error: '%s' is an invalid flag following '-R', please use '-b', '-c', or '-d'\n", args[1]);
            exit(-1);
        }


        DIR* c1 = opendir(args[2]);

        if (c1 == NULL) {

            fprintf(stderr, "Fatal error: '%s' is not a valid path\n", args[2]);            
            exit(-1);
        }

        closedir(c1);
        
        if (strcmp(args[1], "-b") == 0)
            return;

        DIR* c2 = opendir(args[3]);
        int f2 = open(args[3], O_RDONLY);
        
        if (c2 != NULL || f2 == -1) {

            fprintf(stderr, "Fatal Error: '%s' is not a valid file, when using the '%s' flag make sure to put a file path to a huffman codebook\n", args[3], args[1]);
            closedir(c2);
            if (f2 != -1)
                close(f2);
            exit(-1);
        }

        close(f2);

    }
    else {

        DIR* c1 = opendir(args[1]);
        int f1 = open(args[1], O_RDONLY);

        if (c1 != NULL || f1 == -1) {

            fprintf(stderr, "Fatal Error: '%s' is not a file in this directory, try a different file path if the file exists\n", args[1]);          
            closedir(c1);
            if (f1 != -1)
                close(f1);
            exit(-1);
        }

        close(f1);

        int size = strlen(args[1]);

        if (strcmp(args[1] + (size - 4), ".txt") != 0 && strcmp(args[0], "-c") == 0) {
            fprintf(stderr, "Fatal Error: '%s' is not a .txt, when using '-c' you must input a valid .txt file\n", args[1]);
            exit(-1);
        }

        if (strcmp(args[1] + (size - 4), ".txt") != 0 && strcmp(args[0], "-b") == 0) {
            fprintf(stderr, "Fatal Error: '%s' is not a .txt, when using '-b' you must input a valid .txt file\n", args[1]);
            exit(-1);
        }

        if (strcmp(args[1] + (size - 4), ".hcz") != 0 && strcmp(args[0], "-d") == 0) {
            fprintf(stderr, "Fatal Error: '%s' is not a .hcz, when using '-d' you must input a valid .hcz file\n", args[1]);
            exit(-1);
        }

        
        if (strcmp(args[0], "-b") == 0)
            return;

        DIR* c2 = opendir(args[2]);
        int f2 = open(args[2], O_RDONLY);

        if (c2 != NULL || f2 == -1) {

            //printf("\n\topen value is %d\n\n", f2);

            fprintf(stderr, "Fatal Error: '%s' is not a valid file, when using the '%s' flag make sure to put a file path\n", args[2], args[0]);
            closedir(c2);
            if (f2 != -1)
                close(f2);
            exit(-1);
        }

        close(f2);
    }

}


/*
typedef struct _Node {

    char* word;
    int occ;
    struct _Node* left;
    struct _Node* right;
} node;

*/

typedef struct _codeBST { //use strcmp for binary search tree

    char* bits;
    char* word;
    struct _codeBST* left;
    struct _codeBST* right;
} code;


void freeCodes (code* head) {

    if (head == NULL)
        return;

    freeCodes(head->left);
    freeCodes(head->right);

    if (head->word != NULL)
        free(head->word);
    if (head->bits != NULL)
        free(head->bits);
    free(head);
    
}




code* createCode(  /*code *curr*/){ // add malloc block
  	//code* newCode = (code*)malloc(sizeof(code));
    //return newCode;

   	return (code*)malloc(sizeof(code));
}


int insertCode(code* head, char* word, char* bits) {

    code* curr = head;
    int mc = 0;

    while (curr != NULL) {

        int direction = strcmp(word, curr->word);

        if (direction < 0) {

            if (curr->left == NULL) {

                curr->left = createCode();
                while (curr->left == NULL) {

                    if (mc == 2) {

                        fprintf(stderr, "Fatal Error: malloc was unable to allocate memory after %d tries", mc);
                        freeCodes(head);
                        return -1;
                    }
                    fprintf(stderr, "Error: malloc failed to allocate memory, trying again");
                    curr->left = createCode();
                    mc++;
                }
                mc = 0;

                curr->left->word = word;
                curr->left->bits = bits;
                return 1;
            }
            else
                curr = curr->left;
        }
        else {

            if (curr->right == NULL) {

                curr->right = createCode();
                while (curr->right == NULL) {

                    if (mc == 2) {

                        fprintf(stderr, "Fatal Error: malloc was unable to allocate memory after %d tries", mc);
                        freeCodes(head);
                        return -1;
                    }
                    fprintf(stderr, "Error: malloc failed to allocate memory, trying again");
                    curr->right = createCode();
                    mc++;
                }
                mc = 0;

                curr->right->word = word;
                curr->right->bits = bits;
                return 1;
            }
            else
                curr = curr->right;
        }
    }
    return 1;
}



char* fillBuff (char* file) {

    int mc = 0;

    int fTemp = open(file, O_RDONLY);
    off_t fsize = lseek(fTemp, 0, SEEK_END);
    close(fTemp);

    char *buff = malloc(fsize*sizeof(char) + 1); 
    while (buff == NULL) {

        if (mc == 2) {

            fprintf(stderr, "Fatal Error: malloc was unable to allocate memory after %d tries", mc);
            file = NULL;
            return NULL;
        }
        fprintf(stderr, "Error: malloc failed to allocate memory, trying again");
        buff = malloc(fsize*sizeof(char) + 1);
        mc++;
    }
    mc = 0;

    //memset(buff, '\0', fsize);
    buff[fsize] = '\0';
    int readIn = 0;
    int status = 0;
    int fd = open(file, O_RDONLY);


    do{  // reads buffer char by char
            status = read(fd, buff+readIn, fsize - readIn);
            readIn += status;
                
                    
    } while (status > 0 && readIn < fsize); 

    close(fd);

    return buff;
}


code* fillCodes(char* file) {

    int mc = 0; // malloc fail counter

    char* buff = fillBuff(file);
    if (file == NULL) 
        exit(-1);

    if (strlen(buff) <= 2) {

        fprintf(stderr, "Warning: nothing to decompress because HuffmanCodebook has no codes\n");
        free(buff);
        exit(-1);
    }

    //printf("buff: %s\n", buff);

    code* codes = (code*)malloc(sizeof(code));  //tree
    while (codes == NULL) {

        if (mc == 2) {

            fprintf(stderr, "Fatal Error: malloc was unable to allocate memory after %d tries", mc);
            free(buff);
            exit(-1);
        }
        fprintf(stderr, "Error: malloc failed to allocate memory, trying again");
        codes = (code*)malloc(sizeof(code));
        mc++;
    }
    mc = 0;


    codes->left = NULL;
  	codes->right = NULL;
    code* curr = codes;

    int count = 2;
  	//*esc = buff[0];
  	int bit = 0; // when bit is 0 read huffman bits, when 1 read as the word
  
  	//temporary string needed
  	char* tempStr;
    int st = 0;

    while (buff[count] != '\0') {

        //printf("buff[%d] = %c\n", count, buff[count]);

        if (bit == 0 && buff[count] == '0') {        
          
            if (curr->left == NULL) {
            	curr->left = createCode();

                while (curr->left == NULL) {
                    if (mc == 2) {

                        fprintf(stderr, "Fatal Error: malloc was unable to allocate memory after %d tries", mc);
                        free(buff);
                        freeCodes(codes);
                        if (tempStr != NULL)
                            free(tempStr);

                        exit(-1);
                    }
                    fprintf(stderr, "Error: malloc failed to allocate memory, trying again");
                    curr->left = createCode();
                    mc++;
                }
                mc = 0;

            }
            
            curr = curr->left;
        }
      	else if (bit == 0 && buff[count] == '1') {
          
            if (curr->right == NULL) {
            	curr->right = createCode();

                while (curr->right == NULL) {
                    if (mc == 2) {

                        fprintf(stderr, "Fatal Error: malloc was unable to allocate memory after %d tries", mc);
                        free(buff);
                        freeCodes(codes);
                        if (tempStr != NULL)
                            free(tempStr);

                        exit(-1);
                    }
                    fprintf(stderr, "Error: malloc failed to allocate memory, trying again");
                    curr->right = createCode();
                    mc++;
                }
                mc = 0;

            }
                
            
            curr = curr->right;
        }
        else if ( buff[count] == '\t' && buff[count - 1] != '\t' ) {

            int c = count + 1;
            st = 0;
            while (buff[c] != '\n') {
                c++;
            }

            tempStr = (char*)malloc((c - count) * sizeof(char));
            while (tempStr == NULL) {
                if (mc == 2) {

                    fprintf(stderr, "Fatal Error: malloc was unable to allocate memory after %d tries", mc);
                    free(buff);
                    freeCodes(codes);
                    exit(-1);
                }
                fprintf(stderr, "Error: malloc failed to allocate memory, trying again");
                tempStr = (char*)malloc((c - count) * sizeof(char));
                mc++;
            }
            mc = 0;

            tempStr[(c - count) - 1] = '\0';
            bit = 1;
        }
        else if ( buff[count] == '\n' && buff[count + 1] != '\n' ) { //printf("\n\tyo im innit\n");

            curr->word = tempStr;
          	curr = codes;
          	bit = 0;
            st = 0;

        }
        else {

            tempStr[st] = buff[count];
            st++;
        }


        count++;
      
    }
    //go through buffer and load nodes into tree
    free(buff);


    return codes;
}



code* fillCodesComp(char* file) {

    int mc = 0; // malloc fail counter

    char* buff = fillBuff(file);
    if (strlen(buff) <= 3) {

        fprintf(stderr, "Warning: nothing to compress because HuffmanCodebook has no codes\n");
        free(buff);
        exit(-1);
    }

    //printf("buff: %s\nsize is %d\n", buff, strlen(buff));

    code* codes = (code*)malloc(sizeof(code));  //tree
    while (codes == NULL) {

        if (mc == 2) {

            fprintf(stderr, "Fatal Error: malloc was unable to allocate memory after %d tries", mc);
            free(buff);
            exit(-1);
        }
        fprintf(stderr, "Error: malloc failed to allocate memory, trying again");
        codes = (code*)malloc(sizeof(code));
        mc++;
    }
    mc = 0;


    codes->left = NULL;
  	codes->right = NULL;

    int count = 2;
  	//*esc = buff[0];
  	int fill = 0; // changes to 1 when filling words and 0 when filling bitstring
  
  	//temporary string needed
  	char* tempStr;
    int st = 0;
    char* tempBit;
    int bitC = 0;

    // put a while loop that first finds the size of the first bit string in the sequence

    int ct = count + 1;
    while (buff[ct] != '\n') {
        ct++;
    }

    tempBit = (char*)malloc((ct - count) * sizeof(char));
    while (tempBit == NULL) {

        if (mc == 2) {

            fprintf(stderr, "Fatal Error: malloc was unable to allocate memory after %d tries", mc);
            free(buff);
            freeCodes(codes);
            exit(-1);
        }
        fprintf(stderr, "Error: malloc failed to allocate memory, trying again");
        tempBit = (char*)malloc((ct - count) * sizeof(char));
        mc++;
    }
    mc = 0;

    tempBit[(ct - count) - 1] = '\0';

    //printf("buff: %s\nsize is %d\n", buff, strlen(buff));

    while (buff[count] != '\0') { // need to insert them in a BST manner

        
        //printf("buff[%d] = %c\n", count, buff[count]);

        if ( buff[count] == '\t' && buff[count - 1] != '\t' ) {

            //curr->bits = tempBit;

            int c = count + 1;
            st = 0;
            while (buff[c] != '\n') {
                c++;
            }

            tempStr = (char*)malloc((c - count) * sizeof(char));
            while (tempStr == NULL) {

                if (mc == 2) {

                    fprintf(stderr, "Fatal Error: malloc was unable to allocate memory after %d tries", mc);
                    free(buff);
                    freeCodes(codes);
                    if (tempBit != NULL)
                        free(tempBit);
                    exit(-1);
                }
                fprintf(stderr, "Error: malloc failed to allocate memory, trying again");
                tempStr = (char*)malloc((c - count) * sizeof(char));
                mc++;
            }
            mc = 0;

            tempStr[(c - count) - 1] = '\0';
            fill = 1;
        }
        else if ( buff[count] == '\n' && buff[count + 1] != '\n') {

            if (codes->word == NULL) {
                
                codes->word = tempStr;
                codes->bits = tempBit;
            }
            else {

                insertCode(codes, tempStr, tempBit);
            }

          	fill = 0;
            st = 0;
            bitC = 0;


            int c = count + 1;
            while (buff[c] != '\0' && buff[c] != '\n') {
                c++;
            }

            tempBit = (char*)malloc((c - count) * sizeof(char));

            while (tempBit == NULL) {

                if (mc == 2) {

                    fprintf(stderr, "Fatal Error: malloc was unable to allocate memory after %d tries", mc);
                    free(buff);
                    freeCodes(codes);
                    if (tempStr != NULL)
                        free(tempStr);
                    exit(-1);
                }
                fprintf(stderr, "Error: malloc failed to allocate memory, trying again");
                tempBit = (char*)malloc((c - count) * sizeof(char));
                mc++;
            }
            mc = 0;

            tempBit[(c - count) - 1] = '\0';
        }
        else if (fill == 0) {

            tempBit[bitC] = buff[count];
            bitC++;
        }
        else if (fill == 1) {

            tempStr[st] = buff[count];
            st++;
        }


        count++;
      
    }
    //go through buffer and load nodes into tree

    free(tempBit);
    free(buff);

    //printf("left: '%s'\n", codes->left->word);
    //printf("right: '%s'\n", codes->right->word);
    //free(codes->left->word);
    //free(codes->right->word);
    
    return codes;
}





void dispCodes (code* head) {

    if (head == NULL)
        return;

    dispCodes(head->left);
    printf("head->word = %s\thead->bits = %s\n", head->word, head->bits);
    dispCodes(head->right);

    
}





void build(char *args[]) {

    int mc = 0;
    char* file = args[0];

    int fTemp = open(file, O_RDONLY);
    off_t fsize = lseek(fTemp, 0, SEEK_END);
    close(fTemp);

    char *buff = malloc(fsize*sizeof(char) + 1); 
    while (buff == NULL) {

        if (mc == 2) {

            fprintf(stderr, "Fatal Error: malloc was unable to allocate memory after %d tries", mc);
            exit(-1);
        }
        fprintf(stderr, "Error: malloc failed to allocate memory, trying again");
        mc++;
    }
    mc = 0;

    //memset(buff, '\0', fsize);
    buff[fsize] = '\0';
    int readIn = 0;
    int status = 0;
    int fd = open(file, O_RDONLY);


    do{  // reads buffer char by char

        status = read(fd, buff+readIn, fsize - readIn);
        readIn += status;            
    } while (status > 0 && readIn < fsize); 
    close(fd);

    printf("%s\n", buff);
    printf("%d\n", fsize);

    
    
}

/* void probably */ void decompress(code* codes, /*char* args[]*/ char* f, char* huff) { // have arguments to the functions have a flag to tell if it needs to recurse, and make conditional statements based on that

  	int mc = 0;

    code* curr = codes;

    char* hcz = fillBuff(f);

    if (f == NULL) {
        
        freeCodes(codes);
        exit(-1);
    }
        
    char* sub = strstr(f, ".hcz");
    int pos = sub - f;
    char* txt = f;
    txt[pos] = '\0';


    int text = open(txt, O_RDWR | O_CREAT, 00600);

    int count = 0;
    while (hcz[count] != '\0') {  //printf("hcz[%d] = '%c'\n", count, hcz[count]);

        if ( hcz[count] == '0' ) {

            curr = curr->left;

            if ( curr->left == NULL && curr->right == NULL) { //printf("\tcurr->word = '%s'\n", curr->word);

                write(text, curr->word, strlen(curr->word));
                curr = codes;
            }
        }
        else if ( hcz[count] == '1' ) {

            curr = curr->right;

            if ( curr->right == NULL && curr->right == NULL) { //printf("\tcurr->word = '%s'\n", curr->word); printf("\t size = '%d'\tcount = '%d'\n", strlen(hcz), count);

                write(text, curr->word, strlen(curr->word));

                int size = strlen(hcz) - 1;

                if (curr->word[0] == '\n' && count == size)
                    write(text, curr->word, strlen(curr->word));

                curr = codes;
            }
        }

        count++;
    }

	free(hcz);
    close(text);
    //freeCodes(codes);
    //dispCodes(codes);
    //printf("\tsuccess on %s\n", f);
  
  
}   


code* findCode(char* tar, code* head) {

    code* curr = head;

    while (curr != NULL) {

        int direction = strcmp(tar, curr->word);
        //printf("curr->word = '%s'\n", curr->word);

        if (direction < 0) 
            curr = curr->left;
        else if (direction > 0)
            curr = curr->right;
        else
            return curr;
    }
    return NULL;
}




void compress(code* codes, /*char* args[]*/ char* f, char* huff) { //make sure not to include ending newline if its not actually present

    int mc = 0; //printf("\t\tbegin on %s\n", f);

    char* txt = fillBuff(f);

    if (f == NULL) {
        
        freeCodes(codes);
        exit(-1);
    }

    char* fT = (char*)malloc(strlen(f) * sizeof(char) + 5);
    strcpy(fT, f);

    strcat(fT, ".hcz");

    //printf("\t'%s'\t'%s'\n", fT, f);

    int hcz = open(fT, O_RDWR | O_CREAT, 00600);
    free(fT);

    char* temp = NULL;
    int pos = 0;
    int cont = 0;
    int count = 0;

    //printf("The node with 'dog' contains: '%s'\n", findCode("dog", codes)->word);
    //dispCodes(codes);
    
    
    while (count <= strlen(txt)) {

        if (cont == 0) {

            
            int c = count;
            while( txt[c] != '\0' && txt[c] != ' ' && txt[c] != '\n' && txt[c] != '\t' ) {
                c++;
            }

            temp = (char*)malloc(((c - count)) * sizeof(char) + 1); //can be NULL if file starts with ' ', '\n', '\t', or '\0'
            temp[c - count] = '\0';
            pos = 0;
            cont = 1;
            continue;
        }
        else if ( txt[count] == '\0' || txt[count] == ' ' || txt[count] == '\n' || txt[count] == '\t' ) {

            code* tar = findCode(temp, codes);
            if (tar != NULL)
                write(hcz, tar->bits, strlen(tar->bits));

            cont = 0;
            free(temp);
            
            if ( txt[count] == '\0' || (txt[count] == '\n' && txt[count + 1] == '\0') )
                break;
            else {

                char arr[2] = {txt[count], '\0'};
                tar = findCode(arr, codes);
                write(hcz, tar->bits, strlen(tar->bits));
            }
            
        }
        else {  //printf("\t\tsegfault\n");

            //printf("txt[%d] = %c\n", count, txt[count]);
            temp[pos] = txt[count];                                                         
            pos++;
        }


        count++;
    }

    temp = NULL;
	free(txt);	
    close(hcz);
    //freeCodes(codes);
    //printf("\tsuccess on %s\n", f);

    
    
}





void recPrac(char* f, char* huff, char* flag, code* codes, struct Node* nodes) { // dont parse '.' and '..' when recursing through the directories

    //printf("\nSTART RECURSION WITH: \"%s\"\n", f);

    DIR* currentDir = opendir(f);

    struct dirent* currentThing = NULL;
    readdir(currentDir);
    readdir(currentDir);
    //currentThing = readdir(currentDir);

    while (currentThing = readdir(currentDir)) {

        if ( currentThing->d_type == DT_REG ) {

            //printf("\nFile: %s\n", currentThing->d_name);

            int size = strlen(currentThing->d_name);

            if (strcmp(flag, "-d") == 0) { //printf("File: %s\n", currentThing->d_name);

                if (strcmp(currentThing->d_name + (size - 4), ".hcz") == 0) {

                    char* temp = (char*)calloc((strlen(f) + strlen(currentThing->d_name)) * sizeof(char) + 2, 1);
                    strcpy(temp, f);
                    strcat(temp, "/");

                    decompress(codes, strcat(temp, currentThing->d_name), huff);
                    free(temp);
                }
            }
            else if (strcmp(flag, "-c") == 0) {

                if (strcmp(currentThing->d_name + (size - 4), ".txt") == 0) {

                    char* temp = (char*)calloc((strlen(f) + strlen(currentThing->d_name)) * sizeof(char) + 2, 1);
                    strcpy(temp, f);
                    strcat(temp, "/");
                        
                    compress(codes, strcat(temp, currentThing->d_name), huff);
                    free(temp);
                }
            }
            else if (strcmp(flag, "-b") == 0) { //printf("Inside\n");

                if (strcmp(currentThing->d_name + (size - 4), ".txt") == 0) {

                    char* temp = (char*)calloc((strlen(f) + strlen(currentThing->d_name)) * sizeof(char) + 2, 1);
                    strcpy(temp, f);
                    strcat(temp, "/");

                   // printf("temp = %s\n", temp);

                    char *buff = createBuff(strcat(temp, currentThing->d_name)); //filling in the buffer with the .txt file

                    nodes = finalAVL(nodes, buff);
                    free(temp);
                    free(buff);
                }
            }
        }
        else if ( currentThing->d_type == DT_DIR ) { 

            //printf("Dir: %s\n", currentThing->d_name);
            char* temp = (char*)malloc((strlen(f) + strlen(currentThing->d_name)) * sizeof(char) + 2);
            strcpy(temp, f);
            strcat(temp, "/"); //printf("\n\n%s%s\n\n", temp, currentThing->d_name);

            recPrac(strcat(temp, currentThing->d_name), huff, flag, codes, nodes); 
            free(temp);
        }
        
    } 
    closedir(currentDir);
    //printf("Done with %s\n", f);
    //freeCodes(codes);
    return;
}



int main (int argc,  char *argv[]) {

    checkValid(argc - 1, argv + 1);

    if (strcmp(argv[1], "-R") == 0) {           // need openddir(<name>) and readdir(DIR* <dir.desc.>)

        if (strcmp(argv[2], "-b") == 0) {

            struct Node * AVLroot = NULL;

            recPrac(argv[3], NULL, argv[2], NULL, AVLroot);

            int AVLsize = preOrder(AVLroot, 0);  // goes through avl tree to find # of nodes
            //printf("num of nodes: %d\n", AVLsize);
            heapnode ** minheap =  malloc(AVLsize*sizeof(heapnode));
            minheap = finalhuff(AVLroot,  AVLsize, minheap);
        }
        else if (strcmp(argv[2], "-c") == 0) {

            code* codes = fillCodesComp(argv[4]);

            recPrac(argv[3], argv[4], argv[2], codes, NULL);
            freeCodes(codes);
        }
        else if (strcmp(argv[2], "-d") == 0) {

            code* codes = fillCodes(argv[4]);

            recPrac(argv[3], argv[4], argv[2], codes, NULL);
            freeCodes(codes);
        }
    }
    else {

        if (strcmp(argv[1], "-b") == 0) {

            struct Node * AVLroot = NULL;
        
            char * file = argv[2];
            char *buff = createBuff(file); //filling in the buffer with the .txt file

            //build(buff);
            AVLroot = finalAVL(AVLroot, buff);

            int AVLsize = preOrder(AVLroot, 0);  // goes through avl tree to find # of nodes
            printf("num of nodes: %d\n", AVLsize);
            heapnode ** minheap =  malloc(AVLsize*sizeof(heapnode));
            minheap = finalhuff(AVLroot,  AVLsize, minheap);
        }
        else if (strcmp(argv[1], "-c") == 0) {

            code* codes = fillCodesComp(argv[3]);
            compress(codes, argv[2], argv[3]);
            freeCodes(codes);
        }
        else if (strcmp(argv[1], "-d") == 0) {

            code* codes = fillCodes(argv[3]); 
            decompress(codes, argv[2], argv[3]);
            freeCodes(codes);
        }
    }
    




    return 0;
}