#include "backend.hpp"

void LoadTimerFile(Player &MainPlayer, string filelocation)
{
    vector<string> SaveData;
    GetData(SaveData, filelocation);
    if (SaveData.empty())
    {
        MainPlayer.Timer.push_back(0);
        return;
    }
    for (int i = 0; i < SaveData.size(); i++)
    {
        if (SaveData[i] == "\0")
        {
            continue;
        }
        MainPlayer.Timer.push_back(std::stof(SaveData[i]));
        MainPlayer.CurrentLevel = i;
    }
}

void SaveGame(Player &MainPlayer, string filelocation, string timerfile, System &GameSystem)
{
    if (GameSystem.FramesElapsed % FPS != 0)
    {
        return;
    }
    std::ofstream myfile{filelocation};
    if (myfile.fail())
    {
        return;
    }
    myfile << std::to_string(MainPlayer.posx) << '\n';
    myfile << std::to_string(MainPlayer.posy) << '\n';
    myfile << std::to_string(MainPlayer.stamina) << '\n';
    myfile << std::to_string(MainPlayer.CurrentHealth) << '\n';
    int complete = (int)GameObject.Settings.Completed;
    myfile << std::to_string(complete);
    std::ofstream timefile(timerfile);
    if (timefile.fail())
    {
        return;
    }
    for (int i = 0; i < MainPlayer.Timer.size(); i++)
    {
        timefile << std::to_string(MainPlayer.Timer[i]);
        if (i >= MainPlayer.Timer.size() - 1)
        {
            continue;
        }
        timefile << '\n';
    }
}
template <typename T>
void OnScreenButton(T &Affected, int offset0, int offset1, int offset2, int offset3)
{

    // vector<Color> ColorList = {BLACK, ORANGE, YELLOW}

    Affected.UpdatePosition(offset0, offset1);
    Square Frame = {offset0 - offset2, offset1 - offset3, Affected.width + 2 * offset2, Affected.height + 2 * offset3, BLACK};
    Frame.DrawObject();
}
void MonitorFunction(vector<Square> &Bar, Square &BarAffected, Square &BarFrame, Player *MainPlayer,
                     int length, float *statsaffected, float maxstats, float regen, const char *Textbar,
                     int Offsets[])
{
    Bar[0].UpdatePosition(Offsets[0], Offsets[1]);
    Bar[0].DrawObject();
    // vector<Color> ColorList = {BLACK, ORANGE, YELLOW}
    for (int i = 1; i < length; i++)
    {
        Bar[i].UpdatePosition(Offsets[0] + Offsets[2], Offsets[1] + Offsets[3]);
        Bar[i].DrawObject();
    }
    DrawText(Textbar, BarFrame.posx + BarFrame.width / 2 - (StringSize(Textbar) * BarFrame.height / 3), BarFrame.posy, BarFrame.height, BLACK);
    BarAffected.width = *statsaffected * (BarFrame.width / maxstats);
    if (*statsaffected + regen > maxstats || MainPlayer->Alive == false)
    {
        return;
    }
    *statsaffected += regen;
}
void SetUserInterface(int Data[][4], vector<Square> &Putting, int length, vector<Color> ColorPut)
{
    for (int i = 0; i < length; i++)
    {
        Putting.push_back(Square(Data[i][0], Data[i][1], Data[i][2], Data[i][3], ColorPut[i]));
    }
}
template <typename Type>
void ChangeIt(Type &Move)
{

    Move.DrawObject();
    Move.Update();
}
template <typename Type>
void DrawIt(Type &ToDraw)
{
    ToDraw.DrawObject();
}
template <typename Type>
void DrawTheText(Type &ToDraw)
{
    ToDraw.DrawTexts();
}
template <typename Type>
void DrawWinPad(Type &ToDraw)
{
    DrawRectangleGradientV(ToDraw.posx, ToDraw.posy, ToDraw.width, ToDraw.height, Fade(ToDraw.BlockColor, 0), ToDraw.BlockColor);
}
template <typename Type>
void ObjectFunction(vector<Type> &Hitbox, int hitboxsize, void (*Functionality)(Type &Hitbox))
{

    for (int i = 0; i < hitboxsize; i++)
    {
        Functionality(Hitbox[i]);
    }
}
template <typename T>
float ApplyFriction(T ToCheck)
{
    if (ToCheck.OnPlatform == true)
    {
        return 0;
    }
    float velocity = ToCheck.xvelocity;
    // cout << velocity << '\n';
    if (velocity < 3 && velocity > 3)
    {
        velocity = 0;
        return 0;
    }
    if (velocity >= 0)
    {
        if (ToCheck.AtGround == false)
        {
            return -airresistance;
        }
        return -groundresistance;
    }
    if (velocity <= 0)
    {
        if (ToCheck.AtGround == false)
        {
            return airresistance;
        }
        return groundresistance;
    }
    return 0;
}
void PlayerControl(Player &MainPlayer)
{

    if (IsKeyPressed(KEY_W))
    {
        MainPlayer.Jump(65, 5);
    }
    if (IsKeyPressed(KEY_SPACE))
    {
        MainPlayer.Jump(80, 15);
    }
    if (IsKeyDown(KEY_A))
    {
        MainPlayer.Move(MainPlayer.speed);
    }
    if (IsKeyDown(KEY_D))
    {
        MainPlayer.Move(-MainPlayer.speed);
    }
    if (IsKeyPressed(KEY_S))
    {
        MainPlayer.Jump(-50, 5);
    }
    MainPlayer.DrawObject();
    MainPlayer.xvelocity += ApplyFriction<Player>(MainPlayer);
    if (MainPlayer.Alive == false)
    {
        Rectangle eyecrossed = {MainPlayer.posx + MainPlayer.width / 4, MainPlayer.posy + MainPlayer.height / 4, 15, 3};
        // draws an X or something
        DrawRectanglePro(eyecrossed, {0, 0}, (float)45, BLACK);
        DrawRectanglePro({eyecrossed.x + 5 * eyecrossed.width / 8, eyecrossed.y + 2 * eyecrossed.height / 3, eyecrossed.width, eyecrossed.height}, {0, 0}, (float)135, BLACK);
        Rectangle eyecrossed2 = {MainPlayer.posx + 5 * MainPlayer.width / 8, MainPlayer.posy + MainPlayer.height / 4, 15, 3};
        DrawRectanglePro(eyecrossed2, {0, 0}, (float)45, BLACK);
        DrawRectanglePro({eyecrossed2.x + 5 * eyecrossed2.width / 8, eyecrossed2.y + 2 * eyecrossed2.height / 3, eyecrossed2.width, eyecrossed2.height}, {0, 0}, (float)135, BLACK);
        return;
    }
    DrawCircle(MainPlayer.posx + MainPlayer.width / 4 + 2.5, MainPlayer.posy + MainPlayer.height / 4 + 2.5, 7.5, BLACK);
    DrawCircle(MainPlayer.posx + 5 * MainPlayer.width / 8 + 2.5, MainPlayer.posy + MainPlayer.height / 4 + 2.5, 7.5, BLACK);
    DrawRectangle(MainPlayer.posx + MainPlayer.width / 4 - 5, MainPlayer.posy + 2 * MainPlayer.height / 3, (5 * MainPlayer.width / 8 + 10) - (MainPlayer.width / 4 - 5), 8, BLACK);
    // cout << MainPlayer.xvelocity << '\n';
    // DrawRectangleRec(player, GREEN);
    // cout << MainPlayer.xsave << '\n';
}

