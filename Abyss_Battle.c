#include <locale.h>
#include <stdio.h>
#include <stdlib.h>
#include <windows.h>
#include <conio.h>
#include <math.h>
#include <string.h>

//Definicion de las constantes utilizadas para el funcionamiento general del sistema.
#define FILAS 21
#define COLUMNAS 91
#define MAX_SOLDIER 12 //Este numero se puede cambiar para probar le funcionamiento del sistema.
#define MAX_PTR 27 // FILAS - 1
#define MAX_AIM 19		
#define POTENCIA_MAX 11 // (COLUMNAS-6)/8
#define MAX_DAMAGE 4

#define OPCIONES 3
typedef struct //Estructura para el funcionamiento de la posicion por medio de coordenadas.
{
	int posX;
	int posY;
	int active;
} pos;

typedef struct //Estructura para el funcionamiento de las acciones del jugador.
{ 
	
	int turno;
	pos canon;
	pos soldier [MAX_SOLDIER];
	pos aim[MAX_PTR];
	int disparos;
	
} player;

int i,j;

void gotoxy(int x, int y)  //Función para manejar la posicion del cursor.
{
    COORD coord;
    coord.X = x;
    coord.Y = y;
    SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), coord);
}

void imprimirMenu(int opcionSeleccionada) //SALIDA //Función para imprimir el menu principal.
{
    system("cls");
    printf("			ABYSS´S BATTLE:\n\n");
    printf("		      %s Iniciar juego\n", opcionSeleccionada == 0 ? ">  " : " ");
    printf("			 %s Creditos\n", opcionSeleccionada == 1 ? ">  " : " ");
    printf("		    %s Salir del programa\n", opcionSeleccionada == 2 ? ">  " :  " ");
}


//FUNCIONES PARA EL PROCESO DEL VIDEOJUEGO (Opción "jugar" en el menú principal).

void inicializarTablero(char tablero[FILAS][COLUMNAS]) //PROCESO //Inicia los valores guardados en la matriz que dibujara el tablero.
{ 

    for (i = 0; i < FILAS; i++) {
        for (j = 0; j < COLUMNAS; j++) {
            if (i == 0 || i == FILAS - 1) {
                tablero[i][j] = '-';
            } else if (j == 0 || j == COLUMNAS - 1) {
                tablero[i][j] = '|';
            } else if (j == (COLUMNAS/2)-1 || j == (COLUMNAS/2)+1){
            	tablero[i][j] = '|';
			}else {
                tablero[i][j] = ' ';
            }
        }
    }
}

//Funciones para la impresión del tablero de juego. //SALIDAS
void imprimirTableroAux(char tablero[FILAS][COLUMNAS]) 
{ 
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE); 
    CONSOLE_SCREEN_BUFFER_INFO csbiInfo; //Funciones para la actualización del buffer de la consola.
    GetConsoleScreenBufferInfo(hConsole, &csbiInfo);

    for (i = 0; i < FILAS; i++) {
        COORD pos = {0, i};
        SetConsoleCursorPosition(hConsole, pos);
        for (j = 0; j < COLUMNAS; j++) {
            printf("%c", tablero[i][j]);
        }
    }
}

void imprimirTablero(char tablero[FILAS][COLUMNAS], player jugador[]) 
{ 
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    CONSOLE_SCREEN_BUFFER_INFO csbiInfo;
    GetConsoleScreenBufferInfo(hConsole, &csbiInfo);

	printf("\nTurno del jugador %d\n",jugador[0].turno? 1 : 2);
    for (i = 0; i < FILAS; i++) {
        COORD pos = {0, i};
        SetConsoleCursorPosition(hConsole, pos); //Establece la posicion inicial del cursor.
        for (j = 0; j < COLUMNAS; j++) {
            printf("%c", tablero[i][j]);
        }
		
    }
}

//Funciones para los procesos durante la ejecucion de una partida.

int f(int x, float m, int origen) //Función para calcular la trayectoria del proyectil.
{
	//m es la pendiente.
	//x es el valor donde esta ubicado el tablero.
	//origen: sirve para determinar el valor de x, dependiendo del lado de tablero de donde sale la trayectoria.
	
	float fx;
	x-=origen;
	
	if(origen < COLUMNAS/2){
		fx= ((FILAS/2-1) - (float) m*x);
	} else {
		fx= ((FILAS/2-1) + (float) m*x);
	}
	
	if(m>=0){ 
		return floor(fx);
	} else {
		return ceil(fx);
	}

}

