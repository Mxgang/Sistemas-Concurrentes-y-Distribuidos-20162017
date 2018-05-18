#include <iostream>
#include <unistd.h> // necesario para usleep()
#include <stdlib.h> // necesario para random(), srandom()
#include <time.h> // necesario para time()
#include <semaphore.h>

using namespace std;

const int num_fumadores = 3;
sem_t estanquero;
sem_t fumador[num_fumadores];
sem_t mutex;


void retraso_aleatorio( const float smin, const float smax )
{
	static bool primera = true ;
	if ( primera )
	// si es la primera vez:
	{ 
		srand(time(NULL)); // inicializar la semilla del generador
		primera = false ; // no repetir la inicialización
	}
	// calcular un número de segundos aleatorio, entre smin y smax
	const float tsec = smin+(smax-smin)*((float)random()/(float)RAND_MAX);
	// dormir la hebra (los segundos se pasan a microsegundos, multiplicándos por 1 millón)
	usleep( (useconds_t) (tsec*1000000.0) );
}

void fumar( int num_fum )
{
	sem_wait(&mutex);
	cout << "Fumador número " << num_fum << ": comienza a fumar." <<endl;
	sem_post(&mutex);
	retraso_aleatorio( 0.2, 0.8 );
	sem_wait(&mutex);
	cout << "Fumador número " << num_fum << ": termina de fumar." <<endl;
	sem_post(&mutex);
}

void toString(int ingrediente)
{
	sem_wait(&mutex);
	if(ingrediente == 0)
	{
		cout << "Papel " << endl;
	}
	else if (ingrediente == 1)
	{
		cout << "Tabaco " << endl;
	}
	else if(ingrediente == 2)
	{
		cout << "Cerillas " << endl;
	}
	sem_post(&mutex);
}

int producir_ingrediente()
{	

	int ingrediente = rand()%3;

	sem_wait(&mutex);
	cout << "\nEstanquero comienza a producir..." << endl;
	sem_post(&mutex);

	sem_wait(&mutex);
	cout << "Ingrediente producido: ";
	sem_post(&mutex);

	toString(ingrediente);
	
	cout << endl;
	return ingrediente ;
}


void * funcion_estanquero( void * )
{
	while(true){
		
		sem_wait(&estanquero);
		
		int ingrediente = producir_ingrediente();
		

		sem_post(&fumador[ingrediente]); 
	}
	return NULL;


}
void * funcion_fumador( void * num_fum_void )
{

	while(true)
	{
		unsigned long valor = (unsigned long) num_fum_void;

		sem_wait(&fumador[valor]);

		

		sem_post(&estanquero); 
		fumar(valor);
	}


}





int main(int argc, char *argv[])
{

	srand( time(NULL) ); // inicializa la semilla aleatoria

	
	for(int i = 0; i < num_fumadores; i++)
		sem_init(&fumador[i],0,0);
	sem_init(&estanquero,0,1);
	sem_init(&mutex,0,1);

	pthread_t hebra_fumador[num_fumadores], hebra_estanquero;
	
	for(int i = 0; i < num_fumadores; i++)
		pthread_create(&hebra_fumador[i],NULL,funcion_fumador,(void *) i);

	pthread_create(&hebra_estanquero,NULL,funcion_estanquero,NULL);

	
	for(int i = 0; i < num_fumadores; i++)
		pthread_join(hebra_fumador[i],NULL);

	pthread_join(hebra_estanquero,NULL);

	
	for(int i = 0; i < num_fumadores;i++)
		sem_destroy(&fumador[i]);

	sem_destroy(&estanquero);
	sem_destroy(&mutex);

    exit(0);
}