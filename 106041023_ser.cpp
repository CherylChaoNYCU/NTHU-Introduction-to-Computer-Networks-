#include <unistd.h>
#include <cstdlib>
#include <cstring>
#include <cstdio>
#include <iostream> 
#include <string>
#include <cmath>
#include <vector>
#include <sys/types.h> 
#include <sys/socket.h> 
#include <netinet/in.h> 
#include <arpa/inet.h> 
#include <pthread.h>
#include <sstream>
#include <time.h>
using namespace std;


const int MAX_ARGS = 2;		//const used to hold max number of args passed
const int MAX_NUM = 3;		//const used to hold the max # of guesses & max people on leaderBoard
const int PORT_ARG = 1;		//const used to hold index of port
const int MAX_PENDING = 5;	//const used to hold max number of pending incoming requests
const int MAXPORT = 11899;	//const int used to hold max port #
const int MINPORT = 11800;	//const int used to hold min port #

//recording the connection info
struct arg_t
{
    int sock;
    int round_count;
};

struct  Results  //judging the guessing result
{
    int tooHigh;
	int tooLow;
	int correct;
	bool end;
};

//receive and send functions for longs
long receive_long(arg_t connInfo, bool &abort);
void send_long(long num, arg_t connect);

//sending results for server
void send_result(Results result, arg_t connect);




Results notNet(Results toConv);
Results toNet(Results toConvert);

long gen_ran()
{
	return (rand() %1000);
}


void func(void* pass)
{
	//reclaiming variables from args_pa
	arg_t *args_p;
	args_p = (arg_t*)pass;

	//setting initial variables
	srand(time(NULL));	//seeding random variable
	args_p->round_count = 0;	//setting roundCount to 0
	long roundCount = 0;	//long used to keepTrack of rounds
	long actualNums;	//long arr used to hold random #'s generated
	long numsGuess;	//long arr used to hold user's guess
	long numOn;
	bool won = false;		//bool used to test if the client has won
	Results result;		//uninitialized result
	Results *rPointer;	//result Pointer
	bool exit = false;
	bool exit_ = false; // to check if the connection ends
	//welcome message
	cerr <<  " Connected! " << endl;
	//cerr << " The user's number is: ";

	//randomly generate the number for user (the correct guessing number)
	
		actualNums = gen_ran();
		cerr << actualNums << "  ";
	
		do
		{	//receiving the guesses from client and checking them
			
				numOn = 0;
				result.tooLow = 0;
				result.tooHigh = 0;
				result.correct = 0;
				numsGuess = receive_long(*args_p, exit); //the number received from the client
				if(!exit){
					cerr << "Received Guess: " << numsGuess << endl;
					cerr << "Actual Num: "<< actualNums << endl;
					
					if(numsGuess == 1000)
					{
                       exit_ = true;
					   result.end = true;
					}
					else if(numsGuess < actualNums)
					  result.tooLow = 1;
					
					else if(numsGuess > actualNums)
					result.tooHigh = 1;

					else
						won = true;
				}
			/*	else{
					won = true;
					break;
				}*/
			
			if(won)
			{
               result.correct = 1;
			   actualNums = gen_ran();
			   won = false;
			   cerr << "New Round Started, new actual number: "<<endl;
               cerr << actualNums << "  "<<endl;

			}
			
			send_result(result, *args_p);
			  
			
		}while(!exit_);

     //cerr << "exit1"<< exit_ << "  "<<endl;
		
	if(exit_){
		cerr << endl << "User has left prematurely! " << endl;
	}

	cerr << endl << "Now awaiting a new client!" << endl;
	//closing sockets
	close(args_p->sock);
	
}

