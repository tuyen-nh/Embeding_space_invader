#include <gui/screen2_screen/Screen2View.hpp>

Screen2View::Screen2View()
{

}

void Screen2View::setupScreen()
{
    Screen2ViewBase::setupScreen();
}

void Screen2View::tearDownScreen()
{
    Screen2ViewBase::tearDownScreen();
}

void Screen2View::pauseButtonClicked()
{
    isPaused = !isPaused; // Đảo trạng thái: Đang chơi (false) -> Tạm dừng (true) và ngược lại
    if(isPaused)
    {
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

void Screen2View::handleTickEvent()
{

  if (isPaused)
  {
      return;
  }

  // 4. DI CHUYỂN CỤM TRÁI TIM DỰA TRÊN BIÊN CỦA ENEMY CÒN SỐNG
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



  // ĐIỀU KHIỂN HOẠT ẢNH NỔ RẢI ĐỀU QUA CÁC FRAME
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

  }
}
