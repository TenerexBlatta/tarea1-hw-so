/*
========================================================
INF2322 - Hardware y Sistemas Operativos
Integrantes: Tomas Monge, Melissa Rojas, Cristobal Perez
Taller 1 - Vendedor Viajero de 30 Nodos
========================================================
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <semaphore.h>

#define MAX_NODOS 31
#define MAX_OBJETIVOS 5
#define MAX_HEBRAS 40

typedef struct {
    int camino[MAX_NODOS];
    int cantidad_saltos;
} lista_Nodo;

int grafo[MAX_NODOS][MAX_NODOS];
int nodos_objetivo[MAX_OBJETIVOS];
int nodo_inicial;
int mejor_camino[MAX_NODOS];
int mejor_cantidad_saltos = 999999;
sem_t semaforo_hebras;
pthread_mutex_t mutex_mejor_solucion;

void inicializar_Grafo() {
    // Cicla por la matriz llenandola de -1 cada posicion.
    for(int indice_fila = 0; indice_fila < MAX_NODOS; indice_fila++) {
        for(int indice_columna = 0; indice_columna < MAX_NODOS; indice_columna++) {
            grafo[indice_fila][indice_columna] = -1;
        }
    }
    // Realiza lo mismo pero en el vector de mejor camino encontrado
    for(int indice_nodo = 0; indice_nodo < MAX_NODOS; indice_nodo++) {
        mejor_camino[indice_nodo] = -1;
    }
}

void leer_Grafo() {
    // Utilizamos la libreria para leer
    FILE *archivo_grafo = fopen("../grafo.csv", "r");
    char buffer_linea[160];
    char *token_actual;
    int numero_fila = 0;

    if(archivo_grafo == NULL) {
        printf("No se pudo abrir grafo.csv\n");
        exit(1);
    }
    printf("Archivo abierto correctamente.\n");

    while(fgets(buffer_linea, sizeof(buffer_linea), archivo_grafo) != NULL) {
        buffer_linea[strcspn(buffer_linea, "\n")] = '\0'; // Intercambia el fin de linea con el terminador de string
        if(numero_fila == 0) {
            nodo_inicial = atoi(buffer_linea); // pasa token a integer
            printf("Nodo inicial encontrado: %d\n", nodo_inicial);
        } else if(numero_fila == 1) {
            token_actual = strtok(buffer_linea, ";");
            int indice_objetivo = 0;
            while(token_actual != NULL && indice_objetivo < MAX_OBJETIVOS) { // cicla por todo los nodos encontrados en la linea
                nodos_objetivo[indice_objetivo] = atoi(token_actual);
                printf("Nodo objetivo encontrado: %d\n", nodos_objetivo[indice_objetivo]);
                token_actual = strtok(NULL, ";");
                indice_objetivo++;
            }
        } else {
            token_actual = strtok(buffer_linea, ";");
            int nodo_base = atoi(token_actual);
            int indice_vecino = 0;
            token_actual = strtok(NULL, ";");
            while(token_actual != NULL) {
                grafo[nodo_base][indice_vecino] = atoi(token_actual);
                token_actual = strtok(NULL, ";");
                indice_vecino++;
            }
            grafo[nodo_base][indice_vecino] = -1;
        }
        numero_fila++;
    }
    fclose(archivo_grafo);
}

void imprimir_Grafo() { // Cicla por la matriz para imprimir cada nodo
    printf("\nGRAFO:\n");
    for(int indice_nodo = 1; indice_nodo < MAX_NODOS; indice_nodo++) {
        if(grafo[indice_nodo][0] != -1) {
            printf("%d -> ", indice_nodo);
            for(int indice_vecino = 0; grafo[indice_nodo][indice_vecino] != -1; indice_vecino++) {
                printf("%d ", grafo[indice_nodo][indice_vecino]);
            }
            printf("\n");
        }
    }

}

int nodo_Visitado(int *camino, int nodo_buscado) {  // Es una funcion booleana implicitamente, devuelve 1 si el nodo ya fue visitado en el camino o 0 si no.
    for(int indice_nodo = 0; camino[indice_nodo] != -1; indice_nodo++) {
        if(camino[indice_nodo] == nodo_buscado) {
            return 1;
        }
    }
    return 0;
}

int objetivos_Visitados(int *camino) { // Contabiliza cuantos nodos objetivos ha visitado el camino entregado
    int cantidad_objetivos = 0;
    for(int indice_objetivo = 0; indice_objetivo < MAX_OBJETIVOS; indice_objetivo++) {
        if(nodo_Visitado(camino, nodos_objetivo[indice_objetivo])) {
            cantidad_objetivos++;
        }
    }
    return cantidad_objetivos;
}

void guardar_Mejor_Camino(lista_Nodo lista_actual) {
    pthread_mutex_lock(&mutex_mejor_solucion);
    if(lista_actual.cantidad_saltos < mejor_cantidad_saltos) {
        mejor_cantidad_saltos = lista_actual.cantidad_saltos;
        for(int indice_nodo = 0; indice_nodo < MAX_NODOS; indice_nodo++) {
            mejor_camino[indice_nodo] = lista_actual.camino[indice_nodo];
        }

        printf("Nueva mejor solucion encontrada:\n");
        printf("Camino: ");
        for(int indice_nodo = 0; indice_nodo < lista_actual.cantidad_saltos; indice_nodo++) {
            printf("%d ", lista_actual.camino[indice_nodo]);
        }
        printf("\n");
        printf("Saltos: %d\n", lista_actual.cantidad_saltos - 1);
        printf("\n");
    }
    pthread_mutex_unlock(&mutex_mejor_solucion);
}

void explorar_Grafo(lista_Nodo lista_actual) {
    if(lista_actual.cantidad_saltos >= mejor_cantidad_saltos) {
        return;
    }
    int nodo_actual = lista_actual.camino[lista_actual.cantidad_saltos - 1];
    for(int indice_vecino = 0; grafo[nodo_actual][indice_vecino] != -1; indice_vecino++) {
        int nodo_vecino = grafo[nodo_actual][indice_vecino];
        if(!nodo_Visitado(lista_actual.camino, nodo_vecino)) {
            lista_actual.camino[lista_actual.cantidad_saltos] = nodo_vecino;
            lista_actual.camino[lista_actual.cantidad_saltos + 1] = -1;
            lista_actual.cantidad_saltos++;
            if(objetivos_Visitados(lista_actual.camino) == MAX_OBJETIVOS) {
                guardar_Mejor_Camino(lista_actual);
            } else {
                explorar_Grafo(lista_actual);
            }
            lista_actual.cantidad_saltos--;
        }
    }
}

void *funcion_Hebra(void *argumentos) {
    lista_Nodo lista_actual = *((lista_Nodo *)argumentos);
    free(argumentos);
    explorar_Grafo(lista_actual);
    sem_post(&semaforo_hebras);
    pthread_exit(NULL);
}

int main() {
    inicializar_Grafo(); // Rellena el grafo de -1 para
    leer_Grafo();
    imprimir_Grafo();
    sem_init(&semaforo_hebras, 0, MAX_HEBRAS);
    pthread_mutex_init(&mutex_mejor_solucion, NULL);
    int nodo_vecino;
    int indice_hebra = 0;
    pthread_t arreglo_hebras[MAX_HEBRAS];
    for(int indice_vecino = 0; (nodo_vecino = grafo[nodo_inicial][indice_vecino]) != -1; indice_vecino++) {
        sem_wait(&semaforo_hebras);
        lista_Nodo *lista_inicial = malloc(sizeof(lista_Nodo));
        for(int indice_nodo = 0; indice_nodo < MAX_NODOS; indice_nodo++) {
            lista_inicial->camino[indice_nodo] = -1;
        }
        lista_inicial->camino[0] = nodo_inicial;
        lista_inicial->camino[1] = nodo_vecino;
        lista_inicial->camino[2] = -1;
        lista_inicial->cantidad_saltos = 2;
        pthread_create(&arreglo_hebras[indice_hebra], NULL, funcion_Hebra, lista_inicial);
        indice_hebra++;
    }
    for(int indice_hebra_actual = 0; indice_hebra_actual < indice_hebra; indice_hebra_actual++) {
        pthread_join(arreglo_hebras[indice_hebra_actual], NULL);
    }

    printf("Mejor Solucion de Todas:\n");
    printf("=================================\n");
    if(mejor_cantidad_saltos == 999999) {
        printf("No se encontro solucion.\n");
    } else {
        printf("Camino: ");
        for(int indice_nodo = 0; mejor_camino[indice_nodo] != -1; indice_nodo++) {
            printf("%d ", mejor_camino[indice_nodo]);
        }
        printf("\n");
        printf("Cantidad de saltos: %d\n", mejor_cantidad_saltos - 1);
    }
    printf("=================================\n");

    // Borramos los semaforos como una buena practica para el uso de recursos.
    sem_destroy(&semaforo_hebras);
    pthread_mutex_destroy(&mutex_mejor_solucion);
    return 0;
}
