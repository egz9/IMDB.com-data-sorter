/*
 * SYSTEMS PROGRAMMING PROJECT 3
 * @author Eric Zimmerman
 */
#include "sorter_thread.h"
#define BUFFER_SIZE 1000
csvNode* head;

char getTypeFlag(char* columnToken) {
	char typeFlag = '0';
  	char *catStrings[] ={ "color","director_name","actor_2_name","genres","actor_1_name",
  						"movie_title","actor_3_name","plot_keywords","movie_imdb_link",
                    	"language","country","content_rating"};
                    	
  	char *catNums[] = {"num_critic_for_reviews","duration","director_facebook_likes",
                    	"actor_3_facebook_likes","actor_1_facebook_likes","gross",
                    	"num_voted_users","cast_total_facebook_likes","facenumber_in_poster",
                    	"num_user_for_reviews","budget","title_year","actor_2_facebook_likes",
                    	"imdb_score","aspect_ratio","movie_facebook_likes"};
	int k = 0;
  	//printf("entering catSortString differentiator\n");
  	for(k=0;k<=11;k++){
    	if( strcmp(columnToken,catStrings[k]) == 0){
      		typeFlag = 's';
    	}
  	}
  	for(k=0;k<=15;k++){
    	if( strcmp(columnToken,catNums[k]) == 0){
      		typeFlag = 'n';
    	}
  	}
  	return typeFlag;
}


//checks to see if the first two nodes have been merged same number of times 					******remember to increment num mergess*******
//and will merge them if they do. Will call tryMerge again upon successful merge.
//reallocates the array of the larger node so that the smaller array is iterated through
//to concatenate the two arrays. Then calls merge to sort... <= O(n+m).
csvNode* tryToMerge( csvNode* front, char typeFlag){
	//printf("trying to merge!\n");
	//printf("[%d]in tryToMerge\n", pthread_self());
	if (front->next == NULL)				// one node, can't merge
		return front;

	else if (front-> numMerges != front->next->numMerges)		// diffent number of merges, can't merge
		return front;


	else{									//can merge 
		int newLength = (front->length)+(front->next->length);
		int i;

		if(front->length>front->next->length){					// merge case 1: first node has the larger array

			front->data = (rowtype**)realloc(front->data,newLength*sizeof(rowtype*));
			if (front->data == NULL){
				fprintf(stderr, "front->data (tryToMerge) realloc failed\n");
				exit(0);
			}
			for(i=0;i<front->next->length; i++)
				*(front->data+(i+front->length))= *(front->next->data+i);
			//csvNode * deleteThisNode = front->next; //new
			
			//free(front->next->data);
			
			front->next = front->next->next;
			//free(deleteThisNode); //new
			

		}else{										// merge case 2: second node has the larger array

			front->next->data = (rowtype**)realloc(front->next->data,newLength*sizeof(rowtype*));
			if (front->next->data == NULL){
				fprintf(stderr, "front->next->data (tryToMerge) realloc failed\n");
				exit(0);
			}
			for(i=0;i<front->length; i++)
				*(front->next->data+(i+front->next->length))= *(front->data+i);
			//printf("trying to merge!\n");
			//csvNode * deleteThisNode = front; //new
			
			//free(front->data);
			
			front = front->next;
			//free(deleteThisNode); //new

		}

		merge(0,front->length-1,newLength-1,front->data,typeFlag);
		front->numMerges++;
		front->length = newLength;
		front = tryToMerge(front,typeFlag);
		return front;

	}
}
csvNode* addToMaster(csvNode* front, rowtype** newData, int length, char typeFlag ){
	//printf("started addToMaster function\n");
	//printf("[%d] in addToMaster\n", pthread_self());
	csvNode* newGuy = (csvNode*)malloc(sizeof(csvNode));
	if (newGuy == NULL){
		fprintf(stderr, "newGuy malloc failed\n");
		exit(0);
	}
	newGuy->data = newData;
	newGuy->length = length;
	newGuy->numMerges = 0;
	//lock
pthread_mutex_lock(&csv_node_lock);
	newGuy->next = front;
	front = newGuy;

	//printf("front's length is %d...front's numMerges is %d...\n",front->length,front->numMerges);
	front = tryToMerge(front,typeFlag);

pthread_mutex_unlock(&csv_node_lock);

	//printf("[%d] addToMaster will return!!\n", pthread_self());
	return front;
}

