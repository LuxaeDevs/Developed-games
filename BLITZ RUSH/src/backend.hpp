#include <raylib.h>
#include <sstream>
#include <iostream>
#include <cassert>
#include <vector>
#include <ctime>
#include <cmath>
#include <fstream>
#include <sstream>
#include <string>
#include <bits/stdc++.h>
#include <variant>

using std::cout;
using std::string;
using std::vector;
int FPS = 60;
int screenWidth = 800;
int screenHeight = 480;
int groundresistance = 4;
int airresistance = 1;
const int gravity = 2;
struct System
{
    int AtScreen = 0;
    int FramesElapsed = 0;
    int maxlevel = 25;
    float beginscroll = 0;
    float endscroll = 0;
    bool Loaded = false;
    bool Completed = false;
    string playerdata = "SaveTest.dll";
    string leveldata = "LevelTime.dll";
};
struct Coordinate
{
    float posx = 0;
    float posy = 0;
    Coordinate(int posx, int posy)
    {
        this->posx = posx;
        this->posy = posy;
    }
};
struct IngameTexts : Coordinate
{
    int fontsize;
    string text;
    Color TextColor;
    IngameTexts(float posx, float posy, int fontsize, string text, Color TextColor) : Coordinate(posx, posy)
    {
        this->fontsize = fontsize;
        this->text = text;
        this->TextColor = TextColor;
    }
    void DrawTexts()
    {
        DrawText(text.c_str(), posx, posy, fontsize, TextColor);
    }
};
struct Circle : Coordinate
{
    int radius;
    Circle(int setx, int sety, int setradius) : Coordinate(setx, sety)
    {
        radius = setradius;
    }
};
int StringSize(const char *string)
{
    int Size = 0;
    while (string[Size] != '\0')
        Size++;
    return Size;
}
void IsParalel(int beginningpos, int beginningsize, int checkpos, int checksize, bool *collided);
struct Square : Coordinate
{
    int width;
    int height;
    bool Visible = true;
    Color BlockColor;
    Square(int setx, int sety, int setwidth, int setheight, Color SetColor) : Coordinate(setx, sety)
    {
        width = setwidth;
        height = setheight;
        BlockColor = SetColor;
    }
    void UpdatePosition(int posx, int posy)
    {
        this->posx = posx;
        this->posy = posy;
    }
    void DrawObject()
    {

        DrawRectangle(posx, posy, width, height, BlockColor);
    }
};

// template time fuck you
struct General : Square
{
    int xvelocity = 0;
    int yvelocity = 0;
    int width;
    int height;
    int PartHit = -1;
    bool AtGround;
    bool XCollided = false;
    bool CollidedY = false;
    bool OnPlatform = false;
    bool CanInteract = true;
    General(int setx, int sety, int setwidth, int setheight, Color SetColor) : Square(setx, sety, setwidth, setheight, SetColor)
    {
        width = setwidth;
        height = setheight;
    }
    void ApplyGravity(float addvelocity)
    {
        yvelocity += addvelocity;
    }
    void Overlapped(bool *collided, int beginningpos, int beginningsize, int checkpos, int checksize, int deltas, int delta2, int halfs)
    {
        int delta = deltas / 10;
        switch (halfs)
        {
        case 0: // both floor and celiling / left and right
            if (beginningpos + delta <= checkpos + checksize + delta2 && beginningpos + beginningsize + delta >= checkpos + delta2)
            {
                *collided = true;
                return;
            }
            break;
        case 1: // ceiling / left
            if (beginningpos > checkpos + checksize && (beginningpos + delta) - (checkpos + checksize) <= 0)
            {
                *collided = true;
                return;
            }
            break;
        case 2: // floor / right
            if (beginningpos + beginningsize + delta > checkpos)
            {
                *collided = true;
                return;
            }
            break;
        }
        *collided = false;
        return;
    }
    void IsParalel(int beginningpos, int beginningsize, int checkpos, int checksize, bool *collided)
    {
        if (*collided == false)
        {
            return;
        }
        int n = 0;
        for (int x = 0; x < beginningsize / 2; x++)
        {
            if (beginningpos + beginningsize / 2 + x < checkpos || beginningpos + beginningsize / 2 - x > checkpos + checksize)
            {
                continue;
            }
            n++;
        }
        if (n == 0)
        {
            *collided = false;
            return;
        }
        *collided = true;
    }
    template <typename Type>
    bool CheckCollisionY(vector<Type> ToCheck, int vectsize, bool collided, int half, int *PartHit)
    {
        // int test = -1;
        //*PartHit = -1;
        for (int i = 0; i < vectsize; i++)
        {
            if (collided == true || ToCheck[i].CanInteract == false)
            {
                continue;
            }
            Overlapped(&collided, posy, height, ToCheck[i].posy, ToCheck[i].height, yvelocity, ToCheck[i].yvelocity, half);
            IsParalel(posx, width, ToCheck[i].posx, ToCheck[i].width, &collided);
            if (collided == true)
            {
                *PartHit = i;
            }
        }
        return collided;
    }
    // make this generic mayba???????????
    template <typename Type>
    bool CheckCollisionX(vector<Type> &ToCheck, int vectsize, bool collided, int half, int *PartHit)
    {
        //*PartHit = -1;
        for (int i = 0; i < vectsize; i++)
        {
            if (collided == true || ToCheck[i].CanInteract == false)
            {
                continue;
            }
            Overlapped(&collided, posx, width, ToCheck[i].posx, ToCheck[i].width, xvelocity, ToCheck[i].xvelocity, half);
            IsParalel(posy, height, ToCheck[i].posy, ToCheck[i].height, &collided);
            if (collided == true)
            {
                *PartHit = i;
            }
        }
        return collided;
    }
};
struct Disappearing : General
{
    int CurrentFrame;
    int MaxFading;
    int FrameDisappear;
    int FrameAppear;
    int Counter;
    float opacity;
    bool InteractableWhileInvis;
    int damage = 100;
    Color ColorToFeed;
    void FlipLogic()
    {
        if (InteractableWhileInvis == false)
        {
            CanInteract = Visible;
        }
        if (Visible == true)
        {
            Counter = FrameAppear;
            opacity = 1;
        }
        else
        {
            Counter = FrameDisappear;
            opacity = 0;
        }
    }
    Disappearing(int posx, int posy, int width, int height, float timeappear, float timedisappear,
                 float todisappear, bool InteractableWhileInvis, bool Visible, Color BlockColor)
        : General(posx, posy, width, height, BlockColor)
    {

        CurrentFrame = todisappear * FPS;
        MaxFading = todisappear * FPS;
        FrameAppear = (timeappear - todisappear) * FPS;
        FrameDisappear = (timedisappear - todisappear) * FPS;
        this->InteractableWhileInvis = InteractableWhileInvis;
        this->Visible = Visible;
        if (InteractableWhileInvis == true)
        {
            ColorToFeed = {230, 109, 180, 255};
        }
        else
        {
            ColorToFeed = BlockColor;
        }
        FlipLogic();
    }
    void Update()
    {
        BlockColor = Fade(ColorToFeed, opacity);
        if (Counter > 0)
        {
            Counter--;
            return;
        }

        if (CurrentFrame > 0)
        {
            if (Visible == false)
            {
                opacity = ((opacity * MaxFading) + 1) / MaxFading;
            }
            else
            {
                opacity = ((opacity * MaxFading) - 1) / MaxFading;
            }
            CurrentFrame--;
            return;
        }
        CurrentFrame = MaxFading;
        Visible = !Visible;
        FlipLogic();
    }
};
struct Movable : General
{
    float posdefaultx;
    float postdefaulty;
    int maxvelocity;
    int direction;
    int delta;
    bool MoveForward;
    int cooldown = 0;
    Movable(int posx, int posy, int width, int height, int delta, int maxvelocity, int direction, bool MoveForward, Color BlockColor)
        : General(posx, posy, width, height, BlockColor)

