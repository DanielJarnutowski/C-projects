/*
Daniel Jarnutowski
Chris White
Snake project
12-05-18

christopher whites roles:
1) creating border
2) initializing snake speed
3) creating snake

Daniel Jarnutowski roles:
1) signal fuction implementation
2) quit/win functions
3) terminal manipulation for snake use

Daniel/Christopher
1)movement of snake
2)changing of snake direction
3)modification of trophy and trophy collection(when snake eats trophy)
*/

#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <time.h>
#include <signal.h>
#include <sys/time.h>
#include <curses.h>
#include <sys/ioctl.h>
#define DOWN  1
#define UP    2
#define LEFT  3
#define RIGHT 4
#define SNAKEBIT 'O'
#define EMPTY    ' '
#define USER_QUIT  1
#define HITSELF    2
#define HITWALL    3
#define REVERSE    4
#define WIN        5

struct snake_part 
{
    struct snake_part * next;
    int x;
    int y;
    };
typedef struct snake_part SNAKE;
 SNAKE * snake;

 int dir;
 int rows, cols;
 int food =0;
 int TIMESTEP = 200000;
 int SNAKESIZE = 1;

WINDOW * mainwin;
int oldcur;


void SetTimer(void);
void SetSignals(void);
void InitSnake(void);
void MoveSnake(void);
void Draw(void);
void FreeSnake(void);
void PlaceFood(void);
void Error_Quit(char * msg);
void Quit(int reason);
void GetTermSize(int * rows, int * cols);
void handler(int signum);
void ChangeDir(int d);

int main() 
{
    int curs_set(int visibility);// cursor set for invisibilty 
    curs_set (0);
    srand( (unsigned) time(NULL) );
    SetTimer();
    SetSignals();

    if ( (mainwin = initscr()) == NULL ) 
    {
        perror("error initialising ncurses");
        exit(EXIT_FAILURE);
    }

    noecho();
    keypad(mainwin, TRUE);
    oldcur = curs_set(0);

    InitSnake();
    Draw();

    while ( 1 )
    {
        int key = getch();

        switch (key) 
        {

            case KEY_UP:
                ChangeDir(UP);
                break;

            case KEY_DOWN:
                ChangeDir(DOWN);
                break;

            case KEY_LEFT:
                ChangeDir(LEFT);
                break;

            case KEY_RIGHT:
                ChangeDir(RIGHT);
                break;

            case 'Q':
            case 'q':
                Quit(USER_QUIT);
                break;

        }
       
       
        if  (SNAKESIZE== ( ( (rows*2) + (cols*2) )/2) )// perimeter/2 then you win
        {
            Quit(WIN);
        }
        
    }
    
    return EXIT_SUCCESS;
}

void SetTimer(void)//Creates timer for movement speed in milliseconds (christopher white)
{

    struct itimerval it;
    
    timerclear(&it.it_interval);
    timerclear(&it.it_value);
    
    it.it_interval.tv_usec = TIMESTEP;
    it.it_value.tv_usec    = TIMESTEP;
    setitimer(ITIMER_REAL, &it, NULL);
}

void SetSignals(void) // sets the signals that will be used during the snake game (Daniel Jarnutowski)
{

    struct sigaction sa;

    sa.sa_handler = handler;
    sa.sa_flags   = 0;
    sigemptyset(&sa.sa_mask);

    sigaction(SIGTERM, &sa, NULL);
    sigaction(SIGINT,  &sa, NULL);
    sigaction(SIGALRM, &sa, NULL);

    sa.sa_handler = SIG_IGN;
    sigaction(SIGTSTP, &sa, NULL);
}


void InitSnake(void) // uses structure from struct snakepart to create snake in a random position(Christopher White)
{
   
    // uses structure from struct snakepart to create snake(Christopher White)
    dir = rand() % 4 + 1; // gets random int value for direction(chris white)
    SNAKE * temp;
    int x = rand() % (COLS - 10) + 1;
    int y = 10; 
    int i;

    for ( i = 0; i < SNAKESIZE; i++ ) {
        if ( i == 0 ) {
            if ( (snake = malloc(sizeof(SNAKE))) == NULL )
                Error_Quit("couldn't allocate memory for snake");
            temp = snake;
        }
        else {
            if ( (temp->next = malloc(sizeof(SNAKE))) == NULL )
                Error_Quit("couldn't allocate memory for snake");
            temp = temp->next;
        }

        temp->x = x;
        temp->y = y++;
    }
    temp->next = NULL;

    GetTermSize(&rows, &cols);
}

void Draw(void) {  // creates border and places box, snake, and trophy on screen(Christopher White)

    SNAKE * temp = snake;

    box(stdscr, 0, 0);


    while ( temp ) {
        move(temp->y, temp->x);
        
        addch(SNAKEBIT);
        temp = temp->next;
        refresh();
    }

    PlaceFood();
}

