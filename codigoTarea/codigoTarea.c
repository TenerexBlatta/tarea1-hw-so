/*
========================================================
INF2322-02 - Hardware y Sistemas Operativos
Integrantes: Tomas Monge, Melissa Rojas, Cristobal Perez
Taller 1 - Vendedor Viajero de 30 Nodos
========================================================
*/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <semaphore.h>
#include <pthread.h>
#define MAX_NODOS 31
#define MAX_OBJETIVOS 5
#define MAX_HEBRAS 40

int nodo_inicial;
int nodos_a_recorrer[MAX_OBJETIVOS];
int grafo [MAX_NODOS][MAX_NODOS];
const int caracteres_max_por_linea = 160;
const char *delimitador = ";";
FILE *archivo_en_lectura;
sem_t sem_hebras_disponibles;
pthread_mutex_t mutex_mejor_solucion;

typedef struct { 
    int nodo_actual;
    int camino[MAX_NODOS]; // Nodos recorridos por la hebra, con un tamaño máximo del número total de nodos en el grafo.
    int cant_saltos; // Esto serian los saltos dados de nodo a nodo o distancia coloquialmente hablando.
    int nodos_visitados[MAX_NODOS];
    int objetivos_restantes[MAX_OBJETIVOS];
    int cant_objetivos_restantes;
} instancia_Hebra; // Estructura para representar el estado de cada hebra, con la idea de ir propagando el camino de una a otra.

void abrir_Archivo(){
      archivo_en_lectura = fopen("grafo.csv", "r"); // Abre el archivo en modo lectura.
     if (archivo_en_lectura == NULL) {
      printf("No se logro abrir el archivo.\n");
      } else {
      printf("Archivo grafo.csv abierto exitosamente.\n");
     }
}


void inicializar_Grafo() {
   for (int i = 0; i < 30; i++) {
      for (int j = 0; j < 30; j++) {
         grafo[i][j] = -1; // Rellena de -1 la matriz, para cuando se lea la fila del grafo, se ubique como delimitador el -1 significando que los vecinos fueron ya visitado todos.
      }                    // Como fue visto en el ejemplo del profesor en taller.
   }
}

void imprimir_Grafo() {

    for(int i = 0; i < 30; i++) { // Columnas

        if(grafo[i][0] != -1) { // Esto por decirlo de una manera, se esta moviendo de izquierda a derecha en cada fila

            printf("%d -> ", i);

            for(int j = 0; grafo[i][j] != -1; j++) { // Filas, se detiene en el -1

                printf("%d ", grafo[i][j]);
            }

            printf("\n");
        }
    }
}

void leer_Grafo(FILE *archivo_en_lectura) {
   int fila_en_lectura = 0; // Para diferenciar entre la primera línea, la segunda y las siguientes.
   char linea[caracteres_max_por_linea]; // Es el 'buffer' de la fila que esta siendo leida.
      
      while(fgets(linea, caracteres_max_por_linea, archivo_en_lectura) != NULL){
         if (fila_en_lectura == 0) {
            // Lee primera linea

            // Atoi convierte un string a un entero, en este caso el nodo inicial.
            nodo_inicial = atoi(linea);
            printf("Nodo inicial encontrado: %d\n", nodo_inicial);
            fila_en_lectura++;

         } else if (fila_en_lectura == 1) {
            // Procesar segunda linea
            
            // Strtok divide una cadena en tokens basados en un delimitador, en este caso el ';'.
            char *token = strtok(linea, delimitador);
            
            int indice_nodos_a_recorrer = 0;
            while (token != NULL && indice_nodos_a_recorrer < MAX_OBJETIVOS) { // Cicla hasta que no queden tokens o se alcance el máximo de objetivos a recorrer.
               nodos_a_recorrer[indice_nodos_a_recorrer] = atoi(token); // Convierte el token a int y lo guarda en el arreglo.
               printf("Nodo objetivo encontrado: %d\n", nodos_a_recorrer[indice_nodos_a_recorrer]);
               token = strtok(NULL, delimitador); // Continúa dividiendo la cadena hasta que no haya más tokens.
               indice_nodos_a_recorrer++;
            }
            fila_en_lectura++;
         }   
            else {
            // Procesar las siguientes líneas para llenar la matriz del grafo.
               char *token = strtok(linea, delimitador); // Se le manda linea para inicializarlo y despues NULL cuando queremos seguir dividiendo la misma linea.

               int nodo = atoi(token);

               int indice_vecino = 0;

               token = strtok(NULL, delimitador); // Avanca el token hacia el primer vecino y desde ahi empieza el ciclo.

               while(token != NULL) { // Cicla hasta que no queden nodos en la linea

                  grafo[nodo][indice_vecino] = atoi(token);

                  token = strtok(NULL, delimitador);

                  indice_vecino++;
               }
               grafo[nodo][indice_vecino] = -1; // Le agregua ese -1 final que permite detenerse en la lectura de vecinos.

            fila_en_lectura++;
            
         }

      } 

}
   
