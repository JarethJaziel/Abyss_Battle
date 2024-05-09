#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <time.h>
#include <ctype.h>

#define BOARD_HEIGHT 10
#define BOARD_WIDTH 20
#define NUM_TROOPS 5

// Estructura para representar una tropa
typedef struct {
    int size;
    bool vertical;
    bool hit[NUM_TROOPS];
} Troop;

// Función para inicializar el tablero
void init_board(char board[BOARD_HEIGHT][BOARD_WIDTH]) {
    for (int i = 0; i < BOARD_HEIGHT; i++) {
        for (int j = 0; j < BOARD_WIDTH; j++) {
            board[i][j] = '^'; // Representa un acantilado desde vista aérea
        }
    }
}

// Función para imprimir un tablero
void print_board(char board[BOARD_HEIGHT][BOARD_WIDTH], const char* title) {
    printf("%s\n", title);
    printf(" ---------------------\n");
    for (int i = 0; i < BOARD_HEIGHT; i++) {
        printf("|");
        for (int j = 0; j < BOARD_WIDTH; j++) {
            printf("%c", board[i][j]);
        }
        printf("|%c\n", (i == BOARD_HEIGHT / 2 - 1) ? '_' : ' ');
    }
    printf("--___--_____--_____-_-_____\n\n");
}

// Función para validar las coordenadas ingresadas por el usuario
bool is_valid_coordinate(int row, int col) {
    return row >= 0 && row < BOARD_HEIGHT && col >= 0 && col < BOARD_WIDTH;
}

// Función para validar si se puede colocar una tropa en una posición específica
bool can_place_troop(char board[BOARD_HEIGHT][BOARD_WIDTH], int row, int col, Troop *troop) {
    if (troop->vertical) {
        if (row + troop->size > BOARD_HEIGHT) {
            return false; // La tropa se sale del tablero
        }
        for (int i = 0; i < troop->size; i++) {
            if (board[row + i][col] != '^') {
                return false; // Hay una tropa en el camino
            }
        }
    } else {
        if (col + troop->size > BOARD_WIDTH) {
            return false; // La tropa se sale del tablero
        }
        for (int i = 0; i < troop->size; i++) {
            if (board[row][col + i] != '^') {
                return false; // Hay una tropa en el camino
            }
        }
    }
    return true;
}

// Función para colocar una tropa en el tablero
void place_troop(char board[BOARD_HEIGHT][BOARD_WIDTH], int row, int col, Troop *troop) {
    if (troop->vertical) {
        for (int i = 0; i < troop->size; i++) {
            board[row + i][col] = '#'; // Representa una tropa
        }
    } else {
        for (int i = 0; i < troop->size; i++) {
            board[row][col + i] = '#'; // Representa una tropa
        }
    }
}

// Función para mostrar una representación visual de una tropa en el tablero
void visualize_troop(char board[BOARD_HEIGHT][BOARD_WIDTH], int row, int col, Troop *troop) {
    char symbol = '#'; // Representa una tropa
    if (troop->vertical) {
        for (int i = 0; i < troop->size; i++) {
            board[row + i][col] = symbol;
        }
    } else {
        for (int i = 0; i < troop->size; i++) {
            board[row][col + i] = symbol;
        }
    }
    print_board(board, "ABBY'S BATTLE");
}

// Función para que el jugador coloque sus tropas
void place_troops_manually(char board[BOARD_HEIGHT][BOARD_WIDTH], Troop *troops) {
    for (int i = 0; i < NUM_TROOPS; i++) {
        printf("Coloca la tropa de tamaño %d (formato: fila columna orientación (V/H)): ", troops[i].size);
        int row, col;
        char orientation;
        scanf("%d %d %c", &row, &col, &orientation);
        row--; // Ajustamos al índice base 0
        col--; // Ajustamos al índice base 0
        orientation = toupper(orientation);
        if (!is_valid_coordinate(row, col) || (orientation != 'V' && orientation != 'H')) {
            printf("Coordenadas inválidas. Por favor, inténtalo de nuevo.\n");
            i--;
            continue;
        }
        troops[i].vertical = (orientation == 'V');
        if (!can_place_troop(board, row, col, &troops[i])) {
            printf("La tropa no cabe en esta posición o se superpone con otra. Por favor, inténtalo de nuevo.\n");
            i--;
            continue;
        }
        visualize_troop(board, row, col, &troops[i]);
        place_troop(board, row, col, &troops[i]);
    }
}

