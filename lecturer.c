//IT19195780 R.M.R.R Ratnayake
//SOS Assignment 1 - Task 2 - Lecturer Portal

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
#include <pthread.h>

//thread lock
pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;

//definitions
#define SEGSIZE 100

//functions for options
void usage();
void view();
int upload();
int update();
void edit();
int download();
int delete();

//for shared memory
char* getSHM(int k);
int addToSHM(char caseid[], int id);
int validateSHM(int k);

//for file handling
char *Title(char * fblock);
char *Content(char * fblock);
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
	case 'i': upload();
			break;
	case 'd': download();
			break;
	case 'r': delete();
			break;
	case 'u': update();
			break;
	case 'e': edit();
			break;
	default: usage();
	}

//thread release
pthread_mutex_destroy(&lock);
    return 0;

}

//usage option function
void usage()
{
	fprintf(stderr, "Lecturer Portal\n");
	fprintf(stderr, "\nUSAGE:\n (v)- View\n"); //to view
	fprintf(stderr, " (i)- Upload\n"); //to upload
	fprintf(stderr, " (u)- Update\n"); //to update
	fprintf(stderr, " (e)- Edit\n"); //to edit
	fprintf(stderr, " (d)- Download\n"); //to download
	fprintf(stderr, " (r)- Delete\n"); //to delete
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

//upload function
int upload(){

   pthread_mutex_lock(&lock);

	//variables
  	char filename[50];
	FILE *ptr2;
	char segment[999];
	char tag[30];
	char *title;
	char *content;
	char *data;

	int jkey;
	int count;

	//taking file path

	printf("Enter path to case study to upload (filename): ");
	scanf("%s", filename);

	jkey = shareKey; //Passing the initializer key to temp variable
	count = atoi(getSHM(itemKey)); //Getting the current count of records
	jkey += count; //getting new key value
	count++; //increment

	if((ptr2 = fopen(filename, "a")) == NULL) { //open file and check for errors
		puts("Failed to open file");
		return -1;
	}

	//taking the case study to be saved in the shared memory
	printf("Enter the name for case study : ");
	scanf("%s", tag);

	while (fgets(segment, 1000, ptr2) != NULL)

	printf("%s\n", segment);

	//Combining the Name of the case study and it's content together
	
	title = combine(tag,'*');
  	content = combine(segment,'#');
	data = strcat(title,content);

	//Adding data to the shared memory
	addToSHM(data,jkey);

	//Updating the count of records
	char string1[]="";
  	sprintf(string1,"%d",count);
  	addToSHM(string1,itemKey); //Updating Finished

	printf("Case ID is - %i\n",jkey);
  	printf("Please use the new case ID to access the files\n");

	fclose(ptr2);

	return 0;

  	pthread_mutex_unlock(&lock);
  	pthread_exit(0);
}

//update function
int update(){

	//variables
  	int k;
	int shmid;
	key_t key;
	char *shm;

	char filename[30];
	char ctitle[24];
	char *title;
	char *body;
	char *data;
	char string[999];

  	printf("Enter case ID to update: ");//user input
 	scanf("%d", &k);

 	key = k;

	FILE *ptr;

	printf("Enter path to the edited file(filename): ");
	scanf("%s", filename);

	if((ptr = fopen(filename, "r")) == NULL) { //open file and check for errors
		puts("Failed to open file");
		return -1;
	}

	printf("Enter the title: ");
	scanf("%s", ctitle);

	while (fgets(string, 1000, ptr) != NULL)

	printf("%s\n", string);

	title = combine(ctitle,'*');
	body = combine(string,'#');

	data = strcat(title,body);

	if ((shmid = shmget(key, SEGSIZE, 0666)) < 0) //check for shared memory failures
	{
		printf("shmget failed\n");
		exit(1);
	}

	if ((shm = shmat(shmid, NULL, 0)) == (char *) -1)
	{
		printf("shmat failed\n");
		exit(1);
	}

	memcpy(shm,data,strlen(data));

}

//edit function
void edit(){

	//variables
  	char caseid[5];
  	FILE *ptr1;
  	FILE *ptr2;
	char filename[50];

	printf("Enter the case ID: ");  //taking user inputs
	
	scanf("%s", caseid);

    //accessing the shared memory for content
  	char *segment = getSHM(atoi(caseid));

    //splitting title and content
  	char *title = Title(segment);
  	char *content = Content(segment);

	chdir("downloads/"); //cd into the downloads folder
  	
  	snprintf(filename, sizeof(filename), "%s", title); //Getting the case study name as filename
    strcat(filename,".txt");

	if((ptr1 = fopen(filename, "w")) == NULL) { //open file and check for errors
			puts("File failed to open");
	}

    fprintf(ptr1, "%s", content);
	fflush(ptr1);
    fclose(ptr1);
	
	printf("File Downloaded Successfully\n");
	printf("To update, use the update function\n");

    char *arg4[] = {"vi",filename,NULL};
    execvp("vi",arg4);

	printf("New case study downloaded in your downloads folder\n"); //output
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

//delete function
int delete()
{

  	pthread_mutex_lock(&lock);
  	char caseid[4];

  	printf("Enter case ID to delete: ");
  	scanf("%s", caseid);

	int shmid;
	key_t key;
	char *shm;
	key = atoi(caseid);

	if (validateSHM(key)==0) { //valiating shared memory
		printf("Invalid Case ID\n");
		return 0;
	}

	if ((shmid = shmget(key, SEGSIZE, 8666)) < 0)
	{
		printf("shmget failed\n");
		exit(1);
	}

	shmctl(shmid, IPC_RMID, NULL); //using flag

  	printf("Case Delete Successfull\n");

	return 1;
   	pthread_mutex_unlock(&lock);
    pthread_exit(0);

}

//seperate title function
char *Title(char * fsegment)
{
	char *segment1;
   	char delim[] = "*"; //Delimiter used to seperate the name of the case study
	char temp[50]; //temporary array

	strcpy(temp, fsegment);  
	segment1  = strtok(temp, delim); // Seperate by delimiter and passs to segment1

	
  	return segment1;
}

//separate content function
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

//validate the shared memory function
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

//Add member to shared memory function
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

	return 1;  pthread_mutex_unlock(&lock);
  	pthread_exit(0);


}

//Get the shared memory access function
char* getSHM(int k)
{
	int shmid;
	key_t key;
	char *shm;
	key = k;

	if ((shmid = shmget(key, SEGSIZE, 8666)) < 0) //Checking if the segment exists or not?
	{
		printf("shmget failed(Case Id Not Available)\n");
		exit(1);
	}

	if ((shm = shmat(shmid, NULL, 0)) == (char *) -1) //Checking whether you can attach to it or not?
	{
		printf("shmat failed(Case Id Not Available)\n");
		exit(1);
	}

	return shm; //return the segment
}

//end of file