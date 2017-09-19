// Multi-threaded Sudoku solver by Eugene Rivera
// complie with -lpthread and redirect the input to use input.txt file
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <ctype.h>

typedef struct gridCheck{
  // This struct will be used to store the Sudoku board values
  // Along with id values for rows/columns/subgrids
  // isValid is will be set to true or false based on valid or invalid input
  int (* matrix)[9];
  int row_id;
  int col_id;
  int subGrid_id;
  int isValid;
}gridCheck;

void* isValidSudoku(void* input){
  // redefine our pointer parameter
  struct gridCheck* ptr = (struct gridCheck*)input;
  // have seen array will be used to keep track of each number we see
  // using array of size 10, match the values from the table with the index
  // ignoring the first element.
  int haveSeen[10] = {0};
  // check what to validate, row, columns or subgrid.
  if (ptr->row_id >= 0 && ptr->subGrid_id == -1){
  // have seen array will be used to keep track of each number we see
  // using array of size 10, match the values from the table with the index
  // ignoring the first element.
    for (int i = 0; i < 9; i++)
      haveSeen[ptr->matrix[ptr->row_id][i]] = 1;
  }else if (ptr->col_id >= 0 && ptr->subGrid_id == -1){
    // same thing as above but here we are checking columns
    for (int i = 0; i < 9; i++)
      haveSeen[ptr->matrix[i][ptr->col_id]] = 1;
  }else{
    // if we are not checking rows or columns, then we must be evaluating subgrids
    for (int i = ptr->row_id; i < (ptr->row_id + 3); i++){
      for (int j = ptr->col_id; j < (ptr->col_id + 3); j++){
        haveSeen[ptr->matrix[i][j]] = 1;
      }
    }
  }
  // now we start at index 1 and go through the array checking if we have seen
  // each number
  for (int i = 1; i < 10; i++)
    if (haveSeen[i] != 1){
    // if any element of have seen (other than the first) is not a 1, then it
    // was not in the table and the input is invalid
      ptr->isValid = 0;
      return (void *) 0;
    }
  // if we have it to this point we have seen each number 1-9 once and the input
  // is considered valid.
  ptr->isValid = 1;
  return (void *) 0;
}

int main (int argc, char* argv[]) {
  // reads in user input and stores it in a 2D array.
  int input_matrix[9][9];
  for (int i = 0; i < 9; i++){
    for (int j = 0; j < 9; j++){
        scanf("%d", &(input_matrix[i][j]));
        // used print statment to make sure input was being read and stored
        //printf("%d ", input_matrix[i][j]);
    }
  }

  long thread_id;
  // set up arrays to hold our threads and structs
  pthread_t* thread_handles;
  int thread_count = 27;
  thread_handles = malloc(thread_count * sizeof(pthread_t));
  // array to hold each instance of the structs we create
  gridCheck * arrayOfStructs[27];
  // The following variables will be used by the isValidSudoku function to
  // identify which rows/columns/subgrids to check for valid input
  // row and column ids are initialize to 0 to properly index the input matrix
  // subgrid_id is initialized to 1 to properly identify each subgrid
  int row_id = 0;
  int col_id = 0;
  int subGrid_rowId = 0;
  int subGrid_colId = 0;
  int subGrid_id = 1;
  // threads to check each rows, colums, and subgrids
  for(thread_id = 0; thread_id < 27; thread_id++){
    // create threads and initialize their member variables
    struct gridCheck * struct_ptr = (gridCheck *) malloc(sizeof(gridCheck));
    // initialize struct variables to -1 till they are properly assigend id values
    struct_ptr->row_id = -1;
    struct_ptr->col_id = -1;
    struct_ptr->subGrid_id = -1;
    struct_ptr->matrix = input_matrix;
    if (thread_id < 9){
      // this if statment is used to check rows 1-9
      // we set the id, store the pointer to the struct, increment the id and create a new thread
      struct_ptr->row_id = row_id;
      arrayOfStructs[thread_id] = struct_ptr;
      row_id++;
      pthread_create(&thread_handles[thread_id], NULL, isValidSudoku, (void*) struct_ptr);
    } else if (thread_id < 18){
      // same as if statement above but now we check colums 1-9
      struct_ptr->col_id = col_id;
      arrayOfStructs[thread_id] = struct_ptr;
      col_id++;
      pthread_create(&thread_handles[thread_id], NULL, isValidSudoku, (void*) struct_ptr);
    }else{
      // else statement is used to check subgrids 1-9
      // we reset the col_id value when it reachs 9 and then  add 3 to row_id
      if (subGrid_colId == 9){
        subGrid_rowId += 3;
        subGrid_colId = 0;
      }
      struct_ptr->subGrid_id = subGrid_id;
      struct_ptr->row_id = subGrid_rowId;
      struct_ptr->col_id = subGrid_colId;
      arrayOfStructs[thread_id] = struct_ptr;
      subGrid_colId += 3;
      subGrid_id++;
      pthread_create(&thread_handles[thread_id], NULL, isValidSudoku, (void*) struct_ptr);
    }
  }

  // for loop to rejoin threads
  int valid = 1;
  for (thread_id = 0; thread_id < 27; thread_id++){
    pthread_join(thread_handles[thread_id], NULL);
    //printf("thread_id: %d\n", thread_id);
    if (arrayOfStructs[thread_id]->isValid == 0){
      valid = 0;
      // if the input is invalid we print error statements identifying where the invalid input is
      if (thread_id < 9){
        // threads 0-8 were assigned to check rows 1-9
        printf("Row %d doesn't have the required values.\n", (arrayOfStructs[thread_id]->row_id +1));
      }else if (thread_id < 18){
        printf("Column %d doesn't have the required values.\n", (arrayOfStructs[thread_id]->col_id+1));
      }
      else{
        // threads 18-26 used to check subgrids
        switch (arrayOfStructs[thread_id]->subGrid_id) {
          case 1: // subgrid 1
            printf("The top left subgrid doesn't have the required values.\n");
            break;
          case 2: // subgrid 2
            printf("The top middle subgrid doesn't have the required values.\n");
            break;
          case 3: // subgrid 3
            printf("The top right subgrid doesn't have the required values.\n");
            break;
          case 4: // subgrid 4
            printf("The middle left subgrid doesn't have the required values.\n");
            break;
          case 5: // subgrid 5
            printf("The middle subgrid doesn't have the required values.\n");
            break;
          case 6: // subgrid 6
            printf("The middle right subgrid doesn't have the required values.\n");
            break;
          case 7: // subgrid 7
            printf("The bottom left subgrid doesn't have the required values.\n");
            break;
          case 8: // subgrid 8
            printf("The bottom middle subgrid doesn't have the required values.\n");
            break;
          default: // subgrid 9
            printf("The bottom right subgrid doesn't have the required values.\n");
        }
      }
    }
  }
  if (valid)
    printf("The input is a valid Sudoku.\n");
  else
    printf("The input is not a valid Sudoku.\n");
  free(thread_handles);
  return 0;
}
