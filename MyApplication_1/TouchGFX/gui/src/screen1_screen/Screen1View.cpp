#include <gui/screen1_screen/Screen1View.hpp>
#include "audio_player.h"

Screen1View::Screen1View()
{

}

void Screen1View::setupScreen()
{
    Screen1ViewBase::setupScreen();
    // Phát nhạc nền BGM: loop = 0 (chỉ phát 1 lần), volume_level = 1 (mức to)
     Audio_PlayBGM_Config(0, 1);
}

void Screen1View::tearDownScreen()
{
    Screen1ViewBase::tearDownScreen();
}
