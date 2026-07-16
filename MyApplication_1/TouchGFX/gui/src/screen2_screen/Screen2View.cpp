#include <gui/screen2_screen/Screen2View.hpp>
#include <stdio.h>
#include "audio_player.h"

int speedBulletEnemy;
int k = 0;
int explosionTimer = 0;

int playerHealth = 3;

// Lấy biến toàn cục từ main.c và hàm Hardware RNG
extern "C" {
    extern uint32_t globalJoystickX;
    extern uint32_t globalJoystickY;
    uint32_t GetHardwareRandomNumber(void);
}


// KHAI BÁO 5 ĐỘI HÌNH KẺ ĐỊCH (22 KẺ ĐỊCH MỖI ĐỘI HÌNH)

const int Screen2View::formationX[5][22] = {
    // 1. Khối Vuông
    { 5, 35, 65, 95, 125, 155, 185,   20, 50, 80, 110, 140, 170,   35, 65, 95, 125, 155,   50, 80, 110, 140 },
    // 2. Chữ V
    { 95, 65, 125, 35, 155, 5, 185,   95, 65, 125, 35, 155, 5, 185,   95, 65, 125, 35, 155,   95, 65, 125 },
    // 3. Mũi tên
    { 95, 95, 95, 65, 125, 35, 155,  5, 185, 95, 65, 125, 35, 155,   5, 185, 95, 65, 125,   35, 155, 5 },
    // 4. Trái Tim
    { 120, 90, 70, 140, 60, 150, 50, 160, 50, 160, 60, 150, 70, 140, 80, 130, 90, 120, 100, 110, 105, 115 },
    // 5. Ngôi Sao
    { 120, 120, 105, 135, 90, 150, 75, 165, 105, 135, 120, 120, 105, 135, 90, 150, 80, 160, 70, 170, 60, 180 }
};

const int Screen2View::formationY[5][22] = {
    // 1. Y Khối Vuông
    { 10, 10, 10, 10, 10, 10, 10,   35, 35, 35, 35, 35, 35,   60, 60, 60, 60, 60,   85, 85, 85, 85 },
    // 2. Y Chữ V
    { 10, 30, 30, 50, 50, 70, 70,   40, 60, 60, 80, 80, 100, 100,  70, 90, 90, 110, 110,  100, 120, 120 },
    // 3. Y Mũi tên
    { 10, 35, 60, 35, 35, 60, 60,   85, 85, 85, 60, 60, 85, 85,   110, 110, 110, 85, 85,  110, 110, 135 },
    // 4. Y Trái tim
    { 20, 20, 30, 30, 40, 40, 50, 50, 60, 60, 70, 70, 80, 80, 90, 90, 100, 100, 110, 110, 120, 120 },
    // 5. Y Ngôi sao
    { 20, 40, 40, 40, 60, 60, 60, 60, 80, 80, 80, 80, 100, 100, 100, 100, 120, 120, 120, 120, 120, 120 }
};

Screen2View::Screen2View()
{
}

void Screen2View::setupScreen()
{
    Screen2ViewBase::setupScreen();
    // Phát nhạc nền BGM: loop = 1 (lặp vô hạn), volume_level = 2 (nhỏ hơn 1 chút)
    Audio_PlayBGM_Config(1, 2);

    isPaused = false;
    ReleasePause.setVisible(false);
    ReleasePause.invalidate();

    enemies[0]  = &enemy1;   enemies[1]  = &enemy2;   enemies[2]  = &enemy3;
    enemies[3]  = &enemy4;   enemies[4]  = &enemy5;   enemies[5]  = &enemy6;
    enemies[6]  = &enemy7;   enemies[7]  = &enemy8;   enemies[8]  = &enemy9;
    enemies[9]  = &enemy10;  enemies[10] = &enemy11;  enemies[11] = &enemy12;
    enemies[12] = &enemy13;  enemies[13] = &enemy14;  enemies[14] = &enemy15;
    enemies[15] = &enemy16;  enemies[16] = &enemy17;  enemies[17] = &enemy18;
    enemies[18] = &enemy19;  enemies[19] = &enemy20;  enemies[20] = &enemy21;
    enemies[21] = &enemy22;

    enemyBullets[0] = &aline1;
    enemyBullets[1] = &aline2;
    enemyBullets[2] = &aline3;
    enemyBullets[3] = &aline4;
    enemyBullets[4] = &aline;

    exp1.setVisible(false); exp1.invalidate();
    exp2.setVisible(false); exp2.invalidate();
    exp3.setVisible(false); exp3.invalidate();
    exp4.setVisible(false); exp4.invalidate();
    exp5.setVisible(false); exp5.invalidate();
    explosionTimer = 0;

    playerHealth = 3;
    FullHealthBar.setVisible(true);  FullHealthBar.invalidate();
    HealthBar3.setVisible(false);    HealthBar3.invalidate();
    HealthBar1.setVisible(false);    HealthBar1.invalidate();

    lose.setVisible(false);
    lose.invalidate();
    ContinueWin.setVisible(false);
    ContinueWin.invalidate();
    Tital2.setVisible(false);
    Tital2.invalidate();

    isJoystickReleased = true;
    isVictorySoundPlayed = false;

    currentWave = 1;
    loadNextWave();
}

