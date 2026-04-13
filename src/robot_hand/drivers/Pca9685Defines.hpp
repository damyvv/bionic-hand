#pragma once

#include <cstdint>

static constexpr uint8_t MODE1_REGISTER = 0x00;
static constexpr uint8_t MODE1_AUTO_INCREMENT_BIT = (1 << 5); // Auto-increment enabled in MODE1 register

static constexpr uint8_t PRE_SCALE_REGISTER = 0xFE;
static constexpr float INTERNAL_OSCILLATOR_FREQUENCY = 25000000.0; // 25 MHz

static constexpr uint8_t LED0_ON_L_REGISTER = 0x06;
static constexpr uint8_t LEDn_REGISTER_OFFSET = 4; // Each channel has 4 registers: ON_L, ON_H, OFF_L, OFF_H
static constexpr uint16_t MAX_PWM_RESOLUTION = 4096; // (0x1000) 12-bit resolution