//same as tryTMerge, but doesn't check number of merges
csvNode* forceMerge( csvNode* front, char typeFlag){
	//printf("[%d] in forceMerge\n", pthread_self());
	if (front->next == NULL)				// one node, can't merge
		return front;

	else{									//can merge 
		int newLength = (front->length)+(front->next->length);
		int i;

		if(front->length>front->next->length){					// merge case 1: first node has the larger array

			front->data = (rowtype**)realloc(front->data,newLength*sizeof(rowtype*));
			if (front->data == NULL){
				fprintf(stderr, "front->data (forceMerge) realloc failed\n");
				exit(0);
			}
			for(i=0;i<front->next->length; i++)
				*(front->data+(i+front->length))= *(front->next->data+i);
				
			//csvNode * deleteThisNode = front->next; //new
			
			//free(front->next->data);
			
			front->next = front->next->next;
			//free(deleteThisNode); //new

		}else{										// merge case 2: second node has the larger array

			front->next->data = (rowtype**)realloc(front->next->data,newLength*sizeof(rowtype*));
			if (front->next->data == NULL){
				fprintf(stderr, "front->next->data (forceMerge) realloc failed\n");
				exit(0);
			}
			for(i=0;i<front->length; i++)
				*(front->next->data+(i+front->next->length))= *(front->data+i);
			//csvNode * deleteThisNode = front; //new
			
			//free(front->data);
			
			front = front->next;
			//free(deleteThisNode); //new

		}

		merge(0,front->length-1,newLength-1,front->data,typeFlag);
		front->numMerges++;
		front->length = newLength;
		front = forceMerge(front,typeFlag);
		return front;

	}
}

/* sortcsv is a function intended for multithreading situations.
 * you must pass it a Thread_Args struct which will contain the name of 
 * the csv file you wish to sort in the struct member: entry_name
 * and the path to the directory you are currently in which
 * will be in the the struct member path.
 * This function will sort a csv based on the global variable 
 * category. This sorted list is represented by an array of rowtype 
 * struct pointers called dataArray. this dataArray will be passed
 * to the master list*/
