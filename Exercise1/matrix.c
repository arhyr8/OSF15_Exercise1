#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <errno.h>


#include "matrix.h"


#define MAX_CMD_COUNT 50

/*protected functions*/
void load_matrix (Matrix_t* m, unsigned int* data);

/* 
 * PURPOSE: instantiates a new matrix with the passed name, rows, cols 
 * INPUTS: 
 *	name the name of the matrix limited to 50 characters 
 *  rows the number of rows the matrix
 *  cols the number of cols the matrix
 * RETURN:
 *  If no errors occurred during instantiation then true
 *  else false for an error in the process.
 *
 **/

bool create_matrix (Matrix_t** new_matrix, const char* name, const unsigned int rows,
						const unsigned int cols) {

	// ERROR CHECK INCOMING PARAMETERS
	if((*new_matrix) != NULL){
		printf("Matrix already exists\n");
		return false;
	}
	if(name == NULL || strcmp(name, "\n") == 0){
		printf("There's no name for the matrix!\n");
		return false;
	}
	if(rows < 0 || rows > 4294967295){
		printf("Number of rows outside of valid range\n");
		return false;	
	}
	if(cols < 0 || cols > 4294967295){
		printf("Number of cols outside of valid range\n");
		return false;
	}
	
	*new_matrix = calloc(1,sizeof(Matrix_t));
	if (!(*new_matrix)) {
		return false;
	}
	(*new_matrix)->data = calloc(rows * cols,sizeof(unsigned int));
	if (!(*new_matrix)->data) {
		return false;
	}
	(*new_matrix)->rows = rows;
	(*new_matrix)->cols = cols;
	unsigned int len = strlen(name) + 1; 
	if (len > MATRIX_NAME_LEN) {
		return false;
	}
	strncpy((*new_matrix)->name,name,len);
	return true;

}

	// FUNCTION COMMENT
/***
* Purpose: Destroys the matrix and frees the memory at that location
* Input: A matrix
* Return: void
***/
void destroy_matrix (Matrix_t** m) {

	// ERROR CHECK INCOMING PARAMETERS
		if(*m == NULL){
		printf("matrix does not exist\n");
		return;
	}
	
	free((*m)->data);
	free(*m);
	*m = NULL;
}


	
	// FUNCTION COMMENT
/***
* Purpose: Check if the two given matrices are equal
* Input: 2 seperate matrices 
* Return: True/False
***/
bool equal_matrices (Matrix_t* a, Matrix_t* b) {

	// ERROR CHECK INCOMING PARAMETERS
	if(a == NULL){
		printf("First matrix is null\n");
		return false;
	}
	if(!a->data){
		printf("First matrix does not have any data\n");
		return false;
	}
	if(b == NULL){
		printf("Second matrix is null\n");
		return false;
	}
	if(!b->data){
		printf("Second matrix does not have any data\n");
		return false;
	}
	if (!a || !b || !a->data || !b->data) {
		return false;	
	}

	int result = memcmp(a->data,b->data, sizeof(unsigned int) * a->rows * a->cols);
	if (result == 0) {
		return true;
	}
	return false;
}

	// FUNCTION COMMENT
/***
* Purpose: Copy a matrix into a different empty matrix
* Input: The source matrix and an empty matrix
* Return: True/False
***/
bool duplicate_matrix (Matrix_t* src, Matrix_t* dest) {


	// ERROR CHECK INCOMING PARAMETERS
	if (!src){
		printf("Source matrix does not exist\n");
		return false;
	}
	if (!dest){
		printf("Destination matrix does not exist\n");
		return false;
	}
	if (!src->data){
		printf("The source matrix does not have any data\n");
		return false;
	}
	if (!dest->data){
		printf("Destination matrix does not have a place for the data\n");
		return false;
	}
	/*
	 * copy over data
	 */
	unsigned int bytesToCopy = sizeof(unsigned int) * src->rows * src->cols;
	memcpy(dest->data,src->data, bytesToCopy);	
	return equal_matrices (src,dest);
}

	// FUNCTION COMMENT
