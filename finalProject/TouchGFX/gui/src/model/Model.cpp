#include <gui/model/Model.hpp>
#include <gui/model/ModelListener.hpp>

extern "C" {
    #include "audio_player.h"
}

Model::Model() : modelListener(0) {}

void Model::tick() {}

void Model::playLaserSound() {
    Audio_PlayLaser();
}

void Model::playExplosionSound() {
    Audio_PlayExplosion();
}
