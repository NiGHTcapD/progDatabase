// progDatabase.cpp : Defines the entry point for the console application.
//

#pragma comment(lib, "ws2_32")
#include "stdafx.h"
#include <winsock2.h>
#include <stdio.h>

#include <iostream>
#include <string>
#include <algorithm>
#include <vector>

using namespace std;
#define axe 150
#define why 25

//This is the "backend"; in contains and reads from the database at the whims of the front end. It can take requests from the front end via http and also connect to a space shared with the indexer; that's how the indexer delivers its data to this.
//Included within is a vector length "axe", the website names; a vector of arrays "axe" by 25, the heatmaps of programming terms; and a spare vector of vectors "axe" by "why" for any additional search terms.
//well, it was gonna be until I looked at what actually happens in sockets.

int main(int argc, char **argv)
{
	WSADATA              wsaData;

	// Initialize Winsock version 2.2
	WSAStartup(MAKEWORD(2, 2), &wsaData);
	printf("Winsock DLL status is %s.\n", wsaData.szSystemStatus);

	// Code to handle socket:
	SOCKET Socket;

	// Create a new socket to make a client or server connection.
	// AF_INET = 2, The Internet Protocol version 4 (IPv4) address family, TCP protocol
	Socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (Socket == INVALID_SOCKET)
	{
		printf("Socket() failed!Error code : %ld\n", WSAGetLastError());
		// Do the clean up
		WSACleanup();
		// Exit with error
		return -1;
	}

	//Socket address structure
	SOCKADDR_IN          ServerAddr;
	unsigned int         Port = 21001;

	// IPv4
	ServerAddr.sin_family = AF_INET;
	// Port no.
	ServerAddr.sin_port = htons(Port);
	// The IP address
	ServerAddr.sin_addr.s_addr = htonl(INADDR_ANY);




	char *links[axe][4000];//4000 can be longer if websites need to be that long (I doubt it) but it's also there because it was 4000 to begin with.
	int terms[axe][25];//for heatmap of searchterms
	int moreterms[axe][why];

	int full = 0;//The horizontal; how many links are currently stored.
	int more = 0;//The vertical; how many new words are stored.




	////Bind to socket
	// Associate the address information with the socket using bind.
	// Call the bind function, passing the created socket and the sockaddr_in
	// structure as parameters. Check for general errors.
	if (bind(Socket, (SOCKADDR *)&ServerAddr, sizeof(ServerAddr)) == SOCKET_ERROR)
	{
		printf("Server: bind() failed! Error code: %ld.\n", WSAGetLastError());
		// Close the socket
		closesocket(Socket);
		// Do the clean up
		WSACleanup();
		// and exit with error
		return -1;
	}
	else
		printf("Server: bind() is OK!\n");


	////Listen to Socket for client
	// Listen for client connections. We use a backlog of 5, which
	// is normal for many applications.
	if (listen(Socket, 5) == SOCKET_ERROR)
	{
		printf("Server: listen(): Error listening on socket %ld.\n", WSAGetLastError());
		// Close the socket
		closesocket(Socket);
		// Do the clean up
		WSACleanup();
		// Exit with error
		return -1;
	}
	else
		printf("Server: listen() is OK!\n");


	SOCKET NewConnection;
	bool keepconnected = true;
	while (keepconnected == true) {

		NewConnection = accept(Socket, NULL, NULL);
		if (NewConnection == SOCKET_ERROR) {
			printf("Server: accept() failed!Error code : %ld\n", WSAGetLastError());
			// Close the socket
			closesocket(Socket);
			// Do the clean up
			WSACleanup();
			// Exit with error
			return -1;
		}
		else
			printf("Server: accept() is OK!\n");

		int nope = 0;
		while (nope == 0) {
			char recvbuf[4000];
			memset(recvbuf, 0, 4000);
			//char* recvbuf = &recieverbuffer[0];

			int BytesReceived = recv(NewConnection, recvbuf, sizeof(recvbuf), 0);
			if (BytesReceived == SOCKET_ERROR) {
				printf("Client: recv() error %ld.\n", WSAGetLastError());
				nope = 1;
			}

			char wordswordswords[4050];
			//I need an string array to put quotes gotten into depending on 
			if (recvbuf[0] == 'S'&&recvbuf[1] == 'E'&&recvbuf[2] == 'T') {
				int length;//legacy word
				if (recvbuf[4] == '0' || recvbuf[4] == '1' || recvbuf[4] == '2' || recvbuf[4] == '3' || recvbuf[4] == '4' || recvbuf[4] == '5' || recvbuf[4] == '6' || recvbuf[4] == '7' || recvbuf[4] == '8' || recvbuf[4] == '9') {
					length = atoi(recvbuf + 4) - 1;//atoi takes as many integer characters as it can
					int lenny = 1;
					while (10 ^ lenny > length) { lenny++; }//adds 1 if from 1-9, 2 from 10-99, 3 from 100-999...
					links[full][0] = recvbuf + 5 + lenny;//need to only go as long as length. need some way of telling it when to stop.
					
					//if (recvbuf[5] == ' ') {
						//length = atoi(recvbuf + 4) - 1;//atoi takes as many integer characters as it can
						//links[whichquote][0] = recvbuf + 6;

						//strcpy(wordswordswords, "OK ");
						//strcat(wordswordswords, recvbuf + 6);
						//strcat(wordswordswords, "\r\n");
					//}

					int adder = 0;
					for (int zeta = 0; zeta < 25; zeta++) {
						int yeet = atoi(recvbuf + 6 + lenny + length + adder + (2*zeta)/*S-E-T+space+lenny+space+length+space+total characters of numbers with zeta spaces in between and zeta single-digits ones-places of integers*/);
						terms[full][zeta] = yeet;
						int lennys = 1;
						while (10 ^ lennys > yeet) { lennys++; 
						adder++; }//adds 1 if from 1-9, 2 from 10-99, 3 from 100-999...
					}

					full++;

				}
				else {
					strcpy(wordswordswords, "ERR BAD REQUEST\r\n");
				}
			}
			else if (recvbuf[0] == 'G'&&recvbuf[1] == 'E'&&recvbuf[2] == 'T') {//currently only works for a one-terms query...this must be changed, or added
				if (recvbuf[4] == '0' || recvbuf[4] == '1' || recvbuf[4] == '2' || recvbuf[4] == '3' || recvbuf[4] == '4' || recvbuf[4] == '5' || recvbuf[4] == '6' || recvbuf[4] == '7' || recvbuf[4] == '8' || recvbuf[4] == '9') {
					int whichquote;//legacy word

					//search through terms[][atoi(recvbuf + 4) - 1] for the ten largest of the term

					//from here to "bleach": copy this and make another one for the case of multiple words

					int mostmax = 0;//the most that has been achieved for that term; the ceiling.
					int still;//whichever link has the most, right now
					int hasmost[10];//which links to send off


					for (int zeta = 0; zeta < full; zeta++) {
						if (terms[zeta][atoi(recvbuf + 4) - 1] > mostmax) {
							mostmax = terms[zeta][atoi(recvbuf + 4) - 1];
							still = zeta;
						}
					}
					hasmost[0] = still;//whatever has the true most.

					int nen;
					if (full >= 10) { nen = 10; }
					else { nen = full + 1; }
					for (int ten = 1; ten < nen; ten++) {
						int most = 0;
						int vollo;//still, but only for now

						for (int zeta = 0; zeta < full; zeta++) {
							if (terms[zeta][atoi(recvbuf + 4) - 1] > most) {
								if (terms[zeta][atoi(recvbuf + 4) - 1] = mostmax) {
									if (zeta > still && vollo != still) {
										most = terms[zeta][atoi(recvbuf + 4) - 1];
										vollo = zeta;								//if it's a match and occurs *just* later, it's up next. This check equates to a form of skillstop.
										still = zeta;
									}//but if it's a match and is still the same, or isn't _just_ later, do nothing. Besides, "> most" should already take care of that.
								}
								else {
									most = terms[zeta][atoi(recvbuf + 4) - 1];//if it's larger than the prior "most" but lower than the ceiling, just update position of vollo and raise the floor.
									vollo = zeta;
								}
							}
							hasmost[ten] = vollo;
							mostmax = most;
							still = vollo;
						}
					}
					//bleach

					//send all 10. This happens in "char senderbuffer[4000];" up to "nope = 1;" and to loop it, a loop of length 'nen' (while loop, so it can also break if nope=1) around
					//from here to "nope = 1;" should do the trick.

					int tenny = 0;
					while(tenny<nen&&nope==0){
						strcpy(wordswordswords, "OK ");
						strcat(wordswordswords, links[hasmost[tenny]][0]);
						strcat(wordswordswords, "\r\n");
					




						char senderbuffer[4000];
						memset(senderbuffer, 0, 4000);
						const char* sendbuf = &senderbuffer[0];

						strcpy(senderbuffer, wordswordswords);
						int BytesSent = send(NewConnection, sendbuf, strlen(sendbuf), 0);
						//printf("%s %d %d\n", senderbuffer, strlen(sendbuf), BytesSent);
						if (BytesSent == SOCKET_ERROR) {
							printf("Client: send() error %ld.\n", WSAGetLastError());
							nope = 1;
						}
					}
					/*if (recvbuf[5] == ' ') {
						whichquote = atoi(recvbuf + 4) - 1;





						strcpy(wordswordswords, "OK ");
						strcat(wordswordswords, recvbuf + 4);
						strcat(wordswordswords, " ");
						strcat(wordswordswords, links[whichquote][0]);
						strcat(wordswordswords, "\r\n");
					}


					else if (recvbuf[6] == ' ') {
						whichquote = atoi(recvbuf + 4) - 1;

						strcpy(wordswordswords, "OK ");
						strcat(wordswordswords, recvbuf + 4);
						strcat(wordswordswords, " ");
						strcat(wordswordswords, links[whichquote][0]);
						strcat(wordswordswords, "\r\n");
					}
					else {
						whichquote = atoi(recvbuf + 4) - 1;
						if (whichquote > 19) {
							break;
						}
						else {
							strcpy(wordswordswords, "OK ");
							strcat(wordswordswords, recvbuf + 4);
							strcat(wordswordswords, " ");
							strcat(wordswordswords, links[whichquote][0]);
							strcat(wordswordswords, "\r\n");
						}
					}*/
				}
				else {
					strcpy(wordswordswords, "ERR BAD REQUEST\r\n");
				}
			}

			//Those were the normals, the "originals".

			else if (recvbuf[0] == 'N'&&recvbuf[1] == 'E'&&recvbuf[2] == 'W') {//This is what happens when progFrontend finds a new word.
				//first, we wait for another input; but in the meantime we can tuck this answer away.
				strcpy(wordswordswords, "NOK ");
				strcat(wordswordswords, recvbuf + 4);

				//I almost want to make these four pieces into either another function, or each their own functions...whatever.
				//We "call" S-E-T again, or rather, empty out our recvbuf so we can recieve word from the indexer. (Or make a disposible recvbuf2/recvbuftoo instead)
				//Basically...copy that whole mess of code here (note to self: make it send back ""OK"" up there) but instead of saying OK here, we say NOK and give it out new friend.






			
				//~~assume we can send to just progIndexer here for now; or to both progIndexer and progFrontend but progFrontend doesn't care.~~
				char senderbuffer[4000];
				memset(senderbuffer, 0, 4000);
				const char* sendbuf = &senderbuffer[0];

				strcpy(senderbuffer, wordswordswords);
				int BytesSent = send(NewConnection, sendbuf, strlen(sendbuf), 0);
				//printf("%s %d %d\n", senderbuffer, strlen(sendbuf), BytesSent);
				if (BytesSent == SOCKET_ERROR) {
					printf("Client: send() error %ld.\n", WSAGetLastError());
					nope = 1;
				}//after this, the only thing it *should* be recieving is YES.
			}
			else if (recvbuf[0] == 'Y'&&recvbuf[1] == 'E'&&recvbuf[2] == 'S') {//This is the counterpoint; when progIndexer has used its new word.
				int adder = 0;
				int lenny = 1;
				for (int zeta = 0; zeta < full; zeta++) {
					int yeet = atoi(recvbuf + 6 + adder + (2 * zeta)/*S-E-T+space+total characters of numbers with zeta spaces in between and zeta single-digits ones-places of integers*/);
					moreterms[zeta][more] = yeet;
					int lenny = 1;
					while (10 ^ lenny > yeet) {
						lenny++;
						adder++;
					}//adds 1 if from 1-9, 2 from 10-99, 3 from 100-999...
				}

				full++;
				
				
				strcpy(wordswordswords, "YE");

				//assume we can send to just progFrontend this time; progIndexer doesn't care.
				char senderbuffer[4000];
				memset(senderbuffer, 0, 4000);
				const char* sendbuf = &senderbuffer[0];

				strcpy(senderbuffer, wordswordswords);
				int BytesSent = send(NewConnection, sendbuf, strlen(sendbuf), 0);
				//printf("%s %d %d\n", senderbuffer, strlen(sendbuf), BytesSent);
				if (BytesSent == SOCKET_ERROR) {
					printf("Client: send() error %ld.\n", WSAGetLastError());
					nope = 1;
				}
			}



			/*char senderbuffer[4000];
			memset(senderbuffer, 0, 4000);
			const char* sendbuf = &senderbuffer[0];

			strcpy(senderbuffer, wordswordswords);
			int BytesSent = send(NewConnection, sendbuf, strlen(sendbuf), 0);
			//printf("%s %d %d\n", senderbuffer, strlen(sendbuf), BytesSent);
			if (BytesSent == SOCKET_ERROR) {
				printf("Client: send() error %ld.\n", WSAGetLastError());
				nope = 1;
			}
			*/

		}


		if (keepconnected == true)
		{
			keepconnected = true;
		}

	}

	////shutdown connection when done
	// Shutdown sending of data
	if (shutdown(Socket, SD_SEND) != 0)
	{
		printf("Client: Well, there is something wrong with the shutdown(). The error code: %ld\n", WSAGetLastError());
	}
	else
	{
		printf("Client: shutdown() looks OK...\n\n");
	}

	////close socket
	if (closesocket(Socket) != 0)
	{
		printf("Server: Cannot close socket. Error code: %ld\n", WSAGetLastError());
	}
	else
	{
		printf("Server: Closing socket...\n");
	}

	////call cleanup when no more sockets remain
	if (WSACleanup() != 0)
	{
		printf("Client: WSACleanup() failed!...\n");
	}



	// Cleanup socket
	WSACleanup();














	
	
	
	
	
	
	
	return 0;
}