void switchTurno ( player* jugador1, player* jugador2) //Funcion para el cambio de turno.
{ 
	
	jugador1->turno = (jugador1->turno + 1) % 2;
	jugador2->turno = (jugador2->turno + 1) % 2;
	
}

void setCanon(player jugador[], char tablero[FILAS][COLUMNAS]) //Funcion que establece la posición del cañin para cada jugador.
{
	
	jugador[0].canon.posX = (COLUMNAS/4)-1;
    jugador[0].canon.posY = (FILAS/2)-1;
    jugador[1].canon.posX = 3 * (COLUMNAS/4) +2 ;
    jugador[1].canon.posY = (FILAS/2)-1;
    tablero[jugador[0].canon.posY][jugador[0].canon.posX] = '*';
    tablero[jugador[1].canon.posY][jugador[1].canon.posX] = '*';
    for(i = jugador[0].canon.posX-2; i < jugador[0].canon.posX; i++){
    	tablero[jugador[0].canon.posY][i] = '=';
	}
	for(i=jugador[1].canon.posX+2; i > jugador[1].canon.posX;i--){
    	tablero[jugador[1].canon.posY][i] = '=';
	}
}

void setAim(player* jugador){ //Función para controlar el funcionamiento de la dirección de apuntado.
	
	j=jugador->canon.posX;
	
	for(i=0;i<MAX_PTR;i++){

		jugador->aim[i].posX=j;
		jugador->aim[i].posY=(FILAS/2)-1;
		//tablero[jugador[0].aim[i].posY][jugador[0].aim[i].posX] = '$';
		if(jugador->canon.posX<COLUMNAS/2){ 
			j++;
		} else {
			j--;
		}
	}

}

void setSoldier1(player* jugador, char tablero[FILAS][COLUMNAS])
{
	
	for(i=0; i<MAX_SOLDIER; i++){
	//	jugador->soldier[i].posX = jugador->canon.posX;
   	//	jugador->soldier[i].posY = jugador->canon.posY - 2;
   		jugador->soldier[i].active = 1;
	}
    	
	jugador->soldier[0].posX = jugador->canon.posX;
   	jugador->soldier[0].posY = jugador->canon.posY - 2;
    
}

void moveSoldier(player* jugador, int direccion, char tablero[FILAS][COLUMNAS], int* c) //Funcion para controlar la posicion donde se colocan soldados.
{
    int nuevaPosX = jugador->soldier[*c].posX;
    int nuevaPosY = jugador->soldier[*c].posY;

    switch(direccion){
        case 72: case 119: case 87: // Flecha arriba, w y W
      
			if(tablero[nuevaPosY-1][nuevaPosX] == ' '){ //Analiza si la casilla es valida para moverse y colocar un soldado.
				nuevaPosY--;
			}   
            break;
        case 80: case 115: case 83: // Flecha abajo, s y S

			if(tablero[nuevaPosY+1][nuevaPosX] == ' ' ){
				nuevaPosY++;
			}
            
            break;
        case 75: case 97: case 65:  // Flecha izquierda, a y A

			if(tablero[nuevaPosY][nuevaPosX-1] == ' '){
				nuevaPosX--;
			}
            
            break;
        case 77: case 100: case 68: // Flecha derecha, d y D
        
			if(tablero[nuevaPosY][nuevaPosX+1]==' '){
				nuevaPosX++;
			}
            
            break;
        case 13: //Tecla enter
        
        	tablero[jugador->soldier[*c].posY][jugador->soldier[*c].posX] = '#'; //Imprime un "sprite" del soldado.
        	
        	//Guarda las posiciones donde el jugador en turno guarda sus soldados.
        	if (tablero[jugador->soldier[*c].posY][jugador->soldier[*c].posX + 1] == ' '){
        		jugador->soldier[*c + 1].posX = jugador->soldier[*c].posX + 1; 
   				jugador->soldier[*c + 1].posY = jugador->soldier[*c].posY;
			} else if (tablero[jugador->soldier[*c].posY][jugador->soldier[*c].posX - 1] == ' ') {
				jugador->soldier[*c + 1].posX = jugador->soldier[*c].posX - 1;
   				jugador->soldier[*c + 1].posY = jugador->soldier[*c].posY;
			} else if (tablero[jugador->soldier[*c].posY - 1][jugador->soldier[*c].posX] == ' '){
				jugador->soldier[*c + 1].posX = jugador->soldier[*c].posX;
   				jugador->soldier[*c + 1].posY = jugador->soldier[*c].posY - 1;
			} else if (tablero[jugador->soldier[*c].posY + 1][jugador->soldier[*c].posX] == ' '){
				jugador->soldier[*c + 1].posX = jugador->soldier[*c].posX;
   				jugador->soldier[*c + 1].posY = jugador->soldier[*c].posY + 1;
			} else {
				jugador->soldier[*c + 1].posX = jugador->canon.posX;
   				jugador->soldier[*c + 1].posY = jugador->canon.posY - 2;
			}
        	
        	break;
        default:
            return; // No se reconoce la tecla, no hacer nada
            
    }
    
    
    
    if(nuevaPosX != jugador->soldier[*c].posX || nuevaPosY != jugador->soldier[*c].posY)
	{
    	tablero[jugador->soldier[*c].posY][jugador->soldier[*c].posX] = ' ';
    	jugador->soldier[*c].posX = nuevaPosX;
  		jugador->soldier[*c].posY = nuevaPosY;
 
	}

}