void Screen2View::tearDownScreen()
{
    Screen2ViewBase::tearDownScreen();
    Audio_SetTempo(124);
}

void Screen2View::loadNextWave()
{
    uint32_t randomVal = GetHardwareRandomNumber();
    int formationIndex = randomVal % 5;

    Audio_SetTempo(124);
    currentMusicPeriod = 124;

    // Khởi tạo vận tốc tối đa được phép
    maxReachedEnemySpeedX = 1.0f;
    minReachedMusicPeriod = 124;

    baseEnemySpeedX = 1.0f;
    enemySpeedX = 1;
    speedBulletEnemy = 1;
    enemyShootTimer = 90;
    explosionTimer = 0;

    bullet.setVisible(false);
    bullet.invalidate();
    aline.setVisible(false);
    aline.invalidate();

    for (int i = 0; i < 22; i++)
    {
        enemyStatus[i] = 1; // Hồi sinh
        if (enemies[i] != nullptr)
        {
            enemies[i]->invalidate();
            enemies[i]->setXY(formationX[formationIndex][i], formationY[formationIndex][i]);
            enemies[i]->setVisible(true);
            enemies[i]->invalidate();
        }
    }

    for (int b = 0; b < 5; b++)
    {
        if (enemyBullets[b] != nullptr)
        {
            enemyBullets[b]->setVisible(false);
            enemyBullets[b]->invalidate();
        }
    }
}

