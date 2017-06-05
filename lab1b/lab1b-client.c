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

int main(int argc, char** argv)
{
  int h;
  int port_arg;
  int socketfd = 0;
  int encryptfd = 0;
  struct termios initial_settings;
  struct termios temp_settings;
  struct sockaddr_in serv_addr;
  struct hostent *server;
  MCRYPT a,b; 
  

  static struct option long_opts[] =
    {
      {"port",required_argument,NULL,'p'},
      {"log",required_argument,NULL,'l'},
      {0,0,0,0}
    };

  //1.Parsing of the arguments:                                                         
  while((h = getopt_long(argc, argv, "", long_opts, NULL))!=-1)
    {
      switch(h)
        {
        case 'p':
          port_arg = atoi(optarg);
          break;
	case 'l':
	  log_arg = 1;
	  logfilefd = creat(optarg,S_IRWXU); //CHECK CHECK CHECK 
	  if(logfilefd < 0)
	    {
	      fprintf(stderr,"\nError in opening file");
	      exit(1);
	    }
	  break;
	case 'e':
	  encryptfd = 1;
	  break;
        default:
          fprintf(stderr,"\n Unrecognized Argument Error.");
          fprintf(stderr,"\n Usage: ./lab1b-client --port=filename --log=filename...");
          exit(1);
        }
    }


  //2. Initializing encryption:
  
  int key_size = 16; //Normalized                                                                                                                                                     
  int k;
  if(encryptionflag==1)
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
      k = read(encryptionfd,key,key_size);
      if(k<0)
	{
	  fprintf(stderr,"\nError in reading");
	  exit(1);
	}

      IV = malloc(mcrypt_enc_get_iv_size(a));
      //Putting random data into IV:                                                                                                                                                  

      for (i=0; i< mcrypt_enc_get_iv_size(a); i++) {
	IV[i]=rand();}

      k = mcrypt_generic_init(a, key, keysize, IV);
      if(k < 0){
	fprintf(stderr, "\nError in Initialization of encryption");
	exit(1);
      }

      k = mcrypt_generic_init(b, key, keysize, IV);
      if(k < 0){
	fprintf(stderr, "\nError in Initialization of decryption");
	exit(1);
      }
    }

  //3.Storing and changing terminal setttings                                            
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

  //3.Commencing socket:
  socketfd = socket(AF_INET, SOCK_STREAM, 0);
  if(socketfd < 0)
    {
      fprintf(stderr,"\nError in opening socket");
      exit(1);
    }

  server = gethostbyname("localhost");
  if(server == NULL)
    {
      fprint(stderr,"\nError: no such host");
      exit(1):
    }
  
  bzero((char*)&serv_addr,sizeof(serv_addr));
  serv_addr.sin_family = AF_INET;
  bcopy((char*)server->h_addr, (char*)&serv_addr.sin_addr.s_addr, server->h_length);
  serv_addr.sin_port = htons(port_arg); //To take care of little endian 
  
  //4.Connecting to the server:
  
  if(connect(socketfd,(struct sockaddr*)&serv_addr, sizeof(serv_addr)) < 0)
    {
      fprintf(stderr,"\nError in connecting to server");
      exit(1);
    }
  //5. Utilizing sockets for interaction:

  struct pollfd fds[2];
  int num_keyboards;
  fds[0].fd = 0; //STDIN (keyboard here)
  fds[0].events = POLLIN | POLLERR | POLLHUP;
  fds[1].fd = socketfd;
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
	      num_keyboard = read(readfd,buff2,BUFFSIZE);
	      if(num_keyboard<0)
		{
		  fprintf(stderr,"\nError in read");
		  exit(1);
		}
	      int i;
	      for(i=0;i<num_keyboard;i++)
		{
		  /*if(buff2[i] == 0x03)
		    {
		      int ll = kill(ID,SIGINT);
		      if(ll==-1)
			{fprintf(stderr,"\nError in kill");
			  exit(1);}
			  }*/
		  if(buff2[i]=='\r' || buff2[i]=='\n')
		    {
		      char mapping[2] = {'\r','\n'};
		      int k = write(writefd, mapping, 2);

		      if(encryptionfd!=-1)
			mcrypt_generic(enc,&mapping[1],1);

		      int jj = write(socketfd,&mapping[1],1);

		      if(k<0 || jj<0){
			fprintf(stderr,"\nError in writing");
			exit(1);
		      }
		      continue;
		    }
		  /*else if(buff2[i]==0x04)
		    close(terminal_shell[1]);*/
		  else
		    {
		      int j = write(writefd, &buff2[i], 1);
		      if(encryptionfd!=-1)
			mcrypt_generic(a,&buff2[i],1);
		      int kk = write(socketfd,&buff2[i],1);
		      if(j<0 || kk<0)
			{
			  fprintf(stderr,"\nError writing to stdin");
			  exit(1);
			}
		      dprintf(logfilefd,"SENT 1 bytes: %c\n",buff2[i]);
		    }
		}
	    }
	  else if((fds[1].revents & POLLIN))
	    {
	      num_socket = read(fds[1].fd,buff2,BUFFSIZE);
	      if(log_arg)
		dprintf(logfilefd,"RECEIVED %d bytes: %s\n",num_socket,buff2);

	      //DOOOOOOO THIS 
	      if(encryptionfd)
		{
		  int k = mdecrypt_generic(b,&buff2[i],1);
		  if(k<0)
		    {
		      fprintf(stderr,"\nError in mdecrypt");
		      exit(1);
		    }

		}
	    
	      //Check if LF and CR 	  
	      int i;
	      for(i=0;i<num_socket;i++)
		{
		  if(buff2[i]=='\n')
		    {
		      char mapping[2] = {'\r','\n'};
		      write(writefd,mapping,2);
		      i++;
		    }
		  else
		    write(writefd,&buff2[i],1);
		}
	    }

	  //Check for errors now:                                                               
	  
	                                                           			      	
	}
    }


  //Reset the temrinal modes here:
  reset_terminal_modes(initial_settings);
    

}
