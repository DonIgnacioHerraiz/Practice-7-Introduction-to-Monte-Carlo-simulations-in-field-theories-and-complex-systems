CUESTION DE NOTACION:

Recordar que cada arista, en consonancia con los apuntes de Follana, se denota por (n,x), siendo n un natural que especifica a un nodo y x un vector unitario en una de las 3 direcciones posibles.

Además, notar que cada plaqueta puede ser especificada por un entero n y dos vectores x,y. Así, denotaremos por (n,x,y) a cada plaqueta.


# Codificación del Sistema

En el modelo de Ising almacenamos toda la información en un único array unidimensional.
Cada nodo de la red estaba asociado a un único espín, de modo que para una red cúbica 3D de lado L
se necesitaba un array de tamaño L^3.

En el Ising Gauge la situación cambia ligeramente, aunque mantendremos la etiquetacion de los nodos. 
Así, si nos imaginamos una red cubica de lado L, asignaremos a cada nodo un numero n, que se relaciona 
con sus tres coordenadas de una red cubica a traves de:

n=x +L*y+L^2z

donde las tres coordenadas pueden tomar los valores enteros comprendidos entre 0 y L-1.

En Ising-Gauge, los espines están definidos en las aristas, y para representarlos buscamos 
primero una asignación suprayectiva de nodos → aristas. Una manera natural de hacerlo es asignar a 
cada nodo n las tres aristas de la forma (n, α), donde α es un vector positivo de la base (x, y, z). 
Esta construcción garantiza que la asignación sea efectivamente suprayectiva.

Con esta correspondencia definida, organizamos el array de la red siguiendo el patrón:

(1,x ; 1,y ; 1,z ; 2,x ; 2,y ; 2,z ; …)

De este modo, cada nodo contribuye con tres posiciones consecutivas en el array,
correspondientes a los espines de las tres aristas que lo contienen y están orientadas
en sentido positivo.

Al igual que con las aristas, vamos a codificar las plaquetas de alguna forma. Al igual que en el caso anterior,
debemos buscar una asignación sobreyectiva nodos→ plaquetas. La idea es asociar a cada nodo 3 plaquetas: la generado
por n,alfa,beta, siendo alfa y beta los posibles (x,y,z). De esta forma, al igual que ocurria con las aristas, tendremos 
3*L^2 plaquetas, que codificaremos en un array de tamaño 3N con el siguiente patrón:

( (1,x,y) ;    (1,x,z)   ;    (1,y,z)   ;   (2,x,y)     ;     (2,x,z)   ;    (2,y,z)  ...)


# Organizacion del Github
Una vez dicho como codificamos las aristas y las plaquetas, pasamos a describir como se va a organizar el github. La estructura general 
sigue siendo la misma que en la practica 5, por lo que alguna explicacion se obvia:

Codigos en C: Dividimos el codigo del siguiente modo:

funciones_red.c: Aqui escribiremos todas las funciones relativas al manejo de los arrays de plaquetas y aristas

dinamica.c: Aqui escribiremos el "main" con el que generaremos las configuraciones termalizadas mediante el algoritmo de metropolis

funciones_dinamica.c: Aqui crearemos las funciones utilizadas en dinamica.c

analisis.c: Aqui escribiremos el "main" con el que analizaremos las configuraciones creadas en dinamica.c

funciones_analisis.c: obvio   (͡° ͜ʖ ͡°)

A continuación, os describo algunas de las funciones que YA se me han ocurrido escribir en cada fichero:


funciones_dinamica.c: 

- void condiciones_iniciales(int N, int *aristas, int flag)
- int arista_aleatoria(int N)
- void cocientes_prob(int N, double J, double betta, double *cocientes)
- int delta_E(int arista_aleatoria,int *plaquetas)
- double EL_cociente(int delta_E, double *cocientes)
- int un_paso_metropolis(int *aristas, int *plaquetas, double *cocientes)

funciones_red:
- plaqueta_xy(int Nodo, int N)
- plaqueta_xz(int Nodo, int N)
- plaqueta_yz(int Nodo, int N)
- void coordenadas_nodo(int N, int Nodo, int*x, int *y, int *z)
- int numero_nodo(int N, int x, int y, int z)
- dame_plaquetas(int *nodos, int *plaquetas)
-



