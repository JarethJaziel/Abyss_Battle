#include <locale.h>
#include <stdio.h>
#include <stdlib.h>
#include <windows.h>
#include <conio.h>

#define FILAS 20
#define COLUMNAS 60
#define MAX_SOLDIER 5
#define MAX_AIM 4


typedef struct {
	int posX;
	int posY;
} pos;

typedef struct {
	
	int turno;
	pos canon;
	pos soldier[MAX_SOLDIER];
	pos aim[MAX_AIM];
	
} player;

int i,j;

void inicializarTablero(char tablero[FILAS][COLUMNAS]) {

    for (i = 0; i < FILAS; i++) {
        for (j = 0; j < COLUMNAS; j++) {
            if (i == 0 || i == FILAS - 1) {
                tablero[i][j] = '-';
            } else if (j == 0 || j == COLUMNAS - 1) {
                tablero[i][j] = '|';
            } else if (j == (COLUMNAS/2)-2 || j == (COLUMNAS/2)+1){
            	tablero[i][j] = '|';
			}else {
                tablero[i][j] = ' ';
            }
        }
    }
}

void imprimirTablero(char tablero[FILAS][COLUMNAS], player jugador[]) {
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    CONSOLE_SCREEN_BUFFER_INFO csbiInfo;
    GetConsoleScreenBufferInfo(hConsole, &csbiInfo);

	printf("Turno del jugador %d\n\n",jugador[0].turno? 1 : 2);
    for (i = 0; i < FILAS; i++) {
        COORD pos = {0, i};
        SetConsoleCursorPosition(hConsole, pos);
        for (j = 0; j < COLUMNAS; j++) {
            printf("%c", tablero[i][j]);
        }
    }
}

void switchTurno ( player jugador[]){
	
	jugador[0].turno = (jugador[0].turno + 1) % 2;
    jugador[1].turno = (jugador[1].turno + 1) % 2;
	
}

void setCanon(player jugador[], char tablero[FILAS][COLUMNAS]){
	
	jugador[0].canon.posX = (COLUMNAS/4)-1;
    jugador[0].canon.posY = (FILAS/2)-1;
    jugador[1].canon.posX = 3 * (COLUMNAS/4) - 1;
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

void setAim(player jugador[], char tablero [FILAS][COLUMNAS]){
	j=jugador[0].canon.posX;
	for(i=0;i<MAX_AIM;i++){

		jugador[0].aim[i].posX=j;
		jugador[0].aim[i].posY=(FILAS/2)-1;
		//tablero[jugador[0].aim[i].posY][jugador[0].aim[i].posX] = '$';
		j++;
	}
	j = jugador[1].canon.posX;
	for(i=0;i<MAX_AIM;i++){
		
		jugador[1].aim[i].posX=j;
		jugador[1].aim[i].posY=(FILAS/2)-1;
		//tablero[jugador[1].aim[i].posY][jugador[1].aim[i].posX] = '$';	
		j--;
	}
	
}

void setSoldier1(player* jugador, char tablero[FILAS][COLUMNAS]){
	
	for(i=0; i<5; i++){
		jugador->soldier[i].posX = jugador->canon.posX;
   		jugador->soldier[i].posY = jugador->canon.posY - 2;
	}
    

    
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
        moveSoldier(jugador, *key, tablero, &(*c));
        if(*key == 13){
        	(*c)++;
		}

    }
    printf("\nTe quedan %d soldados.\n",MAX_SOLDIER-(*c));
    
}

void moveAim (player jugador, int direccion, char tablero[FILAS][COLUMNAS]){
	
}


void inputAim (player jugador[], char tablero[FILAS][COLUMNAS], char* key){
	
	//Debo implementar primero el que pueda poner a sus jugadores, un validador para cuando lo hagan y ya puedo con esto.
/*
	if(jugador[0].turno){
		if(kbhit()){
			*key = getch();
			
			switch(*key){
				case 72: //Flecha arriba
					
					if(tablero[(FILAS/2)+1][(COLUMNAS/4)+2] != '$'){
						moveAim(jugador[0], 1, tablero);
					}
					
					break;
				case 80: //Flecha abajo
					
					if(tablero[(FILAS/2)-3][(COLUMNAS/4)+2] != '$')
					moveAim(jugador[0], -1, tablero)
					break;
			}
		}
	} else {
		
	}
*/
	
	
}

int main() {
    char tablero[FILAS][COLUMNAS];
    char keyMain; int moment=1, c1=0,c2=0;
    player jugador[2];
    //Establece qué jugador comienza:
    jugador[0].turno=1;
    jugador[1].turno=0;
    	
    inicializarTablero(tablero);
    setCanon(jugador, tablero);
    setAim(jugador, tablero);
    setSoldier1(&jugador[0], tablero);
    setSoldier1(&jugador[1], tablero);
    
    do {
    	
    	imprimirTablero(tablero, jugador);
    	switch(moment){
    		case 1:
    			//if turno jugador 1
    			inputSet(&jugador[0], tablero, &keyMain, &c1);
    			//if c1 = 4 cambiar turnos y aumentar momento
    			if (c1==MAX_SOLDIER){
    				switchTurno(jugador);
    				moment++;
    				c1=0;
				}
    			break;
    		case 2:
    			inputSet(&jugador[1], tablero, &keyMain, &c1);
    			if (c1==5){
    				switchTurno(jugador);
    				moment++;
				}
    			break;
    		case 3:
    			// inputAim(jugador, tablero,&keyMain);
    			keyMain = 27;
				break;
    			
		}	
    	

        Sleep(10);
    } while (keyMain != 27); // '27' es el código ASCII para la tecla 'Escape'
    return 0;
}
