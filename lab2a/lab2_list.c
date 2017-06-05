#include "SortedList.h"
#include <stdlib.h>
#include <stdio.h>
#include <getopt.h>
#include <sched.h>
#include <time.h>
#include <pthread.h>
#include <string.h>
#include <signal.h>
#include <errno.h>


pthread_mutex_t mutex;
int p_len;
SortedList_t* p;
SortedListElement_t* elements;
int length;
int slock = 0;
int num_threads = 1;
int num_iters = 1;
struct timespec tstart, tstop;
char* y_opts;
char* s_opts;
char h;
int yieldopt_size;
int sync_f = 0;
int ret;
int opt_yield = 0;


void insert_s(SortedList_t* p, SortedListElement_t* element)
{
  while(__sync_lock_test_and_set(&slock, 1));
  SortedList_insert(p, element);
  __sync_lock_release(&slock);
}

int length_s(SortedList_t* p)
{
  while(__sync_lock_test_and_set(&slock, 1));
  int l = SortedList_length(p);
  __sync_lock_release(&slock);
  return l;
}

SortedListElement_t* lookup_s(SortedList_t* p, const char* k)
{
  while(__sync_lock_test_and_set(&slock, 1));
  
  SortedListElement_t * gg = SortedList_lookup(p, k);
  __sync_lock_release(&slock);
  return gg;
}

int delete_s(SortedListElement_t* b)
{
  while(__sync_lock_test_and_set(&slock, 1));
  int rett = SortedList_delete(b);
  __sync_lock_release(&slock);
  return rett;
}


void insert_m(SortedList_t* p, SortedListElement_t* element)
{
  pthread_mutex_lock(&mutex);
  SortedList_insert(p,element);
  pthread_mutex_unlock(&mutex);
}

int length_m(SortedList_t* p)
{
  pthread_mutex_lock(&mutex);
  int mm = SortedList_length(p);
  pthread_mutex_unlock(&mutex);
  return mm;
}

SortedListElement_t* lookup_m(SortedList_t* p, const char* k)
{
  pthread_mutex_lock(&mutex);
  SortedListElement_t * kk = SortedList_lookup(p, k);
  pthread_mutex_unlock(&mutex);
  return kk;
}

int delete_m(SortedListElement_t* b)
{
  pthread_mutex_lock(&mutex);
  int rett = SortedList_delete(b);
  pthread_mutex_unlock(&mutex);
  return rett;
}

void* func(void* thread_id)
{

  int fd = *((int*)thread_id);
  int i;
  if(sync_f)
    {
      if(s_opts[0] =='m')
	{

	  for(i=fd;i<p_len;i+=num_threads)
	  {

	    insert_m(p,&(elements[i]));
	  }
	  //Length
	  length_m(p);
	  int k;
	  for(k=fd;k<p_len;k+=num_threads) //Lookup
	    {
	      SortedListElement_t* a = lookup_m(p,elements[k].key);                                                                    
	      if(a==NULL)
		{
		  fprintf(stderr,"\nFailed to look up element. List corrupted");
		  exit(2);
		}
	      int rett = delete_m(a);
	      if(rett){
		fprintf(stderr,"\nFailed to delete element. List corrupted");
		exit(2);
	      }
	    }
	}
      else if(*s_opts=='s')
	{
	  for(i=fd;i<p_len;i+=num_threads)
          {
            insert_s(p,&elements[i]);
          }
          //Length                                                                                                                                                                                               
          length_s(p);
          int k;
          for(k=fd;k<p_len;k+=num_threads) //Lookup                                                                                                                                                              
            {
              SortedListElement_t* a = lookup_s(p,elements[k].key);
              if(a==NULL)
		{
                  fprintf(stderr,"\nFailed to look up element. List corrupted");
		  exit(2);
                }
              int rett = delete_s(a);
	      if(rett){
		fprintf(stderr,"\nFailed to delete element. List corrupted");
		exit(2);
	      }
            }
        }
    }	      
      else //No locks/sync
	{
	  //	  fprintf(stderr,"INSERT\n");
	  for(i=fd;i<p_len;i+=num_threads)
          {
            SortedList_insert(p,&elements[i]);
          }
          //Length                                                                                                                                                                                              
	  // fprintf(stderr,"INSERT2\n");                                                                                                                                                                                                        
          SortedList_length(p);
          int k;
          for(k=fd;k<p_len;k+=num_threads) //Lookup                                                                                                                                                   
                                                                                                                                                                                                                 
            {
              SortedListElement_t* a = SortedList_lookup(p,elements[k].key);
              if(a==NULL)
                {
                  fprintf(stderr,"\nFailed to look up element. List corrupted");
		  exit(2);
                }
              int rett = SortedList_delete(a);
              if(rett){
                fprintf(stderr,"\nFailed to delete element. List corrupted");
		exit(2);
	      }
            }
	}
  int l = SortedList_length(p);      return;
}

