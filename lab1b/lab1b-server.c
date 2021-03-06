#include <termios.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <getopt.h>
#include <sys/wait.h>
#include <poll.h>
#include <signal.h>
#include <mcrypt.h>
#include <sys/types.h>
#include <netdb.h>
#include <netinet/in.h>
#include <fcntl.h>

#define readfd 0
#define writefd 1
#define errorfd 2
#define BUFFSIZE 1024

int main(int argc, char** argv)
{
  //1. Setting up the variables needed: 
  int encryptfd = 0;
  int num_shell;
  int key_fd;
  int port_flag = 0;
  int port_number;
  int port_arg;
  int h;
  int status;
  int socketfd,clientfd;
  struct sockaddr_in serv_addr,client_addr;
  int clientlen;
  int ID;
  int shell_terminal[2];
  int terminal_shell[2];
  char* IV;
  char* key;
  int key_length;
  MCRYPT a,b; //a=encryption , b=decryption.
  char buff2[1024];


  static struct option long_options[] = {
    {"port",1,0,'p'},
    {"encrypt",1,0,'e'},
    {0,0,0,0}
  };

//2.Parsing of the arguments:                                                                         
  while((h = getopt_long(argc, argv, "", long_options, NULL))!=-1)
   {
     switch(h)
       {
       case 'p':
	 port_arg = atoi(optarg);
	 port_flag = 1;
	 break;
       case 'e':
	 encryptfd = 1;
	 key_fd = open(optarg,O_RDONLY);
	 if(encryptfd < 0)
	   {
	     fprintf(stderr,"\nError in opening file");
	     exit(1);
	   }
	 break;
       default:
	 fprintf(stderr,"\n Unrecognized Argument Error.");
	 fprintf(stderr,"\n Usage: ./lab1b-server --port=# --encrypt=filename...");
	 exit(1);
       }
   }

 if(port_flag == 0)
   {
     fprintf(stderr, "\nError in arguments. --port option required");
     exit(1);
   }

 //3.Commencing socket:                                                                                
 socketfd = socket(AF_INET, SOCK_STREAM, 0);
 if(socketfd < 0)
   {
     fprintf(stderr,"\nError in opening socket");
     exit(1);
   }

 bzero((char*)&serv_addr,sizeof(serv_addr));
 serv_addr.sin_family = AF_INET;
 serv_addr.sin_addr.s_addr = INADDR_ANY;
 serv_addr.sin_port = htons(port_arg);
 int k = bind(socketfd,(struct sockaddr*)&serv_addr,sizeof(serv_addr));
   if(k < 0)
   {
     fprintf(stderr,"\nError in binding");
     exit(1);
   }

 //4. Initializing encryption process:
 int key_size = 16; //Normalized
 int k_1;
 if(encryptfd==1)
   {
     a = mcrypt_module_open("twofish",NULL,"cfb",NULL);
     if(a==MCRYPT_FAILED)
       {
	 fprintf(stderr,"\nError: Could not open encryption modules");
	 exit(1);
       }
     b = mcrypt_module_open("twofish",NULL,"cfb",NULL);
     if(b==MCRYPT_FAILED)
       {
	 fprintf(stderr,"\nError: Could not open encryption modules");
	 exit(1);
       }
     
     //Getting the key :
     if(key==0)
       {
	 fprintf(stderr,"\nError in allocating key memory");
	 exit(1);
       }
    k_1 = read(encryptfd,key,key_size);
    if(k_1<0)
      {
	fprintf(stderr,"\nError in reading");
	exit(1);
      }

     IV = malloc(mcrypt_enc_get_iv_size(a));
     //Putting random data into IV:
     int i;
     for (i=0; i< mcrypt_enc_get_iv_size(a); i++) {
       IV[i]=rand();}
     
     k_1 = mcrypt_generic_init(a, key, key_size, IV);
     if(k_1 < 0){
       fprintf(stderr, "\nError in Initialization of encryption");
       exit(1);
     }

     k_1 = mcrypt_generic_init(b, key, key_size, IV);
     if(k_1 < 0){
       fprintf(stderr, "\nError in Initialization of decryption");
       exit(1);
     }
   }
     

 //5. Connecting to client:

 listen(socketfd,5);
 clientlen = sizeof(client_addr);
 clientfd = accept(socketfd,(struct sock_addr*)&client_addr, &clientlen);
 if(clientfd < 0)
   {
     fprintf(stderr,"\nError in accepting client");
     exit(1);
   }

 //6. Setting up the pipes between server and shell 
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
     close(terminal_shell[0]);
     close(shell_terminal[1]);

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

     fds[0].fd = clientfd;
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
		 num_shell = read(clientfd,buff2,BUFFSIZE);
		 if(num_shell<0)
		   {
		     fprintf(stderr,"\nError in read");
		     exit(1);
		   }
		 else if(num_shell==0)
		   {
		     kill(ID,SIGTERM);
		     exit(1);
		   }

		 //We need to now echo to stdout and then forward to shell:                                                                                                     
		 int i;
		 for(i=0;i<num_shell;i++)
		   {
		     if(encryptfd==1)
		       mdecrypt_generic(b,&buff2[i],1);
		     if(buff2[i] == 0x03)
		       {
			 int ll = kill(ID,SIGINT);
			 if(ll==-1)
			   {fprintf(stderr,"\nError in kill");
			     exit(1);}
		       }

		     /*else if(buff2[i]=='\r' || buff2[i]=='\n')
		       {
			 char mapping[2] = {'\r','\n'};
			 int k = write(writefd, mapping, 2);
			 int jj = write(terminal_shell[1],&mapping[1],1);
			 if(k<0 || jj<0){
			   fprintf(stderr,"\nError in writing to stdout");
			   exit(1);
			 }
			 continue;
		       }*/
		     else if(buff2[i]==0x04)
		       close(terminal_shell[1]);
		     else
		       {
			 
			 int kk = write(terminal_shell[1],&buff2[i],1);
			 if(kk<0)
			   {
			     fprintf(stderr,"\nError in writing");
			     exit(1);
			   }
		       }
		   }
	       }
	     //Input from the pipe:                                                                                                                                             
	     if((fds[1].revents & POLLIN))
	       {
		 num_shell = read(fds[1].fd,buff2,BUFFSIZE);
		 int i;
		 for(i=0;i<num_shell;i++)
		   {
		     if(buff2[i]=='\n')
		       {
			 char mapping[2] = {'\r','\n'};
			 if(encryptfd==1)
			   {
			     char ch1 = '\r';
			     char ch2 = '\n';
			     mcrypt_generic(a,&ch1,1);
			     mcrypt_generic(a,&ch2,1);
			     write(clientfd,&ch1,1);
			     write(clientfd,&ch2,1);
			   }
			 else
			   {
			     write(clientfd,mapping,2);
			     i++;
			   }
		       }
		     else
		       {
			 if(encryptfd==1)
			   mcrypt_generic(a,&buff2[i],1);
			 write(clientfd,&buff2[i],1);
			 
		       }
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



		   

 