int main(int argc, char* argv[])
{	
	//checking to see if argc is correct
	if(argc != MAX_ARGS)
	{
		cerr << "Invalid number of arguments. Please input IP address for first arg, then port # for "
			<< " second one. Now exiting program.";
		exit(-1);
	}
	//assigning portNumber & checking if it is alright
	unsigned short portNum = (unsigned short)strtoul(argv[PORT_ARG], NULL, 0);
	if(portNum > MAXPORT || portNum < MINPORT)
	{
			cerr<< "The port number is out of range. Please try again with " << endl
			<<"numbers that are between 11800 & 11899. Now exiting. ";

		exit(-1);
	}
	int status;			//status int used to check TCP functions	
	int clientSock;		//socket used to hold client

	//creating socket
	int sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (sock < 0) {
		cerr << "Error with socket. Now exiting program. " << endl;
		close(sock);
		exit (-1);
	}

	

	//setting the port
	struct sockaddr_in servAddr;
	servAddr.sin_family = AF_INET; // always AF_INET
	servAddr.sin_addr.s_addr = htonl(INADDR_ANY);
	servAddr.sin_port = htons(portNum);

	//binding the sockAdress & checking if it worked
	status = bind(sock, (struct sockaddr *) &servAddr,
		sizeof(servAddr));
	if (status < 0) {
		cerr << "Error with bind. Now exiting program. " << endl;
		close(sock);
		exit (-1);
	}

	//setting the server to listen for a client
	status = listen(sock, MAX_PENDING); 
	cerr << "Now listening for a new client to connect to server!" << endl;
	if (status < 0) {
		cerr << "Error with listen. Now exiting program. " << endl;
		close(sock);
		exit (-1);
	}

	while(true){
		//creating tid to keep track of threads created
		//pthread_t tid;

		//accepting the next client & testing if there are errors
		struct sockaddr_in clientAddr;
		socklen_t addrLen = sizeof(clientAddr);
		clientSock = accept(sock,(struct sockaddr *) &clientAddr, &addrLen); //client:connect function
		if (clientSock < 0) {
			cerr << "Error with accept. Now exiting program. " << endl;
			close(clientSock);
			exit(-1);
		}
		//setting the connectionInfo in args_p, passing it into “func” for game implementation
		arg_t *args_p = new arg_t;
		args_p->sock = clientSock;
        func((void*)args_p);
		
	}
}

Results toNet(Results toConvert)
{
	toConvert.tooHigh = htonl(toConvert.tooHigh);
	toConvert.tooLow = htonl(toConvert.tooLow);
	toConvert.correct = htonl(toConvert.correct);

	return toConvert;
}

Results notNet(Results toConv)
{
	toConv.tooHigh = ntohl(toConv.tooHigh);
	toConv.tooLow = ntohl(toConv.tooLow);
	toConv.correct = ntohl(toConv.correct);

	return toConv;
}



void send_long(long num, arg_t connInfo)
{
	long temp = htonl(num);
	int bytesSent = send(connInfo.sock, (void *) &temp, sizeof(long), 0);
	if (bytesSent != sizeof(long)) {
		cerr << "Error sending long! Now exiting. ";
		close(connInfo.sock);
		exit(-1);
	}
}

long receive_long(arg_t connInfo, bool& abort)
{
	int bytesLeft = sizeof(long);
	long networkInt;	
	char *bp = (char*) &networkInt;

	while(bytesLeft > 0)
	{
		int bytesRecv = recv(connInfo.sock, (void*)bp, bytesLeft, 0);
		if(bytesRecv <= 0){
			abort = true;
			break;
		}
		else{
			bytesLeft = bytesLeft - bytesRecv;
			bp = bp + bytesRecv;
		}
	}
	if(!abort){
		networkInt = ntohl(networkInt);
		return networkInt;
	}
	else
		return 0;
}

void send_result(Results result, arg_t connInfo)
{
	result = toNet(result);
	Results* rPointer;
	rPointer = &result;
	int bytesSent = send(connInfo.sock, (void *) rPointer, sizeof(result), 0);
	if (bytesSent != sizeof(result))
	{
		cerr << "Error sending results! Now exiting program.";
		close(connInfo.sock);
		exit(-1);
	}
}

