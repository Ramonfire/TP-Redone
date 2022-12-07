#include <stdio.h>
#include <mpi.h>
int main(int argc, char* argv[]){
int rank, size;// Initialisation de la biblioth‘que
 MPI_Init(&argc, &argv);// Quel est mon rang ?
 MPI_Comm_rank(MPI_COMM_WORLD, &rank);// Combien de processus dans l?application ?
 MPI_Comm_size(MPI_COMM_WORLD, &size);
 printf("Hello, world, I am %d of %d\n", rank, size);// touts les neouds vous disent bonjour
 MPI_Finalize();
 return 0;
 }
