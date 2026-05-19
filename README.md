Repositorio para la Tarea 1.
:)


Instrucciones:
El clásico problema del vendedor viajero consiste en determinar la ruta óptima que debe recorrer un vendedor para pasar por un conjunto de nodos dentro de un grafo.
	Dado dicho problema, se pide implementar una solución basada en hebras sincronizadas que permita resolver una versión simplificada de dicho problema. Para lo anterior, se deberá considerar: 
	
-	Se trabajará con un grafo de 30 nodos.
-	El grafo será dirigido, es decir los enlaces son dirigidos, por ejemplo, si hay camino entre el nodo 2 y el 4, no necesariamente habrá camino entre en 4 y el 2
-	El “peso” de cada enlace será siempre 1 (medir por “cantidad de saltos”, no por suma de los pesos de los enlaces)
-	Los nodos a visitar serán 5 partiendo de un nodo determinado
-	La información se entregará en un archivo con la siguiente estructura:
o	Primera fila: nodo inicial
o	Segunda fila: lista de los 5 nodos a recorrer, separados por “;”. El recorrido puede ser en cualquier orden (no necesariamente en el orden que fueron dados), pero debe pasar por esos 5 nodos
o	Desde la tercera fila: número de nodo y todos los enlaces a los cuales tiene enlace de salida (a donde se pueden desplazar partiendo de dicho nodo), separados por “;”
o	Ejemplo:
10                                                               partir en 10
2;8;17;20;28                                              debe pasar por nodos 2,8,17,20,28
1;2;4;5;10;14;21;22;25;27;30                   desde nodo 1 se puede ir al 2,3,4 ..
2;1;5;6;11;17;18;20;23;24;26;28              desde nodo 2 se puede ir al 1,5,6 ..
3;2;4;6;7;9;10;15;19;22;25;27;30             desde nodo 3 se puede ir al 2,4,6 ..

  ---> Ver archivo grafo.csv <---

-	Realice un programa en lenguaje C en Linux que recorra los nodos y generando nuevas hebras para cada ruta, y si encuentra una solución la despliegue en pantalla. Luego si encuentra una solución mejor despliegue dicha solución, y así sucesivamente hasta recorrer todas las rutas posibles. Finalmente deberá explicitar cuál es la mejor ruta encontrada y cuantos nodos se deberán visitar.

-	Limite que la cantidad de hebras que corran simultáneamente no superen las 40 (mediante semáforos, no por restricción al sistema en /proc/sys/kernel/threads-max) 

-	Defina las estructuras que requiera para la solución sin restricciones, pero no olvide en manejar integridad si son compartidas. Por simplicidad, se sugiere representar grafo en matriz y usar vectores para resultados (si lo desea puede usar estructuras dinámicas pero aumentará la complejidad).
