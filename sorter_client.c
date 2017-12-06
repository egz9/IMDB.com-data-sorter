#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>


char* category = NULL;
int main(int argc, char ** argv) {

	char * input_dir_name = malloc(2*sizeof(char));//input_dir_name holds the name of the input directory
	char * output_dir_name = malloc(2*sizeof(char));//output_dir_name holds the name of the output directory
	char * host_name = NULL;
	char * port_number = NULL;	
	int port_num;
	int client_socket;
	int i,j,k;
	struct sockaddr_in server_address;

	// traverse commandline to initialize variables
	if (
			( argc == 7 && strcmp(argv[1], "-c") != 0 )
			|| ( argc != 7 &&
				argc != 9 &&
				argc != 11  ) ){
		printf("incorrect input, please enter paramaters as follows\n");
		printf("./sorter -c <catagory> -d <input directory> -o <output directory>\n");
		printf("NOTE: -d <input directory> & -o <output directory> are optional and can be given in either order\n");
		return -1;
	}
	//initialize both input and output dir to current directory
	strcpy(input_dir_name,".");
	strcpy(output_dir_name,".");
	//traverse commandline to reinitialize parameters
	for( i = 1; i < argc; i += 2 ){
		if(strcmp(argv[i],"-c") == 0){
			//if duplicate -c <category> input given, print error
			//otherwise initialize
			if(category != NULL){
				printf("incorrect input, please input only one category to sort by.\n");
				return -1;
			}
			category = malloc( (strlen(argv[i+1])+1)*sizeof(char) );	
			strcpy(category, argv[i+1]);
			continue;
		}	
		if(strcmp(argv[i],"-d") == 0){
			//if duplicate input directory print error
			//otherwise initialize 	
			if(strcmp(input_dir_name,".") != 0){
				printf("incorrect input, please input only one starting directory.\n");
				return -1;

			}
			input_dir_name = realloc(input_dir_name, (strlen(argv[i+1])+1)*sizeof(char) );
			strcpy(input_dir_name, argv[i+1]);
			continue;

		}	
		//if duplicate output directory	print error
		//otherwise initialize
		if(strcmp(argv[i],"-o") == 0){
			if(strcmp(output_dir_name,".") != 0){
				printf("incorrect input, please input only one output directory.\n");
				return -1;
			}
			output_dir_name = realloc(output_dir_name, (strlen(argv[i+1])+1)*sizeof(char) );
			strcpy(output_dir_name, argv[i+1]);
			continue;

		}
		if(strcmp(argv[i],"-h") == 0){
			//if duplicate -h <host_name> input given, print error
			//otherwise initialize
			if(host_name != NULL){
				printf("incorrect input, please input only one host name.\n");
				return -1;
			}
			host_name = malloc( (strlen(argv[i+1])+1)*sizeof(char) );	
			strcpy(host_name, argv[i+1]);
			continue;
		}	
		if(strcmp(argv[i],"-p") == 0){
			//if duplicate -p <port_number> input given, print error
			//otherwise initialize
			if(port_number != NULL){
				printf("incorrect input, please input only one port number.\n");
				return -1;
			}
			port_number = malloc( (strlen(argv[i+1])+1)*sizeof(char) );	
			strcpy(port_number, argv[i+1]);
			continue;
		}	
		//argv[1] || argv[3] || argv[5}  are something other than -c , -d , or -o	
		printf("incorrect input, please enter paramaters as follows\n");
		printf("./sorter -c <catagory> -d <input directory> -o <output directory>\n");
		printf("NOTE: -d <input directory> & -o <output directory> are optional and can be given in either order\n");
		return -1;
}//end for-loop (initialized inputdir, outputdir, and category)
if( category == NULL ||
		host_name == NULL ||
		port_number == NULL){
	printf("incorrect input, [-c <category> -h <host_name> -p <port_number>]  parameters required.\n");
	return -1;
}

port_num = atoi(port_number);
free(port_number);



//create socket

client_socket = socket(AF_INET, SOCK_STREAM, 0);

// initalize  server_address

server_address.sin_family = AF_INET;
server_address.sin_port = htons(port_num);
server_address.sin_addr.s_addr = host_name; 		// This initialization doesn't work **********

// connect 

int connection = connect(client_socket,(struct sockaddr*)&server_address,sizeof(server_address));
if (connection == -1){
	printf("failed to connect to sever :(\n");
	return -1;
}

// send category over
send(connection,category,sizeof(category),0);


//begin traversing 
//while ( has csv ){
//
//		send(Sort_Request,movie_name);
//		recv(sorted csv);
//		doStuff(sorted csv);
//		t
//		t
//		t
//		}	
//
//close socket 
close(client_socket);
free(host_name);
free(input_dir_name);
free(output_dir_name);
free(category);
return 0;
}
