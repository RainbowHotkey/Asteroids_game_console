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

protected:
    // called by olcConsoleGameEngine
    virtual bool OnUserCreate()
    {
        return true;
    }

    // called by olcConsoleGameEngine
    virtual bool OnUserUpdate(float fElapsedTime)
    {
        // clear screen
        Fill(0, 0, ScreenWidth(), ScreenHeight(), PIXEL_SOLID, 0);
        return true;
    }
};

int main()
{
    std::cout << "Hello World!\n";
}

