#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <ctype.h>

char board[3][3]; 
char player;
char computer;

void symbols();
void resetBoard();
int checkFreeSpaces();
void printBoard();
void playerMove();
char checkWinner();
void computerMove();
void printWinner(char);

int main()
{
    char choice;
    symbols();

    do
    {
        char winner = ' ';
        resetBoard();
        
        while(checkFreeSpaces() != 0 && winner == ' ')
        {
            printBoard();

            playerMove();
            winner = checkWinner();
            if(checkFreeSpaces() == 0 || winner != ' ')
            {
                break;
            }

            computerMove();
            winner = checkWinner();
            if(checkFreeSpaces() == 0 || winner != ' ')
            {
                break;
            }
        }

        printBoard();
        printWinner(winner);

        printf("Wanna play again? (Y/N): ");
        scanf(" %c",&choice);
        choice = toupper(choice);

        if(choice == 'Y' || choice == 'y')
        {
            symbols();
        }
        
    } while (choice == 'y' || choice == 'Y');
        
    return 0;
}

void symbols()
{
    printf("Choose your symbol: ");
    scanf(" %c", &player);
    do
    {
        printf("Choose computer's symbol: ");
        scanf(" %c", &computer);
    } while(computer == player);
}

void resetBoard()
{
    for(int i = 0; i < 3; i++)
    {
        for (int j = 0; j < 3; j++)
        {
            board[i][j] = ' ';
        }
    }
}

void printBoard()
{
    printf("  %c  |  %c  |  %c  ", board[0][0], board[0][1], board[0][2]);
    printf("\n-----|-----|-----\n");
    printf("  %c  |  %c  |  %c  ", board[1][0], board[1][1], board[1][2]);
    printf("\n-----|-----|-----\n");
    printf("  %c  |  %c  |  %c  \n", board[2][0], board[2][1], board[2][2]);
}

int checkFreeSpaces()
{
    int freeSpaces = 9;
    for(int i = 0; i < 3; i++)
    {
        for(int j = 0; j < 3; j++)
        {
            if(board[i][j] != ' ')
            {
                freeSpaces--;
            }
        }
    }
    return freeSpaces;
}

char checkWinner()
{
    for(int i = 0; i < 3; i++)
    {
        if(board[i][0] == board[i][1] && board[i][0] == board[i][2])
        {
            return board[i][0];
        }
    }
    for(int i = 0; i < 3; i++)
    {
        if(board[0][i] == board[1][i] && board[0][i] == board[2][i])
        {
            return board[0][i];
        }
    }
    if(board[0][0] == board[1][1] && board[0][0] == board[2][2])
    {
        return board[0][0];
    }
    else if(board[0][2] == board[1][1] && board[0][2] == board[2][0])
    {
        return board[0][2];
    }

    return ' ';
}

void playerMove()
{
    int row, col;   
    do
    {
        printf("Enter row number (1 - 3): ");
        scanf("%d",&row);
        printf("Enter column number (1 - 3): ");
        scanf("%d",&col);

        if(board[row - 1][col - 1] == ' ')
        {
            board[row - 1][col - 1] = player;
            break;
        }
        else
        {
            printf("Invalid Move!\n");
        }

    } while(board[row - 1][col - 1] != ' ');   
}

void computerMove()
{
    if(checkFreeSpaces() > 0)
    {
        srand(time(0));
        int x, y;

        do
        {
            x = rand() % 3;
            y = rand() % 3;

        } while(board[x][y] != ' ');
        
        board[x][y] = computer;
    }
    else
    {
        printWinner(' ');
    }    
}

void printWinner(char winner)
{
    if(winner == player)
    {
        printf("\nCONGRATULATIONS!, YOU WIN!\n");
    }
    else if(winner == computer)
    {    
        printf("\nYOU LOSE :(\n");
    }
    else
    {
        printf("\nIT'S A DRAW!\n");
    }
}

