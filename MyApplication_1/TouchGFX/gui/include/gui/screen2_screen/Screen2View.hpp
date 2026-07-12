#ifndef SCREEN2VIEW_HPP
#define SCREEN2VIEW_HPP

#include <gui_generated/screen2_screen/Screen2ViewBase.hpp>
#include <gui/screen2_screen/Screen2Presenter.hpp>

#include <cstdlib>
#define MAX_ENEMY_BULLETS 4
class Screen2View : public Screen2ViewBase
{
public:
    Screen2View();
    virtual ~Screen2View() {}
    virtual void setupScreen();
    virtual void tearDownScreen();

    virtual void handleTickEvent();
protected:
    bool isJoystickReleased;

//    // --- Enemy Configs ---
        static const int TOTAL_ENEMIES = 22; // Set to exactly 22 enemies
        touchgfx::Image* enemyBullets[MAX_ENEMY_BULLETS];
        touchgfx::ScalableImage* enemies[TOTAL_ENEMIES];
//
        int aliveIndices[TOTAL_ENEMIES];
        int enemySpeedX;
        int enemyShootTimer;

        int enemyStatus[TOTAL_ENEMIES];
};

#endif // SCREEN2VIEW_HPP
