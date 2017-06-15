/*
//========================================================
//UDP FILE TRANSFER CLIENT(submit)
//========================================================
//Developed by: Kamal Panthi 
//Email: kamal210@hotmail.com
//        
//To Compile type make 
//To run the client type: "./submit hostname portnumber filename"
//======================================================== 

Acknowledgement:


http://www.csharphelp.com/archives2/archive335.html
http://www.tcnj.edu/~bush/uftp.html
http://www.linuxquestions.org/questions/programming-9/best-way-to-transfer-files-in-linux-thru-c-programming-657577/
http://www.dreamincode.net/forums/showtopic27741.htm
http://www.allegro.cc/forums/thread/580811/1

Stevens, W.R.: Unix Network Programming, The Sockets Networking API, Vol. 1, 3rd Ed., Prentice Hall, 2004.
Internetworking with TCP/IP Vol. 3: Client-Server Programming and Application, Linux/POSIX Socket Version (Comer, D.E., Stevens, D.), 2000. 


//======================================================== 

*/

//==================Header Declarations===================
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <string.h>
#include <unistd.h>
#include <stdbool.h>
#include <time.h>
#include <fcntl.h>
#include <sys/stat.h>
//===================#Defines=============================
#define NOWORK 0 
#define REQUEST 1
#define DATA 2
#define ACK 3
#define ERR 4
#define COMPLETE 5 


//================Global Variable Declaration=============
int PORT; //integer variable for portno
char FILENAME[512];// character array to store FILENAME

bool firstrequest=true,firstdatasent=false,filetransfercomplete=false;
int totalpacketssend=0;


//=======================Error display Function=============
void err(char *msg)
{
	perror(msg);
	exit(0);
}



