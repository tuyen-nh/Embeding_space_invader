#include <gui/model/Model.hpp>
#include <gui/model/ModelListener.hpp>

// Gọi thư viện C trong môi trường C++
extern "C" {
    #include "audio_player.h"
}

Model::Model() : modelListener(0) {}

void Model::tick() {}

void Model::playLaserSound() {
    Audio_PlayLaser(); // Kích hoạt tiếng Laser phần cứng
}

void Model::playExplosionSound() {
    Audio_PlayExplosion(); // Kích hoạt tiếng Nổ phần cứng
}