void siHayPuesto(player* jugador, char tablero[FILAS][COLUMNAS], int c) //funcion para detectar si hay un soldado puesto en la casilla.
{
	
	if(tablero[jugador->soldier[c].posY][jugador->soldier[c].posX] == '#'){
		jugador->soldier[c].posX ++;
		siHayPuesto(jugador, tablero, c);
	} 
	
}


void inputSet(player* jugador, char tablero[FILAS][COLUMNAS], char* key, int* c)
{
	
	siHayPuesto(jugador, tablero, *c); //llamada a la funcion siHayPuesto
	tablero[jugador->soldier[*c].posY][jugador->soldier[*c].posX] = '0';
	
	
	if(kbhit()){ //Colocar un soldado presionando la tecla "Enter".
        *key = getch();
        moveSoldier(jugador, *key, tablero, c);
        if(*key == 13){
        	(*c)++;
		}

    }
    printf("\n\tTe quedan %d soldados.\n",MAX_SOLDIER-(*c));
    
}

void moveAim (int direccion, player* jugador, int c)
{
	
	for(i=0;i<MAX_PTR; i++){

		jugador->aim[i].posY = f(jugador->aim[i].posX, (float)c/MAX_PTR, jugador->canon.posX);
		
	}
	
}

void determinarPotencia(int* potencia, player* jugador, int c, char* key, char tablero[FILAS][COLUMNAS]) //Funcion para calcular la potencia de tiro.
{
	*key=72;
	system("cls");
	do{
		imprimirTableroAux(tablero);

		printf("\nSeleccione la potencia:\n\n\t"); 
		if (*potencia == 1) {
            printf("< %d > ", *potencia);
        } else {
            printf("%d... < %d > ", *potencia - 1, *potencia);
        }
        fflush(stdout);
        if(*potencia != POTENCIA_MAX){        	
        	printf("...%d", *potencia + 1);
		}
		printf("\n\n\tPara seleccionar la potencia, use las flechas izquierda y derecha, o las teclas 'A' y 'D'");
		if(kbhit()){ //Controlar el movimiento con las fechas o WSAD
			*key = getch();
			switch(*key){
				case 75: case 97: case 65:  // Flecha izquierda, a y A

					if (*potencia > 1){
						(*potencia)--;
					}
		            
		            break;
		        case 77: case 100: case 68: // Flecha derecha, d y D
		        
					if (*potencia < POTENCIA_MAX){
						(*potencia)++;
					}
		            
		            break;
			}
		}
	} while (*key != 13 && *key != 27);
	system("cls");

}

void verifVacio( char tablero[FILAS][COLUMNAS], int posY, int* posX, int direccion, float m, int origen)
{
	
	if(tablero[posY][*posX] != ' '){
		*posX += direccion;
		verifVacio(tablero, f(*posX, m, origen), posX, direccion, m, origen);
	}
	
}
 
