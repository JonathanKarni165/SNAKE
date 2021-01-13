#include <stdlib.h>
#include <stdio.h> 
#include <conio.h>
#include <windows.h>
#include <math.h>


#define ROWNUM 20
#define COLNUM 40
#define PLAYER_PIECES_LEN 15
#define SIGNAL_ARR_LEN 50
#define FOREACH(iterationNum,code) for(int i=0 ; i< iterationNum; i++){code}

//snake is constructed from pieces ++++++++++ each piece has position in grid and direction to go

typedef struct
{
    int x, y;
}point;

typedef struct
{
    int active;
    point pos;
    char direction;
}piece;

#define PEQL(p1, p2) ((p1.x == p2.x) && (p1.y == p2.y)) 

int frameRate;
int pieceCount;
int isGameEnded = 0, isGamePaused = 0;
int * signalDirArr;
int signalCounter;
piece * playerPieces;
piece applePiece;

void Update(HANDLE threadHandle);
void PrintGrid();
void LoseGame();
void Move();
void ChangeDirection(char c);
void FollowHeadDirection();
int LoopSnake(point p);
int OutOfBounds(point p);
void SpawnApple();
void AddSnakePiece();
void EatApple();
void Win();


//input while code running
DWORD WINAPI ThreadFunc(LPVOID lpParam)
{
    while(1)
    {
        char c = getch();

        if(c == '0')
            isGameEnded = 1;
        
        if(c == 'p')
            isGamePaused = 1;
        
        if(c == 'l')
            isGamePaused = 0;
        ChangeDirection(c);
    }
    return 0;
}

int main()
{
    system("cls");  //clear console
    
    printf("--SNAKE GAME--\np -- pause game\nl -- resume game\n0 -- quit game");

    Sleep(1000);
    //initalizing snake struct
    signalCounter = 0;
    pieceCount = 10;
    frameRate = 100;

    signalDirArr = calloc(SIGNAL_ARR_LEN, sizeof(int));

    FOREACH(SIGNAL_ARR_LEN,
        signalDirArr[i] = 0;
    )

    playerPieces = calloc(PLAYER_PIECES_LEN, sizeof(piece));
    
    for(int i =0; i< 10; i++)
    {
        point p = {10-i, 2};
        char c = 'd';
        playerPieces[i].pos = p;
        playerPieces[i].direction = c;
        playerPieces[i].active = 1;
    }

    for(int i=10; i<15; i++)
    {
        point p = {0, 0};
        char c = 'n';
        playerPieces[i].pos = p;
        playerPieces[i].direction = c;
        playerPieces[i].active = 0;
    }

    //establish input
    HANDLE threadHandle;
    DWORD threadID;

    threadHandle = CreateThread(NULL, 0, ThreadFunc, NULL, 0, &threadID);

    SpawnApple();
    Update(threadHandle);
    return 0;
}

void Update(HANDLE threadHandle)
{
    system("cls");
    LoseGame();
    
    Move();
    FollowHeadDirection();
    PrintGrid();
    EatApple();

    Sleep(frameRate);

    while(isGamePaused){Sleep(20);}

    if(isGameEnded == 0)
        Update(playerPieces);
    else
        CloseHandle(threadHandle);
}

void PrintGrid()
{
    printf("***************************************\n"); //40 *
    for(int row = 1; row< ROWNUM; row++)
    {
        char line[] = "*                                     *\n";
    
        for(int col =1; col <COLNUM; col++)
        {
            point p = {col, row};
            FOREACH(PLAYER_PIECES_LEN,
            
                if(PEQL(p, playerPieces[i].pos))
                    line[col] = '+';
            )

            if(PEQL(p, applePiece.pos))
                line[col] = '@';
        }
        printf(line);
    }
    printf("***************************************"); //40 *
}

void Move()
{
    FOREACH(PLAYER_PIECES_LEN,
    
        if(playerPieces[i].active == 1)
        {
            if(playerPieces[i].direction == 'd')
                playerPieces[i].pos.x += 1;
        
            if(playerPieces[i].direction == 'a')
                playerPieces[i].pos.x -= 1;

            if(playerPieces[i].direction == 'w')
                playerPieces[i].pos.y -= 1;

            if(playerPieces[i].direction == 's')
                playerPieces[i].pos.y += 1;
        }
    )
}

void LoseGame()
{
    if(OutOfBounds(playerPieces[0].pos) || LoopSnake(playerPieces[0].pos))
    {
        isGameEnded =  1;
        system("cls");
        printf("--YOU LOSE--");
        Sleep(1000);
    }
}

int OutOfBounds(point p)
{
    if(p.y > (ROWNUM-1) || p.y <= 0 || p.x >= (COLNUM-2) || p.x <= 0)
        return 1;
    else
        return 0;
}

int LoopSnake(point p)
{
    int count = 0;
    FOREACH(PLAYER_PIECES_LEN,
    
        if(playerPieces[i].pos.x == p.x && playerPieces[i].pos.y == p.y)
            count++;
    )

    if(count < 2)
        return 0;
    return 1;
}

void ChangeDirection(char c)
{
    char previous  = playerPieces[0].direction;

    if(previous == 'w' || previous == 's')
    {
        if(c == 'd')
            playerPieces[0].direction = 'd';
    
        if(c == 'a')
            playerPieces[0].direction = 'a';

        signalDirArr[signalCounter % 20] = 1;
        signalCounter++;
    }
    
    if(previous == 'd' || previous == 'a')
    {
        if(c == 'w')
            playerPieces[0].direction = 'w';
    
        if(c == 's')
            playerPieces[0].direction = 's';

        signalDirArr[signalCounter % 20] = 1;
        signalCounter++;
    }
}

void FollowHeadDirection()
{
    FOREACH(SIGNAL_ARR_LEN,

        if(signalDirArr[i] != 0 && signalDirArr[i] < 15)
        {
            playerPieces[signalDirArr[i]].direction = playerPieces[signalDirArr[i]-1].direction;
            signalDirArr[i] += 1;
        }
        printf("");
    )
}

void SpawnApple()
{
    int x = (rand() % (COLNUM -2)) + 1;
    int y = (rand() % (ROWNUM-1)) + 1;

    point p = {x, y};

    //apple spawned on snake
    FOREACH(PLAYER_PIECES_LEN,
        if(PEQL(playerPieces[i].pos, p))
            SpawnApple();
    )

    applePiece.pos = p;
}

void EatApple()
{
    if(PEQL(playerPieces[0].pos, applePiece.pos))
    {
        if(pieceCount == 14)
            Win();
        SpawnApple();
        AddSnakePiece();
        frameRate *= 0.7;
    }
}

void AddSnakePiece()
{
    pieceCount++;
    int x,y,a = 0,b = 0;

    if(playerPieces[pieceCount-2].direction == 'd')
        a = -1;

    if(playerPieces[pieceCount-2].direction == 'a')
        a = 1;

    if(playerPieces[pieceCount-2].direction == 'w')
        b = 1;

    if(playerPieces[pieceCount-2].direction == 's')
        b = -1;

    x = playerPieces[pieceCount-2].pos.x + a;
    y = playerPieces[pieceCount-2].pos.y + b;

    playerPieces[pieceCount-1].active =1;
    playerPieces[pieceCount-1].pos.x = x;
    playerPieces[pieceCount-1].pos.y = y;
    playerPieces[pieceCount-1].direction = playerPieces[pieceCount-2].direction; 
}

void Win()
{
    isGameEnded = 1;
    system("cls");
    printf("--YOU WON--");
    Sleep(1000);
}
