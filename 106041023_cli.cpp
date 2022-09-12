#include <unistd.h>
#include <cstdlib>
#include <cstring>
#include <cstdio>
#include <iostream> 
#include <string>
#include <cmath>	
#include <sys/types.h> // size_t, ssize_t
#include <sys/socket.h> // socket funcs
#include <netinet/in.h> // sockaddr_in
#include <arpa/inet.h> // htons, inet_pton
#include <unistd.h>
#include <pthread.h>
#include <vector>
using namespace std;

const int MAX_ARGS = 3; //max args
const int PORT_ARG = 2;
const int IP_ARG = 1; //port index recording the IP address
const int MAX_NUM = 1; //the number of guesses
const int MAXPORT = 11899; //max port
const int MINPORT = 11800; //min port


struct Results{  //the guessing results returned by server
	int tooHigh;
	int tooLow;
	int correct;
	bool end;
};

 //conversion function for Results
Results toNet(Results toConvert);
Results notNet(Results toConv);
 //receive function for Results
Results rec_result(int sock);

//recv & send function for long
long receive_long(int sock);
void send_long(long num, int sock);



int main(int argc, char *argv[])
{
    int status; //check TCP functions, recording whether the connections are valid
    bool win = false; //whether the user win or lose the game
    int former1 = 0;
	int former2 = 999;
    //string u_name; //for user's name
    //string victory; //hold the winning message from server
    
    long roundCount = 0; // the roundCount of the game
	long numGuess;	//used to hold the user's guesses
	int numActual; // used to hold the actual numbers sent from the server	
	bool exit_ = false;
    bool Input_valid;// whether or not the input passed in is valid (should be between 0 - 999)
	long tooHigh, tooLow, equal; //used to hold the comparisons between guesses & ac
    Results tmp;
    if(argc != MAX_ARGS)
	{
		cerr << "Invalid number of arguments. Please input IP address for first arg, then port # for "
			<< " second one. Now exiting program.";
		exit(-1);
	}

    unsigned short portNum = (unsigned short)strtoul(argv[PORT_ARG], NULL, 0); //converting port number into unsigned short
	
   if(portNum > MAXPORT|| portNum < MINPORT)
	{
		cerr << "The port number is out of range. Please try again with " << endl
			<<"numbers that are between 11800 & 11899. Now exiting. ";
		exit(-1);
	}

    unsigned long servIP;
	status = inet_pton(AF_INET, argv[IP_ARG], &servIP); //to check if the server IP is valid
	if (status <= 0) 
		exit(-1);
	struct sockaddr_in servAddr;
	servAddr.sin_family = AF_INET; // always AF_INET
	servAddr.sin_addr.s_addr = servIP; //this is for server
	servAddr.sin_port = htons(portNum); //converts to TCP/IP port number
    
    //initializing the socket

    int sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP); //SOCK_STREAM goes to TCP protocol
    if (sock < 0) {
		cerr << "Error with socket" << endl;
		exit (-1);
	}

    cerr << "Welcome to the number-guessing game! Now the game starts :-))! " << endl << endl;

    status = connect(sock, (struct sockaddr*)&servAddr,sizeof(servAddr)); //now connecting the socket to IP address
    if(status < 0)
    {
        cerr << "Error with connect" << endl;
		exit (-1);
    } 

    cerr << "Connected successfully~ " << endl << endl;
	cerr << "Please guess a number between 0~999 ! Input 1000 to exit the game." << endl; 

    do{
       
       cerr<<"This is round "<<(roundCount+1)<<endl;
       tmp.tooHigh = 0;
	   tmp.tooLow = 0;
	   tmp.correct = 0;
      // for(int i = 0; i < MAX_NUM;i++)
       //{
           //cerr << "Guess " << (i+1) << ": ";
           Input_valid = false;

           while (!Input_valid) //checking the range of the user's input value
           {
                if(cin >> numGuess && !(numGuess > 999 || numGuess < 0))
					Input_valid = true;
				else if(numGuess == 1000)
				{
				    Input_valid = true;
					exit_ = true;
				}
                else
                {
                    cerr << "Invalid Input. Input must be between 0-999." << endl;
					cerr << "Try again: ";
					cin.clear();
					cin.ignore();
                }
           }

           //if the input is valid, then send it to the server and check whether the user win or not
           send_long(numGuess,sock);

           
      // }

        tmp = rec_result(sock); //receive results from server
        
	   if(tmp.tooLow)
	   { 
		   
          former1 = numGuess;
		  cerr << "lower than "<< former2 <<"higher than "<< numGuess << endl;
		
	   }
       else if(tmp.tooHigh)
	   {   
		   former2 = numGuess;
           cerr << "lower than " << numGuess<<", higher than " <<former1<< endl;
	   }
	   else if (tmp.correct) //to check if the user has won 
       {
           win = true;
		   cerr << "Answer Correct!"<<endl;
		   // cerr << exit_;
           //victory = recv_string(sock);



       }
	   

	   
       roundCount++;

	   if(win)
	   {   
		   former1 = 0;
		   former2 = 999;
		   cerr << "New round started!"<<endl;
		   cerr<<"==============="<<endl;
		   roundCount = 0;
		   win = false;
	   }
       
	  // cerr << "exit:"<<exit_;

    }
    while(!exit_);
    
     cerr << "The game ends";
    //now we close the socket
    status = close(sock);
	if (status < 0) {
		cerr << "Error with close" << endl;
		exit (-1);
	}



}

//converting the result into network readable version
Results toNet(Results toConvert){

   toConvert.tooHigh = htonl(toConvert.tooHigh); //converting 32 bit host to 32 bit network unit
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

Results rec_result(int sock) //this funciton is used to receive the result from server
{
	Results tempRes;
	Results *p = &tempRes;
	int bytesLeft = sizeof(tempRes);
	while(bytesLeft > 0)
	{
		int bytesRecv = recv(sock, (void*)p, sizeof(tempRes), 0); //this will return the length of packet sent by the server
		if(bytesRecv <= 0){
			cerr << "Error receiving results.";
			cin.get();
			exit(-1);
		}
		bytesLeft = bytesLeft - bytesRecv;
	}
	tempRes = *p;
	tempRes = notNet(tempRes);
	return tempRes;
}

void send_long(long num, int sock)
{
	long temp = htonl(num);
	int bytesSent = send(sock, (void *) &temp, sizeof(long), 0);
	if (bytesSent != sizeof(long)) 
		exit(-1);

}

long receive_long(int sock)
{
	int bytesLeft = sizeof(long);
	long networkInt;	
	char *bp = (char*) &networkInt;

	while(bytesLeft > 0)
	{
		int bytesRecv = recv(sock, (void*)bp, bytesLeft, 0);
		if(bytesRecv <= 0){
			break;
		}
		else{
			bytesLeft = bytesLeft - bytesRecv;
			bp = bp + 
				bytesRecv;
		}
	}
	networkInt = ntohl(networkInt);
	return networkInt;
}

