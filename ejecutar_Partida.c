#include <locale.h>
#include <stdio.h>
#include <stdlib.h>
#include <windows.h>
#include <conio.h>
#include <math.h>

#define FILAS 21
#define COLUMNAS 91
#define MAX_SOLDIER 12
#define MAX_PTR 30 // FILAS - 1
#define MAX_AIM 19		
#define POTENCIA_MAX 11 // (COLUMNAS-6)/8
#define MAX_DAMAGE 4

typedef struct {
	int posX;
	int posY;
	int active;
} pos;

typedef struct {
	
	int turno;
	pos canon;
	pos soldier [MAX_SOLDIER];
	pos aim[MAX_PTR];
	
} player;

int i,j;

void inicializarTablero(char tablero[FILAS][COLUMNAS]) {

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

void imprimirTableroAux(char tablero[FILAS][COLUMNAS]) {
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    CONSOLE_SCREEN_BUFFER_INFO csbiInfo;
    GetConsoleScreenBufferInfo(hConsole, &csbiInfo);

    for (i = 0; i < FILAS; i++) {
        COORD pos = {0, i};
        SetConsoleCursorPosition(hConsole, pos);
        for (j = 0; j < COLUMNAS; j++) {
            printf("%c", tablero[i][j]);
        }
    }
}

void imprimirTablero(char tablero[FILAS][COLUMNAS], player jugador[]) {
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    CONSOLE_SCREEN_BUFFER_INFO csbiInfo;
    GetConsoleScreenBufferInfo(hConsole, &csbiInfo);

	printf("\nTurno del jugador %d\n",jugador[0].turno? 1 : 2);
    for (i = 0; i < FILAS; i++) {
        COORD pos = {0, i};
        SetConsoleCursorPosition(hConsole, pos);
        for (j = 0; j < COLUMNAS; j++) {
            printf("%c", tablero[i][j]);
        }
		
    }
}

int f(int x, float m, int origen){
	
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

void switchTurno ( player* jugador1, player* jugador2){
	
	jugador1->turno = (jugador1->turno + 1) % 2;
	jugador2->turno = (jugador2->turno + 1) % 2;
	
}

void setCanon(player jugador[], char tablero[FILAS][COLUMNAS]){
	
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

void setAim(player* jugador){
	
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

void setSoldier1(player* jugador, char tablero[FILAS][COLUMNAS]){
	
	for(i=0; i<MAX_SOLDIER; i++){
	//	jugador->soldier[i].posX = jugador->canon.posX;
   	//	jugador->soldier[i].posY = jugador->canon.posY - 2;
   		jugador->soldier[i].active = 1;
	}
    	
	jugador->soldier[0].posX = jugador->canon.posX;
   	jugador->soldier[0].posY = jugador->canon.posY - 2;
    
}

void moveSoldier(player* jugador, int direccion, char tablero[FILAS][COLUMNAS], int* c){
    int nuevaPosX = jugador->soldier[*c].posX;
    int nuevaPosY = jugador->soldier[*c].posY;

    switch(direccion){
        case 72: case 119: case 87: // Flecha arriba, w y W
      
			if(tablero[nuevaPosY-1][nuevaPosX] == ' '){
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
        
        	tablero[jugador->soldier[*c].posY][jugador->soldier[*c].posX] = '#';
        	
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
    
    
    
    if(nuevaPosX != jugador->soldier[*c].posX || nuevaPosY != jugador->soldier[*c].posY){
    	tablero[jugador->soldier[*c].posY][jugador->soldier[*c].posX] = ' ';
    	jugador->soldier[*c].posX = nuevaPosX;
  		jugador->soldier[*c].posY = nuevaPosY;
 
	}

}

void siHayPuesto(player* jugador, char tablero[FILAS][COLUMNAS], int c){
	
	if(tablero[jugador->soldier[c].posY][jugador->soldier[c].posX] == '#'){
		jugador->soldier[c].posX ++;
		siHayPuesto(jugador, tablero, c);
	} 
	
}


void inputSet(player* jugador, char tablero[FILAS][COLUMNAS], char* key, int* c){
	
	siHayPuesto(jugador, tablero, *c);
	tablero[jugador->soldier[*c].posY][jugador->soldier[*c].posX] = '0';
	
	
	if(kbhit()){
        *key = getch();
        moveSoldier(jugador, *key, tablero, c);
        if(*key == 13){
        	(*c)++;
		}

    }
    printf("\nTe quedan %d soldados.\n",MAX_SOLDIER-(*c));
    
}

void moveAim (int direccion, player* jugador, int c){
	
	for(i=0;i<MAX_PTR; i++){

		jugador->aim[i].posY = f(jugador->aim[i].posX, (float)c/MAX_PTR, jugador->canon.posX);
		
	}
	
}

void determinarPotencia(int* potencia, player* jugador, int c, char* key, char tablero[FILAS][COLUMNAS]){
	*key=72;
	system("cls");
	do{
		imprimirTableroAux(tablero);

		printf("\n\nSeleccione la potencia:\n\n\t");
		if (*potencia == 1) {
            printf("< %d > ", *potencia);
        } else {
            printf("%d... < %d > ", *potencia - 1, *potencia);
        }
        if(*potencia != POTENCIA_MAX){
        	printf("...%d", *potencia + 1);
		}
		if(kbhit()){
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

void verifVacio( char tablero[FILAS][COLUMNAS], int posY, int* posX, int direccion, float m, int origen){
	
	if(tablero[posY][*posX] != ' '){
		*posX += direccion;
		verifVacio(tablero, f(*posX, m, origen), posX, direccion, m, origen);
	}
	
}

void disparar(player* jugador, player* enemigo, int c, int potencia, char tablero[FILAS][COLUMNAS]) {
	int posX, posY, direccion, cont;
	char key;
	
	if(jugador->canon.posX < COLUMNAS/2){
		posX=COLUMNAS/2 + 2 + (potencia * 4);
		direccion = 1;		
	} else {
		posX=COLUMNAS/2 + 2 - (potencia * 4);
		direccion = -1;	
	}
	
	for(i = posX; i > (posX - MAX_DAMAGE); i--){
		posY = f(i, (float) c/MAX_PTR, jugador->canon.posX);
		for(j = 0; j<MAX_SOLDIER; j++){
			if((posY == enemigo->soldier[j].posY || posY - 1 == enemigo->soldier[j].posY || posY + 1 == enemigo->soldier[j].posY) && i == enemigo->soldier[j].posX ){
				enemigo->soldier[j].active = 0;
			}
		}
			
	}
	
	// animaci�n de disparo;
	
	cont = jugador->canon.posX;
	
	for(i=0; i<MAX_AIM; i++){
		tablero[jugador->aim[i].posY][jugador->aim[i].posX] = ' ';
	}
	
	if(jugador->canon.posX < COLUMNAS/2){
		do {
		
			tablero[f(cont, (float) c/MAX_PTR, jugador->canon.posX)][cont] = '*';
			
			imprimirTableroAux(tablero);
			printf("\n\nPresiona enter para omitir.");
		
			Sleep(10);
			
			tablero[f(cont, (float) c/MAX_PTR, jugador->canon.posX)][cont] = ' ';
			
			cont += direccion;
			
			verifVacio(tablero, f(cont, (float) c/MAX_PTR, jugador->canon.posX), &cont, direccion, (float)c/MAX_PTR, jugador->canon.posX);
			if(kbhit()){
				key = getch();
				if (key==13){
					break;
				}
			}
		} while (cont < posX);
	} else {
		do {
		
			tablero[f(cont, (float) c/MAX_PTR, jugador->canon.posX)][cont] = '*';
			
			imprimirTableroAux(tablero);
			printf("\n\nPresiona enter para omitir.");
		
			Sleep(10);
			
			tablero[f(cont, (float) c/MAX_PTR, jugador->canon.posX)][cont] = ' ';
			
			
			
			cont += direccion;
			verifVacio(tablero, f(cont, (float) c/MAX_PTR, jugador->canon.posX), &cont, direccion, (float)c/MAX_PTR, jugador->canon.posX);
			if(kbhit()){
				key = getch();
				if (key==13){
					break;
				}
			}
		} while (cont > posX && f(cont, (float) c/MAX_PTR, jugador->canon.posX) < FILAS-1 && f(cont, (float) c/MAX_PTR, jugador->canon.posX) > 0 && cont > 0 && cont < COLUMNAS-1);
	}
	
	for(i = posX - 1; i > (posX - MAX_DAMAGE + 1); i--){
		posY = f(i, (float) c/MAX_PTR, jugador->canon.posX);
		
		if(tablero[posY][i]==' '){
			tablero[posY][i] = 'O';
		} else if (tablero[posY][i] == '#'){
			tablero[posY][i] = 'X';
		}
			
	}
	
	imprimirTableroAux(tablero);
	Sleep(200);
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
	printf("\n");
	system("pause");
	system("cls");
	
}





void inputAim (player* jugador, player* enemigo, char tablero[FILAS][COLUMNAS], char* key, int* c, int* potencia, int* moment){
	
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

				if(*c > (-1)*(FILAS/2-1) ){
					(*c)--;
					moveAim(1, jugador, *c);
				}
            
            break;
            
            case 13:
            	*potencia = ceil((float) POTENCIA_MAX/2);
            	determinarPotencia(potencia, jugador, *c, key, tablero);

            	disparar(jugador, enemigo, *c, *potencia, tablero);

				if(jugador->canon.posX > COLUMNAS/2){
					if(soldadosActivos(jugador) == 0 || soldadosActivos(enemigo) == 0){
						(*moment)++;
					}
				}
				switchTurno(jugador, enemigo);
            	*c = 0;

            	setAim(enemigo);
            break;
            
			
		}
		
	}
	
}


void updateTablero(player* jugador, player* enemigo, char tablero[FILAS][COLUMNAS] ){
	//reiniciar
	inicializarTablero(tablero);
	//setear el cañón del jugador
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

	//escribir a los soldados enemigos
	
	for(i=0;i<MAX_SOLDIER; i++){
		if(enemigo->soldier[i].active){
			tablero[enemigo->soldier[i].posY][enemigo->soldier[i].posX] = '#';
		} else {
			tablero[enemigo->soldier[i].posY][enemigo->soldier[i].posX] = 'X';
		}	
	}
	
    
}

int soldadosActivos(player* jugador){
	int cont=0;
	for(i=0;i<MAX_SOLDIER;i++){
		
		if(jugador->soldier[i].active){
			cont++;
		}
		
	}
	
	return cont;
}


void updateFinal(player* jugador, char tablero[FILAS][COLUMNAS] ){

	//setear el cañón del jugador
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


int main() {
    char tablero[FILAS][COLUMNAS];
    char keyMain; int moment=1, c1=0,c2=0, potencia;
    player jugador[2];
    //Establece qué jugador comienza:
    jugador[0].turno=1;
    jugador[1].turno=0;
    	
    inicializarTablero(tablero);
    setCanon(jugador, tablero);
    setSoldier1(&jugador[0], tablero); setSoldier1(&jugador[1], tablero);
    
    //printf("Cuantos jugadores")
    
    do {
    	
    	imprimirTablero(tablero, jugador);
    	switch(moment){
    		case 1:
    			//if turno jugador 1
    			inputSet(&jugador[0], tablero, &keyMain, &c1);
    			//if c1 = 4 cambiar turnos y aumentar momento
    			if (c1==MAX_SOLDIER){
    				switchTurno(&jugador[0], &jugador[1]);
    				moment++;
    				c1=0;
    				
				}
    			break;
    		case 2:
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
    			
    			if(jugador[0].turno){
    				updateTablero(&jugador[0], &jugador[1], tablero);
    				inputAim(&jugador[0], &jugador[1], tablero,&keyMain, &c2, &potencia, &moment);
    				printf("\nObjetivos restantes: %d\n\n", soldadosActivos(&jugador[1]));

				} else {
					updateTablero(&jugador[1], &jugador[0], tablero);
					inputAim(&jugador[1], &jugador[0], tablero, &keyMain, &c2, &potencia, &moment);
					printf("\nObjetivos restantes: %d\n\n", soldadosActivos(&jugador[0]));

				}
				
				break;
    			
    		case 4:
    			
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
    			
    			keyMain = 27;
				break;
    			
		}	
    	
    	// if keyMain = 27 => despliega el menu de pausa

        Sleep(10);
    } while (keyMain != 27); // '27' es el código ASCII para la tecla 'Escape'
    return 0;
}