/***
* Purpose: Shift a matrix either to the left or right by a set number of positions
* Input: The matrix to shift,
*		 a direction to shift,
*		 how many positions to shift the matrix
* Return: True/False
***/
bool bitwise_shift_matrix (Matrix_t* a, char direction, unsigned int shift) {
	
	// ERROR CHECK INCOMING PARAMETERS
	if (!a) {
		printf("No matrix found");
		return false;
	}
	if(!a->data){
		printf("No data found in matrix\n");
		return false;
	}
	if(direction != 'l' || direction != 'r'){
		printf("Invalid shift direction!\n");
		return false;
	}
	if(shift < 0 || shift > 4294967295){
		printf("Invalid range for shift\n");
		return false;
	}	

	if (direction == 'l') {
		unsigned int i = 0;
		for (; i < a->rows; ++i) {
			unsigned int j = 0;
			for (; j < a->rows; ++j) {
				a->data[i * a->cols + j] = a->data[i * a->cols + j] << shift;
			}
		}

	}
	else {
		unsigned int i = 0;
		for (; i < a->rows; ++i) {
			unsigned int j = 0;
			for (; j < a->cols; ++j) {
				a->data[i * a->cols + j] = a->data[i * a->cols + j] >> shift;
			}
		}
	}
	
	return true;
}

	// FUNCTION COMMENT
/***
* Purpose: Add to seperate matrices together and put the result into a different matrix
* Input: Two matrices with an equal nubmer of rows and cols,
		 an empty matrix to put the result of the addition of the first two matrices
* Return: True/False
***/
bool add_matrices (Matrix_t* a, Matrix_t* b, Matrix_t* c) {

	// ERROR CHECK INCOMING PARAMETERS
	if(!a){
		printf("Matrix 'a' doesn't exist\n");
		return false;
	}
	if(!b){
		printf("Matrix 'b' doesnt't exist\n");
		return false;
	}
	if(!c){
		printf("Matric 'c' doesn't exist\n");
		return false;
	}
	if(!a->data){
		printf("No data found in matrix 'a'\n");
		return false;
	}
	if(!b->data){
		printf("No data found in matrix 'b'\n");
		return false;
	}	
	if(!c->data){
		printf("No room for data in matrix 'c'\n");
		return false;
	}
	
	if (a->rows != b->rows && a->cols != b->cols) {
		return false;
	}

	for (int i = 0; i < a->rows; ++i) {
		for (int j = 0; j < b->cols; ++j) {
			c->data[i * a->cols +j] = a->data[i * a->cols + j] + b->data[i * a->cols + j];
		}
	}
	return true;
}

	// FUNCTION COMMENT
/***
* Purpose: Prints a matrix to the screen
* Input: a matrix to display
* Return: void
***/
void display_matrix (Matrix_t* m) {
	
	// ERROR CHECK INCOMING PARAMETERS
	if (!m){
		printf("Matrix not found\n");
		return;
	}
	if(!m->data){
		printf("No data found in matrix\n");
		return;
	}
	printf("\nMatrix Contents (%s):\n", m->name);
	printf("DIM = (%u,%u)\n", m->rows, m->cols);
	for (int i = 0; i < m->rows; ++i) {
		for (int j = 0; j < m->cols; ++j) {
			printf("%u ", m->data[i * m->cols + j]);
		}
		printf("\n");
	}
	printf("\n");

}

	// FUNCTION COMMENT
