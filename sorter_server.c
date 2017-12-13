#include <stdio.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <arpa/inet.h>
#include <pthread.h>
#include "sorter_server.h"
#include <semaphore.h>
#include <ctype.h>
#include <sys/types.h>
#include <signal.h>
#include <netinet/tcp.h>
#include <sys/syscall.h>
#include <sys/types.h>

int column = 28;
int sortByColumnNum = -1;
char * sortByColumnName = NULL;
char nameOfColumns[28][30] = {"color","director_name","num_critic_for_reviews","duration","director_facebook_likes","actor_3_facebook_likes","actor_2_name","actor_1_facebook_likes","gross","genres","actor_1_name","movie_title","num_voted_users","cast_total_facebook_likes","actor_3_name","facenumber_in_poster","plot_keywords","movie_imdb_link","num_user_for_reviews,language","country","content_rating","budget","title_year","actor_2_facebook_likes","imdb_score","aspect_ratio","movie_facebook_likes"};

int dataOfColumns[] = {0,0,1,1,1,1,0,1,1,0,0,0,1,1,0,1,0,0,1,0,0,0,1,1,1,1,1,1};
//0 for string
//1 for numeric
int dataType = -1;
sem_t mutex;
Node * allData = NULL;

void * request(void * sock_info);
char * extract(int sockfd, char ** buffer);
Node * decode(char * buffer, Node * front);
char * trim(char * str);
void print(int sockfd);