void signal_handle(int signum){
  if(signum == SIGSEGV)
    {
      fprintf(stderr, "\nError: Segmentation Fault detected! list may be corrupted\n");
      exit(2);
    }
}

char* random_key(size_t length) {
  char * temp;
  char* dest = malloc(sizeof(char)*(length + 1));
  temp = dest;
  char ch[] = "0123456789"
    "abcdefghijklmnopqrstuvwxyz"
    "ABCDEFGHIJKLMNOPQRSTUVWXYZ";

  while (length-- > 0) {
    size_t index = rand() % 62;//(double) rand() / RAND_MAX * (sizeof ch - 1);
    *dest = ch[index];
    dest++;
  }
  *dest = '\0';
  return temp;
}


int main(int argc, char** argv)
{

  signal(SIGSEGV,signal_handle);
  srand(time(NULL));

  static struct option long_opts[] =
    {
      {"threads",required_argument,NULL,'t'},
      {"iterations",required_argument,NULL,'i'},
      {"yield",required_argument,NULL,'y'},
      {"sync",required_argument,NULL,'s'},
      {0,0,0,0}
    };

  s_opts = NULL;
  y_opts = NULL;
  while((h = getopt_long(argc, argv, "", long_opts, NULL))!=-1)
    {
      switch(h)
        {
        case 't':
          num_threads = atoi(optarg);
          break;
        case 'i':
          num_iters = atoi(optarg);
          break;
        case 'y':
          opt_yield = 0;
	  yieldopt_size = strlen(optarg);
	  if(yieldopt_size<1 || yieldopt_size>3)
	    {
	      fprintf(stderr,"\n Unrecognized Argument Error.");
	      fprintf(stderr,"\n Usage: ./lab2_list [--threads=#] [--iterations=#] [--yield=[idl]] [--sync=]");
	      exit(1);
	    }
	  int i;char k;
	    for(i = 0; i < yieldopt_size; i++)
	      {
		k = optarg[i];
		switch(k)
		  {
		  case 'i':
		    opt_yield |= INSERT_YIELD;
		    break;
		  case 'd':
		    opt_yield |= DELETE_YIELD;
		    break;
		  case 'l':
		    opt_yield |= LOOKUP_YIELD;
		    break;
		  default:
		    fprintf(stderr, "usage: lab2_list [--threads=#] [--iterations=#] [--yield=[idl]] [--sync=]\n");
		    exit(1);
		  }
	      }
	    //y_opts = optarg;
	    /*y_opts = (char*) malloc(sizeof(char)*yieldopt_size);
	    strcpy(y_opts, optarg);*/
          break;
        case 's':
          if(strlen(optarg)!=1){
            fprintf(stderr,"\n Usage: ./lab2_add [--threads=#] [--iterations=#] [--yield] [--sync=]");
	    exit(1);
	  }
          else
            {
              if(optarg[0] == 'm' || optarg[0] == 's')
                {
		  s_opts = (char*) malloc(sizeof(char));
		  strcpy(s_opts,optarg);
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
  /*
    if(y_opts=="di")
    y_opts = "id";
  if(y_opts=="li")
    y_opts = "il";
  if(y_opts=="dl")
    y_opts = "ld";
  if(y_opts=="ild" || y_opts=="dil" || y_opts=="dli" || y_opts=="lid" || *y_opts=="ldi")
    y_opts = "idl";*/
  //y_opts = NULL;
  //if(!(opt_yield & (INSERT_YIELD | DELETE_YIELD | LOOKUP_YIELD)))
  if(s_opts == NULL)
    s_opts = "none";
  if(y_opts == NULL)
    y_opts = "none";
  
  if(opt_yield)
    y_opts = (char*) malloc(sizeof(char)*yieldopt_size);
  if (opt_yield & INSERT_YIELD) 
    y_opts = strncat(y_opts, "i", 1);
  if (opt_yield & DELETE_YIELD) 
    y_opts = strncat(y_opts, "d", 1);
  if (opt_yield & LOOKUP_YIELD)
    y_opts = strncat(y_opts, "l", 1);
  
  
  //Setting up the list:
  p = (SortedList_t*) malloc(sizeof(SortedList_t));
  p_len = num_threads * num_iters;
  elements = malloc(p_len*sizeof(SortedListElement_t));
  p->next = NULL;
  p->prev = NULL;
  p->key = NULL;

  //Generating radnom keys:
  int k;
  for(k = 0; k < p_len; k++)
    {
      elements[k].next = NULL;
      elements[k].prev = NULL;
      //char* temp = (char*) malloc(sizeof(char)*16);
      elements[k].key = random_key(8);
      
    }
  //printf("%s\n", elements[0].key);
  //Initializing the threads:
  pthread_t *threads = malloc(num_threads * sizeof(pthread_t));
  int* thread_id = malloc(sizeof(int) * num_threads);
  
  //Starting the clock:
  ret = clock_gettime(CLOCK_REALTIME, &tstart);
  if(ret==-1)
    {
      fprintf(stderr,"\nError in starting clock");
      exit(1);
    }

  // Creating and joining the threads to perform operation:                                                                                                                                                    
  int i;
  for(i = 0; i < num_threads; i++)
    {
      thread_id[i] = i;
      ret = pthread_create(&threads[i], NULL, func, (void*) (thread_id + i));
      if(ret!=0)
        {
          perror("Error creating thread");
          exit(1);
        }
    }

  int j;
  for (j = 0; j < num_threads; j++)
    {
      pthread_join(threads[j], NULL);
    }
  //Stopping the clock:
  ret = clock_gettime(CLOCK_REALTIME, &tstop);
  if(ret==-1)
    {
      fprintf(stderr,"\nError in starting clock");
      exit(1);
    }
  /*
  SortedListElement_t* temp = p;
  while (temp != NULL)
    {
      fprintf(stderr, "%s\n", temp->key);
      temp = temp->next;
    }
  */

  //Checking lenght of list:
  if(SortedList_length(p)!=0)
    {
      fprintf(stderr,"\nError in list: Final length is not zero! Corrupted!");
      exit(1);
    }
  //Data for message:
  long start_sec = tstart.tv_sec;
  long start_nsec = tstart.tv_nsec;
  long end_sec = tstop.tv_sec;
  long end_nsec = tstop.tv_nsec;
  long run_time = (end_sec - start_sec)*1000000000 + (end_nsec - start_nsec);
  long num_opts = 3*num_threads*num_iters;
  long avg_time = run_time/num_opts;
  long one = 1;
  //7. Printing the required message:                                                                                                                                                                         

  printf("list-%s-%s,%lld,%lld,%lld,%lld,%lld,%lld\n", y_opts,s_opts, num_threads, num_iters,one, num_opts, run_time, avg_time);
  
  //Freeing the data structures:
  int m;
  /*  for(m=0;m<yieldopt_size;m++)
    {
      free(elements[i].key);
    }
  free(elements);
  free(thread_id);*/

  return 0;
}