// gather file or some shit

string FormatTime(float timesecond)
{
    string Timer;
    int second = timesecond;
    int milisecond = timesecond * 1000;
    int minute = second / 60;
    string milistr = TextFormat("%03i", (milisecond % 1000));
    string secondstr = TextFormat("%02i", (second % 60));
    string minutestr = TextFormat("%02i", (minute % 60));
    string combine = minutestr + ":" + secondstr + ":" + milistr;
    return combine;
}
void DisplayTimerAchievement(Player &MainPlayer, General Frame)
{

    for (int i = 0; i < MainPlayer.Timer.size(); i++)
    {
        DrawRectangle(screenWidth / 2 - Frame.width / 2, 100 + i * (Frame.height + 10), Frame.width, Frame.height, Frame.BlockColor);
    }
}
void LevelList(Player &MainPlayer, Vector2 MousePos)
{
    if (GameObject.Settings.beginscroll + GetMouseWheelMove() * 20 < 0)
    {
        GameObject.Settings.beginscroll += GetMouseWheelMove() * 20;
    }
    else
    {
        GameObject.Settings.beginscroll = 0;
    }
    for (int i = 0; i < GameObject.GotoLevel.size(); i++)
    {
        Button LevelTab = GameObject.GotoLevel[i];
        OnScreenButton<Button>(LevelTab, screenWidth / 2 - LevelTab.width / 2, (i * 2 * LevelTab.height) + 40 + GameObject.Settings.beginscroll, 5, 5);
        LevelTab.ChangeValue<int>(MousePos, &MainPlayer.CurrentLevel, i, LevelTab.width / 2 - 20, TextFormat("LEVEL %i", i));
        LevelTab.ChangeValue<int>(MousePos, &GameObject.Settings.AtScreen, 2, LevelTab.width / 2 - 20, TextFormat("LEVEL %i", i));
        LevelTab.ChangeLevelButton(MousePos, i, MainPlayer, LevelTab.width / 2 - 20, TextFormat("LEVEL %i", i + 1));
    }
}
void TimeList(Player MainPlayer)
{

    float totaltime = 0;
    if (GameObject.Settings.endscroll + GetMouseWheelMove() * 20 < 0)
    {
        GameObject.Settings.endscroll += GetMouseWheelMove() * 20;
    }
    else
    {
        GameObject.Settings.endscroll = 0;
    }
    for (int i = 0; i < GameObject.GotoLevel.size(); i++)
    {
        Button LevelTab = GameObject.GotoLevel[i];
        LevelTab.BlockColor = GREEN;
        OnScreenButton<Button>(LevelTab, screenWidth / 2 - LevelTab.width / 2, (i * 2 * LevelTab.height) + 90 + GameObject.Settings.endscroll, 5, 5);
        LevelTab.DrawObject();
        string level = TextFormat("LEVEL %i", i + 1);
        string time = FormatTime(MainPlayer.Timer[i]);
        string toput = level + " = " + time;
        DrawText(toput.c_str(), LevelTab.posx + 20, LevelTab.posy + LevelTab.height / 6, LevelTab.height / 1.5, BLACK);
        totaltime += MainPlayer.Timer[i];
    }
    DrawRectangle(0, 0, screenWidth, 55, SKYBLUE);
    if (GameObject.Settings.Completed == false)
    {
        DrawText("Complete the game to see total time", screenWidth / 2 - 275, 8, 30, RED);
        return;
    }
    string formatted = "TOTAL TIME = " + FormatTime(totaltime);
    DrawText(formatted.c_str(), screenWidth / 2 - 250, 20, 40, DARKBLUE);
}
int main()
{
    InitWindow(screenWidth, screenHeight, "BAD PLATFORMER FOR NOW");
    SetWindowState(FLAG_WINDOW_RESIZABLE);
    MaximizeWindow();
    Player MainPlayer = Player(0, -100, 64, 64, BLUE);
    // General PlayerHitbox = General(100, 100, 64, 64);
    SetTargetFPS(FPS);

    Camera2D camera = {0};
    camera.target = (Vector2){MainPlayer.posx, MainPlayer.posy};
    camera.rotation = 0.0f;
    camera.zoom = 1.0f;
    // vector<vector<int>> DamagePartData = {{200, screenHeight - 10, 100, 10}};
    int StaminaBarData[][4] = {{0, 0, 210, 50}, {0, 0, 200, 40}, {0, 0, 200, 40}};
    int HealthBarData[][4] = {{0, 0, 210, 50}, {5, 5, 200, 40}, {5, 5, 200, 40}};

    vector<Square> StaminaBar;
    vector<Square> HealthBar;
    Button NextLevel = Button(300, 60, GREEN, GOLD);
    Button PlayButton = Button(300, 40, RED, GREEN);
    Button NewGame = Button(300, 40, RED, GREEN);
    Button Practice = Button(300, 40, RED, GREEN);
    Button StatsButton = Button(300, 40, RED, GREEN);
    Button RespawnButton = Button(300, 60, RED, GREEN);
    Button ResetButton = Button(50, 50, RED, GREEN);
    Button QuitButton = Button(50, 50, RED, GREEN);
    string SaveFileName = "SaveTest.dll";
    string LevelData = "LevelTime.dll";
    LoadTimerFile(MainPlayer, LevelData);
    vector<Color> PutColor = {BLACK, ORANGE, YELLOW};
    vector<Color> HealthColor = {BLACK, RED, GREEN};
    SetUserInterface(StaminaBarData, StaminaBar, sizeof(StaminaBarData) / sizeof(StaminaBarData[0]), PutColor);
    SetUserInterface(HealthBarData, HealthBar, sizeof(HealthBarData) / sizeof(StaminaBarData[0]), HealthColor);
    LoadGame(MainPlayer, SaveFileName);
    GameObject.Settings.Loaded = true;
    while (!WindowShouldClose())
    {
        screenWidth = GetScreenWidth();
        screenHeight = GetScreenHeight();
        camera.offset = (Vector2){screenWidth / 2.0f - MainPlayer.width / 2, screenHeight / 2.0f - MainPlayer.height / 2};
        int StaminaBarOffset[4] = {screenWidth - 210, 0, 5, 5};
        int HealthBarOffset[4] = {0, 0, 5, 5};
        // cout << MainPlayer.Timer.size() << '\n';
        //   MainPlayer.CheckCollision(Hitbox, MainPlayer);
        BeginDrawing();
        ClearBackground(WHITE);
        Vector2 MousePos = GetMousePosition();
        GameObject.Settings.FramesElapsed++;

        switch (GameObject.Settings.AtScreen)
        {
        case 0: // MAIN MENU
        {
            DrawRectangle(0, 0, screenWidth, screenHeight, Fade(LIME, 0.6f));
            // DisplayTimerAchievement(MainPlayer, LevelFrame)
            OnScreenButton<Button>(NewGame, screenWidth / 2 - PlayButton.width / 2, 150 + PlayButton.height + 40, 3, 3);
            OnScreenButton<Button>(Practice, screenWidth / 2 - PlayButton.width / 2, 150 + 3 * PlayButton.height + 40, 3, 3);
            OnScreenButton<Button>(StatsButton, screenWidth / 2 - PlayButton.width / 2, 150 + 5 * PlayButton.height + 40, 3, 3);
            DrawText("BLITZ RUSH", screenWidth / 2 - 325, 0, 100, BLUE);
            Practice.ChangeValue<bool>(MousePos, &MainPlayer.PracticeMode, true, 100, "PRACTICE");
            DrawText("Support me on patreon.com/MYGAMES630", screenWidth / 2 - 320, screenHeight - 30, 30, BLACK);
            if (GameObject.Settings.Completed == false)
            {
                DrawText(TextFormat("You are currently at level %i", MainPlayer.CurrentLevel + 1), screenWidth / 2 - 300, 100, PlayButton.height, BLACK);
                OnScreenButton<Button>(PlayButton, screenWidth / 2 - PlayButton.width / 2, 150, 3, 3);
                PlayButton.LoadLevel(MousePos, MainPlayer, &GameObject.Settings.AtScreen, 2, 50, "PLAY", GameObject.Settings.playerdata);
            }
            else
            {
                DrawText("You finished the game, thank you <3", screenWidth / 2 - 365, 150, PlayButton.height, GOLD);
                // probably add some function to finish the game or some shit lol
            }
            if (MainPlayer.PracticeMode == false)
            {
                MainPlayer.CurrentLevel = MainPlayer.Timer.size() - 1;
            }
            StatsButton.LoadList(GameObject.GotoLevel, MainPlayer, MousePos, 70, &GameObject.Settings.AtScreen, "STATS");
            StatsButton.ChangeValue<int>(MousePos, &GameObject.Settings.AtScreen, 3, 70, "STATS");
            Practice.LoadList(GameObject.GotoLevel, MainPlayer, MousePos, 100, &GameObject.Settings.AtScreen, "PRACTICE");
            NewGame.WipeGame(MousePos, GameObject.Settings, MainPlayer, &GameObject.Settings.AtScreen, 2, 100, "NEW GAME");
            break;
        }
        case 1: // PRACTICE MODE
        {
            DrawRectangle(0, 0, screenWidth, screenHeight, Fade(LIME, 0.6f));
            QuitButton.ChangeValue<int>(MousePos, &GameObject.Settings.AtScreen, 0, 0, "");
            OnScreenButton<Button>(QuitButton, 5, 5, 5, 5);
            QuitButton.ChangeValue<bool>(MousePos, &MainPlayer.PracticeMode, false, 0, "");
            DrawText("QUIT", QuitButton.posx + QuitButton.width + 20, QuitButton.posy + QuitButton.width / 5, 30, MAROON);
            LevelList(MainPlayer, MousePos);
            DrawRectangle(0, screenHeight - 50, screenWidth, 50, GREEN);
            DrawText("IMPROVE YOUR TIME HERE", screenWidth / 2 - 290, screenHeight - 40, 40, BLACK);
            break;
        }
        case 2:
        {

            ClearBackground(SKYBLUE);

            camera.target = (Vector2){MainPlayer.posx, MainPlayer.posy};

            MainPlayer.PlayerUpdate();
            // BEGIN 2D MODE
            BeginMode2D(camera);
            Vector2 MouseWorldPosition;
            MouseWorldPosition.x = MousePos.x + camera.target.x - screenWidth / 2 + MainPlayer.width / 2;
            MouseWorldPosition.y = MousePos.y + camera.target.y - screenHeight / 2 + MainPlayer.height / 2;
            ObjectFunction<General>(GameObject.Background, GameObject.Background.size(), &DrawIt);
            ObjectFunction<IngameTexts>(GameObject.LevelText, GameObject.LevelText.size(), &DrawTheText);
            PlayerControl(MainPlayer);
            ObjectFunction<General>(GameObject.Hitbox, GameObject.Hitbox.size(), &DrawIt);
            ObjectFunction<General>(GameObject.WinningPad, GameObject.WinningPad.size(), &DrawWinPad);
            ObjectFunction<Damaging>(GameObject.DamagePart, GameObject.DamagePart.size(), &DrawIt);
            ObjectFunction<Movable>(GameObject.MovingPart, GameObject.MovingPart.size(), &ChangeIt);
            ObjectFunction<Disappearing>(GameObject.HarmlessInvisible, GameObject.HarmlessInvisible.size(), &ChangeIt);
            MainPlayer.PlayerCollision<General, Player>(GameObject.Hitbox, GameObject.Hitbox.size(), MainPlayer, &HarmlessEffect);
            MainPlayer.PlayerCollision<Disappearing, Player>(GameObject.HarmlessInvisible, GameObject.HarmlessInvisible.size(), MainPlayer, &HarmlessEffect);
            MainPlayer.PlayerCollision<Damaging, Player>(GameObject.DamagePart, GameObject.DamagePart.size(), MainPlayer, &DamagingEffect);
            MainPlayer.PlayerCollision<Disappearing, Player>(GameObject.HarmlessInvisible, GameObject.HarmlessInvisible.size(), MainPlayer, &DamagingEffect);
            MainPlayer.PlayerCollision<Movable, Player>(GameObject.MovingPart, GameObject.MovingPart.size(), MainPlayer, &MovingEffect);
            MainPlayer.PlayerCollision<General, Player>(GameObject.WinningPad, GameObject.WinningPad.size(), MainPlayer, &WinCollision);
            EndMode2D();
            // ENDS 2D MODE

            // potential levels?????

            if (MainPlayer.Alive == false)
            {
                DrawRectangle(0, 0, screenWidth, screenHeight, Fade(RED, 0.5f));
                OnScreenButton<Button>(RespawnButton, screenWidth / 2 - RespawnButton.width / 2, screenHeight / 1.5, 5, 5);
                RespawnButton.RespawnPlayer(MousePos, GameObject.Settings, MainPlayer, 145, "RESPAWN");
                DrawText("YOU DIED!", screenWidth / 2 - 150, screenHeight / 4, 60, MAROON);
            }
            else
            {
                if (MainPlayer.NextLevel == true)
                {
                    if (MainPlayer.PracticeMode == false)
                    {
                        OnScreenButton<Button>(NextLevel, screenWidth / 2 - NextLevel.width / 2, screenHeight - NextLevel.height - 5, 5, 5);
                        NextLevel.ChangeLevelButton(MousePos, MainPlayer.CurrentLevel + 1, MainPlayer, 125, "Level Up");
                    }
                    else
                    {
                        OnScreenButton<Button>(RespawnButton, screenWidth / 2 - RespawnButton.width / 2, screenHeight - NextLevel.height - 5, 5, 5);
                        RespawnButton.RespawnPlayer(MousePos, GameObject.Settings, MainPlayer, 145, "RESPAWN");
                    }
                }
                else
                {
                    OnScreenButton<Button>(ResetButton, 5, screenHeight - ResetButton.height - QuitButton.height - 20, 5, 5);
                    OnScreenButton<Button>(QuitButton, 5, screenHeight - QuitButton.height - 5, 5, 5);
                    ResetButton.RespawnPlayer(MousePos, GameObject.Settings, MainPlayer, 20, "");

                    DrawText("RESET PLAYER", ResetButton.posx + ResetButton.width + 20, ResetButton.posy + ResetButton.width / 5, 30, BLACK);
                    DrawText("QUIT", QuitButton.posx + QuitButton.width + 20, QuitButton.posy + QuitButton.width / 5, 30, MAROON);
                    if (MainPlayer.PracticeMode == true)
                    {
                        QuitButton.ChangeValue<int>(MousePos, &GameObject.Settings.AtScreen, 1, 0, "");
                        MainPlayer.practicetime += GetFrameTime();
                    }
                    else
                    {
                        QuitButton.ChangeValue<int>(MousePos, &GameObject.Settings.AtScreen, 0, 0, "");
                        MainPlayer.Timer[MainPlayer.CurrentLevel] += GetFrameTime();
                    }
                }
                if (MainPlayer.PracticeMode == true)
                {
                    DrawText(FormatTime(MainPlayer.practicetime).c_str(), screenWidth / 2 - 110, 0, 50, BLACK);
                }
                else
                {
                    DrawText(FormatTime(MainPlayer.Timer[MainPlayer.CurrentLevel]).c_str(), screenWidth / 2 - 110, 0, 50, BLACK);
                }
                DrawRectangle(0, 0, screenWidth, screenHeight, Fade(RED, ((MainPlayer.MaxHealth - MainPlayer.CurrentHealth) / MainPlayer.MaxHealth) / 4));
                MonitorFunction(StaminaBar, StaminaBar[2], StaminaBar[1], &MainPlayer, StaminaBar.size(), &MainPlayer.stamina, MainPlayer.staminamax, MainPlayer.staminaregen, "STAMINA", StaminaBarOffset);
                MonitorFunction(HealthBar, HealthBar[2], HealthBar[1], &MainPlayer, HealthBar.size(), &MainPlayer.CurrentHealth, MainPlayer.MaxHealth, MainPlayer.BaseRegen, "HEALTH", HealthBarOffset);
                DrawText(TextFormat("X POSITION: %i", (int)MainPlayer.posx), HealthBar[1].posx, HealthBar[1].posy + HealthBar[1].height + 20, 20, BLACK);
                DrawText(TextFormat("Y POSITION: %i", (int)MainPlayer.posy + MainPlayer.height), HealthBar[1].posx, HealthBar[1].posy + HealthBar[1].height + 50, 20, BLACK);
            }
            if (MainPlayer.PracticeMode == false)
            {
                SaveGame(MainPlayer, GameObject.Settings.playerdata, GameObject.Settings.leveldata, GameObject.Settings);
            }
            // cout << GetFrameTime() << '\n';
            /* code */
            break;
        }
        case 3:
        {
            DrawRectangle(0, 0, screenWidth, screenHeight, Fade(LIME, 0.6f));
            TimeList(MainPlayer);
            OnScreenButton<Button>(QuitButton, 5, 55, 5, 5);
            QuitButton.ChangeValue<int>(MousePos, &GameObject.Settings.AtScreen, 0, 0, "");
            DrawText("QUIT", QuitButton.posx + QuitButton.width + 20, QuitButton.posy + QuitButton.width / 5, 30, MAROON);
            DrawRectangle(0, screenHeight - 50, screenWidth, 50, GREEN);
            DrawText("IT CAN BE BETTER!", screenWidth / 2 - 210, screenHeight - 40, 40, BLACK);
            break;
        }
        }
        if (GameObject.Settings.Completed == true && MainPlayer.PracticeMode == false)
        {
            SaveGame(MainPlayer, GameObject.Settings.playerdata, GameObject.Settings.leveldata, GameObject.Settings);
        }

        EndDrawing();
    }
    CloseWindow();
}