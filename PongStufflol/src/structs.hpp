#include <iostream>
#include <vector>
#include <raylib.h>
#include <ctime>
using std::cout;
using std::string;
using std::vector;

int FPS = 300;

Rectangle GetFrame(Rectangle ToDraw, Vector2 frameoffset);
int RandomRange(int min, int max);
struct System
{
    int screenWidth = 800;
    int toptab = 100;
    int screenHeight = 580;
    int currentscreen = 0;
    Color BackgroundColor = BLACK;
    float BallBaseVelocity = 200.0f;
    float BallCurrentVelocity = 200.0f;
    bool SinglePlayer = true;
};
System GameSet;
struct Objects
{
    bool Visible = true;
    Color ColorInfo;
    Vector2 Direction = {1, 1};
    Objects(Color ColorInfo)
    {
        this->ColorInfo = ColorInfo;
    }
    void ChangeDirection(Vector2 Direction)
    {
        this->Direction = Direction;
    }
};
struct GeneralRec : Objects
{
    Rectangle Info;
    GeneralRec(Rectangle Rec, Color Idle) : Objects(Idle)
    {
        Info.x = Rec.x;
        Info.y = Rec.y;
        Info.width = Rec.width;
        Info.height = Rec.height;
    }
    void Draw()
    {
        DrawRectangleRec(Info, ColorInfo);
    }
    void FadeDraw()
    {
    }
    void RepositionRectangle(Vector2 Reposition)
    {
        if (Info.x == Reposition.x && Info.y == Reposition.y)
        {
            return;
        }
        Info.x = Reposition.x;
        Info.y = Reposition.y;
    }
};
struct Paddle : GeneralRec
{
    int speed = 200;
    int score = 0;
    Rectangle DefaultSize = {10, 350, 25, 100};
    int randomcooldown = RandomRange(100, 300);
    Vector2 TargetPos;
    KeyboardKey KeyUp = KEY_W;
    KeyboardKey KeyDown = KEY_S;
    Paddle(Rectangle Rec, Color Idle, KeyboardKey KeyUp, KeyboardKey KeyDown) : GeneralRec(Rec, Idle)
    {
        this->KeyUp = KeyUp;
        this->KeyDown = KeyDown;
    }
    void Move(int movespeed)
    {
        float velocity = movespeed * GetFrameTime();
        if (Info.y + Info.height + velocity > GameSet.screenHeight || Info.y + velocity < GameSet.toptab)
        {
            return;
        }
        Info.y += velocity;
    }
    void Update(bool Allowed)
    {
        Draw();
        if (Allowed == true)
        {
            return;
        }
        if (IsKeyDown(KeyUp))
        {
            Move(-speed);
        }
        if (IsKeyDown(KeyDown))
        {
            Move(+speed);
        }
    }
    void MoveAI(Vector2 Target, bool movingaway)
    {
        // cout << Info.y + Info.height / 2 - Target.y << '\n';

        // cout << TargetPos.y << '\n';
        if (Info.y + Info.height / 2 - Target.y == 0)
        {
            return;
        }
        if (Info.y + Info.height / 2 - Target.y > 0)
        {
            // decrement by 0.15 for every difficulty fuck you
            Move(-speed / 1);
        }
        if ((Info.y + Info.height / 2 - Target.y < 0))
        {
            Move(speed / 1);
        }
    }
    void Tracker(Vector2 Target, int velocity, bool Visible)
    {
        Draw();
        if (Visible == true)
        {
            return;
        }
        if (velocity < 0)
        {
            if (randomcooldown > 0)
            {
                MoveAI(TargetPos, true);
                randomcooldown--;
                return;
            }
            randomcooldown += RandomRange(100, 300);
            TargetPos.y = RandomRange(0, GameSet.screenHeight);
            return;
        }
        MoveAI(Target, false);
        TargetPos.y = Target.y;
    }
};

