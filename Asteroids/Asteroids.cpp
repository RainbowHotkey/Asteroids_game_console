#include <iostream>
#include <string>
#include <algorithm>
using namespace std;

#include "Game_engine/olcConsoleGameEngine.h"

class Hotkeys_Asteroids : public olcConsoleGameEngine
{
public:
    Hotkeys_Asteroids()
    {
        m_sAppName = L"Asteroids";
    }
    
private:
    struct SpaceObject
    {
        float x;
        float y;
        float dx;
        float dy;
        int nSize;
        float angle;
    };

    vector<SpaceObject> vecAsteroids;
    vector<SpaceObject> vecBullets;
    SpaceObject player;

    vector<pair<float, float>> vecModelShip;
    vector<pair<float, float>> vecModelAsteroids;

protected:
    // called by olcConsoleGameEngine
    virtual bool OnUserCreate()
    {
        vecAsteroids.push_back({ 20.0f, 20.0f, 8.0f, -6.0f, (int)16 , 0.0f});

        // initialise player position
        player.x = ScreenWidth() / 2.0f;
        player.y = ScreenHeight() / 2.0f;
        player.dx = 0.0f;
        player.dy = 0.0f;
        player.angle = 0.0f;

        vecModelShip =
        {
            {0.0f, -5.0f},
            {-2.5f, +2.5f},
            {+2.5f, +2.5f}
        }; // simple isoceles triangle

        int verts = 20;
        for (int i = 0; i < verts; i++)
        {
            float radius = 5.0f;
            float a = ((float)i / (float)verts) * 6.28318f;
            vecModelAsteroids.push_back(make_pair(radius * sinf(a), radius * cosf(a)));
        }

        return true;
    }

    // called by olcConsoleGameEngine
    virtual bool OnUserUpdate(float fElapsedTime)
    {
        // clear screen
        Fill(0, 0, ScreenWidth(), ScreenHeight(), PIXEL_SOLID, 0);


        // steer
        if (m_keys[VK_LEFT].bHeld)
            player.angle -= 5.0f * fElapsedTime;
        if (m_keys[VK_RIGHT].bHeld)
            player.angle += 5.0f * fElapsedTime;

        // Thrust
        if (m_keys[VK_UP].bHeld)
        {
            // acceleration changes velocity
            player.dx += sin(player.angle) * 20.0f * fElapsedTime;
            player.dy += -cos(player.angle) * 20.0f * fElapsedTime;
        }

        // velocity changes position
        player.x += player.dx * fElapsedTime;
        player.y += player.dy * fElapsedTime;

        // keep player in gamespace 
        WrapCoordinates(player.x, player.y, player.x, player.y);


        if (m_keys[VK_SPACE].bReleased)
            vecBullets.push_back({ player.x, player.y, 50.0f * sinf(player.angle), -50.0f * cosf(player.angle) });

        // update and draw asteroids
        for (auto& a : vecAsteroids)
        {
            a.x += a.dx * fElapsedTime;
            a.y += a.dy * fElapsedTime;
            WrapCoordinates(a.x, a.y, a.x, a.y);

            DrawWireFrameModel(vecModelAsteroids, a.x, a.y, a.angle);
        }

        // update and draw bullets
        for (auto& b : vecBullets)
        {
            b.x += b.dx * fElapsedTime;
            b.y += b.dy * fElapsedTime;
            WrapCoordinates(b.x, b.y, b.x, b.y);

            Draw(b.x, b.y);
        }

        // remove off screen bullets
        if (vecBullets.size() > 0)
        {
            auto i = remove_if(vecBullets.begin(), vecBullets.end(),
                [&](SpaceObject o) {return (o.x < 1 | o.y < 1 || o.x >= ScreenWidth() - 1 || o.y >= ScreenHeight() - 1); });
            if (i != vecBullets.end())
                vecBullets.erase(i);
        }

        //Draw ship?
        DrawWireFrameModel(vecModelShip, player.x, player.y, player.angle);

        return true;
    }

    void DrawWireFrameModel(const vector<pair<float, float>>& vecModelCoordinates, float x, float y, float r = 0.0f, float s = 1.0f, short  col = FG_WHITE)
    {
        // pair.first = x coordinate
        // pair.second = y coordinate 
        
        //create translated model vector of coordinate pairs 
        vector<pair<float, float>> vecTransformedCoordinates;
        int verts = vecModelCoordinates.size();
        vecTransformedCoordinates.resize(verts);

        // rotate 
        for (int i = 0; i < verts; i++)
        {
            vecTransformedCoordinates[i].first = vecModelCoordinates[i].first * cosf(player.angle) - vecModelCoordinates[i].second * sinf(player.angle);
            vecTransformedCoordinates[i].second = vecModelCoordinates[i].first * sinf(player.angle) + vecModelCoordinates[i].second * cosf(player.angle);
        }

        // scale 
        for (int i = 0; i < verts; i++)
        {
            vecTransformedCoordinates[i].first = vecTransformedCoordinates[i].first * s;
            vecTransformedCoordinates[i].second = vecTransformedCoordinates[i].second * s;
        }

        // translate
        for (int i = 0; i < verts; i++)
        {
            vecTransformedCoordinates[i].first = vecTransformedCoordinates[i].first + x;
            vecTransformedCoordinates[i].second = vecTransformedCoordinates[i].second + y;
        }

        // draw closed polygon
        for (int i = 0; i < verts; i++)
        {
            int j = (i + 1);
            DrawLine(vecTransformedCoordinates[i % verts].first, vecTransformedCoordinates[i % verts].second,
                vecTransformedCoordinates[j % verts].first, vecTransformedCoordinates[j % verts].second, PIXEL_SOLID, col);
        }
    }



    void WrapCoordinates(float ix, float iy, float& ox, float& oy)
    {
        ox = ix;
        oy = iy;
        if (ix < 0.0f) ox = ix + (float)ScreenWidth();
        if (ix >= (float)ScreenWidth()) ox = ix - (float)ScreenWidth();
        if (iy < 0.0f) oy = iy + (float)ScreenHeight();
        if (iy >= (float)ScreenHeight()) oy = iy - (float)ScreenHeight();
    }

    virtual void Draw(int x, int y, wchar_t c = 0x2588, short col = 0x000F)
    {
        float fx, fy;
        WrapCoordinates(x, y, fx, fy);
        olcConsoleGameEngine::Draw(fx, fy, c, col);
    }
};

int main()
{  
    // use olcConsoleGameEngine derived app
    Hotkeys_Asteroids game;
    game.ConstructConsole(160, 100, 8, 8);
    game.Start();
    return 0;
}

