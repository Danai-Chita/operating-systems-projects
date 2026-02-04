#include "bench.h"

void _random_key(char *key,int length) {
	int i;
	char salt[36]= "abcdefghijklmnopqrstuvwxyz0123456789";

	for (i = 0; i < length; i++)
		key[i] = salt[rand() % 36];
}

void _print_header(int count)
{
	double index_size = (double)((double)(KSIZE + 8 + 1) * count) / 1048576.0;
	double data_size = (double)((double)(VSIZE + 4) * count) / 1048576.0;

	printf("Keys:\t\t%d bytes each\n", 
			KSIZE);
	printf("Values: \t%d bytes each\n", 
			VSIZE);
	printf("Entries:\t%d\n", 
			count);
	printf("IndexSize:\t%.1f MB (estimated)\n",
			index_size);
	printf("DataSize:\t%.1f MB (estimated)\n",
			data_size);

	printf(LINE1);
}

void _print_environment()
{
	time_t now = time(NULL);

	printf("Date:\t\t%s", 
			(char*)ctime(&now));

	int num_cpus = 0;
	char cpu_type[256] = {0};
	char cache_size[256] = {0};

	FILE* cpuinfo = fopen("/proc/cpuinfo", "r");
	if (cpuinfo) {
		char line[1024] = {0};
		while (fgets(line, sizeof(line), cpuinfo) != NULL) {
			const char* sep = strchr(line, ':');
			if (sep == NULL || strlen(sep) < 10)
				continue;

			char key[1024] = {0};
			char val[1024] = {0};
			strncpy(key, line, sep-1-line);
			strncpy(val, sep+1, strlen(sep)-1);
			if (strcmp("model name", key) == 0) {
				num_cpus++;
				strcpy(cpu_type, val);
			}
			else if (strcmp("cache size", key) == 0)
				strncpy(cache_size, val + 1, strlen(val) - 1);	
		}

		fclose(cpuinfo);
		printf("CPU:\t\t%d * %s", 
				num_cpus, 
				cpu_type);

		printf("CPUCache:\t%s\n", 
				cache_size);
	}
}

//Here we call the writer
void *call_writer(void *arg)
{
	struct figures *f = (struct figures *)arg;  //here we make a struct
	_write_test(f->c_count, f->r_r , f->t_thrds); //call the method _write_test from the file kiwi.c
	return 0;
}

//Here we call the readers
void *call_readers(void *arg)
{
	struct figures *f = (struct figures *)arg; //here we make a struct
	_read_test(f->c_count, f->r_r , f->t_thrds);  //call the method _read_test from the file kiwi.c
	return 0;
}


