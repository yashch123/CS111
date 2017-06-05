#include<stdlib.h>
#include<signal.h>
#include<stdio.h>
#include<unistd.h>
#include<errno.h>
#include<fcntl.h>
#include<getopt.h>

void func(int signum)
{
  if(signum==SIGSEGV)
    {
      fprintf(stderr,"Got SIGSEGV");
      exit(4);
    }
}
int main(int argc, char **argv)
{
  int c;
  int digit_optind = 0;
  int o = 0;
  int longindex = 0;   //We need pointer as argument 
  char* in = NULL;
  char* out = NULL;
  int seg = 0;
  char* point = NULL;
  static struct option long_options[] =
     {
       {"input",optional_argument,0,'i'},
       {"output",optional_argument,0,'o'},
       {"segfault",no_argument,0,'s'},
       {"catch",no_argument,0,'c'},
       {0,0,0,0}
     };

   while((o = getopt_long(argc, argv, "i::o::sc", long_options, &longindex))!= -1)
    {
      switch(o)
	{
	case 'i':
	  in = optarg;
	  break;
        case 'o':
	  out = optarg;
	  break;
        case 's':
	  seg = 1;
	  break;
	case 'c':
	  signal(SIGSEGV,func);
	  break;
	default: 
	  printf("\n Unrecognized Argument Error.");
	  printf("\n Usage: ./lab0 --input=filename --output=filename");
	  exit(1);
	}
    }

   //Checking if input and output files were sent as arguments:
   if(in!=NULL)
     {
       int ifd = open(in, O_RDONLY);
       
       if (ifd < 0)
	 {
	   fprintf(stderr,"\nUnable to open input file" );
	   exit(2);
	 }
       else
       {
	 close(0);
	 dup(ifd);
	 close(ifd);
       }
     }

   if(out!=NULL)
     {
       int ofd = creat(out, 0666);
       
       if(ofd < 0)
	 {
	   fprintf(stderr, "Unable to create output file");
	   exit(3);
	 }
       else
        {
	 close(1);
	 dup(ofd);
	 close(ofd);
        }
     }

   if(seg==1)
     {
       char c = *point;
     }

   //What happens when input and output are NULL
   //Now read from in and write to out using buffer 

   char* buf;
   buf = (char*) malloc(sizeof(char)*4);
   ssize_t i;
   do
     {
       i = read(0,buf,1);
       write(1,buf,1);
     }while(i>0);

   if(i < 0)
     fprintf(stderr,"Error while reading file");

   
   exit(0); //Done!!
}