void * sortcsv(Thread_Args * args){	
	//args holds path to directory csv is located in (args->path). 
	//It also holds the name of the csv (entry_name)  
	//printf("[%d] in sortcsv\n", pthread_self());
	char * csvpath = (char*)malloc( strlen( args->path) + strlen(args->entry_name) + 3); 
	if (csvpath == NULL){
		fprintf(stderr, "csvpath malloc failed\n");
		exit(0);
	}
	/*
	//****************************************
	char outFileName[80];
	strcpy(outFileName, "sortedfiles/");
	strcat(outFileName, args->entry_name);
	strcat(outFileName, "-sorted.txt");
	//****************************************
	*/
	strcpy(csvpath, args->path);
	strcat(csvpath, "/");
	strcat(csvpath, args->entry_name);
	
	free(args->path);
	free(args->entry_name);
	free(args);
	
	char lines[1500];
	char * linesPtr = &lines[0];
	
	FILE * inputFile = fopen(csvpath, "r");
	int i = 0;

	char *token;						// used to store tokens
	char *tokenizedLine[BUFFER_SIZE];	// array of category strings from first line
	rowtype **dataArray;				// array of pointers to rowtype structs
	int catCount = 1;					// integer value of number of categories
	int lineCount = 0;					// integer value of number of lines of data
	//rowtype categories;					// struct for first line of CSV file
  	int maxLine = 500;
  	char typeFlag = '0';
  	
	// Check for no file input case.
	if(inputFile==NULL){
		fprintf(stderr, "Process [%d] Error opening input file of unsorted list\n", getpid());
		exit(1);
	}
	
	
	//look at how many commas exist----------------------
	fgets(lines, 1500*sizeof(char), inputFile);
	rewind(inputFile); 
	
	char * commaPtr = lines;
	int commaCount = 0;
	while( *commaPtr != '\0'){
		if (*commaPtr == ',') commaCount++;
		commaPtr++;
	}
	//printf("comma Count: %d\n", commaCount);
	if (commaCount != 27){
		fclose(inputFile);
		return NULL;
	}
	//--------------------------------------------------
	

  	// Temporary string to hold CSV file lines.
	
	// Iterate through file line by line. Make a distinction between first line, which contains strings denoting different categories.
	// Every subsequent line has the same number of commas as the first line, and populates a row by column.
	i = 0;
  	dataArray = (rowtype**)malloc(sizeof(rowtype*)*maxLine);
  	if (dataArray == NULL){
		fprintf(stderr, "dataArray malloc failed\n");
		exit(0);
	}
  	if( dataArray == NULL) return;
  	//fgets(lines, 1500*sizeof(char), inputFile);
  	
	while(fgets(lines, 1500*sizeof(char), inputFile) !=NULL ){
		linesPtr = &lines[0]; //need to use as an argument for strsep
		
    	//printf("%s\n", lines);
    
		// First line of CSV file is list of categories.
		if(lineCount==0){
			// Store entire first line string into categories.wholerow.
			//categories.wholerow = (char*)malloc(sizeof(char)*BUFFER_SIZE);
			//if(categories.wholerow == NULL){
				//fprintf(stderr, "categories.wholerow malloc failed\n");
				//exit(0);
			//}
      		//printf("malloced categories.wholerow\n");
      		//strcpy(categories.wholerow,lines);
			// Allocate memory for char array to hold the tokens from the first line of the CSV file.
			// Get the first token from the line and store it into the aforementioned array.
			token = strsep(&linesPtr,",");
      		// Store first token (category) into first tokenizedLine position
      		tokenizedLine[0] = (char*)malloc(sizeof(char)*64);
			if(tokenizedLine[0] == NULL){	
				fprintf(stderr, "tokenizedLine[0] malloc failed\n");
				exit(0);
			}
      		strcpy(tokenizedLine[0], token);
			//Continue this process until all categories are tokenized and stored into the tokenizedLine. 
			//Increment catCount for every additional category token.
			int categoryExists = 0;
      		while(token=strsep(&linesPtr,",")){
        		tokenizedLine[catCount] = (char*)malloc(sizeof(char)*100);
        		if(tokenizedLine[catCount] == NULL){
					fprintf(stderr, "tokenizedLine[%d] malloc failed\n", catCount);
					exit(0);
				}
				strcpy(tokenizedLine[catCount], token);
				trim(tokenizedLine[catCount]);
				if(strcmp(tokenizedLine[catCount], category) == 0 ){
					categoryExists = 1;
				}
				catCount++;
			}
			if (categoryExists == 0){
				fprintf(stderr, "thread [%d] category [%s] was not found in %s\n", pthread_self(), category, csvpath);
				return NULL;
			}
			// Search tokenizedLine if input matches any of the tokens. If found, 
			//break loop. i is the number of commas preceding the matched token.
      		
      		i=0;
			while(i<= catCount-1){//i< catCount-1
        		if(strcmp(tokenizedLine[i], category)==0)
					break;
        		// If user-input for catSortString does not match up with last token, then that category is not defined in the CSV file.
				if((i == catCount-1) && (strcmp(tokenizedLine[i],category))!=0 ){ //i==catCount-2
					fprintf(stderr, "\nThread [%d] Error: There does not exist a category [%s]\n\n", pthread_self(), category);
					//exit(1);
					return NULL;
				}
				i++;
			}
      	//populate the dataArray for all lines of csv after first
		}else{
      		if(lineCount>maxLine){
				maxLine = maxLine*2;
        		dataArray = (rowtype**)realloc(dataArray,sizeof(rowtype*)*maxLine);
        		if (dataArray == NULL){
					fprintf(stderr, "dataArray (sortcsv) realloc failed\n");
					exit(0);
				}
      		}
      		
      		dataArray[lineCount-1] = (rowtype*)malloc(sizeof(rowtype) );
      		if(dataArray[lineCount-1] == NULL){
				fprintf(stderr, "dataArray[%d] malloc failed\n", lineCount-1);
				exit(0);
			}
      		dataArray[lineCount-1]->wholerow = (char*)malloc(sizeof(char)*(strlen(lines)+1));
      		if (dataArray[lineCount-1]->wholerow == NULL){
				fprintf(stderr, "dataArray[%d]->wholerow malloc failed\n", lineCount-1);
				exit(0);
			}
      		dataArray[lineCount-1]->key = (char*)malloc(sizeof(char)*200);
      		if (dataArray[lineCount-1]->key == NULL){
				fprintf(stderr, "dataArray[%d]->key malloc failed\n", lineCount-1);
				exit(0);
			}
      		strcpy( dataArray[lineCount-1]->wholerow, lines );

      		token = strsep(&linesPtr, ",");
      		
      		//they may be quotation marks. These indicate there will be commas in 
      		//this string but it should still be treated as a single entry in csv
      		if(token != NULL && token[0] == '"'){
				strcat(token,strsep(&linesPtr,"\"") );
				//make sure we get the whole string in quotes plus the ,
				strcat(token,strsep(&linesPtr,",") );
			}
      		strcpy(tokenizedLine[0],token);
      		if(i==0){
        		strcpy(dataArray[lineCount-1]->key, token);
      		}else{
        		int j = 1;
        		//i corresponds to how many commas inward the key category is
        		for(j=1;j<=i;j++){
          			token = strsep(&linesPtr,",");
          			//quotes in csv indicate there will be commas in this string but it
					//should still be treated as a single entry
          			if(token != NULL && token[0] == '"'){
						strcat(token,strsep(&linesPtr,"\"") );
						//make sure we get the whole string in quotes plus ,
						strcat(token,strsep(&linesPtr,",") );
					}
					
          			strcpy(tokenizedLine[j],token);
          			
          			//printf("copied token to array[%d]\n",j);
        		}
        		//printf("exited for loop\n");
				trim(token);
        		strcpy(dataArray[lineCount-1]->key, token);
        		//printf("copied token to key\n");
      		}
		}
    	lineCount++;
    	//printf("lineCount++\n");
  	}
  	// Get typeFlag for first sorting column
  	typeFlag = getTypeFlag(category);
  	// Sort dataArray with the typeFlag
  	//we use linecount-2 cuz the first line of the csv is not in dataArray
  	//printf("bout to sort\n");
  	
  	
  	
  	sort(0, lineCount-2, dataArray, typeFlag);
	/*
	FILE * outFile = fopen(outFileName, "w");
	for (i=0;i<=lineCount-2;i++){
		//DELETE THIS WHEN U INCLUDE ADDTOMASTER
		fprintf(outFile, "\n[%s]\n", dataArray[i]->key);
		fprintf(outFile, "%s", dataArray[i]->wholerow);
	}
	*/
	//************************this is where we use add to master function**********************************
	head = addToMaster( head, dataArray, lineCount-1, typeFlag);
	//printf("[%d] returning from sortcsv", pthread_self());
	return NULL;

}

/*
//this main is for testing purposes
int main(){
	pthread_t tid;
	char * filename = "stuff.csv";
	//char * filename = "CSVs4";
	char * path = ".";
	Thread_Args * sortcsv_args = malloc(sizeof(Thread_Args));
	sortcsv_args->path =		 malloc(sizeof(char)*(strlen(path) + 1));
	sortcsv_args->entry_name =   malloc(sizeof(char)*(strlen((char*)filename)+1));
	
	strcpy(sortcsv_args->path, path);
	strcpy(sortcsv_args->entry_name, filename);
	
	//char f[30];
	//char line[500];
	//strcpy(f, "./stuff.csv");
	//FILE * fp = fopen(f, "r");
	//fgets(line, 1000, fp);
	//printf("%s\n", line);
	
	category = "movie_title";
	sortcsv(sortcsv_args);
	
	//rowtype ** dataArray;
	//dataArray = (rowtype**)malloc(sizeof(rowtype*)*6000);
	
	return 0;
}
*/
