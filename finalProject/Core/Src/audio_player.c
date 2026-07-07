#include "audio_player.h"
#include "dac.h"
#include "tim.h"
#include "laser_sound.h"
#include "explosion_sound.h"

void Audio_PlayLaser(void) {
    HAL_DAC_Stop_DMA(&hdac, DAC_CHANNEL_1);
    // Truyền mảng laser_data
    HAL_DAC_Start_DMA(&hdac, DAC_CHANNEL_1, (uint32_t*)laser_data, laser_length, DAC_ALIGN_8B_R);
    HAL_TIM_Base_Start(&htim7); 
}

void Audio_PlayExplosion(void) {
    HAL_DAC_Stop_DMA(&hdac, DAC_CHANNEL_1);
    // Truyền mảng explosion_data
    HAL_DAC_Start_DMA(&hdac, DAC_CHANNEL_1, (uint32_t*)explosion_data, explosion_length, DAC_ALIGN_8B_R);
    HAL_TIM_Base_Start(&htim7); 
}