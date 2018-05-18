#include <iostream>
#include <ctime>
#include <cstdlib>
#include <unistd.h> // Funciones para poder llamar sleep()
#include "mpi.h"

using namespace std;

// Atributos, etiquetas
#define soltar     0
#define coger      1

// Metodos
void Filosofo(int id, int nprocesos); // Codigo proc. Filosofo
void Tenedor (int id, int nprocesos); // Codigo proc. Tenedor

// Punto de entrada al programa
int main(int argc, char** argv)
{
    // Inicialización de open-mpi
    int rank,size;
    srand(time(0));
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    // Controlamos que estemos llamando correctamente al programa
    if (size != 10)
    {
        if (rank == 0) // Solo mostramos el mensaje 1 vez
            cout << "El numero de procesos debe ser 10" << endl;
        MPI_Finalize();
        return 0;
    }

    if (rank%2 == 0)
        Filosofo(rank,size); // los pares son filosofos
    else
        Tenedor(rank,size); // los impares son tenedores


    // Finalización de open-mpi
    MPI_Finalize();

    // Fin del programa
    return 0;
}

void Filosofo(int id, int nprocesos)
{
    int izq = (id+1) % nprocesos;
    int der = (id+nprocesos-1) % nprocesos;
    while (true)
    {
        // El primer filosofo deberá de coger los tenedores al revés para que no haya interbloqueo
        if (id == 0)
        {
            // solicita tenedor derecho
            cout << "Filosofo " << id << " coge tenedor der. " << der << endl;
            MPI_Ssend(NULL, 0, MPI_INT, der, coger, MPI_COMM_WORLD);

            // solicita tenedor izquierdo
            cout << "Filosofo " << id << " solicita tenedor izq. " << izq << endl;
            MPI_Ssend(NULL, 0, MPI_INT, izq, coger, MPI_COMM_WORLD);
        }
        else
        {
            // solicita tenedor izquierdo
            cout << "Filosofo " << id << " solicita tenedor izq. " << izq << endl;
            MPI_Ssend(NULL, 0, MPI_INT, izq, coger, MPI_COMM_WORLD);

            // solicita tenedor derecho
            cout << "Filosofo " << id << " coge tenedor der. " << der << endl;
            MPI_Ssend(NULL, 0, MPI_INT, der, coger, MPI_COMM_WORLD);
        }


        cout << "Filosofo " << id << " COMIENDO" << BLACK << endl;
        sleep((rand()%3)+1); // comiendo

        // suelta el tenedor izquierdo
        cout << "Filosofo "<<id<< " suelta tenedor izq. " << izq << endl;
        MPI_Ssend(NULL, 0, MPI_INT, izq, soltar, MPI_COMM_WORLD);


        // suelta el tenedor derecho
        cout << "Filosofo " << id << " suelta tenedor der. " << der << endl;
        MPI_Ssend(NULL, 0, MPI_INT, der, soltar, MPI_COMM_WORLD);


        cout << "Filosofo " << id << " PENSANDO" << endl;
        sleep((rand()%3)+1); // pensando
    }
}

void Tenedor(int id, int nprocesos)
{
    int buf, Filo;
    MPI_Status status;
    while (true)
    {
        // Espera un peticion desde cualquier filosofo vecino ...
        MPI_Recv(&buf, 1, MPI_INT, MPI_ANY_SOURCE, coger, MPI_COMM_WORLD, &status);

        // Recibe la peticion del filosofo ...
        Filo=status.MPI_SOURCE; // Obtiene el rank del filosofo
        cout << "Ten. " << id << " recibe petic. de " << Filo << endl;

        // Espera a que el filosofo suelte el tenedor...
        MPI_Recv(&Filo, 1, MPI_INT, Filo, soltar, MPI_COMM_WORLD, &status);
        cout << "Ten. " << id << " recibe liberac. de " << Filo << endl;
    }
}
