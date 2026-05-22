/* Integrantes: Tomas, Melissa, Cristobal 
   Paralelo: INF2322-2
*/
// Recordar que programa sea compilable finalmente para Linux
// No I.A.
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <semaphore.h>
#include <pthread.h>

int nodo_inicial;
int nodos_a_recorrer[5];
int grafo [30][30];
const char simbolo_token = ';';
sem_t sem_hebras_disponibles;
pthread_mutex_t mutex_mejor_solucion;
const int caracteres_max_por_linea = 160;

void leer_Grafo() {
   int fila_en_lectura = 0; // Para diferenciar entre la primera línea, la segunda y las siguientes.
   char linea[caracteres_max_por_linea]; // Es el 'buffer' de la fila que esta siendo leida.
      
      while(fgets(linea, caracteres_max_por_linea, archivo) != NULL){
         if (fila_en_lectura == 0) {
            // atoi convierte un string a un entero, en este caso el nodo inicial.
            nodo_inicial = atoi(linea);
            printf("Nodo inicial encontrado: %d\n", nodo_inicial);
            fila_en_lectura++;
         } else if (fila_en_lectura == 1) {
         } 
         
            else {

            
         }


      } 



   }

   fclose(archivo); // Cierra el archivo y libera los recursos.
   
}
   
int main() {

      
    FILE *archivo = fopen("grafo.csv", "r"); // Abre el archivo en modo lectura
     if (archivo == NULL) {
      printf("No se logro abrir el archivo.\n");
      } else {
      printf("Archivo grafo.csv abierto exitosamente.\n");
     }
   
    inicializar_Grafo(); // Llenar el grafo con los datos extraidos del archivo

    leer_nodo_Inicial(archivo); // Para asignar el nodo 10

    leer_nodos_Objetivos(archivo);

    leer_Grafo(archivo);

    fclose(archivo);

    pthread_mutex_init(&mutex_mejor_solucion, NULL);

    sem_init(&sem_hebras, 0, 40);

    instancia_Hebra *inicial = malloc(sizeof(instancia_Hebra));

    inicializarHebra(inicial);

    explorar(inicial);

    sleep(3);

    return 0;
}

