//IT19195780 R.M.R.R Ratnayake
//SOS Assignment 1 - Task 2 - Student Portal

#ifndef _GNU_SOURCE
#define _GNU_SOURCE 1
#endif

//headers
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <semaphore.h>
#include <ctype.h>


//definitions
#define SEGSIZE 100

//functions for options
int download();
void view();
void usage();


//for shared memory
char* getSHM(int k);
int addToSHM(char caseid[], int id);
int validateSHM(int k);

//for file handling
char *Title(char * fsegment);
char *Content(char * fsegment);
char *combine(const char *old, const char c);

//shared memory keys
int itemKey = 6060;
int shareKey = 8888;


//main function begin
int main(int argc,char *argv[])
{
  	//Initializing the counter for firsttime use!
	if (validateSHM(itemKey) == 0)
		addToSHM("0",itemKey);

   	if(argc == 1)
		usage();
	
	//switch for options
	switch(tolower(argv[1][0]))
	{
	case 'v': view();
			break;
	case 'd': download();
			break;
	default: usage();
	}


    return 0;

}

//usage option function
void usage()
{
	fprintf(stderr, "Student Portal\n");
	fprintf(stderr, "\nUSAGE:\n (v)- View\n"); //to view
	fprintf(stderr, " (d)- Download\n"); //to download
	exit(1);
}

//list function
void view() //explained in the report
{
	int item_count = atoi(getSHM(itemKey));

  	printf("Case ID \t| Name of the Study\n");

	for(int i = shareKey; i < (shareKey + item_count); i++)
	{
		if (validateSHM(i) == 0){
      		continue;
   	 	}

		char *data = getSHM(i);
		char *tag = Title(data);
		printf("%i \t\t| %s\n", i, tag);
	}
}


//download function
int download()
{

	//variables
  	char caseid[5];
  	FILE *ptr1;
	char filename[50];

  	printf("Enter case ID: ");  //Getting User Input Case ID
  	scanf("%s", caseid);

    //accessing the shared memory for content
	char *segment = getSHM(atoi(caseid));

  	//splitting title and content
	char *title = Title(segment);
	char *content = Content(segment);

	chdir("downloads/"); //cd into the downloads folder

	snprintf(filename, sizeof(filename), "%s", title); //Getting the case study name as filename
  	strcat(filename,".txt"); // Adding the extension
	
	if((ptr1 = fopen(filename, "w")) == NULL) { //openning file and checking for errors
		puts("File failed to open");
		return -1;
	}
  
	fprintf(ptr1, "%s", content);
 	fflush(ptr1);
  	fclose(ptr1);

  	printf("File Downloaded Successfully\n");
  	return 0;
}

//separate title function
char *Title(char * fsegment)
{
	char *segment1;
   	char delim[] = "*"; //Delimiter used to seperate the name of the case study
	char temp[50]; //temporary array

	strcpy(temp, fsegment);  
	segment1  = strtok(temp, delim); // Seperate by delimiter and passs to segment1

	
  	return segment1;
}

//separate Content function
char *Content(char * fsegment)
{
	char temp2[999]; // temporarily array to store the segment from Shared Memory
  	char delim1[] = "*";  // Delimiter to seperate the name of case study
	char delim2[] = "#"; // Delimiter to seperate the content

	strcpy(temp2, fsegment);
	char *segment2 = strtok(temp2, delim1);
	
	segment2 = strtok(NULL,delim2);
	
	return segment2;
}

//combine title and content function
char *combine(const char *old, const char c)
{

    int result;
    char *new;

    //Combines the special character used as the delimiter with the string passed into it
    result = asprintf(&new, "%s%c", old, c);

    if (result == -1){
    	new = NULL;
    }
    return new;
}


//valdate the shared memory function
int validateSHM(int k)
{
	int shmid;
	key_t key;
	key = k;

	if ((shmid = shmget(key, SEGSIZE, 8666)) < 0) //Checking if the segment exists
	{
		return 0;
	}

	return 1;
}


//adding new member to the shared memory function
int addToSHM(char data[], int id)
{

	int shmid;
	key_t key;
	char *shm;
	key = id;

	if ((shmid = shmget(key, SEGSIZE, IPC_CREAT | 8666)) < 0) //Creating the shared Memory block
	{
		printf("shmget failed\n");
		exit(1);
	}

	if ((shm = shmat(shmid, NULL, 0)) == (char *) -1) //Attaching to shared Memory
	{
		printf("shmat failed\n");
		exit(1);
	}

	strncpy(shm, data, strlen(data));  //Passing the content

	return 1;


}

//get shared memory access function
char* getSHM(int k)
{
	int shmid;
	key_t key;
	char *shm;
	key = k;

	if ((shmid = shmget(key, SEGSIZE, 8666)) < 0) //Checking if the segment exists or not?
	{
		printf("shmget failed\n");
		exit(1);
	}

	if ((shm = shmat(shmid, NULL, 0)) == (char *) -1) //Checking whether you can attach to it or not?
	{
		printf("shmat failed\n");
		exit(1);
	}

	return shm; //return the segment
}

//end of file
