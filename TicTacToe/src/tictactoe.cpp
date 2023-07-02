#include <raylib.h>
#include <iostream>
#include <vector>
#include <ctime>
#include <cmath>
using std::cout;
using std::vector;
// have fun understanding this! I also dont understand it either fuck
// I HATE DESIGNING UI I HATE DESIGNING UI I HATE DESIGNING UI
const int screenWidth = 800;
const int screenHeight = 600;
int CurrentlyPlaying = 0;
int ScreenNum = 0;
bool Scoring = false;
bool IsDraw = false;

struct Player
{
    int score = 0;
    Color PlrColor;
    Player(Color SetColor)
    {
        PlrColor = SetColor;
    }
};
vector<Player> Players;
struct GameObject
{
    int xposition;
    int yposition;
    int width;
    int height;
    GameObject(int xset, int yset, int widthset, int heightset)
    {
        xposition = xset;
        yposition = yset;
        width = widthset;
        height = heightset;
    }
};
struct Board : GameObject
{
    Color CurrentColor = GRAY;
    bool Claimed = false;
    bool Crossed = false;
    Board(int xset, int yset, int widthset, int heightset) : GameObject(xset, yset, widthset, heightset)
    {
    }
};
struct Button : GameObject
{
    Color CurrentColor = RED;
    int SetInteger;
    std::string ButtonText;
    Button(int xset, int yset, int widthset, int heightset, int setvalue, std::string SetText) : GameObject(xset, yset, widthset, heightset)
    {
        SetInteger = setvalue;
        ButtonText = SetText;
    }
};
vector<vector<Board>> PlayBoards;
void GenerateBoard(GameObject &MainBoard, int BoardSize);
void DrawBoard(Vector2 &MousePosition, int BoardSize, int plramount);
void ButtonFunction(Vector2 MousePosition, vector<Button> &Buttons, int buttonamount, int *valuetochange, int fontsize);
void ChangeWindowFunction(Vector2 MousePosition, Button &Buttons, GameObject &MarginedBoard, int *valuetochange, int fontsize, int playersize, int boardsize);
int main()
{
    InitWindow(screenWidth, screenHeight, "TIC TAC TOE SIMPLE");
    // IMPORTANT VARIABLES
    int PlayerAmount;
    int BoardSize;
    // MAIN GAME VARIABLES

    vector<Color> PlayerColor = {RED, GREEN};
    for (int i = 0; i < PlayerColor.size(); i++)
    {
        Players.push_back(Player(PlayerColor[i]));
    }
    vector<int> MainBoardCoordinate = {200, 100, 410, 410};
    GameObject MainBoard = GameObject(MainBoardCoordinate[0], MainBoardCoordinate[1], MainBoardCoordinate[2], MainBoardCoordinate[3]);
    vector<vector<Board>> Tiles;
    int margin = 10;
    // TODO: MOVE THIS BRO
    GameObject MarginedBoard = GameObject(MainBoard.xposition + margin, MainBoard.yposition + margin, MainBoard.width - 2 * margin, MainBoard.height - 2 * margin);
    int boardbuttonamount = 4;
    int BoardButtonInfo[2][5] = {{100, 150, 200, 50, 3}, {500, 150, 200, 50, 3 + boardbuttonamount}};
    vector<Button> BoardButton;
    for (int test = 0; test < sizeof(BoardButtonInfo) / sizeof(BoardButtonInfo[0]); test++)
    {
        for (int j = 0; j < boardbuttonamount; j++)
        {
            BoardButton.push_back(Button(BoardButtonInfo[test][0], BoardButtonInfo[test][1] + (j * (BoardButtonInfo[test][3] + 10)), BoardButtonInfo[test][2],
                                         BoardButtonInfo[test][3], BoardButtonInfo[test][4] + j, std::to_string(BoardButtonInfo[test][4] + j) + "x" + std::to_string(BoardButtonInfo[test][4] + j)));
        }
    }
    vector<Button> PlayerButton;
    std::string PlayerText[] = {"One Player", "Two Players"};
    for (int i = 0; i < sizeof(BoardButtonInfo) / sizeof(BoardButtonInfo[0]); i++)
    {
        PlayerButton.push_back(Button(BoardButtonInfo[i][0], BoardButtonInfo[i][1] + (2 + boardbuttonamount) * (BoardButton[0].height), BoardButtonInfo[i][2], BoardButtonInfo[i][3], 1 + i, PlayerText[i]));
    }
    // honestly i gave up at this point
    Button PlayButton = Button(250, 520, 300, 75, 1, "PLAY");
    Button ReturnToMenu = Button(250, 520, 300, 75, 0, "QUIT");

    Button Retry = Button(50, 520, 300, 75, 1, "RETRY");
    Button QuitButton = Button(screenWidth - Retry.width - 50, 520, 300, 75, 0, "QUIT"); // change this later
    SetTargetFPS(60);
    while (!WindowShouldClose())
    {

        Vector2 MousePosition = GetMousePosition();
        BeginDrawing();
        ClearBackground(BROWN);
        // i love boilerplate code!
        switch (ScreenNum)
        {
        case 0:
        {
            DrawText("TIC TAC TOE", 125, 20, 80, BLACK);
            DrawText("Board Size", screenWidth / 2 - 140, BoardButton[0].yposition - 50, 50, ORANGE);
            DrawText("Player Amount", screenWidth / 2 - 185, PlayerButton[0].yposition - 50, 50, ORANGE);
            ButtonFunction(MousePosition, BoardButton, BoardButton.size(), &BoardSize, 40);
            ButtonFunction(MousePosition, PlayerButton, PlayerButton.size(), &PlayerAmount, 30);
            ChangeWindowFunction(MousePosition, PlayButton, MarginedBoard, &ScreenNum, 70, PlayerAmount, BoardSize);
        }
        break;
        case 1:
        {
            ChangeWindowFunction(MousePosition, ReturnToMenu, MarginedBoard, &ScreenNum, 70, PlayerAmount, BoardSize);
            //  probably play your game or something
            DrawRectangle(MainBoard.xposition, MainBoard.yposition, MainBoard.width, MainBoard.height, YELLOW);
            // DrawRectangle(MarginedBoard.xposition, MarginedBoard.yposition, MarginedBoard.width, MarginedBoard.height, PINK);
            DrawBoard(MousePosition, BoardSize, PlayerAmount);
            std::string PlayerTurn = "Player " + std::to_string(CurrentlyPlaying + 1) + "'s turn";
            DrawText(PlayerTurn.c_str(), 0, 0, 30, PlayerColor[CurrentlyPlaying]);
        }
        break;
        case 2:
        {
            std::string Winner = "Player " + std::to_string(CurrentlyPlaying + 1) + " Wins!";
            int font = 60;
            if (IsDraw == true)
            {
                DrawText("It's a draw!", screenWidth / 2 - 163, MainBoard.yposition - font, font, GOLD);
            }
            else
            {
                DrawText(Winner.c_str(), screenWidth / 2 - (Winner.length() * font / 4.12), MainBoard.yposition - font, font, Players[CurrentlyPlaying].PlrColor);
            }
            ChangeWindowFunction(MousePosition, Retry, MarginedBoard, &ScreenNum, 67, PlayerAmount, BoardSize);
            ChangeWindowFunction(MousePosition, QuitButton, MarginedBoard, &ScreenNum, 70, PlayerAmount, BoardSize);
            // debugging purposes
            GenerateBoard(MarginedBoard, BoardSize);
            DrawRectangle(MainBoard.xposition, MainBoard.yposition, MainBoard.width, MainBoard.height, YELLOW);
            DrawBoard(MousePosition, BoardSize, PlayerAmount);
        }
        break;
        }

        EndDrawing();
    }
    CloseWindow();
    return 0;
}
// this fucks my head so much istg

