#include <termios.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <getopt.h>
#include <sys/wait.h>
#include <poll.h>
#include <signal.h>


#define readfd 0
#define writefd 1
#define errorfd 2
#define BUFFSIZE 1024
void reset_terminal_mode(struct termios a)
{
  int m = tcsetattr(STDIN_FILENO, TCSANOW, &a);
  if (m < 0)
    {
      fprintf(stderr, "Error using tcsetattr");
      exit(1);
    }
}

//Here we are first reading 1024 bytes/chars into the buffer and then processing them character at a time. 
void duplex_io(char* buff)
{
  //int eof_detect = 1; 
  while(1)
    {
      int num = read(readfd, buff, BUFFSIZE);
      if(num < 0)
	{
	  fprintf(stderr, "\nError reading");
	  exit(1);
	}
      int i = 0;
      for(i = 0; i < num; i++)
	{
	  if(buff[i] == 0x04)
	    return;
	  else if(buff[i]=='\r' || buff[i]=='\n')
	    {
	      char mapping[2] = {'\r','\n'};
	      int k = write(writefd, mapping, 2);
	      if(k<0){
		fprintf(stderr,"\nError in writing to stdout");
		exit(1);
	      }
	      continue;
	    }
	  else
	    {
	      int j = write(writefd, &buff[i], 1);
	      if(j<0)
		{
		  fprintf(stderr,"\nError writing to stdin");
		  exit(1);
		}
	    }
	}
    }
	  


}
int
main (int argc, char** argv)
{
  char buff[1024]; 
  char buff2[1024];
  struct termios initial_settings;
  struct termios temp_settings;
  int result;
  int h;
  int shell_arg = 0;
  int shell_terminal[2]; //pipe going from shell to terminal
  int terminal_shell[2];//pipe going from terminal to shell
  int ID; //pid for the child process while forking 
  int num_shell;
  int status;

  //parsing options first:
  
  static struct option long_opts[] = 
    {
      {"shell",no_argument,NULL,'s'},
      {0,0,0,0}
    };
  
  //similar to my lab0:
  while((h = getopt_long(argc, argv, "", long_opts, NULL))!=-1)
    {
      switch(h)
	{
	case 's':
	  shell_arg = 1;
	  break;
	default:
	  fprintf(stderr,"\n Unrecognized Argument Error.");
	  fprintf(stderr,"\n Usage: ./lab1a --shell");
	  exit(1);
	}
    }

  //Storing terminal setttings:
  result = tcgetattr(STDIN_FILENO, &initial_settings);
  if (result < 0)
    {
      fprintf(stderr, "Error using tcgetattr");
      exit(1);
    }

  result = tcgetattr(STDIN_FILENO, &temp_settings);
  if(result < 0)
    {
      fprintf(stderr, "Error using tcgetattr");
      exit(1);
    }
    
  //For now don't make changes to lflag:
  temp_settings.c_iflag = ISTRIP;
  temp_settings.c_oflag = 0;
  temp_settings.c_lflag = 0;

  result = tcsetattr(STDIN_FILENO, TCSANOW, &temp_settings);
  if(result < 0)
    {
      fprintf(stderr, "Error using tcgetattr");
      exit(1);
    }

  //Checking if shell argument is available: 
  if(shell_arg==1)
    {
      if(pipe(shell_terminal) < 0)
	{
	  fprintf(stderr,"\nError in forming pipe");
	  exit(1);
	}
      if(pipe(terminal_shell) < 0)
	{
          fprintf(stderr,"\nError in forming pipe");
          exit(1);
	}

      ID = fork();
      
      if(ID < 0)
	{
	  fprintf(stderr,"\nError in fork");
	  exit(1);
	}
      else if(ID==0)
	{
	  close(shell_terminal[0]);  //close unnecessary pipe ends
	  close(terminal_shell[1]); //close unnecessary pipe ends  
	  dup2(terminal_shell[0],readfd); //becomes STDIN 
	  dup2(shell_terminal[1],writefd);//becomes STDOUT 
	  dup2(shell_terminal[1],errorfd);//becomes STDERR
	  close(terminal_shell[1]);
	  close(shell_terminal[0]);
	  
	  //Executing the process using execvp:
	  char* argv_exec[2];//arguments to exec
	  argv_exec[0] = "/bin/bash"; //first one is filename 
	  argv_exec[1] = NULL;  //Last one should be NULL

	  if(execvp(argv_exec[0], argv_exec)==-1)
	    {
	      fprintf(stderr,"\nError in exec");
	      exit(1);
	    }

	}
      else   //parent process
	{
	  close(shell_terminal[1]);//close unnecessary pipe ends 
	  close(terminal_shell[0]);//close unnecessary pipe ends 
	  

	  struct pollfd fds[2];
	  
	  fds[0].fd = 0;
	  fds[0].events = POLLIN | POLLERR | POLLHUP;
	  fds[1].fd = shell_terminal[0]; 
	  fds[1].events = POLLIN | POLLERR | POLLHUP;
	  
	  while(1)
	    {
	      int val = poll(fds,2,0);
	      if(val==-1)
		{
		  fprintf(stderr,"\nError with poll");
		  exit(1);
		}
	      else
		{
		  //Check for keyboard/terminal input:
		  if((fds[0].revents & POLLIN))
		    {
		      num_shell = read(readfd,buff2,BUFFSIZE);
		      if(num_shell<0)
			{
			  fprintf(stderr,"\nError in read");
			  exit(1);
			}
		      //We need to now echo to stdout and then forward to shell:
		      int i;
		      for(i=0;i<num_shell;i++)
			{
			  if(buff2[i] == 0x03)
			    {
			      int ll = kill(ID,SIGINT);
			      if(ll==-1)
				{fprintf(stderr,"\nError in kill");
				  exit(1);}
			    }
			  else if(buff2[i]=='\r' || buff2[i]=='\n')
			    {
			      char mapping[2] = {'\r','\n'};
			      int k = write(writefd, mapping, 2);
			      int jj = write(terminal_shell[1],&mapping[1],1);
			      if(k<0 || jj<0){
				fprintf(stderr,"\nError in writing to stdout");
				exit(1);
			      }
			      continue;
			    }
			  else if(buff2[i]==0x04)
			    close(terminal_shell[1]);
			  else
			    {
			      int j = write(writefd, &buff2[i], 1);
			      int kk = write(terminal_shell[1],&buff2[i],1);
			      if(j<0 || kk<0)
				{
				  fprintf(stderr,"\nError writing to stdin");
				  exit(1);
				}
			    }
			}
		    }
		  //Input from the pipe:
		  else if((fds[1].revents & POLLIN))
		    {
		      num_shell = read(fds[1].fd,buff2,BUFFSIZE);
		      int i;
		      for(i=0;i<num_shell;i++)
			{
			  if(buff2[i]=='\n')
			    {
			      char mapping[2] = {'\r','\n'};
			      write(writefd,mapping,2);
			      i++;
			    }
			  write(writefd,&buff2[i],1);
			}
		    }
		  
		  //Check for errors now:
		  else if((fds[1].revents & (POLLHUP | POLLERR)))
		    {
		      //SHELL HAS ALREADY EXIT!
		      int mm = waitpid(ID,&status,0);
		      if(mm==-1)
			{
			  fprintf(stderr,"\nError in waitpid");
			  exit(1);
			}
		      
		      fprintf(stderr,"\nSHELL EXIT SIGNAL=%d STATUS=%d\n",WTERMSIG(status),WEXITSTATUS(status));
		      break;
				       
				       
		      
		    }
		}
	    }
	}
    }


  //Now we need to read/write to and from the buffer:
  if(!shell_arg)
  duplex_io(buff);
  
  
  reset_terminal_mode(initial_settings);
  exit(0);
}