void disparar(player* jugador, player* enemigo, int c, int potencia, char tablero[FILAS][COLUMNAS]) //Funcion para controlar el comportamiento de un disparo
{
	int posX, posY, direccion, cont;
	char key;
	
	
	if(jugador->canon.posX < COLUMNAS/2){
		posX=COLUMNAS/2 + 1 + (potencia * 4); //Se toma en cuenta el valor almacenado en la potencia
		direccion = 1;		
	} else {
		posX=COLUMNAS/2 + 2 - (potencia * 4);
		direccion = -1;	
	}
	if(posX<1){
		posX=1;
	} else if (posX>COLUMNAS-2){
		posY = COLUMNAS-2;
	}
	
	for(i = posX; i > (posX - MAX_DAMAGE); i--){
		posY = f(i, (float) c/MAX_PTR, jugador->canon.posX);
		
		
		for(j = 0; j<MAX_SOLDIER; j++){
			if((posY == enemigo->soldier[j].posY || posY - 1 == enemigo->soldier[j].posY || posY + 1 == enemigo->soldier[j].posY) && i == enemigo->soldier[j].posX ){
				enemigo->soldier[j].active = 0;
			}
		}

	}
	
	// animacion de disparo;
	
	cont = jugador->canon.posX;
	
	for(i=0; i<MAX_AIM; i++){
		tablero[jugador->aim[i].posY][jugador->aim[i].posX] = ' ';
	}
	
	if(jugador->canon.posX < COLUMNAS/2){
		do {
			posY = f(cont, (float) c/MAX_PTR, jugador->canon.posX);

			tablero[posY][cont] = '*';
			
			imprimirTableroAux(tablero);
			printf("\n\nPresiona enter para omitir.");
		
			Sleep((int)10*POTENCIA_MAX/potencia);
			
			tablero[posY][cont] = ' ';
			
			cont += direccion;
			
			verifVacio(tablero, posY, &cont, direccion, (float)c/MAX_PTR, jugador->canon.posX);
			if(kbhit()){
				key = getch();
				if (key==13){
					break;
				}
			}
		} while (cont < posX);
	} else {
		do {
		
			posY = f(cont, (float) c/MAX_PTR, jugador->canon.posX);

			tablero[posY][cont] = '*';
			
			imprimirTableroAux(tablero);
			printf("\n\nPresiona enter para omitir.");
		
			Sleep((int)10*POTENCIA_MAX/potencia);
			
			tablero[posY][cont] = ' ';
			
			
			
			cont += direccion;
			verifVacio(tablero, posY, &cont, direccion, (float)c/MAX_PTR, jugador->canon.posX);
			if(kbhit()){
				key = getch();
				if (key==13){
					break;
				}
			}
		} while (cont > posX && f(cont, (float) c/MAX_PTR, jugador->canon.posX) < FILAS-1 && f(cont, (float) c/MAX_PTR, jugador->canon.posX) > 0 && cont > 0 && cont < COLUMNAS-1);
	}
	
	for(i = posX - 1; i > (posX - MAX_DAMAGE + 1); i--){ //Deteccion de las colisiones
		posY = f(i, (float) c/MAX_PTR, jugador->canon.posX);

		if(tablero[posY][i]==' '){
			tablero[posY][i] = 'O';
		} else if (tablero[posY][i] == '#'){
			tablero[posY][i] = 'X';
		}
			
	}
	
	imprimirTableroAux(tablero); //Impresion de la colisión
	Sleep(500);
	for(i = posX; i > (posX - MAX_DAMAGE); i--){
		posY = f(i, (float) c/MAX_PTR, jugador->canon.posX);

		for(j=posY-1; j<posY+2; j++){
			if(tablero[j][i]==' '){
				tablero[j][i] = 'O';
			} else if (tablero[j][i] == '#'){
				tablero[j][i] = 'X';
			}
		}

			
	}
	
	imprimirTableroAux(tablero);

	
}



