#include<getopt.h>

//IDEAS: Have a shell script that replaces comma(',') with space maybe(' ')

void read_file(int fd)
{
  int offset;
  char* data;
  data = (char*)malloc(sizeof(char));
  pread(fd,
 

}



int main(int argc, char** argv)
{
  //Getting the csv file:
  if(argc !=2)
    {
      fprintf(stderr,"\nError: Wrong number of arguments");
      exit(1);
    }
  int fd;
  fd = open(argv[1],O_CREAT | O_RDONLY, 0666);
  if(fd<0){
    fprintf(stderr,"\nError: Unable to open file");
    exit(1);}
  
  //Reading file here:
  read_file(fd);

}
  