struct Ball : Objects
{
    Vector2 CenterPos;
    int radius;
    Vector2 Velocity = {0, 0};
    Ball(Vector2 CenterPos, int radius, Color SetColor) : Objects(SetColor)
    {
        this->radius = radius;
        this->CenterPos = CenterPos;
    }
    void UpdateBall(Paddle PaddleLeft, Paddle PaddleRight)
    {
        DrawCircle(CenterPos.x, CenterPos.y, radius, ColorInfo);

        CenterPos.x += Velocity.x * GetFrameTime();
        CenterPos.y += Velocity.y * GetFrameTime();

        if (CenterPos.y - radius + Velocity.y * GetFrameTime() < GameSet.toptab || CenterPos.y + radius + Velocity.y * GetFrameTime() > GameSet.screenHeight)
        {
            Velocity.y *= -1;
        }
        if (CheckCollisionCircleRec(CenterPos, radius, PaddleLeft.Info))
        {

            if (Velocity.x < 0)
            {
                Velocity.x *= -1.02;
            }
        }
        if (CheckCollisionCircleRec(CenterPos, radius, PaddleRight.Info))
        {
            if (Velocity.x > 0)
            {
                Velocity.x *= -1.02;
            }
        }
    }
    void BallMenuChanger(float speed, float base, int axis)
    {
        DrawCircleV(CenterPos, radius, ColorInfo);

        Velocity = {GameSet.BallCurrentVelocity * Direction.x, 0};
        float velocityx = Velocity.x * GetFrameTime();
        CenterPos.x += velocityx;
        CenterPos.y += Velocity.y * GetFrameTime();
        if (CenterPos.x - radius + velocityx < 0 || CenterPos.x + radius + velocityx > GameSet.screenWidth)
        {
            if (velocityx < 0)
            {
                CenterPos.x = radius;
            }
            else
            {
                CenterPos.x = GameSet.screenWidth - radius;
            }
            Direction.x *= -1;
        }
    }
};

float BallSpeedRNG(float speed);
struct Button : GeneralRec
{

    Color ColorIdle;
    Color ColorInteract;
    Color FrameColor;
    Vector2 FrameOffset;
    string Name;

    Button(Rectangle Rec, Vector2 FrameOffset, Color Idle, Color Interact, Color FrameColor, string Name) : GeneralRec(Rec, Idle)
    {
        ColorIdle = Idle;
        ColorInteract = Interact;
        this->FrameOffset = FrameOffset;
        this->Name = Name;
        this->FrameColor = FrameColor;
    }
    bool ClickEvent(Vector2 MousePos, int textoffset, bool changewhenhover)
    {
        if (!Visible)
        {
            return false;
        }
        DrawRectangleRounded(GetFrame(Info, FrameOffset), 0.2, 0.3, FrameColor);
        Draw();
        DrawText(Name.c_str(), Info.x + Info.width / 2 - textoffset, Info.y, Info.height, BLACK);
        if (!CheckCollisionPointRec(MousePos, Info))
        {
            ColorInfo = ColorIdle;
            return false;
        }
        if (changewhenhover == true)
        {
            ColorInfo = ColorInteract;
        }
        if (!IsMouseButtonPressed(MOUSE_BUTTON_LEFT))
        {
            return false;
        }
        return true;
    }
    template <typename Val>
    void InteractDefault(Vector2 MousePos, Val *ToSwitch, Val Switching, int textoffset)
    {
        if (!ClickEvent(MousePos, textoffset, true))
        {
            return;
        }
        *ToSwitch = Switching;
    }
    void PlayGame(Ball &ScoreBall, Paddle &PaddleLeft, Paddle &PaddleRight, Vector2 MousePos, bool *Startvisible, int textoffset)
    {
        if (!ClickEvent(MousePos, textoffset, true))
        {
            return;
        }
        ScoreBall.Velocity = {0, 0};
        ScoreBall.CenterPos = {(float)GameSet.screenWidth / 2, (float)GameSet.screenHeight / 2.0f + GameSet.toptab / 2.0f};
        PaddleLeft.score = 0;
        PaddleRight.score = 0;
        PaddleLeft.Info = {10, GameSet.screenHeight / 2.0f, 25, 100};
        PaddleRight.Info = {GameSet.screenWidth - PaddleLeft.Info.x - PaddleLeft.Info.width,
                            PaddleLeft.Info.y, PaddleLeft.Info.width, PaddleLeft.Info.height};
        *Startvisible = true;
        GameSet.currentscreen = 1;
    }
    void StartGame(Ball &ScoreBall, Vector2 MousePos, float BallVelocity, int textoffset)
    {
        if (!ClickEvent(MousePos, textoffset, true))
        {
            return;
        }
        Visible = false;
        ScoreBall.Velocity = {BallSpeedRNG(BallVelocity), BallSpeedRNG(BallVelocity)};
    }
    void ToggleSomething(Vector2 MousePos, bool *ToSwitch, int textoffsetOff,
                         int textoffsetOn, const char *nameOff, const char *nameOn)
    {
        int textoffset;
        if (*ToSwitch == false)
        {
            ColorInfo = ColorIdle;
            textoffset = textoffsetOff;
            Name = nameOff;
        }
        else
        {
            ColorInfo = ColorInteract;
            textoffset = textoffsetOn;
            Name = nameOn;
        }
        if (!ClickEvent(MousePos, textoffset, false))
        {
            return;
        }
        *ToSwitch = !*ToSwitch;
    }
};
template <typename Val>
struct StateForButtons
{
    Val Value;
    string Name;
    Color CurrentColor;
    int textoffset;
};

