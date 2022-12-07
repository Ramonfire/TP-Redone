#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>

// Fonction pour supprimer un élément d'un tableau
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
        - 0: Traverse()
        - 1: Retour()
*/
void sendMessage(int id, int dest, int count, MPI_Comm comm)
{
    int buffer[2] = {id, count};
    MPI_Send(buffer, 2, MPI_INT, dest, 0, comm);
}

int main(int argc, char *argv[])
{
    int wsize, pi;
    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &wsize);

    // Création du graphe

    int index[6] = {3, 4, 7, 11, 13, 16};
    int edges[16] = {1, 2, 3, 0, 0, 3, 5, 0, 2, 4, 5, 3, 5, 2, 3, 4};

    MPI_Comm graph_comm;
    MPI_Graph_create(MPI_COMM_WORLD, wsize, index, edges, 1, &graph_comm);
    MPI_Comm_rank(graph_comm, &pi);

    // Variables locales

    int message;
    int id;
    int suivant;
    int tailles[6];
    int countfils = 0;
    int fils[6];
    int rcv[2];
    int count;
    int maxNeighbors = 4;
    int neighborsCount;
    int neighbors[maxNeighbors]; // Liste des voisins
    MPI_Graph_neighbors_count(graph_comm, pi, &neighborsCount);
    MPI_Graph_neighbors(graph_comm, pi, maxNeighbors, neighbors);

    int i;
    int visited = 0; // Si ce noeud a était visité ou non
    int pred = -1;   // Le prédécesseur de ce noeud
    MPI_Status status;

    // A la récéption de INIT() par le processus 0

    if (pi == 0)
    {
    count = 1;
        visited = 1;
        printf("Node 0: INIT.\n");
        for (i = 0; i < neighborsCount; i++)
        {
            sendMessage(0, neighbors[i], 0, graph_comm);
        }
    }

    while (1)
    {
        MPI_Recv(&rcv, 2, MPI_INT, MPI_ANY_SOURCE, 0, graph_comm, &status);
        int pj = status.MPI_SOURCE;
        
	message = rcv[0];
	//printf("message %d \n", message); 
	count += rcv[1];
        // A la reception de Traverse() par pi depuis pj

        if (message == 0)
        {
            removeElementFromArray(neighbors, &neighborsCount, pj);
            if (!visited)
            {
                visited = 1;
                pred = pj;
                printf("Node %d: My predecessor is %d\n", pi, pred);
                if (neighborsCount == 0)
                {
                    sendMessage(1, pred, 1, graph_comm);
                    break;

                }
                else
                {
                    for (i = 0; i
                     < neighborsCount; i++)
                    {
                        sendMessage(0, neighbors[i], 0, graph_comm);
                    }
                }
            }
            else
            {
                sendMessage(1, pj, 0, graph_comm);
            }
        }

        // A la reception de Retour() par pi depuis pj

        if (message == 1)
        {
            removeElementFromArray(neighbors, &neighborsCount, pj);
            
            if (rcv[1] > 0){
            	tailles[pj] = rcv[1];
            	fils[countfils] = pj;
            	countfils++;
            }

            if (neighborsCount == 0)
            {
                if (pred == -1)
                {
                    printf("Node %d: END, node count: %d.\n", pi, count);
                    id = 0;
                    suivant = id + 1;
                    for (int k = 0; k < countfils; k++){
                    	sendMessage(2, fils[k], suivant, graph_comm);
                    	suivant = suivant + tailles[fils[k]];
                    }
                }
                else
                {
                    sendMessage(1, pred, count + 1, graph_comm);
                }
                break;
            }
        }
        
        
    }
    while (1){
    	MPI_Recv(&rcv, 2, MPI_INT, MPI_ANY_SOURCE, 0, graph_comm, &status);
        int pj = status.MPI_SOURCE;
        
	message = rcv[0];
	//printf("message %d \n", message); 
	count += rcv[1];
        // A la reception de Traverse() par pi depuis pj
    	if (message == 2){
        	//printf("AA");
        	id = rcv[1];
        	if (countfils > 0){
        		suivant = rcv[1] + 1;
        		for (int k = 0; k < countfils; k++){
        			sendMessage(2, fils[k], suivant, graph_comm);
                    		suivant = suivant + tailles[fils[k]];
        		}
        	}else{
        		sendMessage(3, pred, 0, graph_comm);
        		printf("\nnode %d id: %d\n", pi, id);
        		break;
        	}
        }
        
        if (message == 3){
        	removeElementFromArray(fils, &countfils, pj);
        	if (countfils == 0){
        		if (pred == -1){
        			printf("\nnode %d id: %d\n", pi, id);
        			printf("FIN\n");
        		}else{
        			sendMessage(3, pred, 0, graph_comm);
        			printf("\nnode %d id: %d\n", pi, id);
        		}
        		break;
        	}
        }
    }

    MPI_Finalize();
    return 0;
}