void ChangeWindowFunction(Vector2 MousePosition, Button &Buttons, GameObject &MarginedBoard, int *valuetochange, int fontsize, int playersize, int boardsize)
{
    DrawRectangle(Buttons.xposition, Buttons.yposition, Buttons.width, Buttons.height, Buttons.CurrentColor);
    DrawText(TextFormat(Buttons.ButtonText.c_str()), Buttons.xposition + Buttons.width / 2 - ((fontsize / 3) * (Buttons.ButtonText.length())), Buttons.yposition + (Buttons.height - fontsize), fontsize, BLACK);
    if (playersize <= 0 || playersize > 2 || boardsize < 3 || boardsize > 10)
    {
        return;
    }
    Buttons.CurrentColor = GREEN;
    if (MousePosition.x <= Buttons.xposition || MousePosition.x >= Buttons.xposition + Buttons.width ||
        MousePosition.y <= Buttons.yposition || MousePosition.y >= Buttons.yposition + Buttons.height)
    {
        return;
    }
    if (!IsMouseButtonPressed(MOUSE_BUTTON_LEFT))
    {
        return;
    }
    *valuetochange = Buttons.SetInteger;
    int changedowo = *valuetochange;
    if (changedowo != 1)
    {
        return;
    }
    IsDraw = false;
    CurrentlyPlaying = 0;
    PlayBoards.clear();
    GenerateBoard(MarginedBoard, boardsize);
}
void ButtonFunction(Vector2 MousePosition, vector<Button> &Buttons, int buttonamount, int *valuetochange, int fontsize)
{

    for (int i = 0; i < buttonamount; i++)
    {
        DrawRectangle(Buttons[i].xposition, Buttons[i].yposition, Buttons[i].width, Buttons[i].height, Buttons[i].CurrentColor);
        DrawText(TextFormat(Buttons[i].ButtonText.c_str()), Buttons[i].xposition + Buttons[i].width / 2 - ((fontsize / 3.8) * (Buttons[i].ButtonText.length())), Buttons[i].yposition + (Buttons[i].height - fontsize), fontsize, BLACK);
        if (MousePosition.x <= Buttons[i].xposition || MousePosition.x >= Buttons[i].xposition + Buttons[i].width ||
            MousePosition.y <= Buttons[i].yposition || MousePosition.y >= Buttons[i].yposition + Buttons[i].height)
        {
            continue;
        }
        if (!IsMouseButtonPressed(MOUSE_BUTTON_LEFT))
        {
            continue;
        }
        for (int i = 0; i < buttonamount; i++)
        {
            Buttons[i].CurrentColor = RED;
        }
        Buttons[i].CurrentColor = GREEN;
        *valuetochange = Buttons[i].SetInteger;
    }
}
void GenerateBoard(GameObject &MainBoard, int BoardSize)
{
    int border = 4;
    for (int row = 0; row < BoardSize; row++)
    {
        PlayBoards.push_back(vector<Board>());
        for (int col = 0; col < BoardSize; col++)
        {
            PlayBoards[row].push_back(Board(border / 2 + MainBoard.xposition + col * (MainBoard.width / BoardSize), border / 2 + MainBoard.yposition + row * (MainBoard.height / BoardSize), MainBoard.width / BoardSize - border, MainBoard.height / BoardSize - border));
        }
    }
}