template <typename Val>
struct MultipleStateButton : GeneralRec
{
    vector<Val> StateValue;
    vector<string> StateName;
    vector<Color> StateColor;
    vector<int> StateOffset;
    int states = 0;

    bool ClickEvent(Vector2 MousePos, Vector2 FrameOffset)
    {
        if (!Visible)
        {
            return false;
        }
        DrawRectangleRounded(GetFrame(Info, FrameOffset), 0.2, 0.3, BLUE);
        DrawRectangleRec(Info, StateColor[states]);
        DrawText(StateName[states].c_str(), Info.x + Info.width / 2 - StateOffset[states], Info.y, Info.height, BLACK);
        if (!CheckCollisionPointRec(MousePos, Info))
        {
            return false;
        }
        if (!IsMouseButtonPressed(MOUSE_BUTTON_LEFT))
        {
            return false;
        }
        return true;
    }
    void SwitchBetweenValues(Vector2 MousePos, Val *WillBeSwitched, int size)
    {
        if (ClickEvent())
            states++;
        if (states > size)
        {
            states = 0;
        }
        *WillBeSwitched = StateValue[states];
    }
};
struct DraggableCircle : Objects
{
    Color ColorIdle;
    Color ColorInteract;
    Vector2 CenterPos;
    float minposx;
    float maxposx;
    float radius;
    DraggableCircle(Vector3 Info, Color ColorIdle, Color ColorInteract) : Objects(ColorIdle)
    {
        this->CenterPos.x = Info.x;
        this->CenterPos.y = Info.y;
        this->radius = Info.z;
        this->ColorInteract = ColorInteract;
        this->ColorIdle = ColorIdle;
    }
    void DragCircleXAxis(Vector2 &DeltaPos, Rectangle &Frame)
    {
        if (CenterPos.x + DeltaPos.x < Frame.x)
        {
            CenterPos.x = Frame.x;
            return;
        }
        if (CenterPos.x + DeltaPos.x > Frame.x + Frame.width)
        {
            CenterPos.x = Frame.x + Frame.width;
            return;
        }
        CenterPos.x = CenterPos.x + DeltaPos.x;
    }
    void DragCircleYAxis(Vector2 &DeltaPos, Rectangle &Frame)
    {
        if (CenterPos.y + DeltaPos.y < Frame.y)
        {
            CenterPos.y = Frame.y;
            return;
        }
        if (CenterPos.y + DeltaPos.y > Frame.y + Frame.height)
        {
            CenterPos.y = Frame.y + Frame.height;
            return;
        }
        CenterPos.y = CenterPos.y + DeltaPos.y;
    }
    void DragCircle(Vector2 &MousePos, Vector2 &DeltaPos, Rectangle &Frame, char axis)
    {
        DrawCircleV(CenterPos, (float)radius, ColorInfo);

        if (!CheckCollisionPointCircle(MousePos, CenterPos, radius))
        {
            ColorInfo = ColorIdle;
            return;
        }
        ColorInfo = ColorInteract;
        if (!IsMouseButtonDown(MOUSE_BUTTON_LEFT))
        {
            return;
        }
        switch (axis)
        {
        case 'x':
            DragCircleXAxis(DeltaPos, Frame);
            break;
        case 'y':
            DragCircleYAxis(DeltaPos, Frame);
            break;
        }
    }
};
struct Triangle
{
    Vector2 point1;
    Vector2 point2;
    Vector2 point3;
    Triangle(Vector2 point1, Vector2 point2, Vector2 point3, Color ColorInfo)
    {
        this->point1 = point1;
        this->point2 = point2;
        this->point3 = point3;
    }
};