void inicializar_hebra_Inicial(instancia_Hebra *hebra) {
    /* Decrementa el semaforo para reservar un espacio para la hebra inicial. */
   sem_wait(&sem_hebras_disponibles); 
   // El nodo actual de la hebra inicial es el nodo inicial del grafo.
   hebra->nodo_actual = nodo_inicial; 
   // La hebra inicial no ha dado ningún salto aún, por lo que se inicializa en 0.
   hebra->cant_saltos = 0; 
   // La hebra inicial tiene todos los objetivos a visitar por definicion.
   hebra->cant_objetivos_restantes = MAX_OBJETIVOS; 

   for (int i = 0; i < MAX_NODOS; i++) {
      hebra->nodos_visitados[i] = 0; // Inicializa el arreglo de nodos visitados en 0, indicando que no se ha visitado ningún nodo aún.
      hebra->camino[i] = -1; // Inicializa el camino con -1, indicando que no se ha recorrido ningún nodo aún.
   }
   for (int i = 0; i < MAX_OBJETIVOS; i++) {
      hebra->objetivos_restantes[i] = nodos_a_recorrer[i]; // Copia los nodos objetivos a recorrer en la estructura de la hebra.
   }
   hebra->camino[0] = nodo_inicial; // El primer nodo del camino es el nodo inicial (10).

}

void inicializar_hebra_Hija(instancia_Hebra *hebra_hija, instancia_Hebra *hebra_padre, int nodo_vecino) {
   // el nodo vecino para mi es como la clave de la propagacion
   hebra_hija->nodo_actual = nodo_vecino; // El nodo actual de la hebra hija es el nodo vecino que se va a visitar.
   hebra_hija->cant_saltos = hebra_padre->cant_saltos + 1; // La cantidad de saltos de la hebra hija es la cantidad de saltos de la hebra padre sumandole el actual

   for (int i = 0; i < MAX_NODOS; i++) {
      hebra_hija->nodos_visitados[i] = hebra_padre->nodos_visitados[i]; // Copia el arreglo de nodos visitados de la hebra padre a la hija.
      hebra_hija->camino[i] = hebra_padre->camino[i]; // Copia el camino recorrido por la hebra padre a la hija.
   }

   for (int i = 0; i < MAX_OBJETIVOS; i++) {
      hebra_hija->objetivos_restantes[i] = hebra_padre->objetivos_restantes[i]; // Copia el arreglo de objetivos restantes de la hebra padre a la hija.
   }
   hebra_hija->cant_objetivos_restantes = hebra_padre->cant_objetivos_restantes; // Copia la cantidad de objetivos restantes de la hebra padre a la hija.
   

}

void explorar_Grafo(instancia_Hebra *hebra){
   /* Aqui se implementaria la logica de exploracion del grafo, con la idea de ir creando nuevas hebras 
   para cada nodo vecino a visitar, y asi ir propagando el camino recorrido por cada hebra. */
   
   /* Decrementa el semaforo para reservar un espacio para la nueva hebra hija. */
   sem_wait(&sem_hebras_disponibles); 
   /* Tengo mis observaciones si a lo mejor hay que mandarle mas parametros o no, pero por ahora solo le mando el nodo actual */
   inicializar_hebra_Inicial(hebra);


  
}


   
int main() {
  
   /* Abre el archivo y verifica que se haya abierto correctamente. */
   abrir_Archivo(); 
      
   /* Inicializar la matriz del grafo con -1 para marcar las posiciones vacías. */
   inicializar_Grafo();

   /* Llenar el grafo con los datos extraidos del archivo */
   leer_Grafo(archivo_en_lectura);

   /* Imprime el grafo para verificar que se haya leído correctamente. */
   imprimir_Grafo();

   /* Cierra el archivo y libera los recursos. */
   fclose(archivo_en_lectura);

   /* Inicializa el mutex para proteger el acceso a la mejor solución encontrada y no se sobreescriba en simultaneo. */
   pthread_mutex_init(&mutex_mejor_solucion, NULL);

   /* Inicializa el semaforo en 40 hebras disponibles, para controlar el num max de hebras concurrentes. */
   sem_init(&sem_hebras_disponibles, 0, MAX_HEBRAS);
  
   /* Pide memoria para la hebra inicial, para asi poder empezar la exploracion desde el nodo 10 (inicial). */
   instancia_Hebra *hebra_inicial = malloc(sizeof(instancia_Hebra));

   explorar_Grafo(hebra_inicial);

   return 0;
}
