# NTHU-Introduction-to-Computer-Networks-
2021 client-server connection: The guessing game


1. Description
Implement a simple number guessing game. The server should allow clients
to guess a number between 0 to 999, and reply a guessing result. The client should be able to access the server, guess a number, and read the result replied by the server. Please use TCP socket, and the program should be written in C or C++. You are also required to use Wireshark to capture the packets transmitted by the server and the client, observe the result, and answer the questions.



2. Socket programming 


2.1 Server Program

⚫ Functions of the server program:

◼ Generate a random number for clients to guess.
◼ Allow clients to access the server.
◼ Show a prompt, and let the client guess a number.
◼ Accept a client’s request and receive the guessing number from the client.
◼ Respond the result to the clients.


2.2 Client Program

⚫ Functions of the client program:

◼ Should be able to connect to the server.
◼ Should be able to send a guessing number to the server.
◼ Should be able to receive a response from the server. 2.3 Note


⚫ For simplicity, we assume that there is only one client at a time.
⚫ All data must be transmitted via TCP socket.
⚫ Use fixed port numbers on both the client side and server side.
⚫ When the server receives the correct answer, it should response “Answer Correct” to the client and restart the game.
⚫ Use “winsock2.h”, “socket.h” or any other socket library you can find.



3. Wireshark

⚫ Capture the packets transmitted by the server and the client.
⚫ Observe the result and answer the following questions.



⚫ Game Start:
⚫ Guess a number:
⚫ If it is wrong, output the new range and guess next number:
⚫ If it is correct, output “Answer Correct”, and create a new game.
   
 