/***
* Purpose: Read a matrix from a file
* Inputs: a file on the system,
*		  an empty matrix
* Return: True/False
***/
bool read_matrix (const char* matrix_input_filename, Matrix_t** m) {
	
	// ERROR CHECK INCOMING PARAMETERS
	if (!m){
		printf("Matrix does not exist\n");
		return false;
	}
	if(!(*m)->data){
		printf("Not enough space in matrix to store data\n");
		return false;
	}

	int fd = open(matrix_input_filename,O_RDONLY);
	if (fd < 0) {
		printf("FAILED TO OPEN FOR READING\n");
		if (errno == EACCES ) {
			perror("DO NOT HAVE ACCESS TO FILE\n");
		}
		else if (errno == EADDRINUSE ){
			perror("FILE ALREADY IN USE\n");
		}
		else if (errno == EBADF) {
			perror("BAD FILE DESCRIPTOR\n");	
		}
		else if (errno == EEXIST) {
			perror("FILE EXIST\n");
		}
		return false;
	}

	/*read the wrote dimensions and name length*/
	unsigned int name_len = 0;
	unsigned int rows = 0;
	unsigned int cols = 0;
	
	if (read(fd,&name_len,sizeof(unsigned int)) != sizeof(unsigned int)) {
		printf("FAILED TO READING FILE\n");
		if (errno == EACCES ) {
			perror("DO NOT HAVE ACCESS TO FILE\n");
		}
		else if (errno == EADDRINUSE ){
			perror("FILE ALREADY IN USE\n");
		}
		else if (errno == EBADF) {
			perror("BAD FILE DESCRIPTOR\n");	
		}
		else if (errno == EEXIST) {
			perror("FILE EXIST\n");
		}
		return false;
	}
	char name_buffer[50];
	if (read (fd,name_buffer,sizeof(char) * name_len) != sizeof(char) * name_len) {
		printf("FAILED TO READ MATRIX NAME\n");
		if (errno == EACCES ) {
			perror("DO NOT HAVE ACCESS TO FILE\n");
		}
		else if (errno == EADDRINUSE ){
			perror("FILE ALREADY IN USE\n");
		}
		else if (errno == EBADF) {
			perror("BAD FILE DESCRIPTOR\n");	
		}
		else if (errno == EEXIST) {
			perror("FILE EXIST\n");
		}

		return false;	
	}

	if (read (fd,&rows, sizeof(unsigned int)) != sizeof(unsigned int)) {
		printf("FAILED TO READ MATRIX ROW SIZE\n");
		if (errno == EACCES ) {
			perror("DO NOT HAVE ACCESS TO FILE\n");
		}
		else if (errno == EADDRINUSE ){
			perror("FILE ALREADY IN USE\n");
		}
		else if (errno == EBADF) {
			perror("BAD FILE DESCRIPTOR\n");	
		}
		else if (errno == EEXIST) {
			perror("FILE EXIST\n");
		}

		return false;
	}

	if (read(fd,&cols,sizeof(unsigned int)) != sizeof(unsigned int)) {
		printf("FAILED TO READ MATRIX COLUMN SIZE\n");
		if (errno == EACCES ) {
			perror("DO NOT HAVE ACCESS TO FILE\n");
		}
		else if (errno == EADDRINUSE ){
			perror("FILE ALREADY IN USE\n");
		}
		else if (errno == EBADF) {
			perror("BAD FILE DESCRIPTOR\n");	
		}
		else if (errno == EEXIST) {
			perror("FILE EXIST\n");
		}

		return false;
	}

	unsigned int numberOfDataBytes = rows * cols * sizeof(unsigned int);
	unsigned int *data = calloc(rows * cols, sizeof(unsigned int));
	if (read(fd,data,numberOfDataBytes) != numberOfDataBytes) {
		printf("FAILED TO READ MATRIX DATA\n");
		if (errno == EACCES ) {
			perror("DO NOT HAVE ACCESS TO FILE\n");
		}
		else if (errno == EADDRINUSE ){
			perror("FILE ALREADY IN USE\n");
		}
		else if (errno == EBADF) {
			perror("BAD FILE DESCRIPTOR\n");	
		}
		else if (errno == EEXIST) {
			perror("FILE EXIST\n");
		}

		return false;	
	}

	if (!create_matrix(m,name_buffer,rows,cols)) {
		return false;
	}

	load_matrix(*m,data);

	if (close(fd)) {
		return false;

	}
	return true;
}

	// FUNCTION COMMENT
