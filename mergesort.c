/* SYSTEMS PROGRAMMING PROJECT 3
 * Authors: Alex Vargas
 * Eric Zimmerman
 */

#include "sorter_thread.h"

//trim takes a string and removes any trailing or leading whitespace
char * trim(char *str){
	size_t len = 0;
	char *fptr = str; //front pointer
	char *eptr = NULL;//end pointer

	if( str == NULL ) { 
		return NULL; 
	}
	if( str[0] == '\0' ) { 
		return str; 
	}
	
	len = strlen(str);
	//brings endp to null byte
	eptr = str + len;

	/* This moves the front and back pointers to the address of the first 
	 * non-whitespace chars from each end.
	 */
	while(isspace((unsigned char)*fptr) || *fptr == '"' || (unsigned char)*fptr > 127) { 
		fptr++; 
	}
	if( eptr != fptr ){
		eptr--;
		while( (isspace((unsigned char) *(eptr)) || *eptr == '"' || (unsigned char)*eptr > 127) && eptr != fptr ) {
			
			eptr--;
		}
	}
	//this puts null byte at end of string to eliminate traling whitespace
	//printf("*eptr= [%c]", *eptr );
	if( str + len - 1 != eptr ){
		//printf("*(eptr+1) = %c", *(eptr+1) );
		*(eptr + 1) = '\0';
	}
	else if( fptr != str &&  eptr == fptr ){
		*str = '\0';
	}
	/* Shift the string so that it starts at str.This is so if it's dynamically
	 * allocated, it can still be freed on the returned pointer 
	 * eptr means the front of the string buffer now.
	 */
	eptr = str;
	if( fptr != str ) {
		/*once fptr reaches the null terminator
		 * we have successfully shifted the string
		 */
		while( *fptr ) { 
			*eptr = *fptr; 
			eptr++;
			fptr++;
		}
		*eptr = '\0';
	}
	return str;
}


void merge( int left, int mid, int right, rowtype** arr, char typeFlag ){
	int sizeL =(mid-left)+1;
	int sizeR = right-mid;

	rowtype* L[sizeL];
	rowtype* R[sizeR];

	int i;
	for(i=0;i<sizeL;i++)
		L[i] = arr[left+i];
	for (i=0; i<sizeR;i++)
		R[i] = arr[mid+1+i];

	int l_count=0;
	int r_count=0;
	int j = 0;
	while(l_count<sizeL && r_count<sizeR)
	{
		if( typeFlag == 's' || typeFlag == 'S'){
			int result = strcmp(L[l_count]->key, R[r_count]->key);
			if(result <=0)
			{
				*(arr+left+j) = L[l_count];
				l_count++;
			}
			else
			{
				*(arr+left+j)=R[r_count];
				r_count++;
			}
			j++;
		}

		else if( typeFlag == 'n' || typeFlag == 'N'){
			double  result = (atof((L[l_count])->key) - atof((R[r_count])->key));
			if(result <=0.0)
			{
				*(arr+left+j) = L[l_count];
				l_count++;
			}
			else
			{
				*(arr+left+j)= R[r_count];
				r_count++;
			}
			j++;
		}
	}

	if(l_count==sizeL)
	{
		while(r_count<sizeR)
		{
			*(arr+left+j) = R[r_count];
			r_count++;
			j++;
		}
	}
	if(r_count==sizeR)
	{
		while(l_count<sizeL)
		{
			*(arr+left+j) = L[l_count];
			l_count++;
			j++;
		}
	}
}

void sort( int left, int right, rowtype** arr, char typeFlag)
{
	if (right>left)
	{	
		int mid = (right+left)/2;

		sort(left, mid, arr, typeFlag);
		sort(mid+1, right, arr, typeFlag);

		merge( left, mid, right, arr, typeFlag);
	}
}

/*
//the main here is just for testing purposes
int main(){

	//char lines[1000];
	//printf("input\n");
	//fgets(lines,1000,stdin);
	//printf("B4: [%s]\n",lines);
	//printf("AF: [%s]\n",trim(lines));
	
	
	char * str = (char*)malloc(200);
	char * strs[50];
	char * temp;
	int i = 0;
	strcpy(str,"where,,dank,,be,");
	const char c = ','; 
	temp = strsep(&str, &c);
	do{
		strs[i] = malloc(64*sizeof(char));
		strcpy(strs[i], temp);
		printf("temp: %s\n", temp);
		i++;
	}while(temp = (strsep(&str,&c)) );
	int len = i-1;
	for (i=0;i<=len;i++){
		printf("%s\n", strs[i]);
	}
	
	int high = 16;
	int low = 0;
	i = 0;
	//
	
	char *testStrings[] =
    {
            "nothing to trim",
            "    trim the front",
            "trim the back     ",
            " trim one char front and back ",
            " trim one char front",
            "trim one char back ",
            "                   ",
            " ",
            
            "aaa",
            "Ccc",
            "BBB",
            "bbb",
            "ccc",
            "AAA",
            "dDD",
            "Ddd",
            "ddD",
           
    };  
    
	rowtype * recordArr[high+1];
	for (i=low; i<=high; i++){
		//printf("about to malloc [%d] wholerow\n", i);
		recordArr[i] = (rowtype*)malloc(sizeof(rowtype)); // ************** //
		recordArr[i]->wholerow = (char*)malloc( 64 * sizeof(char) );
		recordArr[i]->key = 		(char*)malloc( 64 * sizeof(char) );
		strcpy(recordArr[i]->wholerow, testStrings[i]);
		strcpy(recordArr[i]->key, testStrings[i]);
		trim(recordArr[i]->key);
		//printf("input wholerow [%d]\n",i);
		//scanf("%s", recordArr[i].wholerow);
		//printf("input key [%d]\n",i);
		//scanf("%s", recordArr[i].key);
		printf("[%d]wholerow: [%s], key: [%s]\n",i,recordArr[i]->wholerow,recordArr[i]->key);
		
	}
	printf("BEFORE\n");
	for(i=low; i<=high; i++){
		printf("[%d]wholerow: [%s], key: [%s]\n",i,recordArr[i]->wholerow,recordArr[i]->key);
	}
	//rowtype * ptr1 = &recordArr[0];
	//rowtype ** ptr2 = &ptr1;
	sort(low, high, recordArr,'s');
	
	printf("\nAFTER\n");
	for(i=low;i<=high;i++){
		printf("[%d]wholerow: [%s], key: [%s]\n",i,recordArr[i]->wholerow,recordArr[i]->key);
		free(recordArr[i]->wholerow);
		free(recordArr[i]->key);
	}
	
	return 0;
}
*/