// Función para que el jugador realice un ataque
void player_attack(char board[BOARD_HEIGHT][BOARD_WIDTH], char opponent_board[BOARD_HEIGHT][BOARD_WIDTH], Troop *troops, bool *extra_shot) {
    int row, col;
    printf("Introduce las coordenadas de tu ataque (fila columna): ");
    scanf("%d %d", &row, &col);
    row--; // Ajustamos al índice base 0
    col--; // Ajustamos al índice base 0

    if (!is_valid_coordinate(row, col)) {
        printf("Coordenadas inválidas. Por favor, inténtalo de nuevo.\n");
        player_attack(board, opponent_board, troops, extra_shot); // Pedir coordenadas nuevamente
        return;
    }

    if (opponent_board[row][col] == '#') {
        printf("¡Impacto!\n");
        board[row][col] = 'X';
        opponent_board[row][col] = 'X';
        // Marcar la tropa como golpeada
        for (int i = 0; i < NUM_TROOPS; i++) {
            if (troops[i].vertical) {
                if (col >= col && col < col + troops[i].size && row >= row && row < row + troops[i].size) {
                    troops[i].hit[col - row] = true;
                    // Comprobar si la tropa ha sido eliminada
                    bool eliminated = true;
                    for (int j = 0; j < troops[i].size; j++) {
                        if (!troops[i].hit[j]) {
                            eliminated = false;
                            break;
                        }
                    }
                    if (eliminated) {
                        printf("¡Has eliminado una tropa enemiga!\n");
                    }
                    break;
                }
            } else {
                if (row >= row && row < row + troops[i].size && col >= col && col < col + troops[i].size) {
                    troops[i].hit[row - col] = true;
                    // Comprobar si la tropa ha sido eliminada
                    bool eliminated = true;
                    for (int j = 0; j < troops[i].size; j++) {
                        if (!troops[i].hit[j]) {
                            eliminated = false;
                            break;
                        }
                    }
                    if (eliminated) {
                        printf("¡Has eliminado una tropa enemiga!\n");
                    }
                    break;
                }
            }
        }
        *extra_shot = true; // Dar un disparo adicional
    } else if (opponent_board[row][col] == 'X' || opponent_board[row][col] == 'O') {
        printf("Ya has atacado esta posición. Por favor, elige otra.\n");
        player_attack(board, opponent_board, troops, extra_shot); // Pedir coordenadas nuevamente
    } else {
        printf("Fallo...\n");
        board[row][col] = 'O';
        opponent_board[row][col] = 'O';
    }
}

// Función para verificar si un jugador ha ganado
bool check_win(char board[BOARD_HEIGHT][BOARD_WIDTH]) {
    for (int i = 0; i < BOARD_HEIGHT; i++) {
        for (int j = 0; j < BOARD_WIDTH; j++) {
            if (board[i][j] == '#') {
                return false;
            }
        }
    }
    return true;
}

int main() {
    char player1_board[BOARD_HEIGHT][BOARD_WIDTH];
    char player2_board[BOARD_HEIGHT][BOARD_WIDTH];
    char player1_shots[BOARD_HEIGHT][BOARD_WIDTH];
    char player2_shots[BOARD_HEIGHT][BOARD_WIDTH];

    // Inicializar tableros
    init_board(player1_board);
    init_board(player2_board);
    init_board(player1_shots);
    init_board(player2_shots);

    // Definir las tropas de cada jugador
    Troop player1_troops[NUM_TROOPS] = {{5, true}, {4, true}, {3, true}, {3, true}, {2, true}};
    Troop player2_troops[NUM_TROOPS] = {{5, true}, {4, true}, {3, true}, {3, true}, {2, true}};

    // Colocar tropas manualmente
    printf("Jugador 1, coloca tus tropas:\n");
    place_troops_manually(player1_board, player1_troops);
    printf("Jugador 2, coloca tus tropas:\n");
    place_troops_manually(player2_board, player2_troops);

    // Juego
    bool game_over = false;
    bool player1_turn = true;
    bool player1_extra_shot = false;
    bool player2_extra_shot = false;
    while (!game_over) {
        print_board(player1_board, "Tablero del Jugador 1:");
        print_board(player2_board, "Tablero del Jugador 2:");

        if (player1_turn) {
            printf("\nTurno del Jugador 1:\n");
            player_attack(player1_shots, player2_board, player2_troops, &player1_extra_shot);
            game_over = check_win(player2_board);
            if (!player1_extra_shot) {
                player1_turn = false;
            }
            player1_extra_shot = false;
        } else {
            printf("\nTurno del Jugador 2:\n");
            player_attack(player2_shots, player1_board, player1_troops, &player2_extra_shot);
            game_over = check_win(player1_board);
            if (!player2_extra_shot) {
                player1_turn = true;
            }
            player2_extra_shot = false;
        }
    }

    printf("¡Juego terminado! ");
    if (player1_turn) {
        printf("¡El Jugador 1 ha ganado!\n");
    } else {
        printf("¡El Jugador 2 ha ganado!\n");
    }

    return 0;
}