void desplegarPausa(char* keyMain, char tablero[FILAS][COLUMNAS], player jugador[], int* moment, int* c1, int* c2)//Funcion para pausar la partida.
{
	
	char reanudar[] = "> Reanudar partida", reiniciar[] =  "Reiniciar partida", salir[] = "Salir del juego", opcion;
	int opc = 1;
	
	system("cls");
	
	do{
		imprimirTableroAux(tablero);
		printf("\n\n=============================================\n");
		printf("                   PAUSA\n\n");
		printf("           %sReanudar partida\n",opc==1?"> " : " ");
		printf("           %sReiniciar partida\n",opc==2?"> " : " ");
		printf("           %sRegresar al Menu\n",opc==3?"> " : " ");
		if(kbhit()){
			opcion = getch();
			switch(opcion){
				case 72: case 119: case 87: // Flecha arriba, w y W.
			
					if(opc>1 ){
						opc--;
					}
					
		      		break;
	       		
				case 80: case 115: case 83: // Flecha abajo, s y S.
	
					
					if(opc<3){
						opc++;
					}
	            
	           		break;
	            
	            case 13: // enter.
	            	
	            	switch(opc){
						case 1:
							
							*keyMain = 1; // Solo le cambio que no sea el escape en ASCII.
							printf("\nReanudando partida...");
							Sleep(500);
							system("cls");
							break;
						case 2:
							printf("\nReiniciando partida...");
							Sleep(500);
							
							*keyMain = 1;
							*moment = 1;
							*c1=0;
							*c2=0;
							
							jugador[0].turno=1;
						    jugador[1].turno=0;
						    jugador[0].disparos=0;
						    jugador[1].disparos=0;
						    	
						    inicializarTablero(tablero);
						    setCanon(jugador, tablero);
						    setSoldier1(&jugador[0], tablero); setSoldier1(&jugador[1], tablero);
							system("cls");
							
							break;
						case 3:
						// HACER NADA.
							printf("\nVolviendo al menu...");
							Sleep(500);
							break;
					}
	            	
            	break;
			}
		}
	} while (opcion!=13);
	
	
}

void inputAim (player* jugador, player* enemigo, char tablero[FILAS][COLUMNAS], char* key, int* c, int* potencia, int* moment)
{
	int aux = soldadosActivos(enemigo);
	if(kbhit()){
		
		*key = getch();
		
		switch(*key){
			
			case 72: case 119: case 87: // Flecha arriba, w y W
			
				if(*c < FILAS/2 - 2){
					(*c)++;
					moveAim(-1, jugador,*c);
				}

	        break;
       		
			case 80: case 115: case 83: // Flecha abajo, s y S

				if(*c > (-1)*(FILAS/2) ){
					(*c)--;
					moveAim(1, jugador, *c);
				}
            
            break;
            
            case 13:
            	*potencia = ceil((float) POTENCIA_MAX/2);
            	determinarPotencia(potencia, jugador, *c, key, tablero);
				if(*key == 27){
					break;
				}
            	disparar(jugador, enemigo, *c, *potencia, tablero);

				//Detecta si el jugador acierta un tiro o en su defecto cambia de turno al terminar el tiro.
				if(soldadosActivos(enemigo) >= aux || soldadosActivos(enemigo) == 0){
					switchTurno(jugador, enemigo); //cambio de turno
					setAim(enemigo);
					if(soldadosActivos(enemigo) == 0){
						fflush(stdout);
						printf("\n\nHas eliminado a todos!     \n");
					}
					if(jugador->canon.posX > COLUMNAS/2 && (soldadosActivos(jugador) == 0 || soldadosActivos(enemigo) == 0)){
						(*moment)++;
					}
					
				} else {
					
					setAim(jugador); //se vuelve a ejecutar el apuntado
					printf("\n\nLe has dado a un objetivo!\nVuelves a tirar!\n");
					
				}
				jugador->disparos ++;
				printf("\n");
				system("pause");
				system("cls");
            	*c = 0;

            	
            break;
            
			
		}
		
	}
	
}


void updateTablero(player* jugador, player* enemigo, char tablero[FILAS][COLUMNAS] )//Funcion para actualizar el tablero mostrado a los usuarios.
{
	//reiniciar
	inicializarTablero(tablero);
	//setear el cañon del jugador
	if(jugador->canon.posX < COLUMNAS/2){
		for(i = jugador->canon.posX-2; i < jugador->canon.posX; i++){
    		tablero[jugador->canon.posY][i] = '=';
		}
	} else {
		for(i = jugador->canon.posX+2; i > jugador->canon.posX; i--){
    		tablero[jugador->canon.posY][i] = '=';

		}
	}
    //setear el aimer del jugador
	for(i=0;i<MAX_AIM;i++){
		if(jugador->canon.posX<COLUMNAS/2){
			tablero[jugador->aim[i].posY][jugador->aim[i].posX] = '>';
		} else {
			tablero[jugador->aim[i].posY][jugador->aim[i].posX] = '<';
		}
		

	}
	
//	tablero[jugador->aim[MAX_PTR-1].posY][jugador->aim[MAX_PTR-1].posX] = '>';

	//escribir a los soldados enemigos.
	
	for(i=0;i<MAX_SOLDIER; i++){
		if(enemigo->soldier[i].active){
			tablero[enemigo->soldier[i].posY][enemigo->soldier[i].posX] = '#';
		} else {
			tablero[enemigo->soldier[i].posY][enemigo->soldier[i].posX] = 'X';
		}	
	}
	
    
}


