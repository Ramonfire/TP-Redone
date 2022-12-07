#include <stdio.h>
#include <mpi.h>
int main(int argc, char* argv[]){
int rank, size;// Initialisation de la biblioth‘que
 MPI_Init(&argc, &argv);// Quel est mon rang ?
 MPI_Comm_rank(MPI_COMM_WORLD, &rank);
 MPI_Comm_size(MPI_COMM_WORLD, &size);
 if( rank%2 == 0){
  printf("Process avec le rang %d est pair\n", rank);
 }else{
  printf("Process avec le rang %d est impair\n", rank);
 }
 MPI_Finalize();
 return 0;
 }
