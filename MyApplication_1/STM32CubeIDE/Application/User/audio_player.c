#include "audio_player.h"
#include "main.h"
#include "laser_sound.h"
#include "explosion_sound.h"
#include "bgm_sound.h"
#include "victory_sound.h"

// KÊNH 1: NHẠC NỀN (BGM)
volatile uint32_t bgm_index = 0;
volatile const uint8_t* bgm_audio_data = bgm_data;
volatile uint32_t bgm_audio_length = bgm_length;
volatile uint8_t is_bgm_playing = 0;
volatile uint8_t bgm_should_loop = 1;
volatile uint8_t bgm_volume_shift = 2; // >> 2 là chia 4, >> 3 là chia 8

// KÊNH 2: HIỆU ỨNG (SFX) & VICTORY
volatile uint32_t sfx_index = 0;
volatile const uint8_t* sfx_audio_data = NULL;
volatile uint32_t sfx_audio_length = 0;
volatile uint8_t is_sfx_playing = 0;

extern TIM_HandleTypeDef htim3;
extern TIM_HandleTypeDef htim7;


void Audio_PlayBGM_Config(uint8_t loop, uint8_t volume_level) {
    HAL_TIM_Base_Stop_IT(&htim7);
    bgm_index = 0;
    bgm_should_loop = loop;

    if (volume_level == 1) {
        bgm_volume_shift = 2;
    } else {
        bgm_volume_shift = 3;
    }

    is_bgm_playing = 1;

    HAL_TIM_PWM_Start(&htim3, TIM_CHANNEL_1);
    __HAL_TIM_CLEAR_IT(&htim7, TIM_IT_UPDATE);
    HAL_TIM_Base_Start_IT(&htim7);
}

void Audio_StopBGM(void) {
    is_bgm_playing = 0;
}

void Audio_PlayLaser(void) {
    sfx_index = 0;
    sfx_audio_data = laser_data;
    sfx_audio_length = laser_length;
    is_sfx_playing = 1;
}

void Audio_PlayExplosion(void) {
    sfx_index = 0;
    sfx_audio_data = explosion_data;
    sfx_audio_length = explosion_length;
    is_sfx_playing = 1;
}

void Audio_PlayVictory(void) {
    Audio_StopBGM();
    sfx_index = 0;
    sfx_audio_data = victory_data;
    sfx_audio_length = victory_length;
    is_sfx_playing = 1;
}


volatile int16_t current_volume_output = 128; // Lưu điện áp xuất thực tế

void Audio_Process_Timer_Interrupt(void) {
    int16_t bgm_signed_sample = 0; // Trạng thái tĩnh dạng có dấu là 0
    int16_t sfx_signed_sample = 0;
    uint8_t is_any_channel_active = 0;

    // Xử lý Kênh 1 (BGM)
    if (is_bgm_playing && bgm_audio_data != NULL) {

        bgm_signed_sample = (int16_t)bgm_audio_data[bgm_index] - 128;

        bgm_signed_sample = bgm_signed_sample >> bgm_volume_shift;

        bgm_index++;
        is_any_channel_active = 1;
        if (bgm_index >= bgm_audio_length) {
            if (bgm_should_loop) {
                bgm_index = 0;
            } else {
                is_bgm_playing = 0;
            }
        }
    }

    // Xử lý Kênh 2 (SFX / Victory)
    if (is_sfx_playing && sfx_audio_data != NULL) {
        if (sfx_index < sfx_audio_length) {
            sfx_signed_sample = (int16_t)sfx_audio_data[sfx_index] - 128;

            sfx_signed_sample = sfx_signed_sample >> 2;

            sfx_index++;
            is_any_channel_active = 1;
        } else {
            is_sfx_playing = 0;
        }
    }

    //  XỬ LÝ ĐẦU RA
    if (is_any_channel_active) {
        // Trộn hai kênh có dấu
        int16_t mixed_signed = bgm_signed_sample + sfx_signed_sample;

        int16_t target_output = mixed_signed + 128;

        if (target_output > 255) target_output = 255;
        if (target_output < 0)   target_output = 0;

        current_volume_output = target_output;
        __HAL_TIM_SET_COMPARE(&htim3, TIM_CHANNEL_1, (uint8_t)current_volume_output);
    }
    else {

        if (current_volume_output > 128) {
            current_volume_output--;
            __HAL_TIM_SET_COMPARE(&htim3, TIM_CHANNEL_1, (uint8_t)current_volume_output);
        }
        else if (current_volume_output < 128) {
            current_volume_output++;
            __HAL_TIM_SET_COMPARE(&htim3, TIM_CHANNEL_1, (uint8_t)current_volume_output);
        }
        else {

            __HAL_TIM_SET_COMPARE(&htim3, TIM_CHANNEL_1, 128);
        }
    }
}

void Audio_SetTempo(uint16_t period) {
    if (period >= 50 && period <= 124) {
        __HAL_TIM_SET_AUTORELOAD(&htim7, period);
    }
}