int soldadosActivos(player* jugador)
{
	int cont=0;
	for(i=0;i<MAX_SOLDIER;i++){
		
		if(jugador->soldier[i].active){
			cont++;
		}
		
	}
	
	return cont;
}


void updateFinal(player* jugador, char tablero[FILAS][COLUMNAS] ){

	//setear el cañon del jugador
	if(jugador->canon.posX < COLUMNAS/2){
		for(i = jugador->canon.posX-2; i < jugador->canon.posX; i++){
    		tablero[jugador->canon.posY][i] = '=';
		}
	} else {
		for(i = jugador->canon.posX+2; i > jugador->canon.posX; i--){
    		tablero[jugador->canon.posY][i] = '=';

		}
	}

	tablero[jugador->canon.posY][jugador->canon.posX] = '*';
//	tablero[jugador->aim[MAX_PTR-1].posY][jugador->aim[MAX_PTR-1].posX] = '>';

	//escribir a los soldados enemigos
	
	for(i=0;i<MAX_SOLDIER; i++){
		if(jugador->soldier[i].active){
			tablero[jugador->soldier[i].posY][jugador->soldier[i].posX] = '#';
		} else {
			tablero[jugador->soldier[i].posY][jugador->soldier[i].posX] = 'X';
		}	
	}
	
    
}

//SALIDAS 

void mostrarFeedback(player* jugador1, player* jugador2)//Imprime en la pantalla una retroalimentación para los jugadores.
{
	
	printf("\n\n=============================================\n");
	printf("          ESTADISTICAS JUGADOR 1\n\n");
	printf("El jugador 1 realizo %d disparos\n", jugador1->disparos);
	printf("El jugador 1 elimino a %d soldados enemigos\n", (MAX_SOLDIER - soldadosActivos(jugador2)));
	printf("El jugador 1 se quedo con %d soldados\n\n", soldadosActivos(jugador1));
	printf("=============================================\n");
	printf("          ESTADISTICAS JUGADOR 2\n\n");
	printf("El jugador 2 realizo %d disparos\n", jugador2->disparos);
	printf("El jugador 2 elimino a %d soldados enemigos\n", (MAX_SOLDIER - soldadosActivos(jugador1)));
	printf("El jugador 2 se quedo con %d soldados\n\n", soldadosActivos(jugador2));
	
}


//LLAMADA A LAS OTRAS FUNCIONES DEL PROCESO

