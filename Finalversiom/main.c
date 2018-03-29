/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   main.c
 * Author: Jonat
 *
 * Created on 11 January 2017, 21:54
 */

/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
*/
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>

typedef struct info_s{
    int nwords;
    char** words; //Pointers to create a 2 Dimensional Array
    struct info_s *next; // Pointer to link the list

}info_t;

void filechecker (FILE *inp, FILE *outp); 
//this function checks the input file for any errors in it's format
info_t *dma (FILE *inp, FILE *outp, info_t *previous);
//this is the dynamic memory allocator (dma) where the linked list is created
void printlist (info_t *start, int *nlines, FILE *inp, FILE *outp);
//contains the algorithm that switches the words and prints them.
char *readline(FILE *inp, int *endoffile);
// this function reads each sentence (line) and stores it in a dynamic array

int main(void) 

{
 
    time_t timestart; 
    time_t timeend; 
    double execution; // variable for execution time

    FILE *inp;         /* pointer to input file */
    FILE *outp;        /* pointer to output file */
  
    time(&timestart); 
           
    inp = fopen("textin.txt", "r");
    outp = fopen("textout.txt", "w");

    fprintf(outp,"Start Time = %s", ctime(&timestart)); 

    filechecker(inp, outp); // Calling file checker function
   
    time (&timeend);
    fprintf(outp,"End Time = %s", ctime(&timeend));
    
    execution = difftime(timeend, timestart);
    fprintf(outp,"Execution time = %.0f Seconds\n", execution);
    
    fclose(inp); //Closing the files 
    fclose(outp);//Closing the files
     
    return (EXIT_SUCCESS);
}

void filechecker(FILE *inp, FILE *outp)
{
    int nlines=0, i=0;
    int nchar=0;
    int nwords=0, flag=0;
    int endoflinespacecheck=0;
    int endoffile =0;
    int throwaway;
   
    const char s[2] = " ";
    char *token;
    char *string;
   
    static info_t *start=NULL;
 
    
    fscanf(inp,"%d",&nlines); // Scanning the first line to get the number
    fgetc(inp); //moving file cursor to /r
    fgetc(inp);//moving file cursor to /n i.e. next line
    //fix here for /r/n and recursion
   
    if (nlines > 1000) 
        // to print an error if the number of lines is more than 100
    {
         printf("ERROR: The maximum number of lines allowed is 1000, please reduce your file \n");
         fprintf(outp,"ERROR: The maximum number of lines allowed is 1000, please reduce your file \n");
         flag = -1; //Error flag
    }

    /*the endoffile variable is sent into a custom function which reads the
     input file line by line. The endoffile variable is set to -1 when the 
     * end of file is reached. This function is similar in result to the fgets()
     however the data is stored in a dynamic string*/
    while(endoffile != -1) 
   
    //To keep looping until end of file has been reached
    { 
         string = readline(inp, &endoffile); 
         /*Custom function to store the line from input file dynamically where
          the output is returned into string*/
        
         if (string[0]== ' ' || string[0]== '\r' || string[0]== '\n' || string[0] == '\0') 
             // Checking for empty lines or spaces in the beginning of line
         {
             printf("ERROR: Line %d does not begin with a character or it is empty\n", i+1);
             fprintf(outp,"ERROR: Line %d does not begin with a character or it is empty\n", i+1);
             flag = -1; //Error Flag
         }
         
        endoflinespacecheck = strlen(string) - 1;
        if (string[endoflinespacecheck] == ' ')
            // Checks for spaces at the end of a line
        {
             printf("ERROR: Line %d ends with a space, please remove the space at the end of the line\n", i+1);
             fprintf(outp,"ERROR: Line %d ends with a space, please remove the space at the end of the line\n", i+1);
             flag = -1;
             
        }
        
        //Splits the string on the occurrence of a space
        token = strtok(string, s);
        while( token != NULL ) 
        {
            nchar = nchar + strlen(token); //counts the number of characters 
            token = strtok(NULL, s);
            nwords++; //increments the number of words in the file
        }
        i++; //Counting the number of lines
   }

    if(i != nlines)
    {
        //if the number of lines counted does not match the number of lines stated in the file
        printf("ERROR: The number of lines specified in the file does not match the lines counted\n");
        fprintf(outp,"ERROR: The number of lines specified in the file does not match the lines counted\n");
        flag = -1;
 
        
    }
   
    if(flag != -1 && string != NULL)
        //if there is no error flag print the file statistics
    {
        fprintf(outp,"The number of words in the file is %d\n", nwords);    
        fprintf(outp,"The number of characters in the file is %d\n", nchar); 
    }
 
   
   rewind(inp); //Moves the file cursor to the beginning of the file
    
   endoffile = 0;
   fscanf(inp,"%d",&throwaway); // Scanning the first line to get the number
   getc(inp); //moving file cursor to /r
   getc(inp);//moving file cursor to /n i.e. next line
  
   start = dma(inp,outp,start);
   
   if (flag !=-1 && string != NULL)
   {
       printlist(start, &nlines, inp, outp); //printing the list
   }
   
}