void ComputerTurn(int PlayerAmount, int BoardSize)
{
    vector<int> RandomTable;
    int numbertoput = 0;
    for (int row = 0; row < BoardSize; row++)
    {
        for (int col = 0; col < BoardSize; col++)
        {
            if (PlayBoards[row][col].Claimed == false)
            {
                RandomTable.push_back(numbertoput);
            }
            numbertoput++;
        }
    }
    srand(time(0));
    if (RandomTable.size() == 0)
    {
        return;
    }
    int random = rand() % RandomTable.size();
    int chosen = RandomTable[random];
    int randrow = floor(chosen / BoardSize);
    int randcol = chosen % BoardSize;
    PlayBoards[randrow][randcol].CurrentColor = Players[CurrentlyPlaying].PlrColor;
    PlayBoards[randrow][randcol].Claimed = true;
}
bool ColorCheck(Color &BColor, Color &PColor)
{
    return BColor.r == PColor.r &&
           BColor.g == PColor.g &&
           BColor.b == PColor.b &&
           BColor.a == PColor.a;
}
bool CheckWinner(vector<vector<Board>> &PlayBoards, int row, int collumn, int req, bool winning, int BoardSize)
{
    if (winning == true)
    {
        return true;
    }
    int WinConditions[][3][2] =
        {{{row, collumn}, {row + 1, collumn}, {row + 2, collumn}}, // straight down

         {{row, collumn}, {row + 1, collumn + 1}, {row + 2, collumn + 2}}, // diagonal left to right

         {{row, collumn}, {row + 1, collumn - 1}, {row + 2, collumn - 2}}, // diagonal right to left(NO BUGS YAY)

         {{row, collumn}, {row, collumn + 1}, {row, collumn + 2}}}; // straight line
    for (int i = 0; i < 3; i++)
    {
        int checkrow = WinConditions[req][i][0];
        int checkcollumn = WinConditions[req][i][1];
        // stupid checks fuck you
        if (checkrow >= BoardSize || checkrow < 0 || checkcollumn >= BoardSize || checkcollumn < 0 || PlayBoards[checkrow][checkcollumn].Crossed == true)
        {
            return false;
        }
        Color BColor = PlayBoards[checkrow][checkcollumn].CurrentColor;
        Color PColor = Players[CurrentlyPlaying].PlrColor;
        bool IsMatch = ColorCheck(BColor, PColor);
        if (IsMatch == false)
        {
            return false;
        }
    }

    return true;
}
void CheckBoard(int BoardSize)
{
    bool IsWinning = false;
    vector<int> EmptySpace;
    int emptycount = 0;
    for (int row = 0; row < BoardSize; row++)
    {
        for (int col = 0; col < BoardSize; col++)
        {
            if (PlayBoards[row][col].Claimed == false)
            {
                emptycount++;
                continue;
            }
            for (int req = 0; req < 4; req++)
            {
                IsWinning = CheckWinner(PlayBoards, row, col, req, IsWinning, BoardSize);
            }
        }
    }

    if (emptycount == 0 && IsWinning == false)
    {
        ScreenNum = 2;
        IsDraw = true;
    }
    if (IsWinning == false || Scoring == true)
    {
        return;
    }
    ScreenNum = 2; // idk change to 3 or something
}
void ClickEvent(int row, int col, int BoardSize, int plramount)
{
    if (PlayBoards[row][col].Claimed == true)
    {
        return;
    }
    PlayBoards[row][col].CurrentColor = Players[CurrentlyPlaying].PlrColor;
    PlayBoards[row][col].Claimed = true;
    CheckBoard(BoardSize);
    if (ScreenNum != 1)
    {
        return;
    }
    CurrentlyPlaying++;
    if (plramount == 1)
    {
        ComputerTurn(plramount, BoardSize);
        CheckBoard(BoardSize);
    }
    if (CurrentlyPlaying >= plramount && ScreenNum == 1)
    {
        CurrentlyPlaying = 0;
    }
}
void DrawBoard(Vector2 &MousePosition, int BoardSize, int plramount)
{
    for (int row = 0; row < BoardSize; row++)
    {
        for (int col = 0; col < BoardSize; col++)
        {
            if (MousePosition.x <= PlayBoards[row][col].xposition || MousePosition.x >= PlayBoards[row][col].xposition + PlayBoards[row][col].width ||
                MousePosition.y <= PlayBoards[row][col].yposition || MousePosition.y >= PlayBoards[row][col].yposition + PlayBoards[row][col].height || PlayBoards[row][col].Claimed == true || ScreenNum != 1)
            {
                DrawRectangle(PlayBoards[row][col].xposition, PlayBoards[row][col].yposition, PlayBoards[row][col].width, PlayBoards[row][col].height, PlayBoards[row][col].CurrentColor);
                continue;
            }
            DrawRectangle(PlayBoards[row][col].xposition, PlayBoards[row][col].yposition, PlayBoards[row][col].width, PlayBoards[row][col].height, Players[CurrentlyPlaying].PlrColor);
            if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT))
            {
                ClickEvent(row, col, BoardSize, plramount);
            }
        }
    }
}