int main(int argc,char** argv)
{
	int mythreads; 
	long int count;
	int i;
	int per,per2; //percentages
	
	//Create threads
	pthread_t *number_code;
    	pthread_t *number_code1;
	pthread_t *number_code2;

	//Memory Commitment dynamically
	number_code = (pthread_t*)malloc(1000000);
	number_code1= (pthread_t*)malloc(1000000);
	number_code2= (pthread_t*)malloc(1000000);
	struct figures fig1,fig2,fig3;

	//initialization of variables and threads 
	timeOfread = 0; 
	timeOfwrite = 0; 
	pthread_mutex_init(&write_c,NULL);
	pthread_mutex_init(&read_c,NULL);

	srand(time(NULL));
	if (argc < 4) //here we change argc from 3 to 4 because we accept 4 arguments from the input
	{  
		fprintf(stderr,"Usage: db-bench <write | read | readwrite> <count> <mythreads> <r> \n"); //Format which accept the program
		exit(1);
	}
	
	if (strcmp(argv[1], "write") == 0) {
		int r = 0;

		count = atoi(argv[2]);
		_print_header(count);
		_print_environment();
		if (argc == 5) //we change argc from 4 to 5
			r = 1;
		
		mythreads = atoi(argv[3]); // Take the number of threads from the input

		open_close(1); //Open  base

		fig1.c_count = count;
		fig1.r_r = r;
		fig1.t_thrds = mythreads;

		//Creation of threads
		for(i=0; i<mythreads; i++)
		{
			pthread_create(&number_code[i],NULL,call_writer,(void *)&fig1);
		}
		for(i=0; i<mythreads; i++)
		{
			pthread_join(number_code[i],NULL);
		}
		
		open_close(2); //Close base

		printf(LINE);
		printf("|Random-Write (done:%ld): %.6f sec/op; %.1f writes/sec(estimated); cost:%.3f(sec);\n"
		,fig1.c_count, (double)(timeOfwrite / fig1.c_count)
		,(double)(fig1.c_count / timeOfwrite)
		,timeOfwrite);
	} 
	else if (strcmp(argv[1], "read") == 0) {
		int r = 0;

		count = atoi(argv[2]);
		_print_header(count);
		_print_environment();
		if (argc == 5) //we change argc from 4 to 5
			r = 1;
		mythreads = atoi(argv[3]); //Take the number of threads from the input

		open_close(1); //Open base

		fig1.c_count = count;
		fig1.r_r = r;
		fig1.t_thrds = mythreads;
		
		//Creation of threads
		for(i=0; i<mythreads; i++)
		{
			pthread_create(&number_code[i],NULL,call_readers,(void *)&fig1);
		}
		for(i=0; i<mythreads; i++)
		{
			pthread_join(number_code[i],NULL);
		}
		open_close(2); //Close base

		printf(LINE);
		printf("|Random-Read (done:%ld): %.6f sec/op; %.1f reads/sec(estimated); cost:%.3f(sec);\n"
		,fig1.c_count, (double)(timeOfread / fig1.c_count)
		,(double)(fig1.c_count / timeOfread)
		,timeOfread);
	} 
	else if (strcmp(argv[1], "readwrite") == 0) 
	{
		int r = 0;

		count = atoi(argv[2]);
		_print_header(count);
		_print_environment();
		if (argc == 5) //we change argc from 4 to 5
			r = 1;
		mythreads = atoi(argv[3]); //Take the number of threads from the input

		open_close(1); //Open base
		fig2.r_r=r;								
		printf("Enter a percentage for write:"); //Ask from the user to enter a percentage for  write			
		scanf("%d", &per);
		fig2.c_count=(long) ((count*per)/100); //calculating total amount of write operations
		fig2.t_thrds=(int) ((mythreads*per)/100);
		fig3.r_r=r;
		per2=100-per; //calculate percentage of read
		fig3.c_count=(long) ((count*per2)/100); //calculating total amount of read operations
		fig3.t_thrds=(int) ((mythreads*per2)/100);
		
		//Creation of threads
		for(i=0;i<(mythreads*per/100);i++)
		{
			pthread_create(&number_code1[i],NULL,call_writer,(void *)&fig2);
		}
		for(i=0;i<(mythreads*per2/100);i++)
		{
			pthread_create(&number_code2[i],NULL,call_readers,(void *)&fig3);
		}

		for(i=0;i<(mythreads*per/100);i++)
		{
			pthread_join(number_code1[i],NULL);
		}
	
		for(i=0;i<(mythreads*per2/100);i++)
		{
			pthread_join(number_code2[i],NULL);
		}
		open_close(2); //Close base

		printf(LINE);
		printf("|Random-Write (done:%ld): %.6f sec/op; %.1f writes/sec(estimated); cost:%.3f(sec);\n"
		,fig2.c_count, (double)(timeOfwrite / fig2.c_count)
		,(double)(fig2.c_count / timeOfwrite)
		 	,timeOfwrite);

		printf(LINE);
		printf("|Random-Read (done:%ld): %.6f sec/op; %.1f reads /sec(estimated); cost:%.3f(sec)\n",
		fig3.c_count, (double)(timeOfread / fig3.c_count),
		(double)(fig3.c_count / timeOfread),
		timeOfread);
		
	}
	else 
	{
		fprintf(stderr,"Usage: db-bench <write | read | readwrite> <count> <mythreads> <r> \n"); //Format which accept the program
		exit(1);
	}
	//free memory
	free(number_code); 
	free(number_code1);
	free(number_code2);
	return 1;
}