void MoveSnake(void) { //Daniel/Christopher alows movement of snake bits while moving
  
    SNAKE * temp = snake;
    int x, y, ch;
    while ( temp->next != NULL )
        temp = temp->next;


    if ( (temp->next = malloc(sizeof(SNAKE))) == NULL )
        Error_Quit("couldn't allocate memory in MoveSnake()");

    switch ( dir ) {
        case DOWN:
            x = temp->x;
            y = temp->y + 1;
            break;

        case UP:
            x = temp->x;
            y = temp->y - 1;
            break;

        case LEFT:
            x = temp->x - 1;
            y = temp->y;
            break;

        case RIGHT:
            x = temp->x + 1;
            y = temp->y;
            break;
    }

    temp       = temp->next;
    temp->next = NULL;
    temp->x    = x;
    temp->y    = y;

    move(y, x);
    int random =0;
    switch ( (ch = inch()) ) 
    {
        case EMPTY:
            addch(SNAKEBIT);

            /*  Delete first snake bit  */

            temp = snake->next;
            move(snake->y, snake->x);
            addch(EMPTY);
            free(snake);
            snake = temp;    /*  Fallthrough  */
                    break;
        case '1': 
        
        case '2': 
        
        case '3':  
     
        case '4': 
   
        case '5':  
       
        case '6': 
         
        case '7': 
      
        case '8':  
        
        case '9': 
      


            /*  Add new snakebit  */
        
            move(y, x);

            if ( ch == food + 48 ) { //places food at random location and increases speed(christopher White)
              
                 addch(SNAKEBIT);
                 SNAKESIZE++;
                 SetTimer();
                 TIMESTEP -= (100*SNAKESIZE);
                 SetTimer();
              
                /*  Place some new food  */
                
                PlaceFood();
            }
           
            refresh();
            break;

        case SNAKEBIT:
            Quit(HITSELF);

        default:
            Quit(HITWALL);
    }
}

void PlaceFood(void) {// adds a food character in a random position to terminal(christopher White)
    food = (rand() % 9) + 1;
    int x, y;

    do {
        x = rand() % (cols - 3) + 1;
        y = rand() % (rows - 3) + 1;
        move(y, x);
    } while ( inch() != EMPTY );
        addch(food + 48);
}

void ChangeDir(int d)//Change direction of each snake bit in the snake (Daniel/Christopher)
{

    SNAKE * temp = snake;

    while ( temp->next != NULL )
        temp = temp->next;

    switch ( d ) {
        case LEFT:
            if ( dir == RIGHT )
                 Quit(REVERSE);
            move(temp->y, temp->x - 1);
            break;

        case RIGHT:
            if ( dir == LEFT )
                 Quit(REVERSE);
            move(temp->y, temp->x + 1);
            break;

        case UP:
            if ( dir == DOWN )
                Quit(REVERSE);
            move(temp->y - 1, temp->x);
            break;

        case DOWN:
            if ( dir == UP )
                Quit(REVERSE);
            move(temp->y + 1, temp->x);
            break;
    }

    dir = d;
}

void FreeSnake(void) { //deallocates the memory in the snake (Christopher/Dan)

    SNAKE * temp = snake;

    while (snake) {
        temp = snake->next;
        free(snake);
        snake = temp;
    }
}

void Error_Quit(char * msg) //handles error message when quitting
{ 

    extern WINDOW * mainwin;
    extern int oldcur;

    delwin(mainwin);
    curs_set(oldcur);
    endwin();
    refresh();
    FreeSnake();

    perror(msg);
    exit(EXIT_FAILURE);
}

void Quit(int reason) { // displays quit message(Daniel Jarnutowski)

    extern WINDOW * mainwin;

    delwin(mainwin);
    endwin();
    refresh();
    FreeSnake();

    switch ( reason ) 
    {
        case HITWALL:
            printf("\nYou hit a wall!\n");
            break;

        case HITSELF:
            printf("\nYou ran into yourself!\n");
            break;
       
        case REVERSE:
            printf("\nyou reversed yourself!\n");
            break;
      
       case WIN:
           printf("\nYou have won the game.Congratulations!\n");
           break;
        
        default:
            printf("\nGoodbye!\n");
            break;
    }

    exit(EXIT_SUCCESS);
}

void GetTermSize(int * rows, int * cols) // gets termianl size(Danniel/Christopher)
{

    struct winsize ws;

    if ( ioctl(0, TIOCGWINSZ, &ws) < 0 ) {
        perror("couldn't get window size");
        exit(EXIT_FAILURE);
    }

    *rows = ws.ws_row;
    *cols = ws.ws_col;
}

void handler(int signum) { // Handles Signals (Daniel Jarnutowski)

    extern WINDOW * mainwin;
    extern int oldcur;

    switch ( signum ) {

        case SIGALRM:

            MoveSnake();
            return;

        case SIGTERM:
        case SIGINT:

            delwin(mainwin);
            curs_set(oldcur);
            endwin();
            refresh();
            FreeSnake();
            exit(EXIT_SUCCESS);

    }
}

