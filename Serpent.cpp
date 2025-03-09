#include <stdio.h>
#include <stdlib.h>
#include <conio.h>
#include <windows.h>
#include <time.h>

#define WIDTH 40
#define HEIGHT 20
#define MAX_LENGTH 100
#define APPLES 3  // Nombre de pommes à générer

typedef struct {
    int x, y;
} Point;

Point snake[MAX_LENGTH];
Point apples[APPLES];  // Tableau pour les 3 pommes
int length = 5;
int direction = 77;  // Direction initiale: droite
int speed = 150;      // Vitesse initiale
int game_over = 0;  // 0 = en cours, 1 = perdu

void gotoxy(int x, int y) {			//procédure pour définir où un nouveau caractère sera écrit.
    COORD coord;
    coord.X = x;	
    coord.Y = y;
    SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), coord);
}

void hidecursor() {					//procédure pour cache le courseur de texte
    CONSOLE_CURSOR_INFO cursorInfo;
    cursorInfo.bVisible = FALSE;
    cursorInfo.dwSize = 1;
    SetConsoleCursorInfo(GetStdHandle(STD_OUTPUT_HANDLE), &cursorInfo);
}

void draw_border() {				//procédure pour créer le périmètre du jeu
    for (int i = 0; i <= WIDTH; i++) {
        gotoxy(i, 0);
        printf("-");
        gotoxy(i, HEIGHT);
        printf("-");
    }
    for (int i = 0; i <= HEIGHT; i++) {
        gotoxy(0, i);
        printf("|");
        gotoxy(WIDTH, i);
        printf("|");
    }
}

int is_on_snake(int x, int y) {       	//fonction qui empêche la creation de la nourriture si se trouve sur le serpent
    for (int i = 0; i < length; i++) {
        if (x == snake[i].x && y == snake[i].y) {
            return 1;
        }
    }
    return 0;
}

void generate_apples() {			//procédure pour la génération de la nourriture
    for (int i = 0; i < APPLES; i++) {
        do {
            apples[i].x = rand() % (WIDTH - 1) + 1;
            apples[i].y = rand() % (HEIGHT - 1) + 1;
        } while (is_on_snake(apples[i].x, apples[i].y));
    }
}

void init_game() {			//démarrage du jeu avec l'appell des procédures nécéssaires et la création du serpent
    system("cls");
    hidecursor();
    srand(time(NULL));
    for (int i = 0; i < length; i++) {
        snake[i].x = WIDTH / 2 - i;
        snake[i].y = HEIGHT / 2;
    }
    generate_apples();
    draw_border();
}

void draw() {			//écriture des caractères du serpent et de la nourriture
    for (int i = 0; i < APPLES; i++) {
        gotoxy(apples[i].x, apples[i].y);
        printf("O");
    }
    for (int i = 0; i < length; i++) {
        gotoxy(snake[i].x, snake[i].y);
        printf("#");
    }
    gotoxy(snake[length].x, snake[length].y);
    printf(" ");
}

void save_game() {			//procédure pour sauvegarder le jeu et le mettre en pause
    if (game_over) {  // Interdire la sauvegarde si le jeu est terminé
        gotoxy(0, HEIGHT + 1);
        printf("Impossible de sauvegarder : Partie terminée !");
        fflush(stdout);
        return;
    }

    FILE *file = fopen("savegame.txt", "w");
    if (file == NULL) {
        gotoxy(0, HEIGHT + 1);
        printf("Erreur : Impossible de sauvegarder la partie !");
        fflush(stdout);
        return;
    }

    // Sauvegarde des données de la partie
    fprintf(file, "%d %d %d\n", length, direction, game_over);
    for (int i = 0; i < length; i++) {
        fprintf(file, "%d %d\n", snake[i].x, snake[i].y);
    }
    fclose(file);

    gotoxy(0, HEIGHT + 2);
    printf("Partie sauvegardée avec succès ! Appuyez sur une touche pour continuer...");
    fflush(stdout);
    _getch();

    gotoxy(0, HEIGHT + 2);
    printf("                                                                         ");
    fflush(stdout);
}



void update() {			//procédure pour gérer la direction du serpent
    if (game_over) return;  // Arrêter les mises à jour si le jeu est terminé

    Point new_head = snake[0];
    if (_kbhit()) {
        int key = _getch();
        if (key == 224) {  // Gestion des touches fléchées
            key = _getch();
            if ((key == 72 && direction != 80) || (key == 80 && direction != 72) ||
                (key == 75 && direction != 77) || (key == 77 && direction != 75)) {
                direction = key;
            }
        } else if ((key == 's' || key == 'S') && !game_over) {  // Sauvegarde seulement si le jeu n'est pas terminé
            save_game();
        }
    }

    switch (direction) {
        case 72: new_head.y--; break;
        case 80: new_head.y++; break;
        case 75: new_head.x--; break;
        case 77: new_head.x++; break;
    }

    if (new_head.x <= 0 || new_head.x >= WIDTH || new_head.y <= 0 || new_head.y >= HEIGHT) {
        game_over = 1;
        printf("Game Over!\n");
        _getch();
        return;
    }

    for (int i = 1; i < length; i++) {
        if (new_head.x == snake[i].x && new_head.y == snake[i].y) {
            game_over = 1;
            printf("Game Over!\n");
            _getch();
            return;
        }
    }

    for (int i = length; i > 0; i--) {
        snake[i] = snake[i - 1];
    }
    snake[0] = new_head;

    for (int i = 0; i < APPLES; i++) {
        if (new_head.x == apples[i].x && new_head.y == apples[i].y) {
            generate_apples();
            length++;
        }
    }
}


void load_game() {			//procédure pour rélancer une partie souvegardée
    FILE *file = fopen("savegame.txt", "r");
    if (file) {
        fscanf(file, "%d %d %d", &length, &direction, &game_over);
        for (int i = 0; i < length; i++) {
            fscanf(file, "%d %d", &snake[i].x, &snake[i].y);
        }
        fclose(file);
        if (game_over) {  // Si la partie sauvegardée est déjà perdue
            printf("Impossible de charger : Partie déjà perdue !\n");
            _getch();
            exit(0);
        }
    } else {
        printf("Aucune sauvegarde trouvée.\n");
        _getch();
    }
}

void menu() {   //procédure pour l'affichage du menu
    system("cls");
    printf("Jeu du Serpent\n");
    printf("1. Nouvelle Partie\n");
    printf("2. Charger Partie\n");
    printf("3. Quitter\n");
    printf("Choisissez une option : ");
    char choice = _getch();
    if (choice == '2') {
        load_game();
    } else if (choice == '3') {
        exit(0);
    }
    system("cls");
    printf("Choisissez la vitesse:\n");
    printf("1. Lente\n");
    printf("2. Rapide\n");
    char speed_choice = _getch();
    if (speed_choice == '2') {
        speed = 100;
    }
}

int main() {	//programme principale
    menu();
    
    init_game();
    while (1) {
        update();
        draw();
        Sleep(speed);
    }
    return 0;
}


