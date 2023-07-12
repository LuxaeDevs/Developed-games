#include "functions.hpp"

int main()
{
    InitWindow(GameSet.screenWidth, GameSet.screenHeight, "Game of Pong");

    Paddle Player1 = Paddle({10, GameSet.screenHeight / 2.0f, 25, 100}, GREEN, KEY_W, KEY_S);
    Paddle Player2 = Paddle{{GameSet.screenWidth - Player1.Info.x - Player1.Info.width, Player1.Info.y, Player1.Info.width, Player1.Info.height}, RED, KEY_UP, KEY_DOWN};

    Button QuitButton = Button({GameSet.screenWidth / 2.0f - 6 - 100, 25 + 3, 200, 50}, {6, 6}, RED, GREEN, SKYBLUE, "QUIT");
    Button PlayButton = Button({GameSet.screenWidth / 2.0f - 200, 100, 400, 50}, {6, 6}, RED, GREEN, BLUE, "PLAY");
    Button EditGameplay = Button({GameSet.screenWidth / 2.0f - 200, 300, 400, 50}, {6, 6}, RED, GREEN, BLUE, "EDIT GAME");
    Button TogglePlayer = Button({GameSet.screenWidth / 2.0f - 200, 200, 400, 50}, {6, 6}, GREEN, RED, BLUE, "ONE PLAYER");
    Button StartGame = Button({GameSet.screenWidth / 2.0f - 100, 500, 200, 50}, {8, 8}, RED, GREEN, SKYBLUE, "START");

    Ball ScoreBall = Ball({(float)GameSet.screenWidth / 2, (float)GameSet.screenHeight / 2 + GameSet.toptab / 2.0f}, 20, PINK);
    Ball MenuBall = Ball({(float)GameSet.screenWidth / 2, 100}, 20, PINK);
    GeneralRec ScoreBoard = GeneralRec({0, 0, (float)GameSet.screenWidth, (float)GameSet.toptab}, Fade(WHITE, 0.5f));
    vector<GeneralRec> Outline =
        {
            {GeneralRec({(float)GameSet.screenWidth / 2 - 5, (float)GameSet.toptab, 10, (float)GameSet.screenHeight - GameSet.toptab}, WHITE)},
            {GeneralRec({0, (float)GameSet.toptab, (float)GameSet.screenWidth, 10}, WHITE)},
            {GeneralRec({0, (float)GameSet.toptab, 10, (float)GameSet.screenHeight}, WHITE)},
            {GeneralRec({0, (float)GameSet.screenHeight - 10, (float)GameSet.screenWidth, 10}, WHITE)},
            {GeneralRec({(float)GameSet.screenWidth - 10, (float)GameSet.toptab, 10, (float)GameSet.screenWidth}, WHITE)}};

    GeneralRec SpeedChangeFrame = {{GameSet.screenWidth / 2.0f - 200, 160, 400, 10}, GRAY};
    DraggableCircle SpeedChanger = DraggableCircle({SpeedChangeFrame.Info.x, SpeedChangeFrame.Info.y + SpeedChangeFrame.Info.height / 2, 20},
                                                   (Color){80, 80, 80, 255}, (Color){220, 220, 220, 255});
    MenuBall.Velocity = {BallSpeedRNG(RandomRange(200, 600)), BallSpeedRNG(RandomRange(200, 600))};
    SetTargetFPS(FPS);
    while (!WindowShouldClose())
    {
        Vector2 MousePos = GetMousePosition();
        Vector2 MouseDelta = GetMouseDelta();
        ClearBackground(WHITE);
        BeginDrawing();
        DrawRectangle(0, 0, GameSet.screenWidth, GameSet.screenHeight, Fade(GameSet.BackgroundColor, 0.4F));
        switch (GameSet.currentscreen)
        {
        case 0: // main menu
            DrawText("THE PONG GAME", GameSet.screenWidth / 2 - 300, 0, 70, SKYBLUE);
            EditGameplay.InteractDefault<int>(MousePos, &GameSet.currentscreen, 2, 140);
            PlayButton.PlayGame(ScoreBall, Player1, Player2, MousePos, &StartGame.Visible, 60);
            TogglePlayer.ToggleSomething(MousePos, &GameSet.SinglePlayer, 185, 155, "TWO PLAYERS", "ONE PLAYER");
            break;
        case 1: // main gameplay
            DrawRectangleRec(ScoreBoard.Info, ScoreBoard.ColorInfo);
            DrawRectangleBulk(Outline, (int)Outline.size());
            QuitButton.RepositionRectangle({GameSet.screenWidth / 2.0f - QuitButton.Info.width / 2.0f - QuitButton.FrameOffset.x, 25 + QuitButton.FrameOffset.y});
            QuitButton.InteractDefault<int>(MousePos, &GameSet.currentscreen, 0, 65);
            if (GameSet.SinglePlayer == true)
            {
                Player2.Tracker(ScoreBall.CenterPos, ScoreBall.Velocity.x, StartGame.Visible);
            }
            else
            {
                Player2.Update(StartGame.Visible);
                if (StartGame.Visible == true)
                {
                    DrawText("PRESS UP ARROW TO MOVE UP", GameSet.screenWidth / 2.0F + 10, 400, 20, BLACK);
                    DrawText("PRESS DOWN ARROW TO MOVE DOWN", GameSet.screenWidth / 2.0F + 10, 450, 20, BLACK);
                }
            }
            if (StartGame.Visible == true)
            {
                DrawText("PRESS W TO MOVE UP", 20, 400, 20, BLACK);
                DrawText("PRESS S TO MOVE DOWN", 20, 450, 20, BLACK);
            }
            ScoreBall.UpdateBall(Player1, Player2);
            Player1.Update(StartGame.Visible);
            DrawText(TextFormat("%02i", Player1.score), 160, 0, 80, BLACK);
            DrawText(TextFormat("%02i", Player2.score), GameSet.screenWidth - 200 - 60, 0, 80, BLACK);
            StartGame.StartGame(ScoreBall, MousePos, GameSet.BallCurrentVelocity, 87);
            CheckAtCorner(ScoreBall, StartGame, Player1, Player2);
            break;
        case 2: // gameplay edit
        {
            QuitButton.RepositionRectangle({0 + QuitButton.FrameOffset.x, 0 + QuitButton.FrameOffset.y});
            QuitButton.InteractDefault<int>(MousePos, &GameSet.currentscreen, 0, 65);
            MenuBall.BallMenuChanger(GameSet.BallCurrentVelocity, GameSet.BallBaseVelocity, 0);
            DrawText("1.00X", SpeedChangeFrame.Info.x - SpeedChanger.radius, SpeedChangeFrame.Info.y - SpeedChanger.radius - 20, 20, BLACK);
            DrawText("3.00X", SpeedChangeFrame.Info.x + SpeedChangeFrame.Info.width - SpeedChanger.radius, SpeedChangeFrame.Info.y - SpeedChanger.radius - 20, 20, BLACK);
            string Multiplier = TextFormat("%0.2f", GameSet.BallCurrentVelocity / GameSet.BallBaseVelocity);
            string ToShow = Multiplier + "X";
            DrawText(ToShow.c_str(), SpeedChanger.CenterPos.x - 20, SpeedChanger.CenterPos.y + SpeedChanger.radius, 20, BLACK);
            DrawText("BALL SPEED", GameSet.screenWidth / 2.0f - 150, 0, 50, BLACK);
            GameSet.BallCurrentVelocity = GameSet.BallBaseVelocity + SpeedChanger.CenterPos.x - SpeedChangeFrame.Info.x;
            SpeedChangeFrame.Draw();
            SpeedChanger.DragCircle(MousePos, MouseDelta, SpeedChangeFrame.Info, 'x');

            cout << GameSet.BallCurrentVelocity << '\n';
            break;
        }
        case 3: // decorative element

            break;
        default:
            break;
        }

        EndDrawing();
    }
    CloseWindow();
}