/***
* Purpose: Writes a matrix to a file
* Input: The desired filepath,
*		 matrix to write to the file
* Return: True/False
***/
bool write_matrix (const char* matrix_output_filename, Matrix_t* m) {
	
	// ERROR CHECK INCOMING PARAMETERS
	if (!m){
		printf("Matrix does not exist\n");
		return false;
	}
	if(!(*m)->data){
		printf("Not enough space in matrix to store data\n");
		return false;
	}

	int fd = open (matrix_output_filename, O_CREAT | O_RDWR | O_TRUNC, 0644);
	/* ERROR HANDLING USING errorno*/
	if (fd < 0) {
		printf("FAILED TO CREATE/OPEN FILE FOR WRITING\n");
		if (errno == EACCES ) {
			perror("DO NOT HAVE ACCESS TO FILE\n");
		}
		else if (errno == EADDRINUSE ){
			perror("FILE ALREADY IN USE\n");
		}
		else if (errno == EBADF) {
			perror("BAD FILE DESCRIPTOR\n");	
		}
		else if (errno == EEXIST) {
			perror("FILE EXISTS\n");
		}
		return false;
	}
	/* Calculate the needed buffer for our matrix */
	unsigned int name_len = strlen(m->name) + 1;
	unsigned int numberOfBytes = sizeof(unsigned int) + (sizeof(unsigned int)  * 2) + name_len + sizeof(unsigned int) * m->rows * m->cols + 1;
	/* Allocate the output_buffer in bytes
	 * IMPORTANT TO UNDERSTAND THIS WAY OF MOVING MEMORY
	 */
	unsigned char* output_buffer = calloc(numberOfBytes,sizeof(unsigned char));
	unsigned int offset = 0;
	memcpy(&output_buffer[offset], &name_len, sizeof(unsigned int)); // IMPORTANT C FUNCTION TO KNOW
	offset += sizeof(unsigned int);	
	memcpy(&output_buffer[offset], m->name,name_len);
	offset += name_len;
	memcpy(&output_buffer[offset],&m->rows,sizeof(unsigned int));
	offset += sizeof(unsigned int);
	memcpy(&output_buffer[offset],&m->cols,sizeof(unsigned int));
	offset += sizeof(unsigned int);
	memcpy (&output_buffer[offset],m->data,m->rows * m->cols * sizeof(unsigned int));
	offset += (m->rows * m->cols * sizeof(unsigned int));
	output_buffer[numberOfBytes - 1] = EOF;

	if (write(fd,output_buffer,numberOfBytes) != numberOfBytes) {
		printf("FAILED TO WRITE MATRIX TO FILE\n");
		if (errno == EACCES ) {
			perror("DO NOT HAVE ACCESS TO FILE\n");
		}
		else if (errno == EADDRINUSE ){
			perror("FILE ALREADY IN USE\n");
		}
		else if (errno == EBADF) {
			perror("BAD FILE DESCRIPTOR\n");	
		}
		else if (errno == EEXIST) {
			perror("FILE EXIST\n");
		}
		return false;
	}
	
	if (close(fd)) {
		return false;
	}
	free(output_buffer);

	return true;
}

	// FUNCTION COMMENT
/***
* Purpose: Fills a matrix with random values based upon a given range
* Input: An empty matrix,
*		 start point of range,
*		 end point of range
* Return: True/False
***/
bool random_matrix(Matrix_t* m, unsigned int start_range, unsigned int end_range) {
	
	// ERROR CHECK INCOMING PARAMETERS
	if(!m)	{
		printf("No matrix found\n");
		return false;
	}
	if(!m->data){
		printf("Not enough space in matrix\n");
		return false;
	}
	if(start_range < 0 || start_range > 4294967295){
		printf("Start range is invalid\n");
		return false;
	}
	if(end_range < 0 || end_range > 4294967295){
		printf("End range is invalid\n");
		return false;
	}
	
	for (unsigned int i = 0; i < m->rows; ++i) {
		for (unsigned int j = 0; j < m->cols; ++j) {
			m->data[i * m->cols + j] = rand() % end_range + start_range;
		}
	}
	return true;
}

/*Protected Functions in C*/

	// FUNCTION COMMENT
/***
* Purpose: Fill a matrix with data
* Input: An empty matrix,
*		 the data to be loaded into the matrix
* Return: void
***/

void load_matrix (Matrix_t* m, unsigned int* data) {
	
	// ERROR CHECK INCOMING PARAMETERS
	if(!m){
		printf("Matrix not found\n");
		return;
	}
	if(!m->data){
		printf("Not enough space in matrix\n");
		return;
	}
	if(!data){
		printf("No data found\n");
		return;
	}
	
	memcpy(m->data,data,m->rows * m->cols * sizeof(unsigned int));
}

	// FUNCTION COMMENT
/***
* Purpose: Add a matrix to the list of matrices. Overwrites old
*		   matrix if a matrix already exists in the desired location
* Input: The list of matrices,
* 		 the new matrix,
* 		 the number of matrices in the list
* Return: The positon of the new matrix in the list
***/
unsigned int add_matrix_to_array (Matrix_t** mats, Matrix_t* new_matrix, unsigned int num_mats) {
	
	// ERROR CHECK INCOMING PARAMETERS
	if(!mats){
		printf("No list found\n");
		return -1;
	}
	if(num_mats > 10 || num_mats < 0 ){
		printf("Invalid number of matrices\n");
		return -1;
	}
	if(!new_matrix){
		printf("No matrix found");
		return -1;
	}
	if(!new_matrix->data){
		printf("No data found in new matrix\n");
		return -1;
	}
	
	static long int current_position = 0;
	const long int pos = current_position % num_mats;
	if ( mats[pos] ) {
		destroy_matrix(&mats[pos]);
	} 
	mats[pos] = new_matrix;
	current_position++;
	return pos;
}
