#ifndef SCREEN2VIEW_HPP
#define SCREEN2VIEW_HPP

#include <gui_generated/screen2_screen/Screen2ViewBase.hpp>
#include <gui/screen2_screen/Screen2Presenter.hpp>

#include <cstdlib>
#define MAX_ENEMY_BULLETS 5
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


//    //  Enemy Configs 
        static const int TOTAL_ENEMIES = 22; // Set to exactly 22 enemies
        touchgfx::Image* enemyBullets[MAX_ENEMY_BULLETS];
        touchgfx::ScalableImage* enemies[TOTAL_ENEMIES];
//
        int aliveIndices[TOTAL_ENEMIES];
        int enemySpeedX;
        int enemyShootTimer;

        int enemyStatus[TOTAL_ENEMIES];

            bool isPaused;
             virtual void pauseButtonClicked();
             virtual void PauseRelease();
        bool isVictorySoundPlayed = false;

        float baseEnemySpeedX = 1.0f; // Tốc độ di chuyển ngang cơ bản
        int enemyStepDownY = 8;       // Khoảng cách cụm địch dịch xuống mỗi khi chạm biên (px)
        uint16_t currentMusicPeriod = 124; // Giá trị nạp lại của TIM7 (Period)

        float maxReachedEnemySpeedX; 
        uint16_t minReachedMusicPeriod;  
    int currentWave = 1; // Quản lý màn chơi hiện tại (1, 2, 3)
    void loadNextWave(); // Hàm load lại kẻ địch

    //  ĐỊNH NGHĨA 5 ĐỘI HÌNH TỌA ĐỘ (X, Y) 
    static const int formationX[5][TOTAL_ENEMIES];
    static const int formationY[5][TOTAL_ENEMIES];
};

#endif // SCREEN2VIEW_HPP