info_t *dma (FILE *inp, FILE *outp, info_t *previous) // dynamic memory allocator
{
    const char s[2] = " ";
    char *token, *string;
    
    int d = 0; 
    //Variable to move through the 2 dimensional array veritcaly i.e. word by word
    int wordlength = 0, stringlength = 0, throwaway;
    static int endoffile = 0;
    static info_t *start=NULL;
    static info_t *newest=NULL;
    
   //inp = fopen("textin.txt", "r");
  // outp = fopen("textout.txt", "w");


   if (start==NULL)
       //if the first entry is being entered
   {
        info_t *info = malloc(sizeof(info_t)); 
        
        if (info == NULL)
            {
                printf("Not enough memory");
                fprintf(outp, "Not enough memory");
                return(NULL);
            }
        //Allocating the memory dynamically for the structure type info_t
        string = readline(inp, &endoffile);
        //Making a copy of the string since strtok() is used twice
        stringlength = strlen(string); //getting the length of the string
        char string2[stringlength];
        //dynamically creating an array of the same size as the string
        strcpy(string2,string); //making a copy of the string
        
        info->nwords = 0;
        /*This part counts the words in the string so we can dynamically create 
         a 2d array of the correct size*/
        token = strtok(string, s);
        while( token != NULL ) 
        {
            info->nwords++; 
            token = strtok(NULL, s);
        }
        
        info->words = malloc(info->nwords * sizeof(char*));
                
        if (info->words == NULL)
        {
            printf("Not enough memory");
            fprintf(outp, "Not enough memory");
            return(NULL);
        }
        /*creating the vertical dimension of the array to the number of words in
         a line (sentence) dynamically*/
        
        /*Splitting the sentence into words and saving them dynamically in the
         2d array*/
        d=0;
        token = strtok(string2, s);
        while( token != NULL ) 
        {
            wordlength = strlen(token);
            //getting the number of characters in a given word
            info->words[d]= malloc(wordlength * sizeof(char));
            
            if (info->words[d] == NULL)
            {
                printf("Not enough memory");
                fprintf(outp, "Not enough memory");
                return(NULL);
            }
            /*creating an array of the right length to hold the word this means
             that the array is of the right size exactly i.e. dynamic*/
            strcpy(info->words[d],token);
            //copying the word in the space dynamically created for it
            token = strtok(NULL, s); 
            d++;
        }

        info->next=NULL;
        //Setting the pointer to next element of list to NULL
        if(previous != NULL)
        {
            previous->next = info;
            //Linking the list if there was a previous entry
        }
        
        start = info; //Calling function
        newest = dma(inp,outp,start);
   }
   else 
   {
   while(endoffile != -1) 
    
   
    //To keep looping until end of file has been reached
    { 
        string = readline(inp, &endoffile); 
        info_t *info = malloc(sizeof(info_t));
        
            if (info == NULL)
            {
                printf("Not enough memory");
                fprintf(outp, "Not enough memory");
                return(NULL);
            }
        
        //Allocating the memory dynamically for the structure type info_t

        //Making a copy of the string since strtok() is used twice
        stringlength = strlen(string); //getting the length of the string
        char string2[stringlength];
        //dynamically creating an array of the same size as the string
        strcpy(string2,string); //making a copy of the string
        
        info->nwords = 0;
        /*This part counts the words in the string so we can dynamically create 
         a 2d array of the correct size*/
        token = strtok(string, s);
        while( token != NULL ) 
        {
            info->nwords++; 
            token = strtok(NULL, s);
        }
        
        info->words = malloc(info->nwords * sizeof(char*));
        if (info->words == NULL)
        {
            printf("Not enough memory");
            fprintf(outp, "Not enough memory");
            return(NULL);
        }
        /*creating the vertical dimension of the array to the number of words in
         a line (sentence) dynamically*/
        
        /*Splitting the sentence into words and saving them dynamically in the
         2d array*/
        token = strtok(string2, s);
        d=0;
        while( token != NULL ) 
        {
            wordlength = strlen(token);
            //getting the number of characters in a given word
            info->words[d]= malloc(wordlength * sizeof(char));
            
            if (info->words[d] == NULL)
            {
                printf("Not enough memory");
                fprintf(outp, "Not enough memory");
                return(NULL);
            }
            /*creating an array of the right length to hold the word this means
             that the array is of the right size exactly i.e. dynamic*/
            strcpy(info->words[d],token);
            //copying the word in the space dynamically created for it
            token = strtok(NULL, s); 
            d++;
        }

        info->next=NULL;
        //Setting the pointer to next element of list to NULL
        if(previous != NULL)
        {
            previous->next = info;
            //Linking the list if there was a previous entry
        }       
       
        if (endoffile != -1)
        {
            newest = dma(inp,outp, info); //recursion
        }
      
   
   }
   }
    return (start); //Returning the structure       
}