int main(int argc, char * argv[])
{
	int sockfd = -1;			//file descriptor for our server socket
	int newsockfd = -1;			//file descriptor for a client socket
	int portno = -1;			//server port to connect to
	socklen_t clilen = -1;			//utility variable - size of clientAddressInfo below
	struct sockaddr_in serverAddressInfo;	//super-special secret C struct that holds address info for building our server socket
	struct sockaddr_in clientAddressInfo;	//super-special secret C struct that holds addres info about our client socket


	//if the user didn't enter enough arguments, complain and exit
	if (argc < 3)
	{
		fprintf(stderr, "ERROR no port provided\n");
		exit(1);
	}
	sem_init(&mutex, 0,1);

	//if the user gave enough arguments, try to use them to get a port number and address
	//convert the text representation of the port number given by the user to an int
	portno = atoi(argv[2]);

	//try to build a socket ...if it doesn't work, complain and exit
	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if (sockfd < 0)
	{
		perror("ERROR opening socket");
		exit(EXIT_FAILURE);
	}

	//we now have the port to build our server socket on ... time to set up the address struct
	//zero out the socket addres info struct ... always initialize!
	bzero((char *) &serverAddressInfo, sizeof(serverAddressInfo));

	//set the remote port ... translate from a 'normal' int to a super special 'network-port-int'
	serverAddressInfo.sin_port = htons(portno);

	//set a flag to indicate the type of network address we'll be using
	serverAddressInfo.sin_family = AF_INET;

	//set a flag to indicate the type of network address we'll be willing to accept connections from
	serverAddressInfo.sin_addr.s_addr = INADDR_ANY;

	//we have an address struct and a socket ... time to build up the server socket
	//bind the server socket to a specific local port so the client has a target to connect to
	if (bind(sockfd, (struct sockaddr *) &serverAddressInfo, sizeof(serverAddressInfo)) < 0)
	{
		perror("ERROR on binding");
		exit(EXIT_FAILURE);
	}

	//set up the server socket to listen for client connections
	listen(sockfd,5);

	// the size of a clientAddressInfo struct
	clilen = sizeof(clientAddressInfo);

	pthread_t thread_id = NULL;	
	printf("Received connections from: ");
	//block until a client connects, when it does, create a client socket
	//accept() extract the first connection on the queue of pending connections 
	//return -1 if accepts fail
	while( (newsockfd = accept(sockfd,(struct sockaddr *)&clientAddressInfo,&clilen) ))
	{
		int flag = 0; //0 for off, 1 for on
		setsockopt(sockfd,IPPROTO_TCP, TCP_NODELAY, (char *)&flag, sizeof(int));

		char ipAddress[INET_ADDRSTRLEN];
		if(inet_ntop(AF_INET, &clientAddressInfo.sin_addr.s_addr,ipAddress, INET_ADDRSTRLEN)!=NULL)
		{
			printf("%s, ", ipAddress);
		}
		else
		{
			printf("Error: inet_ntop() failled\n");
		}
		if(pthread_create(&thread_id, NULL, request, (void*)&newsockfd) != 0)
		{
			perror("ERROR on accept");
			exit(0);
		}
	}
	pthread_join(thread_id, NULL);
	//if we're here, a client tried to connect
	//if the connection blew up for some reason, complain and exit
	if (newsockfd < 0)
	{
		perror("ERROR on accept");
		exit(EXIT_FAILURE);
	}

	//This should kill the server im not too sure
	kill(getpid(),SIGKILL);
	return 0;
}
void * request(void * sock_info)
{
	int sockfd = *(int*)sock_info; 
	int capacity = 500;			//Capacity of the buffer
	char * buffer = (char *)malloc(sizeof(char)*capacity);
	if(buffer == NULL)
	{
		printf("error: malloc failed\n");
		exit(0);
	}
	printf("\n");
	
	//zero out the char buffer to receive a client message
	bzero(buffer,capacity);
	//Lets assume the first message will be a request
	char * data;
	data = extract(sockfd, &buffer);
	fprintf(stdout,"data = %s\n", data);
	fprintf(stdout,"buffer = %s\n\n", buffer);
	if(strstr(data, "Sort_Request") != NULL)
	{
		sortByColumnName = strchr(data, ',');
		sortByColumnName = (sortByColumnName+1);
		//printf("sortByColumnName %s\n", sortByColumnName);
		while(data!=NULL)
		{
			data = extract(sockfd, &buffer);
			fprintf(stdout,"data = %s\n", data);
			fprintf(stdout,"buffer = %s\n\n", buffer);
			/*
			Node * front = NULL;
			front = decode(data, front);
			*/
		}
/*		bzero(buffer, 500);
		sprintf(buffer,"2?13Complete_Sort",);
		int written = 0;
		do{
			written = write(sockfd,buffer+written,strlen(buffer)-written) + written;
			//if we couldn't write to the server for some reason, complain and exit
			if (written < 0)
			{
				perror("ERROR writing to socket");
				exit(0);
			}
		}while(strlen(buffer)
*/
	}
	else if(strstr(data, "Dump_Request") != NULL)
	{
		int i = 0;
		while (i < column)
		{
			if(strcmp(sortByColumnName, nameOfColumns[i]) == 0)	
			{
				sortByColumnNum = i;
				dataType = dataOfColumns[i];
				break;
			}
			i++;
		}
		//mergesort(&allData,sortByColumnNum, dataType);
		//print(sockfd);
	}
	else
	{
		//not supposed to go here
		printf("error\n");
	}

	shutdown(sockfd,SHUT_WR);
	pthread_exit(0);
}
//////////////////////////////////////////
char * extract(int sockfd, char ** buffer){
	
	int readed = 0;
	int capacity = 500;
	char data[capacity];
	bzero(data,capacity);
	memcpy(data, *buffer, capacity);
	if(strlen(data) == 0)
	{
		readed = read(sockfd,data+readed, capacity-readed) + readed;
		if(readed < 0)
		{
			perror("Error reading from socket");
			exit(0);	
		}
		fprintf(stdout,"READ1 = %s\n", data);
	}
	if(strlen(data) <= 5)
	{
		//concatenate old and new buffer		
		sprintf(data,"%s", *buffer);
		int index = (int)strlen(data);
		readed = read(sockfd,&data[index], capacity-readed) + readed;
		if(readed < 0)
		{
			perror("Error reading from socket");
			exit(0);	
		}
		fprintf(stdout,"READ2 = %s\n", data);
	}

	int i = 0;
	char size_Of_Header[5] = {0};
	while(i < 5)
	{
		if(data[i]=='?')
		{
			size_Of_Header[i] = '\0';
			i++;
			break;
		}
		size_Of_Header[i] = data[i];
		i++;
	}
	//fprintf(stdout, "\nlength of header %s\n", size_Of_Header);
	int sizeOfHeader = -1;
	sizeOfHeader = atoi(size_Of_Header);
	if(sizeOfHeader == 0 && strlen(data)>0)
	{
		//this is a temporary solution
		//lmao it was deleteding this term sooo i just added an extra space and lmao it works
		sizeOfHeader = 3;
	}
	else if(sizeOfHeader == 0 && strlen(data) < 0)
	{
		return NULL;
	}
	char size_of_data[5] = {0};
	int j = 0;
	while(j < sizeOfHeader)
	{
		size_of_data[j] = data[i];
		i++;
		j++;
	}
	size_of_data[j] = '\0';
	//fprintf(stdout, "\nlength of data %s\n", size_of_data);
	int size = 0;
	size = atoi(size_of_data);
	fprintf(stdout, "sizeofHeader= %d\tsizeofdata= %d\n", sizeOfHeader,size);
	char line[500];
	j = 0;
	fprintf(stdout,"size= %d\tstrlen= %d\n", size, (int)strlen(data));
	int nbuf = size;
	//int nbuf = 0; //number of characters to read into the buffer
	while(size>strlen(data)&&nbuf<capacity)
	{
		int bytesToRead = size-(int)(strlen(data))+5;
		//This 5 may break
		fprintf(stdout, "bytesToRead= %d\n", bytesToRead);
		// pretend the buffer will never overflow okay fine i'll but it in
		if(readed+bytesToRead > capacity)
		{
			bytesToRead = capacity-readed;
		}
		int index = (int)strlen(data);
		readed = read(sockfd,&data[index], bytesToRead);
		if(readed < 0)
		{
			perror("Error reading from socket");
			exit(0);	
		}
		nbuf +=readed;
		//printf("size = %d\n",readed);
		printf("READ3 = %s\n", data);
	}
	//fprintf(stdout,"size %d\tstrlen %d\n", size, (int)strlen(data));

	while(j < size)
	{
		line[j] = data[i];
		i++;
		j++;
	}
	line[j] = '\0';
	fprintf(stdout, "line %s\n", line);

	bzero(*buffer,capacity);
	char temp [500];
	bzero(temp,500);
	i = size+sizeOfHeader+2;
	j = 0;
	while(j < capacity&&i<capacity)
	{
		temp[j] = data[i];
		j++;
		i++;
	}
	fprintf(stdout, "temp [%s]\n", temp);
	memcpy(*buffer, temp,500);
	fprintf(stdout,"buffer [%s]\n", *buffer);
	char * ptr = line;
	return ptr;
}
/////////////////////////////////////////
Node * decode(char * lineptr, Node * front)
{
	int i = -1;
	const char * delim = ",";
	char * token = (char *)malloc(200*sizeof(char));
	if(token == NULL)
	{
		return NULL;
	}
	char ** tempArray = (char **)malloc(column*sizeof(char*));
	if(tempArray == NULL)
	{
		return NULL;
	}
	for(i = 0; i < column; i++)
	{
		tempArray[i] = (char *)malloc(200*sizeof(char));
		if(tempArray[i] == NULL)
		{
			return NULL;
		}
	}
	//printf("%s\n", lineptr);
	while( (token = strsep(&lineptr, delim)) != NULL )
	{
		if( (strchr(token, '"')) != NULL)
		{
			char * buffer = (char*)malloc(200*sizeof(char));
			token++;
			strcpy(buffer, token);
			//printf("\n");	
			//printf("BUFFER:%s\n", buffer);		
			while( (token = strsep(&lineptr, delim)) != NULL)
			{
				strcat(buffer, ",");
				strcat(buffer, token);
				//printf("BUFFER:%s\n", buffer);
				char * ptr;
				if((ptr = strchr(buffer, '"')) != NULL)
				{
					*ptr = '\0';
					//printf("BUFFER:%s\n", buffer);
					buffer = trim(buffer);
					strcpy(tempArray[i],buffer);
					//printf("temparray:%s\tbuffer:%s\n", tempArray[i], buffer);
					break;
				}

			}
			free(buffer);			
		}
		else
		{
			token = trim(token);
			strcat(token, "\0");
			tempArray[i] = token;
			//printf("%s\t", tempArray[i]);
		}
		i = i+1;
	}
	//insert(tempArray);
	//printf("\n");

	Node * newNode = (Node*)malloc(sizeof(Node));
	if(newNode == NULL)
	{
		printf("error\n");
		return NULL;
	}
	newNode -> next = NULL;
	newNode -> data = (char**)malloc(column*sizeof(char*));
	if(newNode -> data == NULL)
	{
		printf("error: malloc() failed\n");
		return NULL;
	}
	for(i = 0; i < column; i++)
	{
		newNode -> data[i] = (char *)malloc(200*sizeof(char));
		if(newNode -> data[i] == NULL)
		{
			printf("error: malloc() failed\n");
			return NULL;
		}
		strcpy(newNode -> data[i], tempArray[i]);
		//printf("newNode ->data[%d] = %s\ttempArray[%d] = %s\n", i, newNode -> data[i], i, tempArray[i]);
	}

	Node * temp = front;
	if(temp == NULL)
	{
		front = newNode;
	}
	else
	{
		while(temp -> next != NULL)
		{
			temp = temp -> next;
		}
		temp -> next = newNode;
	}
	//printf("here %d\t row %d\n", (int) getpid(), row);		
	for(i = 0; i < column; i++)
	{
		tempArray[i] = '\0';
	}
	free(lineptr);
	free(token);
	return front;
}
char * trim(char * str)
{
	char * front = str;
	char * end = NULL;
	size_t len = 0;

	if(str == NULL)
	{
		return NULL;
	}
	if(str[0] == '\0')
	{
		return str;
	}
	len = strlen(str);
	end = str + len;

	while( isspace((unsigned char) *front))
	{
		++front;
	}
	if(end != front)
	{
		while( isspace((unsigned char) * (--end)) && end != front)
		{
			//do nothing
		}
	}
	if(str + len -1 != end)
	{
		*(end + 1) = '\0';
	}
	else if(front != str && end == front)
	{
		*str = '\0';
	}
	//adjust the pointer that it can be returned to main where it was malloc'ed and then can be freed
	end = str;
	if( front != str )
	{
		while(*front)
		{
			*end++ = *front++;
		}
		*end = '\0';
	}
	char * newline = strchr(str, '\n');
	while ( newline != NULL)
	{
		*newline = '\0';
		newline = strchr(str, '\n');
	}
	return str;
}

