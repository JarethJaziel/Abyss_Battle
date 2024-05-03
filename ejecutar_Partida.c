#include <locale.h>
#include <stdio.h>
#include <stdlib.h>
#include <windows.h>
#include <conio.h>

#define FILAS 20
#define COLUMNAS 60
#define MAX_SOLDIER 6
#define MAX_AIM 7


typedef struct {
	int posX;
	int posY;
	int active;
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

	printf("\nTurno del jugador %d\n",jugador[0].turno? 1 : 2);
    for (i = 0; i < FILAS; i++) {
        COORD pos = {0, i};
        SetConsoleCursorPosition(hConsole, pos);
        for (j = 0; j < COLUMNAS; j++) {
            printf("%c", tablero[i][j]);
        }
    }
}

void switchTurno ( player* jugador1, player* jugador2){
	
	jugador1->turno = (jugador1->turno + 1) % 2;
	jugador2->turno = (jugador2->turno + 1) % 2;
	
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

void setAim(player* jugador){
	
	j=jugador->canon.posX;
	
	for(i=0;i<MAX_AIM;i++){

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
		jugador->soldier[i].posX = jugador->canon.posX;
   		jugador->soldier[i].posY = jugador->canon.posY - 2;
   		jugador->soldier[i].active = 1;
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

void moveAim (int direccion, player* jugador, char tablero[FILAS][COLUMNAS], int p, int* c){
	
	if(p != 0){
		
		if(jugador->canon.posX < COLUMNAS/2){ //verifica qué jugador es
			if(tablero[jugador->aim[p].posY][jugador->aim[p].posX + 1 ] == ' '){
				jugador->aim[p].posY += direccion * -1;
				tablero[jugador->aim[p].posY][jugador->aim[p].posX] = '$';
				if(p == MAX_AIM-1){
					(*c) += direccion;
				}				
				moveAim(direccion, jugador, tablero, p-1, c);
				
			}
		} else {
			if(tablero[jugador->aim[p].posY][jugador->aim[p].posX - 1 ] == ' '){
				jugador->aim[p].posY += direccion * -1;
				if(p == MAX_AIM-1){
					(*c) += direccion;
				}
				tablero[jugador->aim[p].posY][jugador->aim[p].posX] = '$';
				moveAim(direccion, jugador, tablero, p-1, c);
			}
		}
		
		
	}
	
	
	
}


void inputAim (player* jugador, player* enemigo, char tablero[FILAS][COLUMNAS], char* key, int* c){
	
	if(kbhit()){
		
		*key = getch();
		
		switch(*key){
			
			case 72: case 119: case 87: // Flecha arriba, w y W
			
				if(*c < MAX_AIM - 1){
					moveAim(1, jugador, tablero, MAX_AIM-1, c);
				}
				
				
				
	            break;
       		
			case 80: case 115: case 83: // Flecha abajo, s y S

				if(*c > (-1)*MAX_AIM + 1 ){
					moveAim(-1, jugador, tablero, MAX_AIM-1, c);
				}
            
            	break;
            
            case 13:
            	switchTurno(jugador, enemigo);
            	setAim(jugador);
            	*c = 0;
            	break;
            
			
		}
		
	}

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

		tablero[jugador->aim[i].posY][jugador->aim[i].posX] = '$';

	}
	//escribir a los soldados enemigos
	
	for(i=0;i<MAX_SOLDIER; i++){
		if(enemigo->soldier[i].active){
			tablero[enemigo->soldier[i].posY][enemigo->soldier[i].posX] = '#';
		}		
	}
	
    
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
    setAim(&jugador[0]); setAim(&jugador[1]);
    setSoldier1(&jugador[0], tablero); setSoldier1(&jugador[1], tablero);
    
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
				}
    			break;
    		case 3:
    			
    			if(jugador[0].turno){
    				updateTablero(&jugador[0], &jugador[1], tablero);
    				inputAim(&jugador[0], &jugador[1], tablero,&keyMain, &c2);
    				printf("\n%d \n",c2);
				} else {
					updateTablero(&jugador[1], &jugador[0], tablero);
					inputAim(&jugador[1], &jugador[0], tablero, &keyMain, &c2);
					printf("\n%d \n",c2);
				}
    			
    			// if todos los de uno están inactivos: moment++
    			
    			
    			
    			//keyMain = 27;
				break;
    			
		}	
    	

        Sleep(10);
    } while (keyMain != 27); // '27' es el código ASCII para la tecla 'Escape'
    return 0;
}
