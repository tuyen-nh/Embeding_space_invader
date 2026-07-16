#ifndef AUDIO_PLAYER_H
#define AUDIO_PLAYER_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

// Các hàm gọi phát âm thanh điều hướng từ giao diện game
void Audio_PlayLaser(void);
void Audio_PlayExplosion(void);

// Cấu hình BGM : loop (1: có, 0: không), volume_level (1: To, 2: Nhỏ hơn)
void Audio_PlayBGM_Config(uint8_t loop, uint8_t volume_level);
void Audio_StopBGM(void);
void Audio_PlayVictory(void);

void Audio_SetTempo(uint16_t period);

void Audio_Process_Timer_Interrupt(void);

#ifdef __cplusplus
}
#endif

#endif /* AUDIO_PLAYER_H */