    {
        switch (direction)
        {
        case 0:
            this->posx = posx + delta / 2 - width / 2;
            posdefaultx = posx;
            this->delta = delta - width;
            break;

        case 1:
            this->posy = posy - delta / 2 + height / 2;
            postdefaulty = posy;
            this->delta = delta;
            break;
        }
        this->direction = direction;
        this->maxvelocity = maxvelocity;
        this->MoveForward = MoveForward;
    }

    void MoveXDirection()
    {
        if (posx + xvelocity > posdefaultx + delta || posx + xvelocity < posdefaultx)
        {
            xvelocity = 0;
            cooldown += 60;
            MoveForward = !MoveForward;
            return;
        }

        if (MoveForward == true)
        {
            xvelocity = maxvelocity / 10;
        }
        else
        {
            xvelocity = -maxvelocity / 10;
        }
    }
    void MoveYDirection()
    {
        if (posy + yvelocity <= postdefaulty - delta || posy + yvelocity >= postdefaulty)
        {
            yvelocity = 0;

            cooldown += 60;
            MoveForward = !MoveForward;
            return;
        }

        if (MoveForward == true)
        {
            yvelocity = maxvelocity / 10;
        }
        else
        {
            yvelocity = -maxvelocity / 10;
        }
    }
    void Update()
    {
        if (cooldown > 0)
        {
            cooldown--;
            return;
        }
        switch (direction)
        {
        case 0:
            MoveXDirection();
            break;

        case 1:
            MoveYDirection();
            break;
        }
        posx += xvelocity;
        posy += yvelocity;
    }
};
struct DamageMoving : Movable
{
    int damage;
};
struct Damaging : General
{
    float damage;
    Damaging(int setx, int sety, int setwidth, int setheight, int damage, Color SetColor)
        : General(setx, sety, setwidth, setheight, SetColor)
    {
        this->damage = damage;
    }
};
struct Player : General
{
    vector<float> Timer;
    float practicetime = 0;
    int speed = 50;
    float stamina = 100;
    float staminamax = 100;
    float staminaregen = 0.1;
    float CurrentHealth = 100;
    float MaxHealth = 100;
    int environmentxspeed = 0;

    int Defense = FPS;
    float BaseRegen = 0.05;
    int CurrentLevel = 0;

    bool Alive = true;
    bool NextLevel = false;
    bool PracticeMode = false;

    Player(int setx, int sety, int setwidth, int setheight, Color SetColor)
        : General(setx, sety, setwidth, setheight, SetColor)
    {
    }

