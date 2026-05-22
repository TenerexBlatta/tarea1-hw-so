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

typedef struct { // Estructura para representar el estado de cada hebra, con la idea de ir propagando el camino de una a otra.
    int nodo_actual;
    int camino[30]; // Nodos recorridos por la hebra, con un tamaño máximo del número total de nodos en el grafo.
    int largo_camino; // Esto serian los saltos
    int visitados[30];
    int objetivos_restantes[5];
    int restantes;
} instancia_Hebra;

void inicializar_Hebra(instancia_Hebra *hebra, int nodo_actual) {
   //Recibo el nodo actual para diferenciar si es la primera, o ya trae herencia de informacion/camino de una hebra previa.
   
   if (nodo_actual == NULL) {
      hebra->nodo_actual = nodo_inicial;
   } else {
      hebra->nodo_actual = nodo_actual;
   }


}
   

void inicializar_Grafo() {
   for (int i = 0; i < 30; i++) {
      for (int j = 0; j < 30; j++) {
         grafo[i][j] = -1; // Rellena de -1 la matriz, para cuando se lea la fila del grafo, se ubique como delimitador el -1 significando que los vecinos fueron ya visitado todos.
      }                    // Como fue visto en el ejemplo del profesor en taller.
   }
}

void leer_Grafo(FILE *archivo) {
   int fila_en_lectura = 0; // Para diferenciar entre la primera línea, la segunda y las siguientes.
   char linea[caracteres_max_por_linea]; // Es el 'buffer' de la fila que esta siendo leida.
      
      while(fgets(linea, caracteres_max_por_linea, archivo) != NULL){
         if (fila_en_lectura == 0) {
            // atoi convierte un string a un entero, en este caso el nodo inicial.
            nodo_inicial = atoi(linea);
            printf("Nodo inicial encontrado: %d\n", nodo_inicial);
            fila_en_lectura++;
         } else if (fila_en_lectura == 1) {
            //Procesar segunda linea
            char *token = strtok(linea, &simbolo_token); // Strtok es una función que divide una cadena en tokens basados en un delimitador, en este caso el ';'.
            int indice_nodos_a_recorrer = 0;
            while (token != NULL && indice_nodos_a_recorrer < 5) {
               nodos_a_recorrer[indice_nodos_a_recorrer] = atoi(token);
               printf("Nodo objetivo encontrado: %d\n", nodos_a_recorrer[indice_nodos_a_recorrer]);
               token = strtok(NULL, &simbolo_token); // Continúa dividiendo la cadena hasta que no haya más tokens.
               indice_nodos_a_recorrer++;
            }
            fila_en_lectura++;
         }   
            else {
            //Procesar siguientes lineas
            fila_en_lectura++;
            
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

    leer_Grafo(archivo);

    fclose(archivo);

    pthread_mutex_init(&mutex_mejor_solucion, NULL);

    sem_init(&sem_hebras_disponibles, 0, 40);

    instancia_Hebra *inicial = malloc(sizeof(instancia_Hebra));

    inicializar_Hebra(inicial, NULL);

    explorar(inicial);

    sleep(3);

    return 0;
}