int iniciarjuego() //Funcion principal para inicializar el juego, iniciando en ella los turnos.
{
	setlocale(LC_ALL,"");
	
    char tablero[FILAS][COLUMNAS];
    char keyMain; int moment=1, c1=0,c2=0, potencia;
    player jugador[2];
    //Establece que jugador comienza:
    jugador[0].turno=1;
    jugador[1].turno=0;
    jugador[0].disparos=0;
    jugador[1].disparos=0;
    
    inicializarTablero(tablero);
    setCanon(jugador, tablero);
    setSoldier1(&jugador[0], tablero); setSoldier1(&jugador[1], tablero);
    
    do {
    	
    	imprimirTableroAux(tablero);
    	switch(moment){
    		case 1:
    			//if turno jugador 1
    			printf("\nJugador 1, coloca a tus soldados\n\n"); //Explicaciones para el usuario.
    			printf("\tMueve el cursor con las flechas o con WASD\n");
    			printf("\tPara seleccionar la posicion, presiona enter\n");
    			inputSet(&jugador[0], tablero, &keyMain, &c1);
    			//if c1 = 4 cambiar turnos y aumentar momento
    			if (c1==MAX_SOLDIER){
    				switchTurno(&jugador[0], &jugador[1]);
    				moment++;
    				c1=0;
    				
				}
    			break;
    		case 2:
    			printf("\nJugador 2, coloca a tus soldados\n\n");
    			printf("\tMueve el cursor con las flechas o con WASD\n");
    			printf("\tPara seleccionar la posicion, presiona enter\n");
    			inputSet(&jugador[1], tablero, &keyMain, &c1);
    			if (c1==MAX_SOLDIER){
    				switchTurno(&jugador[0], &jugador[1]);
    				moment++;
    				system("cls");
    				
    				setAim(&jugador[0]); 
					setAim(&jugador[1]);
				}
    			break;
    		case 3:
    			
    			printf("\nTurno del jugador %d\n", jugador[0].turno?1:2);
    			printf("\n\tMueve la trayectoria del disparo con las flechas de arriba y abajo, o con 'W' o 'S'\n");
    			
    			if(jugador[0].turno){
    				updateTablero(&jugador[0], &jugador[1], tablero);
    				inputAim(&jugador[0], &jugador[1], tablero,&keyMain, &c2, &potencia, &moment);
    				printf("\n\tObjetivos restantes: %d\n\n", soldadosActivos(&jugador[1]));

				} else {
					updateTablero(&jugador[1], &jugador[0], tablero);
					inputAim(&jugador[1], &jugador[0], tablero, &keyMain, &c2, &potencia, &moment);
					printf("\n\tObjetivos restantes: %d\n\n", soldadosActivos(&jugador[0]));

				}
				
				break;
    			
    		case 4:
    			
    			//Imprime cual de los jugadores gano la partida.
    			system("cls");
    			inicializarTablero(tablero); updateFinal(&jugador[0], tablero); updateFinal(&jugador[1], tablero);
    			imprimirTableroAux(tablero);
    			
    			printf("\n\n================================\n\n");
    			if(soldadosActivos(&jugador[0]) == soldadosActivos(&jugador[1])){
    				printf("Ha sido un empate");
				} else {
					printf("El jugador %d ha ganado.", (soldadosActivos(&jugador[0]) > soldadosActivos(&jugador[1]))? 1: 2);
				}
    			
    			printf("\n\n================================\n\n");
				
				moment++;
				break;
    			
		}	
    	
    	if(keyMain == 27){
    		desplegarPausa(&keyMain, tablero, jugador, &moment, &c1, &c2);
		}

        Sleep(10);
    } while (keyMain != 27 && moment<=4); // '27' es el codigo ASCII para la tecla 'Escape'
    
    if(keyMain!=27){
    	printf("Presione una tecla para ver el feedback del juego.");
	    getch();
	    mostrarFeedback(&jugador[0], &jugador[1]);
	    system("pause");
	}
    
    
    return 0;
}

int main() {
	setlocale(LC_ALL, "");
    int opcionSeleccionada = 0;
    char tecla;

    do {
        imprimirMenu(opcionSeleccionada);

        tecla = getch();
        
        //ENTRADAS
        switch (tecla) {
            case 72: case 119: case 87: // Flecha arriba, w y W.
                opcionSeleccionada = (opcionSeleccionada - 1 + OPCIONES) % OPCIONES;
                break;
            case 80: case 115: case 83: // Flecha abajo, s y S.
                opcionSeleccionada = (opcionSeleccionada + 1) % OPCIONES;
                break;
            case 13: //Tecla "Enter".
                switch (opcionSeleccionada) {
                    case 0:
                        printf("\nIniciando juego...\n"); //jugar.
                        Sleep(500);
                        iniciarjuego(); //PROCESO - Revisar funciones relacionadas para conocer sus ENTRADAS, PROCESOS Y SALIDAS de la funciones relacionadas.
                        //Nota: Revisar primero esta función para entender el resto de funciones relacionadas. Se encuentra arriba de la función main.
                        break;
                        
                    //SALIDAS
                    case 1:
                        printf("\nMostrando creditos...\n");     //Mostrar creditos.
						Sleep(500);               
   						system("cls");
                        printf("\t\t---ABYSS BATTLE--- \n\t       \t Equipo C-FORCE \n \tAlonzo Palacios Rodrigo Alonzo \n \tCuevas Garcia Braulio Samuel \n \tMartincez Martincez pablo \n \tMoo Pan Jareth Jaziel\n");
                        printf("\nPresiona una tecla para volver al menú");
						getch();
						break;
                    case 2:
                        printf("\nSaliendo del programa...\n"); //Saliendo.
                        return 0;
						break;
                }
                break;
        }
    } while (1);

    return 0;
}