    void PlayerUpdate()
    {
        // cout << posy + height + yvelocity << '\n';
        // cout << "Screen's height" << screenHeight << '\n';

        posy += yvelocity / 10;
        if (CollidedY == false)
        {
            AtGround = false;
        }

        ApplyGravity(gravity);
        posx += xvelocity / 10;
        if (Alive == false)
        {
            return;
        }
        if (CurrentHealth < 0)
        {
            Alive = false;
        }
        // cout << OnPlatform;
    }
    void Jump(int power, int drain)
    {
        if (AtGround == false || stamina - drain < 0 || Alive == false)
        {
            return;
        }
        AtGround = false;
        yvelocity -= power;
        stamina -= drain;
    }
    void Move(int speedchange)
    {
        if (Alive == false)
        {
            return;
        }

        if (IsKeyDown(KEY_LEFT_SHIFT) && stamina - 0.1 > 0)
        {
            xvelocity = -speedchange * 1.5;
            stamina -= staminaregen + 0.1;
            return;
        }
        xvelocity = -speedchange;
        // cout << xvelocity << '\n';
    }
    template <typename Type, typename Collide>
    void PlayerCollision(vector<Type> &ToCheck, int size, Player &MainPlayer,
                         void (*CollisionEffects)(vector<Type> &Aggressor, Collide &Affected, bool XCollide, bool YCollide, bool Ceiling, int parttouched))
    {
        PartHit = -1;
        bool YCollided = false;
        YCollided = CheckCollisionY<Type>(ToCheck, size, YCollided, 0, &PartHit);
        XCollided = false;
        XCollided = CheckCollisionX<Type>(ToCheck, size, XCollided, 0, &PartHit);
        bool AtCeiling = false;
        AtCeiling = CheckCollisionY<Type>(ToCheck, size, AtCeiling, 1, &PartHit);

        CollisionEffects(ToCheck, MainPlayer, XCollided, YCollided, AtCeiling, PartHit);
    }
};
template <typename Type, typename Collide>
void HarmlessEffect(vector<Type> &Aggressor, Collide &Affected, bool XCollided, bool YCollided, bool Ceiling, int parttouched)
{
    if (XCollided == true)
    {
        Affected.xvelocity = Aggressor[parttouched].xvelocity * 10;
    }
    if (Ceiling == true)
    {
        Affected.CollidedY = true;
        Affected.yvelocity = 0;
    }
    if (YCollided == false || Ceiling == true)
    {
        return;
    }
    Affected.AtGround = true;
    Affected.CollidedY = true;
    Affected.yvelocity = Aggressor[parttouched].yvelocity * 10;
}
template <typename Type, typename Collide>
void DamagingEffect(vector<Type> &Aggresor, Collide &Affected, bool XCollided, bool YCollided, bool Ceiling, int parttouched)
{
    if (XCollided == false || YCollided == false)
    {
        return;
    }
    float damage = Aggresor[parttouched].damage / Affected.Defense;
    Affected.CurrentHealth -= damage;
}
template <typename Type, typename Collide>
void MovingEffect(vector<Type> &Aggressor, Collide &Affected, bool XCollided, bool YCollided, bool Ceiling, int parttouched)
{
    if (XCollided == true)
    {
        Affected.xvelocity = Aggressor[parttouched].xvelocity * 10;
        // garbage
    }
    if (Ceiling == true)
    {
        Affected.CollidedY = true;
        Affected.yvelocity = 0;
    }
    Affected.OnPlatform = false;
    if (YCollided == false || Ceiling == true)
    {
        return;
    }
    if (Aggressor[parttouched].direction == 0)
    {
        if (Aggressor[parttouched].xvelocity > 0)
        {
            Affected.xvelocity = Aggressor[parttouched].xvelocity * 10 + 2.25 * groundresistance;
        }
        if (Aggressor[parttouched].xvelocity < 0)
        {
            Affected.xvelocity = Aggressor[parttouched].xvelocity * 10 - 2.25 * groundresistance;
        }
        Affected.OnPlatform = true;
    }
    Affected.AtGround = true;
    Affected.CollidedY = true;
    // cout << "Player's velocity: " << Affected.yvelocity << '\n';
    // cout << "Block velocity: " << Aggressor[parttouched].yvelocity * 10 << '\n';
    Affected.yvelocity = Aggressor[parttouched].yvelocity * 10;
}
template <typename Type, typename Collide>
void WinCollision(vector<Type> &Aggressor, Collide &Affected, bool XCollided, bool YCollided, bool Ceiling, int parttouched)
{
    Affected.NextLevel = false;
    if (XCollided == false || YCollided == false || Affected.Alive == false)
    {
        return;
    }
    Affected.NextLevel = true;
}
void SwitchLevel(Player &MainPlayer, bool resetpos);
void PlayerReset(Player &MainPlayer, System Settings);
template <typename T>
void InsertLevelTabs(vector<T> &GotoLevel, Player &MainPlayer);
void LoadGame(Player &MainPlayer, string filelocation);
struct Button : Square
{

    bool clicked = false;
    bool Visible = true;
    Color ColorAccept;
    Button(int setwidth, int setheight, Color SetColor, Color ColorAccept) : Square(0, 0, setwidth, setheight, SetColor)
    {
        this->ColorAccept = ColorAccept;
    }
    bool ButtonOnClick(Vector2 MouseButton, const char *text, int textoffset)
    {
        if (Visible == false)
        {
            return false;
        }

        if (MouseButton.x < posx || MouseButton.x > posx + width || MouseButton.y < posy || MouseButton.y > posy + height)
        {
            DrawRectangle(posx, posy, width, height, BlockColor);
            DrawText(text, posx + width / 2 - textoffset, posy, height, BLACK);
            return false;
        }
        DrawRectangle(posx, posy, width, height, ColorAccept);
        DrawText(text, posx + width / 2 - textoffset, posy, height, BLACK);
        if (!IsMouseButtonPressed(MOUSE_BUTTON_LEFT))
        {
            return false;
        }
        return true;
    }
    template <typename Value>
    void ChangeValue(Vector2 MousePos, Value *Affected, Value valueupdate, int textoffset, const char *text)
    {
        bool Clicked = ButtonOnClick(MousePos, text, textoffset);
        if (Clicked == false)
        {
            return;
        }
        *Affected = valueupdate;
    }
    void RespawnPlayer(Vector2 MousePos, System Settings, Player &MainPlayer, int textoffset, const char *text)
    {
        bool Clicked = ButtonOnClick(MousePos, text, textoffset);
        if (Clicked == false)
        {
            return;
        }
        PlayerReset(MainPlayer, Settings);
    }
    void ChangeLevelButton(Vector2 MousePos, int valueupdate, Player &MainPlayer, int textoffset, const char *text)
    {
        bool Clicked = ButtonOnClick(MousePos, text, textoffset);
        if (Clicked == false)
        {
            return;
        }
        MainPlayer.posx = -MainPlayer.width / 2;
        MainPlayer.posy = -100;
        MainPlayer.CurrentLevel = valueupdate;
        SwitchLevel(MainPlayer, true);
    }
    void WipeGame(Vector2 MousePos, System &GameSetting, Player &MainPlayer, int *Affected, int valueupdate, int textoffset, const char *text)
    {
        bool Clicked = ButtonOnClick(MousePos, text, textoffset);
        if (Clicked == false)
        {
            return;
        }
        MainPlayer.Timer.clear();
        MainPlayer.CurrentLevel = 0;

        GameSetting.Completed = false;
        *Affected = valueupdate;
        SwitchLevel(MainPlayer, true);
    }
    void LoadLevel(Vector2 MousePos, Player &MainPlayer, int *Affected, int valueupdate, int textoffset, const char *text, string filename)
    {
        bool Clicked = ButtonOnClick(MousePos, text, textoffset);
        if (Clicked == false)
        {
            return;
        }
        *Affected = valueupdate;
        SwitchLevel(MainPlayer, false);
        LoadGame(MainPlayer, filename);
        cout << MainPlayer.posx << '\n';
    }
    void LoadList(vector<Button> &GotoLevel, Player &MainPlayer, Vector2 MousePos, int textoffset, int *Screenie, const char *text)
    {
        bool Clicked = ButtonOnClick(MousePos, text, textoffset);
        if (Clicked == false)
        {
            return;
        }
        *Screenie = 1;
        GotoLevel.clear();
        InsertLevelTabs<Button>(GotoLevel, MainPlayer);
    }
};

