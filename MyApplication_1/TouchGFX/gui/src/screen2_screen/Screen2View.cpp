#include <gui/screen2_screen/Screen2View.hpp>
#include <stdio.h>

int speedBulletEnemy ;
int k=0;
int explosionTimer = 0;// quản lý hiệu ứng nổ

int playerHealth = 3;// Biến quản lý máu của player (3 tương ứng full, giảm dần về 0)
// Lấy thẳng 2 biến từ main.c sang dùng trực tiếp
extern "C" {
    extern uint32_t globalJoystickX;
    extern uint32_t globalJoystickY;
}

Screen2View::Screen2View()
{
}

void Screen2View::setupScreen()
{
    Screen2ViewBase::setupScreen();
    speedBulletEnemy =1;
    isPaused = false;
    ReleasePause.setVisible(false);
	ReleasePause.invalidate();
    // Mới vào game ẩn viên đạn đi
    bullet.setVisible(false);
    bullet.invalidate();

//    hình trái tim di chuyển và nhả đạn
    	aline.setVisible(false);
        aline.invalidate();



        enemySpeedX = 1;
        enemyShootTimer = 90;
//
//        // Map all 22 unique heart formation enemies into the array
        enemies[0]  = &enemy1;   enemies[1]  = &enemy2;   enemies[2]  = &enemy3;
        enemies[3]  = &enemy4;   enemies[4]  = &enemy5;   enemies[5]  = &enemy6;
        enemies[6]  = &enemy7;   enemies[7]  = &enemy8;   enemies[8]  = &enemy9;
        enemies[9]  = &enemy10;  enemies[10] = &enemy11;  enemies[11] = &enemy12;
        enemies[12] = &enemy13;  enemies[13] = &enemy14;  enemies[14] = &enemy15;
        enemies[15] = &enemy16;  enemies[16] = &enemy17;  enemies[17] = &enemy18;
        enemies[18] = &enemy19;  enemies[19] = &enemy20;  enemies[20] = &enemy21;
        enemies[21] = &enemy22;

        // Map các viên đạn địch vào mảng
            enemyBullets[0] = &aline1;
            enemyBullets[1] = &aline2;
            enemyBullets[2] = &aline3;
            enemyBullets[3] = &aline4;
            enemyBullets[4] = &aline;

            // Ẩn toàn bộ đạn địch lúc bắt đầu
                for (int b = 0; b < MAX_ENEMY_BULLETS; b++)
                {
                    if (enemyBullets[b] != nullptr)
                    {
                        enemyBullets[b]->setVisible(false);
                        enemyBullets[b]->invalidate();
                    }
                }

                // THÊM MỚI: Ẩn tất cả các hình ảnh nổ khi vừa vào game
                    exp1.setVisible(false); exp1.invalidate();
                    exp2.setVisible(false); exp2.invalidate();
                    exp3.setVisible(false); exp3.invalidate();
                    exp4.setVisible(false); exp4.invalidate();
                    exp5.setVisible(false); exp5.invalidate();
                    explosionTimer = 0; // Đặt bộ đếm về trạng thái nghỉ


                    // THÊM MỚI: Khởi tạo thanh máu ban đầu (Hiện full, ẩn các thanh mất máu)
                        playerHealth = 3;
                        FullHealthBar.setVisible(true);  FullHealthBar.invalidate();
                        HealthBar3.setVisible(false);    HealthBar3.invalidate(); // Thanh này ứng với còn 2/3 máu
                        HealthBar1.setVisible(false);    HealthBar1.invalidate(); // Thanh này ứng với còn 1/3 máu

        // CRITICAL FIX 1: Phải đặt trạng thái ban đầu cho 22 enemy là CÒN SỐNG (1)
            for (int i = 0; i < TOTAL_ENEMIES; i++)
            {
                enemyStatus[i] = 1;
            }

//           xét Lose, continue, tital ẩn đi  trước
            lose.setVisible(false);
            lose.invalidate();
            ContinueWin.setVisible(false);
			ContinueWin.invalidate();
			Tital2.setVisible(false);
			Tital2.invalidate();

    // Khởi tạo cờ nhả joystick ban đầu là true
    isJoystickReleased = true;
}