void printlist (info_t *start, int *nlines, FILE *inp, FILE *outp)
{
    info_t *current = start;
    int x=0, b=0, z=0, h=0, l=0, j=0, n=0, k=0;
    
    printf("Input");
    printf("%69s", "Output\n"); //Justification
    printf("%d", *nlines);
    
    while (current != NULL)
    { 
        printf("\r");
        printf("\t\t\t\t\t\t\t\t   Case %d: ",++l); //Justification
        fprintf(outp,"Case %d: ",l);
        
        for(x=current->nwords-1;x>-1;x--)
            //for loop according to the number of words in a line
        {
            printf("%s",current->words[x]);
            fprintf(outp,"%s",current->words[x]);
            printf(" ");
            fprintf(outp," ");
        }
        printf("\n");
        fprintf(outp,"\n");
       
         //Similar algorithm to print original words onto console without flipping
         for(j=0;j<current->nwords;j++)
         {
             printf("%s",current->words[j]);
             printf(" ");
         }

        
        current = current->next;
    }
}

char *readline(FILE *inp, int *endoffile)
{
  char *str, c = NULL; 
  int i = 0, j = 1;

  str = (char*)malloc(sizeof(char));
  //creating an array
  
  while (1) {
      
    c = getc(inp);
    // read the character by character form the input file
    
    if (c == '\r')
    {
        c = getc(inp);
        break;
    }
    
        if (c == '\n')
    {
        break;
    }
        
    
    *endoffile =0; //Resetting the end of file variable
    if (c == -1)
    {
        *endoffile = -1;
        break;
        //stops if the end of file has been reached
    }


    /* re-allocate (resize) memory for character read to be stored by adding an
       extra element to the array*/
    str = (char*)realloc(str, j * sizeof(char));

    // store read character by making pointer point to c
    str[i] = c;

    i++;
    j++;
  }
  
   str[i] = '\0'; //adding the null terminator to know when end end of sentence

  return(str); //Returns the dynamic string
}