struct IntStringPair
{
    float posx;
    float posy;
    int fontsize;
    string str;
};
struct PuttingInvis
{
    float posx;
    float posy;
    int width;
    int height;
    float appeartime;
    float disappeartime;
    float todisappear;
    bool invisinteractable;
    bool visible;
};
struct Objects
{
    System Settings;
    vector<Disappearing> HarmlessInvisible;
    vector<General> WinningPad;
    vector<General> Background;
    vector<General> Hitbox;
    vector<Damaging> DamagePart;
    vector<Movable> MovingPart;
    vector<IngameTexts> LevelText;
    vector<Button> GotoLevel;
};
Objects GameObject;
void PlayerReset(Player &MainPlayer, System Settings)
{

    MainPlayer.xvelocity = 0;
    MainPlayer.yvelocity = 0;
    MainPlayer.posx = -MainPlayer.width / 2;
    MainPlayer.posy = -100;
    MainPlayer.CurrentHealth = MainPlayer.MaxHealth;
    MainPlayer.stamina = MainPlayer.staminamax;
    MainPlayer.Alive = true;
    if (MainPlayer.PracticeMode == true)
    {
        MainPlayer.practicetime = 0;
        return;
    }
    if (MainPlayer.NextLevel == true || Settings.Loaded == false)
    {
        return;
    }
    MainPlayer.Timer[MainPlayer.CurrentLevel] = 0;
}
template <typename T>
void InsertLevelTabs(vector<T> &GotoLevel, Player &MainPlayer)
{
    GotoLevel.clear();
    for (int i = 0; i < MainPlayer.Timer.size(); i++)
    {
        GotoLevel.push_back(Button(400, 50, RED, GREEN));
    }
}
void InjectInvis(PuttingInvis Data[], vector<Disappearing> &ToPut, int length, Color Putting)
{
    for (int i = 0; i < length; i++)
    {
        ToPut.push_back(Disappearing(Data[i].posx, -Data[i].posy, Data[i].width, Data[i].height, Data[i].appeartime,
                                     Data[i].disappeartime, Data[i].todisappear, Data[i].invisinteractable, Data[i].visible, Putting));
    }
}
void InjectText(IntStringPair Data[], vector<IngameTexts> &LvlText, int length, Color Putting)
{
    for (int i = 0; i < length; i++)
    {
        LvlText.push_back(IngameTexts(Data[i].posx, -Data[i].posy, Data[i].fontsize, Data[i].str, Putting));
    }
}
template <typename T>
void InjectHitbox(vector<vector<int>> Data, vector<T> &ToPut, int datalength, Color Putting)
{
    for (int i = 0; i < datalength; i++)
    {
        ToPut.push_back(General(Data[i][0], -Data[i][1], Data[i][2], Data[i][3], Putting));
    }
}
void GetData(vector<string> &vectortoput, string filelocation)
{
    std::ifstream myfile(filelocation);
    std::string myline;
    if (myfile.is_open())
    {
        while (myfile.good())
        { // equivalent to myfile.good()
            std::getline(myfile, myline);
            vectortoput.push_back(myline);
        }
    }
    else
    {
        std::cout << "Couldn't open file\n";
    };
}

void LoadGame(Player &MainPlayer, string filelocation)
{
    vector<string> SaveData;
    GetData(SaveData, filelocation);
    if (SaveData.empty() || SaveData.size() < 5)
    {

        // MainPlayer.posy = 0;
        return;
    }
    MainPlayer.posx = std::stof(SaveData[0]);
    MainPlayer.posy = std::stof(SaveData[1]);
    MainPlayer.stamina = std::stof(SaveData[2]);
    MainPlayer.CurrentHealth = std::stof(SaveData[3]);
    std::istringstream(SaveData[4]) >> GameObject.Settings.Completed;
}