void Screen2View::tearDownScreen()
{
    Screen2ViewBase::tearDownScreen();
}

// Hàm này TouchGFX tự động gọi liên tục (60 lần/giây)
void Screen2View::handleTickEvent()
{
	if (isPaused)
	    {
	        return;
	    }
    // ------------------------------------------------------
    // 1. DI CHUYỂN MÁY BAY (Trái / Phải)
    // ------------------------------------------------------
	if (playerHealth > 0)
	    {
	        int currentX = plane.getX();
    int moveSpeed = 5; // Tốc độ di chuyển máy bay

    // Xóa hình ảnh máy bay ở vị trí CŨ trước khi dịch chuyển (SỬA LỖI DƯ ẢNH)
    plane.invalidate();

    // Kiểm tra Joystick X (Đã đảo ngược lại hướng đúng)
    if (globalJoystickX > 2500) // Gạt joystick sang TRÁI (ADC giảm)
    {
        currentX -= moveSpeed;
        if (currentX < 0) currentX = 0;
    }
    else if (globalJoystickX < 1500) // Gạt joystick sang PHẢI (ADC tăng)
    {
        currentX += moveSpeed;
        if (currentX + plane.getWidth() > 240) // Chặn biên phải màn hình 240px
        {
            currentX = 240 - plane.getWidth();
        }
    }

    // Cập nhật vị trí máy bay MỚI và vẽ lại (SỬA LỖI DƯ ẢNH)
    plane.setX(currentX);
    plane.invalidate();
    // ======================================================
        // THÊM MỚI: ĐỒNG BỘ TỌA ĐỘ THANH MÁU THEO MÁY BAY
        // ======================================================
        if (playerHealth > 0)
        {
            // 1. Chỉ thực hiện xóa vết vẽ cũ (invalidate) trên màn hình đối với thanh máu đang HIỆN THỰC TẾ
            if (playerHealth == 3)      FullHealthBar.invalidate();
            else if (playerHealth == 2) HealthBar3.invalidate();
            else if (playerHealth == 1) HealthBar1.invalidate();

            // 2. Tính toán tọa độ X, Y để thanh máu bám sát theo máy bay (Căn giữa phía dưới máy bay 5px)
            // Nếu muốn thanh máu nằm TRÊN ĐẦU máy bay, hãy sửa dòng dưới thành: int barY = plane.getY() - FullHealthBar.getHeight() - 5;
            int barY = plane.getY() + plane.getHeight() + 5;

            int fullBarX = plane.getX() + (plane.getWidth() / 2) - (FullHealthBar.getWidth() / 2);
            int bar3X    = plane.getX() + (plane.getWidth() / 2) - (HealthBar3.getWidth() / 2);
            int bar1X    = plane.getX() + (plane.getWidth() / 2) - (HealthBar1.getWidth() / 2);

            // 3. Set vị trí mới cho cả 3 thanh (để khi chuyển đổi trạng thái máu không bị lệch tọa độ)
            FullHealthBar.setXY(fullBarX, barY);
            HealthBar3.setXY(bar3X, barY);
            HealthBar1.setXY(bar1X, barY);

            // 4. Vẽ lại thanh máu ở vị trí mới
            if (playerHealth == 3)      FullHealthBar.invalidate();
            else if (playerHealth == 2) HealthBar3.invalidate();
            else if (playerHealth == 1) HealthBar1.invalidate();
        }

    // ------------------------------------------------------
    // 2. KÍCH HOẠT BẮN ĐẠN (Mỗi lần đẩy chỉ ra 1 viên)
    // ------------------------------------------------------
    // LƯU Ý: Nếu trên UART debug của bạn đẩy lên mà ADC tăng thì sửa dưới đây thành: globalJoystickY > 3000
    if (globalJoystickY < 1000) // Người chơi đang ĐẨY joystick lên trên
    {
        // Chỉ cho phép bắn nếu viên đạn đang ẩn VÀ người chơi đã từng nhả joystick ra trước đó
        if (!bullet.isVisible() && isJoystickReleased)
        {
            // Xóa ảnh cũ viên đạn (nếu có)
            bullet.invalidate();

            // Đặt viên đạn vào chính giữa đầu máy bay
            int bulletX = plane.getX() + (plane.getWidth() / 2) - (bullet.getWidth() / 2);
            int bulletY = plane.getY() - bullet.getHeight();

            bullet.setXY(bulletX, bulletY);
            bullet.setVisible(true); // Hiện đạn
            bullet.invalidate();     // Vẽ đạn ở vị trí mới

            // Khóa lại, không cho bắn liên tục cho đến khi nhả joystick ra
            isJoystickReleased = false;
        }
    }
    else // Người chơi đã thả joystick về vị trí giữa (hoặc không đẩy lên)
    {
        isJoystickReleased = true; // Mở khóa cho lần bắn tiếp theo
    }



	    }


    // ------------------------------------------------------
    // 3. DI CHUYỂN VIÊN ĐẠN (Nếu nó đang bay)
    // ------------------------------------------------------
    if (bullet.isVisible())
    {
        bullet.invalidate(); // Xóa ảnh viên đạn ở vị trí cũ

        bullet.setY(bullet.getY() - 7); // Cho đạn bay thẳng lên trên



        // Lấy thông tin tọa độ/kích thước đạn ta
                int bX = bullet.getX();
                int bY = bullet.getY();
                int bW = bullet.getWidth();
                int bH = bullet.getHeight();

                // Quét tất cả enemy để kiểm tra va chạm
                for (int i = 0; i < TOTAL_ENEMIES; i++)
                {
                    if (enemyStatus[i] == 1 && enemies[i] != nullptr && enemies[i]->isVisible())
                    {
                        int eX = enemies[i]->getX();
                        int eY = enemies[i]->getY();
                        int eW = enemies[i]->getWidth();
                        int eH = enemies[i]->getHeight();

                        // Thuật toán kiểm tra va chạm (AABB Bounding Box)
                        if (bX < eX + eW && bX + bW > eX && bY < eY + eH && bY + bH > eY)
                        {
                            // 1. Ẩn enemy bị trúng đạn và đánh dấu đã chết
                            enemies[i]->setVisible(false);
                            enemies[i]->invalidate();
                            enemyStatus[i] = 0;

                            // 2. Tính toán tâm vị trí nổ dựa trên tọa độ enemy
                            int centerX = eX + (eW / 2);
                            int centerY = eY + (eH / 2);

                            // Di chuyển toàn bộ các Widget nổ về tâm vụ nổ
                            exp1.setXY(centerX - exp1.getWidth()/2, centerY - exp1.getHeight()/2);
                            exp2.setXY(centerX - exp2.getWidth()/2, centerY - exp2.getHeight()/2);
                            exp3.setXY(centerX - exp3.getWidth()/2, centerY - exp3.getHeight()/2);
                            exp4.setXY(centerX - exp4.getWidth()/2, centerY - exp4.getHeight()/2);
                            exp5.setXY(centerX - exp5.getWidth()/2, centerY - exp5.getHeight()/2);

                            // 3. Kích hoạt bộ đếm bắt đầu chạy hiệu ứng nổ từ frame sau
                            explosionTimer = 1;

                            // 4. Ẩn viên đạn ta đi
                            bullet.setVisible(false);
                            break; // Thoát vòng lặp quét va chạm vì đạn đã nổ
                        }
                    }
                }

        // Nếu đạn bay mất xác khỏi mép đỉnh màn hình (Y < 0)
        if (bullet.getY() + bullet.getHeight() < 0)
        {
            bullet.setVisible(false); // Ẩn đi để giải phóng bộ nhớ vẽ
        }
        bullet.invalidate(); // Vẽ viên đạn ở vị trí mới
    }


    // ======================================================
        // 4. DI CHUYỂN CỤM TRÁI TIM DỰA TRÊN BIÊN CỦA ENEMY CÒN SỐNG (FIX TREO MÀN HÌNH)
        // ======================================================
        bool shiftDirection = false;

        // Khởi tạo giá trị biên
        int minAliveX = 999;
        int maxAliveX = -999;
        int rightmostWidth = 0;
        int aliveCountForMove = 0;

        // Bước 1: Quét tìm biên thực tế của những con thực sự còn sống
        for (int i = 0; i < TOTAL_ENEMIES; i++)
        {
            // Kiểm tra đúng mảng enemyStatus bạn dùng để quản lý sinh tử
            if (enemyStatus[i] == 1 && enemies[i] != nullptr)
            {
                aliveCountForMove++;
                int currentEnemyX = enemies[i]->getX();

                if (currentEnemyX < minAliveX)
                {
                    minAliveX = currentEnemyX;
                }
                if (currentEnemyX > maxAliveX)
                {
                    maxAliveX = currentEnemyX;
                    rightmostWidth = enemies[i]->getWidth();
                }
            }
        }

        // Bước 2: Chỉ tính toán di chuyển khi còn ít nhất 1 con sống
        if (aliveCountForMove > 0)
        {
            // Kiểm tra va chạm biên trái thực tế sau khi cộng vận tốc hiện tại
            if ((minAliveX + enemySpeedX) < 5)
            {
                shiftDirection = true;
            }
            // Kiểm tra va chạm biên phải thực tế
            else if ((maxAliveX + rightmostWidth + enemySpeedX) > 235)
            {
                shiftDirection = true;
            }

            // Nếu chạm biên thì đảo hướng ngay lập tức
            if (shiftDirection)
            {
                enemySpeedX = -enemySpeedX;
            }

            // Bước 3: Di chuyển và cập nhật lại giao diện cho các con còn sống
            for (int i = 0; i < TOTAL_ENEMIES; i++)
            {
                if (enemyStatus[i] == 1 && enemies[i] != nullptr)
                {
                    enemies[i]->invalidate(); // Xóa vết cũ của riêng con này
                    enemies[i]->setX(enemies[i]->getX() + enemySpeedX); // Dịch vị trí mới
                    enemies[i]->invalidate(); // Vẽ lại vị trí mới
                }
            }
        }








    // ======================================================
        // THÊM MỚI: ĐOẠN ĐIỀU KHIỂN HOẠT ẢNH NỔ RẢI ĐỀU QUA CÁC FRAME
        // ======================================================
        if (explosionTimer > 0)
        {
            // Cứ sau mỗi 4 frame (khoảng 0.06 giây) ta thực hiện ẩn hình cũ và hiện hình tiếp theo
            if (explosionTimer == 1)
            {
                exp1.setVisible(true);
                exp1.invalidate();
            }
            else if (explosionTimer == 5)
            {
                exp1.setVisible(false); exp1.invalidate();
                exp2.setVisible(true);  exp2.invalidate();
            }
            else if (explosionTimer == 9)
            {
                exp2.setVisible(false); exp2.invalidate();
                exp3.setVisible(true);  exp3.invalidate();
            }
            else if (explosionTimer == 13)
            {
                exp3.setVisible(false); exp3.invalidate();
                exp4.setVisible(true);  exp4.invalidate();
            }
            else if (explosionTimer == 17)
            {
                exp4.setVisible(false); exp4.invalidate();
                exp5.setVisible(true);  exp5.invalidate();
            }
            else if (explosionTimer == 21)
            {
                // Frame thứ 21: Ẩn nốt hình cuối cùng và đưa bộ đếm về 0 (kết thúc nổ)
                exp5.setVisible(false); exp5.invalidate();
                explosionTimer = 0;
            }

            // Nếu bộ đếm vẫn đang chạy hiệu ứng thì tăng dần qua mỗi frame
            if (explosionTimer > 0)
            {
                explosionTimer++;
            }
        }

        int totalAlive = 0;
            for (int i = 0; i < TOTAL_ENEMIES; i++)
            {
                if (enemyStatus[i] == 1)
                {
                    totalAlive++;
                }
            }
        if (totalAlive == 0 && explosionTimer == 0)
            {

        	ContinueWin.setVisible(true);
        	ContinueWin.invalidate();
        	Tital2.setVisible(true);
			Tital2.invalidate();
			plane.setVisible(false);
			plane.invalidate();
			HealthBar1.setVisible(false);
			HealthBar1.invalidate();
        	return;

                // Chữ S viết hoa theo đúng chuẩn gợi ý của hệ thống
//                application().gotoScreen1ScreenNoTransition();
//                return;
            }


//        // ======================================================
//        // 4. KÍCH HOẠT BẮN ĐẠN NGẪU NHIÊN CỦA ENEMY
//        // ======================================================
    // 5. CỨ 0.8 GIÂY LÀ CHỌN ENEMY BẮN TIẾP (KHÔNG ĐỢI ĐẠN CŨ RƠI XUỐNG CUỐI)
        // ------------------------------------------------------
        if (enemyShootTimer > 0)
        {
            enemyShootTimer--; // Bộ đếm liên tục giảm mỗi frame độc lập
        }
        else
        {
            // Khi đếm về 0 (đủ 1.5 giây), tìm 1 viên đạn đang rảnh (đang ẩn) trong mảng để bắn
            touchgfx::Image* availableBullet = nullptr;
            for (int b = 0; b < MAX_ENEMY_BULLETS; b++)
            {
                if (enemyBullets[b] != nullptr && !enemyBullets[b]->isVisible())
                {
                    availableBullet = enemyBullets[b];
                    break; // Tìm thấy viên đang rảnh rồi thì dừng vòng lặp
                }
            }


            int aliveCount = 0;
            // Nếu tìm được viên đạn rảnh để bắn
            if (availableBullet != nullptr)
            {
//                int aliveCount = 0;
                // Quét tìm xem những ai còn sống
                for (int i = 0; i < TOTAL_ENEMIES; i++)
                {
                    if (enemyStatus[i] == 1)
                    {
                        aliveIndices[aliveCount] = i;
                        aliveCount++;
                    }
                }


                // Nếu còn enemy sống thì bốc ngẫu nhiên 1 con để bắn viên đạn rảnh này
                if (aliveCount > 0)
                {

                	for(int i = 0; i <aliveCount;i++){
                		if( k < aliveCount ){
                			k++;
                			break;
                		}else{
                			k= k % aliveCount;
                			break;
                		}
                	}
                    int randomIndex = k;
                    int shooterIndex = aliveIndices[randomIndex];
                    touchgfx::ScalableImage* shooter = enemies[shooterIndex];

                    if (shooter != nullptr)
                    {
                        // Tính tọa độ xuất hiện dựa trên viên đạn rảnh vừa tìm được
                        int alineX = shooter->getX() + (shooter->getWidth() / 2) - (availableBullet->getWidth() / 2);
                        int alineY = shooter->getY() + shooter->getHeight();

                        availableBullet->setXY(alineX, alineY);
                        availableBullet->setVisible(true);
                        availableBullet->invalidate();
                    }
                }
            }
            // cứ hết 0.8 giây là phải reset bộ đếm để tính lượt tiếp theo
            enemyShootTimer = 80;
            if( aliveCount < 11){
            	enemyShootTimer = 50;
            	speedBulletEnemy=1.5;
            }
        }


        // ------------------------------------------------------
        // 6. DI CHUYỂN ĐỘC LẬP TOÀN BỘ CÁC VIÊN ĐẠN ĐỊCH ĐANG BAY
        // ------------------------------------------------------

        if(playerHealth > 0)
        {
        	int pX = plane.getX();
        	int pY = plane.getY();
        	int pW = plane.getWidth();
        	int pH = plane.getHeight();

        		for (int b = 0; b < MAX_ENEMY_BULLETS; b++)
        		{
        			if (enemyBullets[b] != nullptr && enemyBullets[b]->isVisible())
        			{
        				enemyBullets[b]->invalidate(); // Xóa vết cũ

        				enemyBullets[b]->setY(enemyBullets[b]->getY() + speedBulletEnemy); // Đạn rơi xuống


        				// --- ĐOẠN THÊM MỚI: KIỂM TRA VA CHẠM VỚI MÁY BAY TA ---
        				                int ebX = enemyBullets[b]->getX();
        				                int ebY = enemyBullets[b]->getY();
        				                int ebW = enemyBullets[b]->getWidth();
        				                int ebH = enemyBullets[b]->getHeight();

        				                // Kiểm tra 2 hình chữ nhật va chạm (Đạn địch giao Máy bay ta)
        				                if (ebX < pX + pW && ebX + ebW > pX && ebY < pY + pH && ebY + ebH > pY)
        				                {
        				                    // 1. Ẩn viên đạn địch vừa đâm trúng ta
        				                    enemyBullets[b]->setVisible(false);
        				                    enemyBullets[b]->invalidate();

        				                    // 2. Giảm máu của ta đi 1/3
        				                    playerHealth--;

        				                    // 3. Cập nhật hiển thị thanh máu tương ứng dựa vào số máu còn lại
        				                    // Xóa vết tất cả các thanh máu cũ trước
        				                    FullHealthBar.invalidate(); FullHealthBar.setVisible(false);
        				                    HealthBar3.invalidate();     HealthBar3.setVisible(false);
        				                    HealthBar1.invalidate();     HealthBar1.setVisible(false);

        				                    if (playerHealth == 1)
        				                    {
        				                        // Còn 2/3 máu -> Hiện thanh HealthBar3
        				                        HealthBar3.setVisible(true);
        				                        HealthBar3.invalidate();
        				                    }
        				                    else if (playerHealth == 2)
        				                    {
        				                        // Còn 1/3 máu -> Hiện thanh HealthBar1
        				                        HealthBar1.setVisible(true);
        				                        HealthBar1.invalidate();
        				                    }
        				                    else if (playerHealth <= 0)
        				                    {
        				                        // Hết máu -> Xử lý THUA CUỘC (Game Over)
        				                        plane.setVisible(false); // Ẩn máy bay ta đi
        				                        plane.invalidate();
        				                        HealthBar1.setVisible(false);
        				                        HealthBar1.invalidate();// Gọi thẳng hàm chuyển sang Screen4 của Interaction2 từ lớp Base
        				                        Tital2.setVisible(true);
        				                        Tital2.invalidate();
        				                        lose.setVisible(true);
												lose.invalidate();
        				                        return;

        				                        // Bạn có thể thêm lệnh chuyển screen hoặc hiện chữ Game Over tại đây
        				                    }

        				                    continue; // Bỏ qua đoạn kiểm tra biên dưới vì đạn đã bị ẩn
        				                }





        				if (enemyBullets[b]->getY() > 320) // Nếu rơi quá màn hình thì ẩn đi
        				{
        					enemyBullets[b]->invalidate(); // Xóa vết cuối cùng trước khi ẩn
        					enemyBullets[b]->setVisible(false);
        				}
        				else
        				{
        					enemyBullets[b]->invalidate(); // Vẽ ở vị trí mới
        				}
            }
        }
        }

}
void Screen2View::pauseButtonClicked()
{
    isPaused = !isPaused; // Đảo trạng thái: Đang chơi (false) -> Tạm dừng (true) và ngược lại
    if(isPaused){
    	Tital2.setVisible(true);
		Tital2.invalidate();
		ReleasePause.setVisible(true);
		ReleasePause.invalidate();
    }
}
void Screen2View::PauseRelease()
{
    isPaused = false; // Đảo trạng thái: Đang chơi (false) -> Tạm dừng (true) và ngược lại
    Tital2.setVisible(false);
    Tital2.invalidate();
    ReleasePause.setVisible(false);
    ReleasePause.invalidate();
}
