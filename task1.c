//IT19195780 R.M.R.R Ratnayake
//SOS Assignment Task1

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <pthread.h>

// Initializing the mutex
pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER; 

//global variables
int i = 0;
int j = 0; 

//structure
struct global{

  int number[100]; //can be changed
  int cmd;
  float answer;
  
};

//calculation and writting
void* average(void* arg);   


//main function
int main(void)
{
    //to count lines
    int lines= 0;
    char ch;
    int count = 0;

    FILE *ptr;

    if((ptr = fopen("dataset.txt", "r")) == NULL) { //opening and checking for errors
        printf("File failed to open\n");
        exit(0);
    }
    else {

        ch = getc(ptr);

        while (ch != EOF) {
            if (ch == '\n') {
                lines += 1;
            }

            ch = getc(ptr);
        }
    }
    fclose(ptr);
//end of counting


    FILE *ptr3;

    if((ptr3 = fopen("dataset.txt", "r")) == NULL) { //opening and checking for errors
        printf("File failed to open\n");
        exit(0);
    }
    if(ptr3 != NULL){

        char line[BUFSIZ]; // default buffer size
        

        pthread_t thrd[lines + 1]; //declaration of thread

        struct global str[lines + 1]; //declaring struct

        while (fgets(line, sizeof line, ptr3) != NULL) { //scan line by line
            int num;
            int flag = 0;
            char *l = line;
            int count = 0;
            j++;
            //printf("Line: %s\n", line); //since the threads are being runned in their own speed, enabeling a print function would give a mixed output


            while (sscanf(l, "%d%n", &num, &flag) == 1) { //separate numbers
                str[j].number[count] = num;
                l += flag; //Adding flag
                count++;
            }

                str[j].cmd = count;

                // Creating the threads
                pthread_create(&thrd[j], NULL, average, &str[j]);
        }
        
            // joining the threads
            for(i = 0; i < j; i ++) {
                pthread_join(thrd[i], NULL);
            }
        
    fclose(ptr3);
    pthread_mutex_destroy(&lock); //thread lock released

    return 0;
    }
    
}


void* average(void* arg)
{
    pthread_mutex_lock(&lock); //implementing the lock
    struct global *str2 = (struct global*) arg;

    int total = 0;   // variables
    float average = 0;
    int count2 = 0, i = 0;
    count2 = (str2->cmd);

    for(i = 0; i < count2; i ++){
      total = total + (str2->number[i]);
    }
    //printf("Line %d\n", i);
    printf("Total: %d\tCount: %d\t", total, count2);
    average = (total / (float)count2); //calculaion of average
    printf("Average: %.2f\n", average);

    FILE *ptr2; //writting to the file

        ptr2 = fopen("Average.txt", "a+");
        fprintf(ptr2,"%.2f\n", average); 
    

    fclose(ptr2);
    
    pthread_mutex_unlock(&lock); //releasing lock
    pthread_exit(0);
}

