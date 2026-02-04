#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdint.h>
#include <string.h>
#include <time.h>
#include <sys/time.h>
#include <pthread.h> //library for threads

#define KSIZE (16)
#define VSIZE (1000)

#define LINE "+-----------------------------+----------------+------------------------------+-------------------+\n"
#define LINE1 "---------------------------------------------------------------------------------------------------\n"

long long get_ustime_sec(void);
void _random_key(char *key,int length);

pthread_mutex_t read_c;  
pthread_mutex_t write_c; 
double timeOfread;  
double timeOfwrite;  

struct figures
{
    long int c_count;
    int r_r;
    int t_thrds;
};