void Screen2View::handleTickEvent()
{
    if (isPaused || playerHealth <= 0) // Thêm chặn luồng khi chết để không xung đột
    {
        return;
    }


    // 1. DI CHUYỂN MÁY BAY (Trái / Phải)

    int currentX = plane.getX();
    int moveSpeed = 5;

    plane.invalidate();

    if (globalJoystickX > 2500)
    {
        currentX -= moveSpeed;
        if (currentX < 0) currentX = 0;
    }
    else if (globalJoystickX < 1500)
    {
        currentX += moveSpeed;
        if (currentX + plane.getWidth() > 240)
        {
            currentX = 240 - plane.getWidth();
        }
    }

    plane.setX(currentX);
    plane.invalidate();

    // Đồng bộ tọa độ thanh máu bám theo máy bay
    int barY = plane.getY() + plane.getHeight() + 5;
    int fullBarX = plane.getX() + (plane.getWidth() / 2) - (FullHealthBar.getWidth() / 2);
    int bar3X    = plane.getX() + (plane.getWidth() / 2) - (HealthBar3.getWidth() / 2);
    int bar1X    = plane.getX() + (plane.getWidth() / 2) - (HealthBar1.getWidth() / 2);

    FullHealthBar.setXY(fullBarX, barY);
    HealthBar3.setXY(bar3X, barY);
    HealthBar1.setXY(bar1X, barY);

    if (playerHealth == 3)      FullHealthBar.invalidate();
    else if (playerHealth == 2) HealthBar3.invalidate();
    else if (playerHealth == 1) HealthBar1.invalidate();


    // 2. KÍCH HOẠT BẮN ĐẠN TỪ MÁY BAY

    if (globalJoystickY < 1000)
    {
        if (!bullet.isVisible() && isJoystickReleased)
        {
            bullet.invalidate();
            int bulletX = plane.getX() + (plane.getWidth() / 2) - (bullet.getWidth() / 2);
            int bulletY = plane.getY() - bullet.getHeight();

            bullet.setXY(bulletX, bulletY);
            bullet.setVisible(true);
            bullet.invalidate();

            presenter->playerFired();
            isJoystickReleased = false;
        }
    }
    else
    {
        isJoystickReleased = true;
    }

    // ------------------------------------------------------
    // 3. DI CHUYỂN VIÊN ĐẠN TA VÀ VA CHẠM
    // ------------------------------------------------------
    if (bullet.isVisible())
    {
        bullet.invalidate();
        bullet.setY(bullet.getY() - 7);

        int bX = bullet.getX();
        int bY = bullet.getY();
        int bW = bullet.getWidth();
        int bH = bullet.getHeight();
        bool bulletHit = false;

        // KIỂM TRA VA CHẠM ĐẠN VỚI ĐẠN ĐỊCH
        for (int b = 0; b < 5; b++)
        {
            if (enemyBullets[b] != nullptr && enemyBullets[b]->isVisible())
            {
                int ebX = enemyBullets[b]->getX();
                int ebY = enemyBullets[b]->getY();
                int ebW = enemyBullets[b]->getWidth();
                int ebH = enemyBullets[b]->getHeight();

                if (bX < ebX + ebW && bX + bW > ebX && bY < ebY + ebH && bY + bH > ebY)
                {
                    // Cả 2 viên đạn biến mất
                    enemyBullets[b]->setVisible(false);
                    enemyBullets[b]->invalidate();

                    bullet.setVisible(false);
                    bulletHit = true;
                    break;
                }
            }
        }

        // Nếu đạn chưa chạm đạn địch, tiếp tục kiểm tra va chạm với kẻ địch
        if (!bulletHit)
        {
            for (int i = 0; i < 22; i++)
            {
                if (enemyStatus[i] == 1 && enemies[i] != nullptr && enemies[i]->isVisible())
                {
                    int eX = enemies[i]->getX();
                    int eY = enemies[i]->getY();
                    int eW = enemies[i]->getWidth();
                    int eH = enemies[i]->getHeight();

                    if (bX < eX + eW && bX + bW > eX && bY < eY + eH && bY + bH > eY)
                    {
                        enemies[i]->setVisible(false);
                        enemies[i]->invalidate();
                        enemyStatus[i] = 0;

                        int centerX = eX + (eW / 2);
                        int centerY = eY + (eH / 2);

                        exp1.setXY(centerX - exp1.getWidth()/2, centerY - exp1.getHeight()/2);
                        exp2.setXY(centerX - exp2.getWidth()/2, centerY - exp2.getHeight()/2);
                        exp3.setXY(centerX - exp3.getWidth()/2, centerY - exp3.getHeight()/2);
                        exp4.setXY(centerX - exp4.getWidth()/2, centerY - exp4.getHeight()/2);
                        exp5.setXY(centerX - exp5.getWidth()/2, centerY - exp5.getHeight()/2);

                        explosionTimer = 1;
                        bullet.setVisible(false);
                        break;
                    }
                }
            }
        }

        if (bullet.isVisible() && bullet.getY() + bullet.getHeight() < 0)
        {
            bullet.setVisible(false);
        }
        if(bullet.isVisible()){
            bullet.invalidate();
        }
    }


    // 4. DI CHUYỂN KẺ ĐỊCH, TĂNG TỐC ĐỘ, VÀ VA CHẠM MÁY BAY

    bool shiftDirection = false;
    int minAliveX = 999;
    int maxAliveX = -999;
    int rightmostWidth = 0;
    int maxAliveY = -999;
    int aliveCountForMove = 0;

    for (int i = 0; i < 22; i++)
    {
        if (enemyStatus[i] == 1 && enemies[i] != nullptr)
        {
            aliveCountForMove++;
            int currentEnemyX = enemies[i]->getX();
            int currentEnemyY = enemies[i]->getY();
            int currentEnemyH = enemies[i]->getHeight();

            if (currentEnemyX < minAliveX) minAliveX = currentEnemyX;
            if (currentEnemyX > maxAliveX)
            {
                maxAliveX = currentEnemyX;
                rightmostWidth = enemies[i]->getWidth();
            }
            if ((currentEnemyY + currentEnemyH) > maxAliveY)
            {
                maxAliveY = currentEnemyY + currentEnemyH;
            }
        }
    }

    if (aliveCountForMove > 0)
    {
        bool limitReached = (maxAliveY >= plane.getY());

        // TĂNG TỐC ĐỘ CHẬM HƠN VÀ KHÔNG GIẢM TỐC
        if (aliveCountForMove < 7 && !limitReached)
        {
            // Trọng số tăng chậm hơn: 0.1f mỗi 2 kẻ địch bị hạ
            int speedLevel = (7 - aliveCountForMove) / 2;
            float targetSpeedX = 1.0f + speedLevel * 0.1f;

            if(targetSpeedX > maxReachedEnemySpeedX) {
                maxReachedEnemySpeedX = targetSpeedX;
            }

            int musicLevel = (7 - aliveCountForMove) / 2;
            uint16_t targetPeriod = 124 - musicLevel * 2;

            if(targetPeriod < minReachedMusicPeriod) {
                minReachedMusicPeriod = targetPeriod;
            }

            // Chốt chặn tốc độ
            if (maxReachedEnemySpeedX > 1.5f) maxReachedEnemySpeedX = 1.5f;
            if (minReachedMusicPeriod < 110) minReachedMusicPeriod = 110;

            baseEnemySpeedX = maxReachedEnemySpeedX;
            if (minReachedMusicPeriod != currentMusicPeriod) {
                currentMusicPeriod = minReachedMusicPeriod;
                Audio_SetTempo(currentMusicPeriod);
            }
        }
        else
        {
           baseEnemySpeedX = maxReachedEnemySpeedX; // Giữ tốc độ tối đa đã đạt được
        }

        float currentSpeedWithDirection = (enemySpeedX > 0) ? baseEnemySpeedX : -baseEnemySpeedX;

        // KIỂM TRA CHẠM BIÊN VỚI RÀNG BUỘC HƯỚNG ĐI
        if ((enemySpeedX > 0) && (maxAliveX + rightmostWidth + currentSpeedWithDirection > 235))
        {
            shiftDirection = true;
        }
        else if ((enemySpeedX < 0) && (minAliveX + currentSpeedWithDirection < 5))
        {
            shiftDirection = true;
        }

        if (shiftDirection)
        {
            enemySpeedX = -enemySpeedX; // Đảo hướng di chuyển ngang


            // CHỈ XUỐNG HÀNG KHI CHƯA CHẠM ĐÁY VÀ CÒN < 7 CON

            if (!limitReached && aliveCountForMove < 7)
            {
                for (int i = 0; i < 22; i++)
                {
                    if (enemyStatus[i] == 1 && enemies[i] != nullptr)
                    {
                        enemies[i]->invalidate();
                        int nextY = enemies[i]->getY() + enemyStepDownY;
                        enemies[i]->setY(nextY);
                        enemies[i]->invalidate();
                    }
                }
            }
        }
        else
        {
            // Tiến hành dịch chuyển ngang
            for (int i = 0; i < 22; i++)
            {
                if (enemyStatus[i] == 1 && enemies[i] != nullptr)
                {
                    enemies[i]->invalidate();
                    enemies[i]->setX(enemies[i]->getX() + currentSpeedWithDirection);
                    enemies[i]->invalidate();
                }
            }
        }

        // KIỂM TRA VA CHẠM TRỰC TIẾP GIỮA ĐỊCH VÀ MÁY BAY TA
        int pX = plane.getX();
        int pY = plane.getY();
        int pW = plane.getWidth();
        int pH = plane.getHeight();

        for (int i = 0; i < 22; i++)
        {
            if (enemyStatus[i] == 1 && enemies[i] != nullptr)
            {
                int eX = enemies[i]->getX();
                int eY = enemies[i]->getY();
                int eW = enemies[i]->getWidth();
                int eH = enemies[i]->getHeight();

                if (eX < pX + pW && eX + eW > pX && eY < pY + pH && eY + eH > pY)
                {
                    enemies[i]->setVisible(false);
                    enemies[i]->invalidate();
                    enemyStatus[i] = 0;

                    int centerX = eX + (eW / 2);
                    int centerY = eY + (eH / 2);
                    exp1.setXY(centerX - exp1.getWidth()/2, centerY - exp1.getHeight()/2);
                    exp2.setXY(centerX - exp2.getWidth()/2, centerY - exp2.getHeight()/2);
                    exp3.setXY(centerX - exp3.getWidth()/2, centerY - exp3.getHeight()/2);
                    exp4.setXY(centerX - exp4.getWidth()/2, centerY - exp4.getHeight()/2);
                    exp5.setXY(centerX - exp5.getWidth()/2, centerY - exp5.getHeight()/2);
                    explosionTimer = 1;

                    playerHealth--;

                    FullHealthBar.invalidate(); FullHealthBar.setVisible(false);
                    HealthBar3.invalidate();     HealthBar3.setVisible(false);
                    HealthBar1.invalidate();     HealthBar1.setVisible(false);

                    if (playerHealth == 1)
                    {
                        HealthBar3.setVisible(true);
                        HealthBar3.invalidate();
                    }
                    else if (playerHealth == 2)
                    {
                        HealthBar1.setVisible(true);
                        HealthBar1.invalidate();
                    }
                    else if (playerHealth <= 0)
                    {
                        plane.setVisible(false);
                        plane.invalidate();
                        HealthBar1.setVisible(false);
                        HealthBar1.invalidate();
                        Tital2.setVisible(true);
                        Tital2.invalidate();
                        lose.setVisible(true);
                        lose.invalidate();
                        presenter->playerDestroyed();
                        return;
                    }
                }
            }
        }
    }


    // 5. ĐIỀU KHIỂN HOẠT ẢNH NỔ RẢI ĐỀU QUA CÁC FRAME

    if (explosionTimer > 0)
    {
        if (explosionTimer == 1) { exp1.setVisible(true); exp1.invalidate(); }
        else if (explosionTimer == 5) { exp1.setVisible(false); exp1.invalidate(); exp2.setVisible(true); exp2.invalidate(); }
        else if (explosionTimer == 9) { exp2.setVisible(false); exp2.invalidate(); exp3.setVisible(true); exp3.invalidate(); }
        else if (explosionTimer == 13) { exp3.setVisible(false); exp3.invalidate(); exp4.setVisible(true); exp4.invalidate(); }
        else if (explosionTimer == 17) { exp4.setVisible(false); exp4.invalidate(); exp5.setVisible(true); exp5.invalidate(); }
        else if (explosionTimer == 21) { exp5.setVisible(false); exp5.invalidate(); explosionTimer = 0; }

        if (explosionTimer > 0) explosionTimer++;
    }


    // LOGIC CHUYỂN MÀN CHƠI (WAVE) VÀ CHIẾN THẮNG

    int totalAlive = 0;
    for (int i = 0; i < 22; i++)
    {
        if (enemyStatus[i] == 1) totalAlive++;
    }

    if (totalAlive == 0 && explosionTimer == 0)
    {
        if (currentWave < 3)
        {
            currentWave++;
            loadNextWave();
        }
        else
        {
            if (!isVictorySoundPlayed)
            {
                Audio_PlayVictory();
                isVictorySoundPlayed = true;
            }

            if (!ContinueWin.isVisible())
            {
                ContinueWin.setVisible(true);
                ContinueWin.invalidate();
                Tital2.setVisible(true);
                Tital2.invalidate();
                plane.setVisible(false);
                plane.invalidate();
                HealthBar1.setVisible(false);
                HealthBar1.invalidate();
            }
        }
    }


    // 6. KÍCH HOẠT BẮN ĐẠN NGẪU NHIÊN CỦA ENEMY

    if (enemyShootTimer > 0)
    {
        enemyShootTimer--;
    }
    else
    {
        touchgfx::Image* availableBullet = nullptr;
        for (int b = 0; b < 5; b++)
        {
            if (enemyBullets[b] != nullptr && !enemyBullets[b]->isVisible())
            {
                availableBullet = enemyBullets[b];
                break;
            }
        }

        int aliveCount = 0;
        if (availableBullet != nullptr)
        {
            for (int i = 0; i < 22; i++)
            {
                if (enemyStatus[i] == 1)
                {
                    aliveIndices[aliveCount] = i;
                    aliveCount++;
                }
            }

            if (aliveCount > 0)
            {
                for(int i = 0; i < aliveCount; i++){
                    if( k < aliveCount ){ k++; break; }
                    else { k = k % aliveCount; break; }
                }
                int randomIndex = k;
                int shooterIndex = aliveIndices[randomIndex];
                touchgfx::ScalableImage* shooter = enemies[shooterIndex];

                if (shooter != nullptr)
                {
                    int alineX = shooter->getX() + (shooter->getWidth() / 2) - (availableBullet->getWidth() / 2);
                    int alineY = shooter->getY() + shooter->getHeight();
                    availableBullet->setXY(alineX, alineY);
                    availableBullet->setVisible(true);
                    availableBullet->invalidate();
                }
            }
        }

        enemyShootTimer = 80;
        if (aliveCount < 11){
            enemyShootTimer = 50;
            speedBulletEnemy = 1.5;
        }
    }


    // 7. DI CHUYỂN TOÀN BỘ ĐẠN ĐỊCH ĐANG BAY & KIỂM TRA VA CHẠM

    int pX = plane.getX();
    int pY = plane.getY();
    int pW = plane.getWidth();
    int pH = plane.getHeight();

    for (int b = 0; b < 5; b++)
    {
        if (enemyBullets[b] != nullptr && enemyBullets[b]->isVisible())
        {
            enemyBullets[b]->invalidate();
            enemyBullets[b]->setY(enemyBullets[b]->getY() + speedBulletEnemy);

            int ebX = enemyBullets[b]->getX();
            int ebY = enemyBullets[b]->getY();
            int ebW = enemyBullets[b]->getWidth();
            int ebH = enemyBullets[b]->getHeight();

            if (ebX < pX + pW && ebX + ebW > pX && ebY < pY + pH && ebY + ebH > pY)
            {
                enemyBullets[b]->setVisible(false);
                enemyBullets[b]->invalidate();

                playerHealth--;

                FullHealthBar.invalidate(); FullHealthBar.setVisible(false);
                HealthBar3.invalidate();     HealthBar3.setVisible(false);
                HealthBar1.invalidate();     HealthBar1.setVisible(false);

                if (playerHealth == 1)
                {
                    HealthBar3.setVisible(true);
                    HealthBar3.invalidate();
                }
                else if (playerHealth == 2)
                {
                    HealthBar1.setVisible(true);
                    HealthBar1.invalidate();
                }
                else if (playerHealth <= 0)
                {
                    plane.setVisible(false);
                    plane.invalidate();
                    HealthBar1.setVisible(false);
                    HealthBar1.invalidate();
                    Tital2.setVisible(true);
                    Tital2.invalidate();
                    lose.setVisible(true);
                    lose.invalidate();
                    presenter->playerDestroyed();
                    return;
                }
                continue;
            }

            if (enemyBullets[b]->getY() > 320)
            {
                enemyBullets[b]->invalidate();
                enemyBullets[b]->setVisible(false);
            }
            else
            {
                enemyBullets[b]->invalidate();
            }
        }
    }
}

void Screen2View::pauseButtonClicked()
{
    isPaused = !isPaused;
    if(isPaused){
        Tital2.setVisible(true);
        Tital2.invalidate();
        ReleasePause.setVisible(true);
        ReleasePause.invalidate();
    }
}

void Screen2View::PauseRelease()
{
    isPaused = false;
    Tital2.setVisible(false);
    Tital2.invalidate();
    ReleasePause.setVisible(false);
    ReleasePause.invalidate();
}