void InjectDamaging(vector<vector<int>> Data, vector<Damaging> &ToPut, int datalength, int damage, Color Putting)
{
    for (int i = 0; i < datalength; i++)
    {
        ToPut.push_back(Damaging(Data[i][0], -Data[i][1], Data[i][2], Data[i][3], damage, Putting));
    }
}
template <typename T>
void InjectMoving(vector<vector<int>> Data, vector<T> &ToPut, int datalength, int maxvelocity, bool direction, Color Putting)
{
    for (int i = 0; i < datalength; i++)
    {
        ToPut.push_back(T(Data[i][0], -Data[i][1], Data[i][2], Data[i][3], Data[i][4], maxvelocity, Data[i][5], direction, Putting));
    }
}
void RepeatingBlockX(vector<vector<int>> &Putting, Rectangle Data, int delta, int size, int amount)
{
    for (int i = 0; i < amount; i++)
    {
        Putting.push_back(vector<int>());
        Putting[size + i].push_back(Data.x + i * delta);
        Putting[size + i].push_back(Data.y);
        Putting[size + i].push_back(Data.width);
        Putting[size + i].push_back(Data.height);
    }
}
void RepeatingBlockY(vector<vector<int>> &Putting, Rectangle Data, int delta, int size, int amount)
{
    for (int i = 0; i < amount; i++)
    {
        Putting.push_back(vector<int>());
        Putting[size + i].push_back(Data.x);
        Putting[size + i].push_back(Data.y + i * delta);
        Putting[size + i].push_back(Data.width);
        Putting[size + i].push_back(Data.height);
    }
}
void MirrorAtX(vector<vector<int>> &WillBePut, vector<vector<int>> &ReferTo, int size, int mirrorlocation)
{
    for (int i = 0; i < size; i++)
    {
        WillBePut.push_back(vector<int>());
        WillBePut[i].push_back(-(ReferTo[i][0] + ReferTo[i][2] - mirrorlocation));

        for (int j = 1; j < ReferTo[i].size(); j++)
        {
            WillBePut[i].push_back(ReferTo[i][j]);
        }
    }
}
void MirrorInvisibleX(PuttingInvis &ToPut, PuttingInvis &Refer)
{
}
void GenerateLevel9()
{
    vector<vector<int>> HitboxData =
        {
            {300, 1125, 50, 1025},
            {600, 1125, 50, 1125},
            {2650, 1200, 50, 1200},
            {3700, 2050, 50, 800},
            {1000, 2450, 500, 100},
            {950, 2950, 50, 600},
            {1300, 2950, 50, 400}};
    RepeatingBlockY(HitboxData, {550, 125, 50, 25}, 250, HitboxData.size(), 4);
    RepeatingBlockY(HitboxData, {350, 250, 50, 25}, 250, HitboxData.size(), 4);
    InjectHitbox<General>(HitboxData, GameObject.Hitbox, HitboxData.size(), GREEN);
    vector<vector<int>> WinBox = {{-1500, 2650, 500, 100}};
    InjectHitbox<General>(WinBox, GameObject.Hitbox, WinBox.size(), GOLD);
    vector<vector<int>> BackgroundData =
        {{300, 1100, 300, 1100}};
    InjectHitbox<General>(BackgroundData, GameObject.Background, sizeof(BackgroundData) / sizeof(BackgroundData[0]), Fade((Color){0, 150, 40, 100}, 0.6f));

    vector<vector<int>>
        MoveForward = {
            {650, 1125, 300, 50, 1000, 0},
        };
    InjectMoving<Movable>(MoveForward, GameObject.MovingPart, MoveForward.size(), 30, false, PURPLE);
    vector<vector<int>>
        MoveBackwards =
            {
                {1650, 1125, 300, 50, 1000, 0}};
    InjectMoving<Movable>(MoveBackwards, GameObject.MovingPart, MoveBackwards.size(), 30, true, PURPLE);

    vector<vector<int>> BigDamage;
    RepeatingBlockX(BigDamage, {850, 1225, 100, 100}, 300, BigDamage.size(), 6);
    InjectDamaging(BigDamage, GameObject.DamagePart, BigDamage.size(), 120, Fade(RED, 0.8f));

    PuttingInvis InvisData[] =
        {
            {2700, 1200, 500, 100, 2.0f, 2.0f, 0.5f, true, true},
            {3600, 1300, 100, 50, 0.5f, 3.0f, 0.5f, true, true},
            {3600, 1425, 100, 50, 0.5f, 3.0f, 0.5f, true, false},
            {3600, 1550, 100, 50, 0.7f, 3.3f, 0.7f, true, true},
            {3600, 1675, 100, 50, 0.6f, 3.2f, 0.6f, true, false},
            {3600, 1800, 100, 50, 0.55f, 2.9f, 0.55f, true, true},
            {3600, 1925, 100, 50, 0.4f, 2.9f, 0.4f, true, false},
            {3600, 2050, 100, 50, 0.3f, 4.0f, 0.3f, true, false},
            {3200, 2150, 50, 50, 0.6f, 2.0f, 0.6f, true, false},
            {2800, 2250, 50, 50, 0.55f, 2.1f, 0.55f, true, true},
            {2400, 2350, 50, 50, 0.50f, 2.2f, 0.50f, true, false},
            {2000, 2450, 50, 50, 0.45f, 2.3f, 0.45f, true, true},
            {300, 2850, 50, 50, 0.3f, 3.2f, 0.3f, true, false},
            {-400, 2750, 50, 50, 0.3f, 3.3f, 0.3f, true, true},
            {1000, 2575, 100, 25, 3.0f, 3.0f, 0.5f, false, true},
            {1000, 2825, 100, 25, 3.0f, 3.0f, 0.5f, false, true},
            {1200, 2700, 100, 25, 3.0f, 3.0f, 0.5f, false, false},
            {1200, 2950, 100, 25, 3.0f, 3.0f, 0.5f, false, false}};
    InjectInvis(InvisData, GameObject.HarmlessInvisible, sizeof(InvisData) / sizeof(InvisData[0]), PINK);

    vector<vector<int>> WinningPadData = {{-1400, 2750, 300, 100}};
    InjectHitbox<General>(WinningPadData, GameObject.WinningPad, WinningPadData.size(), Fade(GOLD, 0.7f));

    IntStringPair LevelTexts[] = {{-1500, 2850, 50, "CONGRATULATIONS!"}};
    InjectText(LevelTexts, GameObject.LevelText, sizeof(LevelTexts) / sizeof(LevelTexts[0]), GOLD);

    IntStringPair LevelTexts2[] = {{-300, 170, 40, "YOUR TRIAL STARTS HERE"}};
    InjectText(LevelTexts2, GameObject.LevelText, sizeof(LevelTexts2) / sizeof(LevelTexts2[0]), MAROON);
}
void GenerateLevel8()
{
    vector<vector<int>> HitboxData =
        {
            {25, 1100, 400, 100},
        };
    InjectHitbox<General>(HitboxData, GameObject.Hitbox, HitboxData.size(), GREEN);
    PuttingInvis InvisData[] =
        {
            {200, 100, 100, 100, 2.0f, 2.0f, 0.5f, false, true},
            {300, 200, 100, 200, 2.0f, 2.0f, 0.5f, false, false},
            {400, 300, 100, 300, 2.0f, 2.0f, 0.5f, false, true},
            {500, 400, 100, 400, 2.0f, 2.0f, 0.5f, false, false},
            {600, 500, 600, 100, 0.5f, 1.5f, 0.5f, true, true},
            {1300, 600, 100, 100, 0.5f, 1.5f, 0.5f, true, false},
            {1800, 600, 100, 100, 0.5f, 1.5f, 0.5f, true, true},
            {1575, 800, 50, 500, 2.0f, 2.0f, 0.5f, false, false},
            {2100, 700, 100, 100, 0.5f, 1.5f, 0.5f, true, true},
            {2400, 800, 100, 100, 0.5f, 1.5f, 0.5f, true, false},
            {2700, 900, 100, 100, 0.5f, 1.5f, 0.5f, true, false},
            {2300, 1000, 100, 100, 0.5f, 2.0f, 0.5f, true, true},
            {1800, 1100, 100, 100, 0.25f, 2.0f, 0.25f, true, true},
            {1000, 1100, 400, 100, 0.25f, 2.0f, 0.25f, true, true},
            {675, 1500, 20, 500, 2.0f, 2.0f, 0.5f, false, true},
        };
    InjectInvis(InvisData, GameObject.HarmlessInvisible, sizeof(InvisData) / sizeof(InvisData[0]), PINK);
    vector<vector<int>> WinningPadData = {{75, 1200, 300, 100}};
    InjectHitbox<General>(WinningPadData, GameObject.WinningPad, WinningPadData.size(), Fade(GOLD, 0.7f));
}
void GenerateLevel7()
{
    vector<vector<int>> HitboxRight =
        {
            {200, 3400, 300, 100},
            {150, 3700, 50, 400},
            {200, 50, 200, 50},
            {550, 1725, 400, 50},
            {950, 2025, 50, 350},
            {1000, 1850, 1300, 50},
            {2300, 2025, 500, 225},
            {2000, 3400, 400, 100}};
    RepeatingBlockY(HitboxRight, {900, 1825, 50, 25}, 100, HitboxRight.size(), 3);
    RepeatingBlockX(HitboxRight, {1200, 2025, 20, 200}, 220, HitboxRight.size(), 5);
    RepeatingBlockX(HitboxRight, {1200, 2625, 20, 500}, 220, HitboxRight.size(), 5);
    // cout << HitboxRight.size();
    InjectHitbox<General>(HitboxRight, GameObject.Hitbox, HitboxRight.size(), GREEN);
    vector<vector<int>> HitboxLeft;
    MirrorAtX(HitboxLeft, HitboxRight, HitboxRight.size(), 0);
    InjectHitbox<General>(HitboxLeft, GameObject.Hitbox, HitboxRight.size(), GREEN);

    vector<vector<int>> DamageDataRight;
    RepeatingBlockX(DamageDataRight, {1000, 1900, 200, 50}, 220, DamageDataRight.size(), 6);
    RepeatingBlockY(DamageDataRight, {2400, 2500, 100, 100}, 400, DamageDataRight.size(), 3);
    RepeatingBlockY(DamageDataRight, {2500, 2300, 100, 100}, 400, DamageDataRight.size(), 3);
    RepeatingBlockY(DamageDataRight, {2600, 2500, 100, 100}, 400, DamageDataRight.size(), 3);
    InjectDamaging(DamageDataRight, GameObject.DamagePart, DamageDataRight.size(), 60, Fade(RED, 0.8f));

    vector<vector<int>> DamageDataLeft;
    MirrorAtX(DamageDataLeft, DamageDataRight, DamageDataRight.size(), 0);
    InjectDamaging(DamageDataLeft, GameObject.DamagePart, DamageDataLeft.size(), 60, Fade(RED, 0.8f));
    vector<vector<int>>
        MoveForwardR = {
            {350, 900, 100, 25, 825, 1},
            {2400, 2100, 300, 75, 1300, 1},
        };
    InjectMoving<Movable>(MoveForwardR, GameObject.MovingPart, MoveForwardR.size(), 30, true, PURPLE);

    vector<vector<int>> MoveForwardL;
    MirrorAtX(MoveForwardL, MoveForwardR, MoveForwardR.size(), 0);
    InjectMoving<Movable>(MoveForwardL, GameObject.MovingPart, MoveForwardL.size(), 30, true, PURPLE);

    vector<vector<int>> MoveBackwardsR = {
        {250, 75, 100, 25, 825, 1},
    };
    InjectMoving<Movable>(MoveBackwardsR, GameObject.MovingPart, MoveBackwardsR.size(), 30, false, PURPLE);

    vector<vector<int>> MoveBackwardsL;
    MirrorAtX(MoveBackwardsL, MoveBackwardsR, MoveBackwardsR.size(), 0);
    InjectMoving<Movable>(MoveBackwardsL, GameObject.MovingPart, MoveBackwardsL.size(), 30, false, PURPLE);

    PuttingInvis InvisData[] =
        {
            {1500, 3400, 100, 200, 2.0f, 2.0f, 0.5f, true, true},
            {1000, 3400, 100, 200, 2.0f, 2.0f, 0.5f, true, true},
            {-1500, 3400, 100, 200, 2.0f, 2.0f, 0.5f, true, true},
            {-1000, 3400, 100, 200, 2.0f, 2.0f, 0.5f, true, true}};
    InjectInvis(InvisData, GameObject.HarmlessInvisible, sizeof(InvisData) / sizeof(InvisData[0]), PINK);

    vector<vector<int>> WinningPadData = {{-500, 3500, 300, 100}};
    InjectHitbox<General>(WinningPadData, GameObject.WinningPad, WinningPadData.size(), Fade(GOLD, 0.7f));

    IntStringPair LevelTexts[] = {{-145, 150, 30, "It is time to decide"},
                                  {200, 3500, 40, "WRONG CHOICE"}};
    InjectText(LevelTexts, GameObject.LevelText, sizeof(LevelTexts) / sizeof(LevelTexts[0]), BLACK);
}

