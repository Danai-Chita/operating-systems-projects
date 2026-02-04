#include <string.h>
#include "../engine/db.h"
#include "../engine/variant.h"
#include "bench.h"

#define DATAS ("testdb")

DB* db; //global db 

//function which open and closes db according with flag
void open_close(int flag)
{
	if(flag==1)
	{
		db = db_open(DATAS);
	}
	else if(flag==2)
	{
		db_close(db);
	}
}
//Time setting in case of read 
void time_cost_readers(double cost) 
{
	pthread_mutex_lock(&read_c);
	timeOfread+= cost;  //timeOfread = timeOfread + cost
	pthread_mutex_unlock(&read_c);
}

//Time setting in case of write
void time_cost_writer(double cost)	
{
	pthread_mutex_lock(&write_c);
	timeOfwrite+= cost; //timeOfwrite = timeOfwrite + cost
	pthread_mutex_unlock(&write_c);
}

void _write_test(long int count, int r,int thrds) // We put an extra definition for threads
{
	int i;
	double cost;
	int secondcount; // We use this variable for sharing threads the functions.
	long long start,end;
	Variant sk, sv;

	char key[KSIZE + 1];
	char val[VSIZE + 1];
	char sbuf[1024];

	memset(key, 0, KSIZE + 1);
	memset(val, 0, VSIZE + 1);
	memset(sbuf, 0, 1024);
	

	start = get_ustime_sec();
	secondcount=count/thrds; // sharing of threads the PUT(db_add)
	for (i = 0; i < secondcount; i++) //here we change count and it replaced by secondcount
	{ 
		if (r)
			_random_key(key, KSIZE);
		else
			snprintf(key, KSIZE, "key-%d", i);
		fprintf(stderr, "%d adding %s\n", i, key);
		snprintf(val, VSIZE, "val-%d", i);

		sk.length = KSIZE;
		sk.mem = key;
		sv.length = VSIZE;
		sv.mem = val;

		db_add(db, &sk, &sv);
		if ((i % 10000) == 0) {
			fprintf(stderr,"random write finished %d ops%30s\r", 
					i, 
					"");

			fflush(stderr);
		}
	}

	end = get_ustime_sec();
	cost = end -start;
	time_cost_writer(cost); //call of function
}


void _read_test(long int count, int r,int thrds)
{
	int i;
	int ret;
	int found = 0;
	double cost;
	int secondcount; // We use this variable for sharing threads the functions.
	long long start,end;
	Variant sk;
	Variant sv;
	char key[KSIZE + 1];
	start = get_ustime_sec();
	secondcount=count/thrds; // sharing of threads the PUT(db_get)
	for (i = 0; i < secondcount; i++) //here we change count and it replaced by secondcount
	{ 
		memset(key, 0, KSIZE + 1);

		/* if you want to test random write, use the following */
		if (r)
			_random_key(key, KSIZE);
		else
			snprintf(key, KSIZE, "key-%d", i);
		fprintf(stderr, "%d searching %s\n", i, key);
		sk.length = KSIZE;
		sk.mem = key;
		ret = db_get(db, &sk, &sv);
		if (ret) {
			//db_free_data(sv.mem);
			found++;
		} else {
			INFO("not found key#%s", 
					sk.mem);
    	}

		if ((i % 10000) == 0) {
			fprintf(stderr,"random read finished %d ops%30s\r", 
					i, 
					"");

			fflush(stderr);
		}
	}
	end = get_ustime_sec();
	cost = end - start;
	time_cost_readers(cost); //call of function
}
