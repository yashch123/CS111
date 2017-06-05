/* NAME: Yash Choudhary
   EMAIL: yashc@g.ucla.edu
   UID: 704630134
*/
#include <time.h>
#include <pthread.h>
#include <stdlib.h>
#include <getopt.h>
#include <sched.h>
#include <string.h>
#include <stdio.h>


long long num_iterations = 1;
int sync_f = 0;
int sync_opt = 0;
int opt_yield = 0;
int s_lock = 0;
pthread_mutex_t mutex;

void add(long long *pointer, long long value) {
  long long sum = *pointer + value;
  if (opt_yield)
    sched_yield();
  *pointer = sum;
}

void add_spin(long long *pointer, long long value)
 {
  while(__sync_lock_test_and_set(&s_lock, 1)==1);

  //CRS
  long long sum = *pointer + value;
  if(opt_yield)
    sched_yield();
  *pointer = sum;
  //CRS

  __sync_lock_release(&s_lock);
 }

void add_mutex(long long *pointer, long long value) {
  pthread_mutex_lock(&mutex);

 
  long long sum = *pointer + value;
  if(opt_yield)
    sched_yield();
  *pointer = sum;
   
  pthread_mutex_unlock(&mutex);
}

void add_cands(long long *pointer, long long value) {
  long long sum;
  long long p;
  do{
    p = *pointer;
    sum = p + value;
    if(opt_yield)
      sched_yield();
  }while(__sync_val_compare_and_swap(pointer, p, sum) !=p);
 


}

void* func(void* ctr)
{
  long long * counter = (long long*) ctr;
  long k;
  char* f;
  long long one = 1;
  if(!sync_f){
    for(k = 0; k < num_iterations; k++)
      add(counter, one);
    for(k = 0; k < num_iterations; k++)
      add(counter, -one);
    
  }else
    {
      switch(sync_opt)
	{
	case 'm': 
	  for(k=0;k<num_iterations;k++)
	    add_mutex(counter,one);
	  for(k = 0; k < num_iterations; k++)
	    add_mutex(counter, -one);
	  break;
        case 's':
	  for(k=0;k<num_iterations;k++)
	    add_spin(counter,one);
	  for(k = 0; k < num_iterations; k++)
            add_spin(counter, -one);
	  break;
	case 'c':
	  for(k=0;k<num_iterations;k++)
	    add_cands(counter,one);
	  for(k = 0; k < num_iterations; k++)
            add_cands(counter, -one);
	  break;
	}
    }
 


  return NULL;


}

int main(int argc, char** argv)
{
  int h;
  long num_threads = 1;
  long long counter = 0;
  int ret;
  struct timespec tstart,tstop;
 

  //1. Parsing the arguments 
  static struct option long_opts[] =
    {
      {"threads",required_argument,NULL,'t'},
      {"iterations",required_argument,NULL,'i'},
      {"yield",no_argument,NULL,'y'},
      {"sync",required_argument,NULL,'s'},
      {0,0,0,0}
    };

  //similar to my lab0:                                                                     
  while((h = getopt_long(argc, argv, "", long_opts, NULL))!=-1)
    {
      switch(h)
        {
        case 't':
          num_threads = atoi(optarg);
          break;
	case 'i':
	  num_iterations = atoi(optarg);
	  break;
	case 'y':
	  opt_yield = 1;
	  break;
	case 's':
	  if(strlen(optarg)!=1)
	    fprintf(stderr,"\n Usage: ./lab2_add [--threads=#] [--iterations=#] [--yield] [--sync=]");
	  else
	    {
	      if(optarg[0] == 'm' || optarg[0] == 's' || optarg[0] == 'c')
		{
		sync_opt = optarg[0];
		sync_f = 1;
		}
	    }
	  break;
        default:
          fprintf(stderr,"\n Unrecognized Argument Error.");
          fprintf(stderr,"\n Usage: ./lab2_add [--threads=#] [--iterations=#] [--yield] [--sync=]");
          exit(1);
        }
    }

  //2. Generating threads:
  pthread_t *thr_id = malloc(num_threads*sizeof(pthread_t));

  //3. Now we can start the clock to measure run time :
  ret = clock_gettime(CLOCK_REALTIME, &tstart);
  if(ret==-1)
    {
      fprintf(stderr,"\nError in starting clock");
      exit(1);
    }

  //4. Creating and joining the threads to perform operation:
  int i;
  for(i = 0; i < num_threads; i++)
    {
      ret = pthread_create(&thr_id[i], NULL, func, (void*)&counter);
      if(ret!=0)
	{ 
	  perror("Error creating thread"); 
	  exit(1); 
	} 
    }
  
  int j;
  for (j = 0; j < num_threads; j++)
    {
      pthread_join(thr_id[j], NULL);
    }

  //5. Stopping the clock to get stoptime:
  ret = clock_gettime(CLOCK_REALTIME, &tstop);
  if(ret==-1)
     {
	fprintf(stderr,"\nError in stopping clock");
	exit(1);
     } 

  //6. Values for the CSV file:
  long start_sec = tstart.tv_sec;
  long start_nsec = tstart.tv_nsec;
  long end_sec = tstop.tv_sec;
  long end_nsec = tstop.tv_nsec;
  long run_time = (end_sec - start_sec)*1000000000 + (end_nsec - start_nsec);
  long num_opts = 2*num_threads*num_iterations;
  long avg_time = run_time/num_opts;
  
  //7. Printing the required message:
  
  char* tag; 
  tag = "add-none";
  if(opt_yield)
    {
      if(!sync_f)
	{
	  tag = "add-yield-none";
	}
      else
	{
	  switch(sync_opt)
	    {
	    case 'm': tag = "add-yield-m"; break;
	    case 's': tag = "add-yield-s";break;
	    case 'c': tag = "add-yield-c";break;
	    }
	}
    }
  else
    {
      if(sync_f)
	{
	  switch(sync_opt)
            {
            case 'm': tag = "add-m"; break;
            case 's': tag = "add-s";break;
            case 'c': tag = "add-c";break;
            }
	}
    }

  printf("%s,%lld,%lld,%lld,%lld,%lld,%lld\n", tag, num_threads, num_iterations, num_opts, run_time, avg_time, counter);
  return 0;
}