void GenerateLevel6()
{
    vector<vector<int>> HitboxData =
        {
            {-400, 100, 100, 100},
            {-500, 200, 100, 200},
            {-700, 400, 100, 400},
            {-800, 500, 100, 500},
            {-3000, 600, 700, 600},
            {-2840, 700, 50, 20},
            {-2500, 800, 50, 20},
            {-2200, 910, 50, 20},
            {-1800, 1000, 50, 20},
            {-1500, 1120, 50, 20},
            {-1400, 1250, 500, 100},
            {-1400, 1150, 1500, 150},
            {-450, 1250, 550, 100}};
    InjectHitbox<General>(HitboxData, GameObject.Hitbox, HitboxData.size(), GREEN);
    vector<vector<int>> MoveBackwards = {
        {-1600, 600, 200, 50, 700, 0},
    };
    InjectMoving<Movable>(MoveBackwards, GameObject.MovingPart, MoveBackwards.size(), 30, false, PURPLE);

    vector<vector<int>> MoveForward = {
        {-2300, 600, 200, 50, 700, 0},
    };
    InjectMoving<Movable>(MoveForward, GameObject.MovingPart, MoveForward.size(), 30, true, PURPLE);

    PuttingInvis InvisData[] =
        {
            {-600, 300, 100, 300, 2.0f, 2.0f, 0.5f, false, false},
            {-900, 600, 100, 600, 2.0f, 2.0f, 0.5f, false, true},
        };
    InjectInvis(InvisData, GameObject.HarmlessInvisible, sizeof(InvisData) / sizeof(InvisData[0]), PINK);

    IntStringPair LevelTexts[] = {{-1400, 1300, 30, "S key exist too you know :/"}};
    InjectText(LevelTexts, GameObject.LevelText, sizeof(LevelTexts) / sizeof(LevelTexts[0]), BLACK);

    vector<vector<int>> BigDamage = {{-900, 1250, 450, 150}, {-1400, 1500, 1500, 90}, {-2300, 400, 1500, 400}};
    InjectDamaging(BigDamage, GameObject.DamagePart, BigDamage.size(), 6000, RED);

    vector<vector<int>> WinningPadData = {{-325, 1350, 300, 100}};
    InjectHitbox<General>(WinningPadData, GameObject.WinningPad, WinningPadData.size(), Fade(GOLD, 0.7f));
}
void GenerateLevel5()
{
    vector<vector<int>> HitboxData =
        {
            {400, 50, 50, 50},
            {450, 100, 50, 100},
            {2100, 100, 50, 100},
            {2150, 50, 50, 50},
            {2550, 1050, 1000, 50}};

    InjectHitbox<General>(HitboxData, GameObject.Hitbox, HitboxData.size(), GREEN);

    vector<vector<int>> MoveBackwards =
        {
            {2400, 50, 150, 50, 1000, 1},
        };
    InjectMoving<Movable>(MoveBackwards, GameObject.MovingPart, MoveBackwards.size(), 30, false, PURPLE);

    vector<vector<int>> WinningPadData = {{3200, 1150, 300, 100}};
    InjectHitbox<General>(WinningPadData, GameObject.WinningPad, WinningPadData.size(), Fade(GOLD, 0.7f));

    IntStringPair LevelTexts[] =
        {{-280, 120, 30, "This platform occassionally disappears"},
         {600, 235, 30, "There are 2 types of it, the one that goes through when invisible, and the one that doesn't"},
         {900, 200, 30, "fortunately there are some way to distinguish them"},
         {2150, 120, 30, "Here, take this lift, I have a little suprise for you :)"},
         {2550, 1200, 30, "This one does go through, try it!"},
         {2570, 1165, 30, "Watch out, it might damage you!"}};
    InjectText(LevelTexts, GameObject.LevelText, sizeof(LevelTexts) / sizeof(LevelTexts[0]), BLACK);

    // my idea ran out
    PuttingInvis InvisData[] =
        {
            {-25, 200, 100, 50, 2.0f, 2.0f, 0.5f, true, true},
            {500, 100, 1600, 25, 1.5f, 1.0f, 0.8f, true, true},
            {3100, 1300, 50, 250, 2.0f, 2.0f, 0.5f, false, true}};
    InjectInvis(InvisData, GameObject.HarmlessInvisible, sizeof(InvisData) / sizeof(InvisData[0]), PINK);
}
void GenerateLevel4()
{
    IntStringPair LevelTexts[] = {{-70, 120, 30, "Where am I?"}};
    InjectText(LevelTexts, GameObject.LevelText, sizeof(LevelTexts) / sizeof(LevelTexts[0]), BLACK);

    vector<vector<int>> WinningPadData = {{6000, 100, 300, 100}};
    InjectHitbox<General>(WinningPadData, GameObject.WinningPad, WinningPadData.size(), Fade(GOLD, 0.7f));
}
void GenerateLevel3()
{
    vector<vector<int>> HitboxData = {
        {200, 50, 50, 50},
        {250, 100, 50, 100},
        {500, 100, 20, 100},
        {500, 800, 20, 600},
        {720, 100, 20, 100},
        {720, 800, 20, 600},
        {940, 100, 20, 100},
        {940, 800, 20, 600},
        {1160, 100, 20, 100},
        {1160, 800, 20, 600},
        {1400, 800, 220, 600},
        {1400, 100, 20, 100},
        {1600, 100, 20, 100},
    };
    InjectHitbox<General>(HitboxData, GameObject.Hitbox, HitboxData.size(), GREEN);

    vector<vector<int>> MediumDamage = {
        {300, 30, 200, 30},
        {520, 30, 200, 30},
        {740, 30, 200, 30},
        {960, 30, 200, 30},
        {1180, 30, 220, 30},
        {1420, 50, 180, 50}};
    InjectDamaging(MediumDamage, GameObject.DamagePart, sizeof(MediumDamage) / sizeof(MediumDamage[0]), 80, Fade(RED, 0.75f));

    IntStringPair LevelTexts[] = {{-350, 120, 30, "Remember level 2? Well here's a reminder :)"}};
    InjectText(LevelTexts, GameObject.LevelText, sizeof(LevelTexts) / sizeof(LevelTexts[0]), BLACK);

    vector<vector<int>> WinningPadData = {{1900, 100, 300, 100}};
    InjectHitbox<General>(WinningPadData, GameObject.WinningPad, WinningPadData.size(), Fade(GOLD, 0.7f));
}
void GenerateLevel2()
{
    vector<vector<int>> HitboxData = {
        {300, 100, 200, 100},
        {-300, 400, 150, 400},
        {0, 1000, 400, 50},
        {-600, 50, 50, 50},
        {-650, 100, 50, 100},
        {-700, 150, 50, 150},
        {-750, 200, 50, 200},
        {-1700, 200, 50, 200},
        {-1750, 150, 50, 150},
        {-1800, 100, 50, 100},
        {-1850, 50, 50, 50}};
    InjectHitbox<General>(HitboxData, GameObject.Hitbox, HitboxData.size(), GREEN);

    vector<vector<int>> Damaging = {{-1650, 50, 900, 50}};
    InjectDamaging(Damaging, GameObject.DamagePart, sizeof(Damaging) / sizeof(Damaging[0]), 48, Fade(RED, 0.7f));

    vector<vector<int>> MoveBackwards = {
        {450, 450, 100, 50, 550, 1},
        {-1650, 200, 200, 50, 450, 0}};
    InjectMoving<Movable>(MoveBackwards, GameObject.MovingPart, MoveBackwards.size(), 30, false, PURPLE);
    vector<vector<int>> MoveForward =
        {{550, 100, 100, 50, 550, 1}};
    InjectMoving<Movable>(MoveForward, GameObject.MovingPart, MoveForward.size(), 30, true, PURPLE);
    vector<vector<int>> WinningPadData = {{-2300, 100, 300, 100}};
    InjectHitbox<General>(WinningPadData, GameObject.WinningPad, WinningPadData.size(), Fade(GOLD, 0.7f));

    IntStringPair LevelTexts[] = {{-80, 60, 30, "Basic navigation test!"}};
    InjectText(LevelTexts, GameObject.LevelText, sizeof(LevelTexts) / sizeof(LevelTexts[0]), BLACK);
}
void GenerateLevel1()
{
    vector<vector<int>> HitboxData = {
        {200, 100, 50, 100},
        {-300, 400, 150, 400},
        {2100, 100, 50, 100},
        {2400, 50, 50, 50},
        {2450, 100, 50, 100},
        {2700, 100, 20, 100},
        {2700, 700, 20, 500},
        {2900, 100, 50, 100},
        {2950, 50, 50, 50}};
    InjectHitbox<General>(HitboxData, GameObject.Hitbox, HitboxData.size(), GREEN);

    vector<vector<int>> SmallDamage = {
        {2500, 30, 200, 30},
        {2720, 30, 180, 30}};
    InjectDamaging(SmallDamage, GameObject.DamagePart, SmallDamage.size(), 60, Fade(RED, 0.72f));

    vector<vector<int>> BigDamage = {{250, 50, 1850, 50}};
    InjectDamaging(BigDamage, GameObject.DamagePart, BigDamage.size(), 6000, RED);

    vector<vector<int>> MoveBackwards = {{250, 100, 200, 50, 1850, 0}};
    InjectMoving<Movable>(MoveBackwards, GameObject.MovingPart, MoveBackwards.size(), 25, false, PURPLE);

    vector<vector<int>> WinningPadData = {{3100, 100, 300, 100}};
    InjectHitbox<General>(WinningPadData, GameObject.WinningPad, WinningPadData.size(), Fade(GOLD, 0.7f));

    IntStringPair LevelTexts[] = {{300, 250, 30, "Lava's damage can be attributed to it's opacity, this lava patch down here instantly kills you"}};
    InjectText(LevelTexts, GameObject.LevelText, sizeof(LevelTexts) / sizeof(LevelTexts[0]), BLACK);
}
void GenerateLevel0()
{
    vector<vector<int>> HitboxData = {
        {600, 100, 100, 100},
        {800, 100, 100, 100},
        {700, 50, 100, 50},
        {600, 400, 300, 100},
        {-100, 25, 25, 25},
        {1000, 200, 300, 50},
        {2100, 200, 200, 200},
        {3000, 100, 50, 100},
        {3050, 200, 50, 200},
        {3650, 200, 50, 200}};

    vector<vector<int>> Damaging = {{700, 100, 100, 50}, {900, 50, 1200, 50}, {3100, 50, 550, 50}};

    vector<vector<int>> MovingStuff = {
        {1300, 300, 100, 50, 800, 0}};

    vector<vector<int>> WinningPadData = {{3940, 100, 300, 100}};

    IntStringPair LevelTexts[] = {
        {-275, 100, 30, "Use A and S to move left and right"},
        {300, 200, 30, "Press SPACE or W to jump"},
        {250, 165, 30, "Space is more powerful than W key!"},
        {900, 265, 30, "Watch out for the lava! it will kill you"},
        {800, 300, 30, "Please wait patiently for the moving purple block"},
        {1200, 400, 30, "Use the reset button located in the left corner if you're stuck"},
        {2320, 135, 30, "Hold SHIFT while moving to move faster"},
        {2320, 100, 30, "Now, test your ability by doing the BIG JUMP"},
        {3760, 130, 30, "Congratulations! You finished the tutorial"},
        {3700, 95, 30, "Now, finish it up by getting to the gold rectangle"},
        {3830, 60, 30, "And pressing \"Level Up\" button"}};

    InjectText(LevelTexts, GameObject.LevelText, sizeof(LevelTexts) / sizeof(LevelTexts[0]), BLACK);
    InjectHitbox<General>(HitboxData, GameObject.Hitbox, HitboxData.size(), GREEN);
    InjectHitbox<General>(WinningPadData, GameObject.WinningPad, WinningPadData.size(), Fade(GOLD, 0.7f));
    InjectMoving<Movable>(MovingStuff, GameObject.MovingPart, MovingStuff.size(), 20, true, PURPLE);
    InjectDamaging(Damaging, GameObject.DamagePart, Damaging.size(), 48, Fade(RED, 0.7f));
}
void SwitchLevel(Player &MainPlayer, bool resetpos)
{
    GameObject.Hitbox.clear();
    GameObject.DamagePart.clear();
    GameObject.MovingPart.clear();
    GameObject.WinningPad.clear();
    GameObject.LevelText.clear();
    GameObject.HarmlessInvisible.clear();
    GameObject.Background.clear();
    if (resetpos == true)
    {
        PlayerReset(MainPlayer, GameObject.Settings);
    }

    switch (MainPlayer.CurrentLevel)
    {
    case 0:
        GenerateLevel0();
        break;
    case 1:
        GenerateLevel1();
        break;
    case 2:
        GenerateLevel2();
        break;
    case 3:
        GenerateLevel3();
        break;
    case 4:
        GenerateLevel4();
        break;
    case 5:
        GenerateLevel5();
        break;
    case 6:
        GenerateLevel6();
        break;
    case 7:
        GenerateLevel7();
        break;
    case 8:
        GenerateLevel8();
        break;
    case 9:
        GenerateLevel9();
        break;
    default:

        InsertLevelTabs<Button>(GameObject.GotoLevel, MainPlayer);
        GameObject.Settings.AtScreen = 3;
        GameObject.Settings.Completed = true;
        MainPlayer.CurrentLevel--;
        return;
    }

    vector<vector<int>> GroundData = {{-10000, 0, 20000, 500}, {-11000, 500, 1000, 2000}, {10000, 500, 1000, 2000}};
    InjectHitbox<General>(GroundData, GameObject.Hitbox, sizeof(GroundData) / sizeof(GroundData[0]), BROWN);

    if (MainPlayer.Timer.size() > MainPlayer.CurrentLevel || MainPlayer.PracticeMode == true || GameObject.Settings.Loaded == false)
    {

        return;
    }
    MainPlayer.Timer.push_back(0);
}