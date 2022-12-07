// Propagation contrôlée

#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>

// Fonction pour supprimer un élément d'un tableau
void  addElementToArray(int array[], int *length, int element){
       array[*length] = element;
       (*length)++;
}


void removeElementFromArray(int array[], int *length, int element)
{

    int i, pos = -1;
    for (i = 0; i < *length; i++)
    {
        if (array[i] == element)
        {
            pos = i;
            break;
        }
    }

    // If the element was found
    if (pos != -1)
    {
        for (i = pos; i < *length - 1; i++)
        {
            array[i] = array[i + 1];
        }

        (*length)--;
    }
}

/*
    Messages utilisés (chacun son id):
        - 0: Election()
        - 1: Elu()
*/
void sendMessage(int id,int data, int dest, MPI_Comm comm)
{
   int buffer[2];
    buffer[0] = id;
    buffer[1] = data;
    MPI_Send(buffer, 2, MPI_INT, dest, 0, comm);
 
}

int main(int argc, char *argv[])
{
    int wsize, pi;
    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &wsize);
 
  int index[5] = {2, 4, 6, 8, 10};
   int edges[] = {1, 2,0,4, 0, 3, 2, 4, 1, 3}; 
  
  
    // Création du graphe
    MPI_Comm graph_comm;
    MPI_Graph_create(MPI_COMM_WORLD, wsize, index, edges, 1, &graph_comm);
    MPI_Comm_rank(graph_comm, &pi);

    // Variables locales
    int buffer[2];
    int message;
    int maxNeighbors = 2;
    int neighborsCount;
    int neighbors[maxNeighbors]; // Liste des voisins
    MPI_Graph_neighbors_count(graph_comm, pi, &neighborsCount);
    MPI_Graph_neighbors(graph_comm, pi, maxNeighbors, neighbors);
    int c = 1;
    int i;
    int elu = pi;
    int fils[3];
    int filsCount;
    int visited = 0; // Si ce noeud a était visité ou non
    int pred = -1;   // Le prédécesseur de ce noeud
    MPI_Status status;

    // A la récéption de INIT() par le processus 0

    if (pi == 0)
    {
        visited = 1;
        printf("Node 0: INIT.\n");
       
            sendMessage(0, pi ,neighbors[1], graph_comm);
        
    }

    while (1)
    {
        MPI_Recv(buffer, 2, MPI_INT, MPI_ANY_SOURCE, 0, graph_comm, &status);
        int pj = status.MPI_SOURCE;

        // A la reception Election par pi depuis pj

        if (buffer[0] == 0)
        {

            if (pi < buffer[1])
            {
                    visited = 1;
                    sendMessage(0, buffer[1] ,neighbors[1], graph_comm);

             } 
             else if (pi > buffer[1] && (!visited))
                {   
                   visited = 1;
                    sendMessage(0, pi ,neighbors[1], graph_comm);

                  
                }
                else if (pi == buffer[1] || (visited))
                {	
                    sendMessage(1,  buffer[1] ,neighbors[1], graph_comm);
                    
                    
                }

         
        }

        // A la reception de Elu() par pi depuis pj

        if (buffer[0] == 1)
        {
         if ((visited))
            {
                    visited = 0;
                    elu = buffer[1];
                    sendMessage(1, elu ,neighbors[1], graph_comm);

             } 
             else 
                {   
                    printf("cordinateur %d\n", elu);
                    break;
                }
               
        }
        
}
    MPI_Finalize();
    return 0;
}
