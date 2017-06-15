The attached folder contains the following files
	receive/(directory)
		receive.c
		Makefile
	submit.c	
	Makefile
	readme.txt

//========================================================
// UDP FILE TRANSFER
This is an application to transfer file between two location using UDP protocol. The file to be transferred should be in the parent directory
The file after the successfull transfer will exist inside the receive directory.

The client program can track all the packet losses and records the essential data transfer statistics
It also can tell which packet was lost and how many times retransmission done during the file transfer process.

The server can track which packet was lost or delayed and keeps track of how many times the server send acknowledgement for certain packet.

This program can transfer all types of file including jpeg, pdf, wmv, wma, mp3, etc all

Also it can transfer any size of file(big enough).


//========================================================
//Developed by: Kamal Panthi 
///kamal210@hotmail.com


// Use Makefile to compile.



//To compile and run the server type: 
	To Compile:
	-->Enter the receive directory
	--> Type make to compile
	To Run:
	--> Type ./receive portnumber

	// The server should run for ever


//To Compile and run the client type: 
	To compile:
		--> be in parent directory
		--> Type make
	To Run:
		--> ./submit hostname portnumber filename"






//======================================================== 

Acknowledgement:

http://www.csharphelp.com/archives2/archive335.html
http://www.tcnj.edu/~bush/uftp.html
http://www.linuxquestions.org/questions/programming-9/best-way-to-transfer-files-in-linux-thru-c-programming-657577/
http://www.dreamincode.net/forums/showtopic27741.htm
http://www.allegro.cc/forums/thread/580811/1

Stevens, W.R.: Unix Network Programming, The Sockets Networking API, Vol. 1, 3rd Ed., Prentice Hall, 2004.
Internetworking with TCP/IP Vol. 3: Client-Server Programming and Application, Linux/POSIX Socket Version (Comer, D.E., Stevens, D.), 2000. 


 
