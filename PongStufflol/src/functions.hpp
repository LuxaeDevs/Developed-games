#include "structs.hpp"
template <typename T>
void DrawRectangleBulk(vector<T> &Rec, int size)
{
    for (int i = 0; i < size; i++)
    {
        DrawRectangleRec(Rec[i].Info, Rec[i].ColorInfo);
    }
}

int RandomRange(int min, int max)
{
    int range;
    if (max < min)
    {
        range = min - max + 1;
        return (rand() % range) + max;
    }
    else
    {
        range = max - min + 1;
        return (rand() % range) + min;
    }
}

float BallSpeedRNG(float speed)
{
    if (RandomRange(0, 1) == 1)
    {

        return speed *= 1;
    }
    else
    {

        return speed *= -1;
    }
}

Rectangle GetFrame(Rectangle ToDraw, Vector2 frameoffset)
{
    return Rectangle{ToDraw.x - frameoffset.x, ToDraw.y - frameoffset.y, ToDraw.width + 2 * frameoffset.x, ToDraw.height + 2 * frameoffset.y};
}

void ResetGame(Paddle &PlayerWon, Paddle &PlayerLose, Ball &ScoreBall, Button &ToShow)
{
    ScoreBall.CenterPos = {(float)GameSet.screenWidth / 2, (float)GameSet.screenHeight / 2.0f + GameSet.toptab / 2.0f};

    ScoreBall.Velocity = {0, 0};
    ToShow.Visible = true;
    PlayerWon.score += 1;
}
void ResetPaddlePosition(Paddle &PaddleLeft, Paddle &PaddleRight)
{
    PaddleLeft.Info = {10, GameSet.screenHeight / 2.0f, 25, 100};
    PaddleRight.Info = {GameSet.screenWidth - PaddleLeft.Info.x - PaddleLeft.Info.width,
                        PaddleLeft.Info.y, PaddleLeft.Info.width, PaddleLeft.Info.height};
}
void CheckAtCorner(Ball &ScoreBall, Button &ToShow, Paddle &Player1, Paddle &Player2)
{
    if (ScoreBall.CenterPos.x + ScoreBall.radius < 0)
    {
        ResetGame(Player2, Player1, ScoreBall, ToShow);
        ResetPaddlePosition(Player1, Player2);
    }
    if (ScoreBall.CenterPos.x - ScoreBall.radius > GameSet.screenWidth)
    {
        ResetGame(Player1, Player2, ScoreBall, ToShow);
        ResetPaddlePosition(Player1, Player2);
    }
}