void print(int sockfd)
{

	if(allData == NULL)
	{
		printf("no data to ouput\n");
		return;
	}
	char buffer[500] ={0};
	int i;
	int count = column;
	bzero(buffer,500);
	for(i = 0; i < column; i++)
	{
		count--;
		//sprintf(buffer, "%s", nameOfColumns[i]);
		if(count > 0)
		{
			sprintf(buffer,",");
		}
	}
	if(send(sockfd, buffer, 500, 0) == -1)
	{
		printf("error: could not send buffer\n");
		exit(0);
	}

	Node * temp = allData;
	while(temp != NULL)
	{
		bzero(buffer,500);
		i = 0;
		count = column;
		for(i = 0; i < column; i++)
		{
			if(temp == NULL)
			{
				printf("error\n");
				return;
			}
			count--;	
			if( strchr(temp -> data[i], ',') != NULL)
			{
				sprintf(buffer,"\"%s\"", temp -> data[i]);
			}
			else
			{
				sprintf(buffer,"%s", temp -> data[i]);
			}

			if(count > 0)
			{
				sprintf(buffer,",");
			}
		}
		if(send(sockfd, buffer, 500, 0) == -1)
		{
			printf("error: could not send buffer\n");
			exit(0);
		}
		temp = temp -> next;
	}
}
