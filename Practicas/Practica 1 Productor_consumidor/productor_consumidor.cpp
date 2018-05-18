#include <iostream>
#include <unistd.h> // necesario para usleep()
#include <stdlib.h> // necesario para random(), srandom()
#include <time.h> // necesario para time()
#include <semaphore.h>

using namespace std;

const int tam_vec = 4;
const int num_items = 30;

sem_t productor;
sem_t consumidor;

int buffer[tam_vec]; 
int indice = 0; 

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

int producir_dato()
{
	static int contador = 0 ;
	contador = contador + 1 ;
	retraso_aleatorio( 0.1, 0.5 );
	cout << "Productor: dato producido: " << contador << endl ;
	return contador ;
}

void consumir_dato( int dato )
{
	retraso_aleatorio( 0.1, 0.5 );
	cout << "Consumidor: dato consumido: " << dato << endl ;
}

void * funcion_productor( void * )
{
	for( unsigned i = 0 ; i < num_items ; i++ )
	{
		
		if(indice < tam_vec)
		{ 	
			sem_wait(&productor);
			int dato = producir_dato(); 
			buffer[indice] = dato;
        	indice++;
        	sem_post(&consumidor);
			
		}

		else if(indice == tam_vec)
		{
			sem_wait(&productor);

		}
		
	}
	return NULL ;
}
void * funcion_consumidor( void * )
{
	for( unsigned i = 0 ; i < num_items ; i++ )
	{
		
		if(indice==0)
		{
			sem_wait(&consumidor);

		}

		else if(indice>0)
		{
			sem_wait(&consumidor);
			int dato = buffer[indice-1];
			indice--;
			consumir_dato(dato);
			sem_post(&productor);
			
		}
		
	}
	return NULL ;
}
int main(int argc, char *argv[])
{
  
    sem_init(&productor, 0, tam_vec); 
    sem_init(&consumidor, 0, 0); 


    pthread_t hebra_productor, hebra_consumidor;

    pthread_create(&hebra_productor,NULL,funcion_productor,NULL);
    pthread_create(&hebra_consumidor,NULL,funcion_consumidor,NULL);


    pthread_join(hebra_productor, NULL);
    pthread_join(hebra_consumidor, NULL);

    sem_destroy(&productor);
    sem_destroy(&consumidor);

    exit(0);

}