//======================MAIN==============================
int main(int argc, char *argv[])
{
   	fd_set fd_readfd,fd_testfd;//file descripter set for select
   	int fd_socket, length, n,r,len,c,s,lenm;// integer variables
   	struct sockaddr_in ser_sock, cli_sock;//sockaddress for server and client
   	struct hostent *hp;//structure used for the entry in the host database
   	char buffer[512], recvbuffer[512], message[51200], file_read_bfr[100000];//string buffer   
	int fd;
	int flags;
    	mode_t mode;
	int size=0,file_open;
	double time;	
	struct timeval timerout, begin,end;
	timerout.tv_sec = 120;//initializaztion of timer
	timerout.tv_usec = 500000;
	int samepacketsendercounter, marker=0,no_of_fullpackets,length_of_lastpackets,datablockno=0;
	unsigned int lenpack;
	unsigned short int blockno=0,opcodebyte=0,datapacketnumber,errno=0;
   	//========checking for valid run parameters======
   	if (argc < 4) 
	{
		if (argc < 3) 
		{
			if(argc<2)
			{
				printf("Usage: ./submit hostname portnumber filename\n");
				fprintf(stderr, "Provide Host,port and FILENAME\n");
      				exit(0);
			}
			else 
			{
				printf("Usage: ./submit hostname portnumber filename\n");
      				fprintf(stderr, "Provide Port and FILENAME\n");
      				exit(0);
   			}
		}
		else 
		{
			printf("Usage: ./submit hostname portnumber filename\n");
      			fprintf(stderr, "Provide FILENAME\n");
      			exit(0);
   		}

	}

	if (argc > 4)printf("The arguments after FILENAME: (%s) are ignored",argv[3]);


	//========creating socket for communication======
   	fd_socket= socket(AF_INET, SOCK_DGRAM, 0);
   	if (fd_socket < 0) 
		err("socket");

   	ser_sock.sin_family = AF_INET;
   	hp = gethostbyname(argv[1]);
   	if (hp==0) 
		err("Unknown host");

   	bcopy((char *)hp->h_addr,(char *)&ser_sock.sin_addr,hp->h_length);
   	PORT =atoi(argv[2]);
   	ser_sock.sin_port = htons(PORT);
   	length=sizeof(struct sockaddr_in);	

	bzero(FILENAME,512);
   	strcpy(FILENAME,argv[3]);//FILENAME from run arguments
   	
   	
	flags=O_RDONLY; // read only
        
	//================Check whether file exists or not======================
	fd=open(FILENAME, flags, mode);
	if(fd<0)//if error
	{
		printf("File with name: %s does not exist.\n",FILENAME);
		exit(0);
	}
	close(fd);


   	//fill the select macros
	FD_ZERO(&fd_readfd);
	FD_SET(fd_socket,&fd_readfd);
	FD_SET(0,&fd_readfd);
	//=================Head Display For client==================
	printf("\n ========================================");
   	printf("\n          UDP FILE TRANSFER CLIENT");	
	printf("\n ========================================");
	printf("\n Developed by: Kamal Panthi ");
	printf("\n Student ID: c0346747");
	printf("\n Email: kamal.panth@lut.fi");
	printf("\n This program can transfer any type of file and any size of file.");
	printf("\n ========================================\n");
	
 	bzero(message,51200);
	memset(buffer,0,512); 	
	opcodebyte=htons(REQUEST);//setting the upcode for file transfer request	
	//Packaging the request buffer
	memcpy(buffer,&opcodebyte,2);
	memcpy(&buffer[2],FILENAME,strlen(FILENAME)+1);
	lenpack=2+strlen(FILENAME);

	//=============Connect()=====================================
	//if (connect(fd_socket, (struct sockaddr *)&ser_sock, sizeof(struct sockaddr)) < 0) 
		//err("Connect");

	// sending Transfer 1FILENAME name to server
        n=sendto(fd_socket,buffer,lenpack,0,(struct sockaddr *)&ser_sock,length);
        if (n < 0) 
		err("Sendto");
	
	else
	{
		while(1)//while loop
		{	
			//set the select command
			fd_testfd=fd_readfd;
			s=select(FD_SETSIZE,&fd_testfd,0,0,&timerout);	///
			if(s<0)
			{
				perror("error in select\n");
				exit(1);
			}
			else if(s==0)
			{
				if(firstrequest==true)
				{
					printf("The Server didnot respond to your file upload request in this 2 minutes!!\n");
					printf("Please try again later after some time!!\n");
					exit(0);
				}
				else
				{					
					

					//check if samepacketsendcounter exceeds 10
					//if(exceeds 10) then give the message for abnormal transfer  
					if(samepacketsendercounter>=10)
					{
						printf("The packet number: %d was sent for 10 times or more",datablockno); 
						printf("still no successful cooperation with server achieved.\n");
						printf("So, The client is being terminated abnornally!!\n");
						printf("The transfer statistics will be displayed only on successfull transfer.\n");
						exit(0);
						
					}
					else
					{
						//send the same packet
						n=sendto(fd_socket,buffer,lenpack,0,(struct sockaddr *)&ser_sock,length);
						if (n < 0) 
							err("Sendto");

						samepacketsendercounter++;//increase same packet sender counter
						totalpacketssend++;//increases total packet send counter
						//resetting the timer
						timerout.tv_sec = 10;
						timerout.tv_usec = 500000;
					}
					
				}	
			}
			else
			{
				if(FD_ISSET(fd_socket,&fd_testfd))//check if input came from socket
				{
					bzero(recvbuffer,512); 
					//receive buffer from socket
		    			n = recvfrom(fd_socket,recvbuffer,sizeof(recvbuffer),0,(struct sockaddr *)&cli_sock, &length);
					recvbuffer[n]=0;
					if (n < 0) 
						err("recvfrom");

					//print the buffer				
			       		//printf("%s\n",recvbuffer);


					//Reading the Opcode
					memcpy(&opcodebyte,recvbuffer,2);
					opcodebyte=ntohs(opcodebyte);
					//&recvbuffer[4]
					if(opcodebyte==ACK)
					{
						if(firstrequest==true)
							firstrequest=false;
						if(firstdatasent==false)
						{
							
							//=============Start the timer to calculate the transfer speed============
							gettimeofday(&begin,NULL);
							//open the  file
							//file_open=open(FILENAME, flags, mode);
							//bzero(file_read_bfr,sizeof(file_read_bfr));
							//read the buff 
							//read(file_open,file_read_bfr,100000);
							//close(file_open);
							//size=strlen(file_read_bfr);

							struct stat stbuf;
							stat(FILENAME, &stbuf);
							size = stbuf.st_size;

							printf("\n\tFile Size: %d bytes\n",size);
							no_of_fullpackets=size/500;//total no of full packets
							length_of_lastpackets=size%500;//the size of last paclet
							
							firstdatasent=true;
							file_open=open(FILENAME, flags, mode);
						}
						
					
						//get the packet number from ACK message
						memcpy(&blockno,&recvbuffer[2],2);//acquired the acknowledged block number
						blockno=ntohs(blockno);

						
						//if(the received ack pack number==datapacketnumber) then send new packet
						if(blockno==datablockno)
						{
							if(samepacketsendercounter>0)
							{
								sprintf(message,"%sThe Packet No: %d was sent %d times.\n",message,datablockno,samepacketsendercounter+1);
							}
							if(filetransfercomplete==true)//if all packets are send successfully
							{
								//send the transfer complete message to server
								memset(buffer,0,512); 	
								opcodebyte=htons(COMPLETE);//setting the upcode for file transfer completed	
								lenpack=2;
								//Packaging the COMPLETE buffer
								memcpy(buffer,&opcodebyte,2);
								n=sendto(fd_socket,buffer,lenpack,0,(struct sockaddr *)&ser_sock,length);
								if (n < 0) 
									err("Sendto");

								//display the statistics
								//==========Mark end time for file transfer
								gettimeofday(&end,NULL);

								printf("The file transfer completed successfully.\n");
								printf("=======================================================\n\n");
								printf("%s\n",message);
								//display total retransmission
								printf("Total number of packets in file = %d\n",datablockno);
								printf("Total packets send = %d\n",totalpacketssend);
								printf("Total Retransmission = %d\n",totalpacketssend-datablockno);
								printf("Total packet loss percentage = %lf(o/o)\n\n",(((double)totalpacketssend-(double)datablockno)*100)/(double)datablockno);
							m	//print total file transfer time and transfer rate per packet and per mb
								//========Calculate the total file transfer time(in secs)==================== 
								time = end.tv_sec - begin.tv_sec + ( end.tv_usec - begin.tv_usec ) / 1.e6f;
								
								//=======Printing the transfer speed=========================================
								printf("%d bytes sent in %lf secs   (%lf Kb/sec) \n",size, time,(double)size/(time*1024));
								printf("=======================================================\n");
								exit(0);

							}

							datablockno++;
							datapacketnumber=htons(datablockno);
							//send new packet
							char only_message[500];
							memset(only_message,0,500);
						
							if(datablockno<=no_of_fullpackets)
							{
								//memcpy(only_message,&file_read_bfr[marker],500);
								read(file_open,only_message,500);
								///marker+=500;
								opcodebyte=htons(DATA);

								//Making the DATA Header
								memset(buffer,0,512);
								memcpy(buffer,&opcodebyte,2);
								memcpy(&buffer[2],&datapacketnumber,2);							
								memcpy(&buffer[4],&only_message,500);	
								lenpack=2+2+500;
								//Sending The Data with DATA header and packetnumber
								n=sendto(fd_socket,buffer,lenpack,0,(struct sockaddr *)&ser_sock,length);
								if (n < 0) 
									err("Sendto");
								if(length_of_lastpackets==0 && datablockno==no_of_fullpackets)
								{
									filetransfercomplete=true;
									close(file_open);
								}
							}
							else
							{
								//memcpy(only_message,&file_read_bfr[marker],length_of_lastpackets);
								read(file_open,only_message,length_of_lastpackets);
								marker+=length_of_lastpackets;
								opcodebyte=htons(DATA);
								//Making the DATA Header
								memset(buffer,0,512);
								memcpy(buffer,&opcodebyte,2);
								memcpy(&buffer[2],&datapacketnumber,2);							
								memcpy(&buffer[4],&only_message,length_of_lastpackets);	
								lenpack=2+2+length_of_lastpackets;
								//Sending The Data with DATA header and packetnumber
								n=sendto(fd_socket,buffer,lenpack,0,(struct sockaddr *)&ser_sock,length);
								if (n < 0) 
									err("Sendto");
								filetransfercomplete=true;
								close(file_open);
							}
							samepacketsendercounter=0;//resetting the same packet sender counter
							totalpacketssend++;//increases total packet send counter
							//resetting the timer
							timerout.tv_sec = 10;
							timerout.tv_usec = 500000;
						
						}
						else
						{
							//check if samepacketsendcounter exceeds 10
							//if(exceeds 10) then give the message for abnormal transfer  
							if(samepacketsendercounter>=10)
							{
								printf("The packet number: %d was sent for 10 times or more",datablockno); 
								printf("still no successful cooperation with server achieved.\n");
								printf("So, The client is being terminated abnornally!!\n");
								printf("The transfer statistics will be displayed only on successfull transfer.\n");
								exit(0);
							}
							else
							{
								//send the same packet
								n=sendto(fd_socket,buffer,lenpack,0,(struct sockaddr *)&ser_sock,length);
								if (n < 0) 
									err("Sendto");

								samepacketsendercounter++;//increase same packet sender counter
								totalpacketssend++;//increases total packet send counter
								//resetting the timer
								timerout.tv_sec = 10;
								timerout.tv_usec = 500000;
								
							}
							
						}
					}
					else if(opcodebyte==ERR)
					{
						//get the error number from ERR message
						memcpy(&errno,&recvbuffer[2],2);//acquired the acknowledged block number
						errno=ntohs(errno);
						if(errno==1)//currently server is busy with another client
						{
							printf("Currently Server is busy with another client.\n");
							printf("The client is terminating. Please try again later.\n");
							exit(0);
						}
						else if(errno==2)//file with the same name already exist..change filename and transfer again
						{
							printf("The file with the same name already exists with server.\n");
							printf("The client is terminating. Please try again later with different filename.\n");
							exit(0);
						}
						else
						{
							printf("Some unrecognized error was received.\n");
							printf("The client is terminating. Please try again later.\n");
							exit(0);
						}
						
					}
					else
					{
						//ignore
					}
					
					
					
				}
			}
			

		}//end of while  
	}
}















 
