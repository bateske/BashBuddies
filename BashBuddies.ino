
#include <Arduboy2.h>
#include <Wire.h>

#include <ArduboyPlaytune.h>

Arduboy2 arduboy;

// Define player role (1 for Player 1, 2 for Player 2)
int playerRole = 1;  // Set to 2 on the second device

// I2C Addresses
const int PLAYER_1_ADDRESS = 8;
const int PLAYER_2_ADDRESS = 9;

// Command Types
const uint8_t CMD_UPDATE = 0;
const uint8_t CMD_ATTACK = 1;
const uint8_t CMD_PLATFORM_UPDATE = 2;  // New command type for platform updates

// Game parameters (same as before)
const int SCREEN_WIDTH = 128;
const int SCREEN_HEIGHT = 64;
const int PLAYER_WIDTH = 8;
const int PLAYER_HEIGHT = 8;
const int PLATFORM_COUNT = 4;
const int PLAYER_SPEED = 4;
const int JUMP_STRENGTH = -3;
const int ATTACK_DURATION = 10;
const int CHARGED_ATTACK_DURATION = 20;
const int BASE_KNOCKBACK = 2;
const int CHARGED_KNOCKBACK = 5;
const int MAX_HEALTH = 120;
const int CHARGE_TIME = 30;
const int ATTACK_DAMAGE = 8;
const int CHARGED_DAMAGE = 18;
const float GRAVITY = 0.3;
const float FRICTION = 0.5;  // Set friction to 0 for immediate stopping


constexpr uint8_t bashTitleWidth = 128;
constexpr uint8_t bashTitleHeight = 128;

constexpr uint8_t bashTitle[] PROGMEM{
  bashTitleWidth, bashTitleHeight,

  //Frame 0
  0xF8, 0xFC, 0xFE, 0xFE, 0xFE, 0xFE, 0xFE, 0xFC, 0xF8, 0xF0, 0xF0, 0x00, 0x00, 0x80, 0x80, 0x80, 0x80, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x80, 0x00, 0x80, 0x80, 0x00, 0x00, 0x00, 0xF0, 0xF8, 0xFC, 0xFC, 0xFE, 0xFE, 0xFE, 0xFE, 0xFC, 0xF8, 0xF8,
  0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFE, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFE, 0xEC, 0xE0, 0xE0, 0xF0, 0xC0, 0xC0, 0xE0, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x80, 0xF0, 0xFC, 0xFF, 0xFF, 0xFF, 0x87, 0x87, 0x87, 0xC7, 0xFF, 0xFF, 0xFF, 0xFE, 0x78, 0x00, 0x00, 0x80, 0xE0, 0xFC, 0xFE, 0xFF, 0x7F, 0x0F, 0x1F, 0x7F, 0xFF, 0xFC, 0xE0, 0xE0, 0x00, 0x00, 0xFC, 0xFE, 0xFE, 0xFF, 0xFF, 0xE7, 0x87, 0x87, 0x87, 0x9F, 0x3F, 0x3E, 0x3E, 0x1C, 0x00, 0x1F, 0xFF, 0xFF, 0xFF, 0xFF, 0xC0, 0x80, 0x80, 0x80, 0x8F, 0xBF, 0xFF, 0xFF, 0xFF, 0xFC, 0x80, 0x80, 0x00, 0x00, 0x00, 0x00, 0x80, 0x80, 0xC0, 0xC0, 0xC0, 0xF0, 0xE0, 0xEC, 0xEC, 0xFE, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFE, 0xFE, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
  0x01, 0x01, 0x41, 0x81, 0x01, 0x41, 0x81, 0x01, 0x01, 0x81, 0x01, 0x81, 0x41, 0x01, 0x01, 0x01, 0x00, 0x00, 0x41, 0x91, 0x01, 0x41, 0x90, 0x00, 0x00, 0x90, 0x00, 0x00, 0x80, 0xE0, 0xF8, 0xFE, 0xFF, 0xFF, 0x7F, 0x0F, 0x07, 0x07, 0x07, 0xC7, 0xFF, 0xFF, 0xFF, 0xFE, 0xFC, 0x38, 0x00, 0xF8, 0xFE, 0xFF, 0xFF, 0x7F, 0x1F, 0x1F, 0x1E, 0x1E, 0x1E, 0x1E, 0xFF, 0xFF, 0xFF, 0xFF, 0xF8, 0x00, 0xE0, 0xE1, 0xE1, 0xE3, 0xE7, 0xC7, 0x87, 0x87, 0x87, 0x8F, 0xFF, 0xFF, 0xFE, 0xFC, 0xF8, 0xF8, 0x03, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x87, 0x07, 0x07, 0x07, 0x07, 0x1F, 0x7F, 0xFF, 0xFF, 0xFF, 0xFE, 0xE0, 0x80, 0x00, 0x00, 0x00, 0x90, 0x00, 0x01, 0x11, 0x81, 0x01, 0x01, 0x00, 0x80, 0x40, 0x01, 0x91, 0x01, 0x41, 0x91, 0x01, 0x01, 0x91, 0x01, 0x01, 0x91, 0x01, 0x01, 0x91, 0x41, 0x01,
  0x00, 0x04, 0x32, 0x84, 0x00, 0x22, 0x90, 0x00, 0x20, 0x90, 0x00, 0x24, 0x12, 0x92, 0x24, 0x00, 0x90, 0x20, 0x12, 0x84, 0x00, 0x12, 0xC4, 0xC0, 0xC0, 0xC0, 0xCC, 0xC8, 0xCF, 0xCF, 0xCF, 0xCF, 0x8F, 0x0F, 0x0F, 0x0F, 0xCF, 0xCF, 0xCF, 0xCF, 0x0F, 0x07, 0x01, 0x01, 0xC0, 0xC0, 0xCF, 0xCF, 0xCF, 0x0F, 0x8F, 0xC0, 0xC0, 0xC0, 0xC0, 0xC0, 0xC0, 0xC0, 0xCF, 0x8F, 0x8F, 0x0F, 0x0F, 0x00, 0xC1, 0xC1, 0xC7, 0xCF, 0xCF, 0xCF, 0xCF, 0xCF, 0xCF, 0x8F, 0x0F, 0x0F, 0x0F, 0x03, 0xC1, 0xC0, 0xC0, 0x80, 0x0F, 0xCF, 0xCF, 0xCF, 0xCF, 0xC0, 0xC0, 0xC0, 0xC0, 0xC0, 0xC0, 0x01, 0x0F, 0x8F, 0xCF, 0xCF, 0xCF, 0xCC, 0xC0, 0xC0, 0xC4, 0xC0, 0xC0, 0xC4, 0x10, 0x04, 0x02, 0x90, 0x24, 0x02, 0x90, 0x24, 0x02, 0x92, 0x24, 0x00, 0x92, 0x24, 0x00, 0x92, 0x24, 0x00, 0x00, 0x24, 0x90, 0x00,
  0xC0, 0xC0, 0xC9, 0x04, 0x00, 0x49, 0x00, 0x00, 0x09, 0x40, 0x80, 0x49, 0x40, 0x82, 0x01, 0x00, 0x12, 0xC1, 0xF0, 0xF0, 0xFF, 0xFF, 0xFF, 0xFF, 0xE7, 0xE3, 0xE3, 0xE3, 0xFF, 0xFF, 0x7F, 0x3F, 0x1F, 0x0F, 0xE0, 0xFE, 0xFF, 0xFF, 0xFF, 0x0F, 0x00, 0x00, 0x00, 0xE0, 0xFF, 0xFF, 0xFF, 0xFF, 0x03, 0x80, 0xFF, 0xFF, 0xFF, 0xFF, 0x03, 0x03, 0x03, 0x07, 0xFF, 0xFF, 0xFF, 0xFF, 0xFE, 0x00, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x03, 0x03, 0x07, 0x07, 0xFF, 0xFF, 0xFE, 0xFE, 0x00, 0x3F, 0xFF, 0xFF, 0xFF, 0xE0, 0x07, 0xFF, 0xFF, 0xFF, 0xFF, 0xE3, 0xE3, 0xE3, 0xE3, 0xE3, 0x80, 0x00, 0x0F, 0x3F, 0x7F, 0x7F, 0xFF, 0xF3, 0xF3, 0xE3, 0xE7, 0xE7, 0xE7, 0xC7, 0xC7, 0x80, 0x80, 0x09, 0x00, 0x96, 0x49, 0x00, 0x92, 0x49, 0x00, 0x94, 0x49, 0x00, 0x00, 0x49, 0x00, 0x00, 0xC9, 0x94, 0xC0,
  0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFA, 0xF1, 0xE0, 0x80, 0x10, 0x08, 0x20, 0x12, 0x08, 0xE0, 0xF0, 0xFE, 0xFF, 0xFF, 0xFF, 0xC7, 0xC3, 0xC3, 0xC3, 0xE3, 0xE3, 0xFF, 0xFF, 0xFF, 0x7F, 0x3F, 0x00, 0xF0, 0xFF, 0xFF, 0xFF, 0xFF, 0xDF, 0xC0, 0xC0, 0xC0, 0xF0, 0xFE, 0xFF, 0xFF, 0xFF, 0x7F, 0x03, 0x80, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xC0, 0xC0, 0xC0, 0xF0, 0xFF, 0xFF, 0xFF, 0xFF, 0x7F, 0x00, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xC0, 0xC0, 0xC0, 0xE0, 0xFF, 0xFF, 0xFF, 0xFF, 0x7C, 0x00, 0x3F, 0xFF, 0xFF, 0xFF, 0xF8, 0x00, 0x1F, 0xFF, 0xFF, 0xFF, 0xFF, 0xC3, 0xC3, 0xC3, 0xC3, 0xC0, 0xC0, 0xC0, 0xC0, 0x10, 0xF0, 0xF0, 0xF3, 0xF3, 0xE3, 0xC3, 0xC3, 0xC7, 0xC7, 0xFF, 0xFF, 0xFF, 0xFE, 0xF8, 0xF2, 0x00, 0x24, 0x92, 0x00, 0x00, 0x92, 0xFC, 0xFC, 0xFE, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
  0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFE, 0x00, 0x00, 0x56, 0x87, 0x07, 0x47, 0x87, 0x07, 0x07, 0x87, 0x07, 0x07, 0x07, 0x07, 0x07, 0x03, 0x03, 0x01, 0x00, 0x40, 0x00, 0x00, 0x03, 0x07, 0x07, 0x07, 0x07, 0x07, 0x07, 0x07, 0x07, 0x07, 0x07, 0x03, 0x01, 0x01, 0x00, 0x00, 0x07, 0x07, 0x07, 0x07, 0x07, 0x07, 0x07, 0x07, 0x07, 0x07, 0x07, 0x03, 0x03, 0x01, 0x00, 0x00, 0x07, 0x07, 0x07, 0x07, 0x07, 0x07, 0x07, 0x07, 0x07, 0x07, 0x07, 0x03, 0x01, 0x00, 0x00, 0x00, 0x07, 0x07, 0x07, 0x07, 0x07, 0x00, 0x07, 0x07, 0x07, 0x07, 0x07, 0x07, 0x07, 0x07, 0x07, 0x07, 0x07, 0x07, 0x04, 0x00, 0x00, 0x03, 0x03, 0x07, 0x07, 0x07, 0x07, 0x07, 0x07, 0x07, 0x07, 0x07, 0x83, 0x50, 0x08, 0x98, 0x70, 0xFE, 0xFE, 0xFF, 0xFF, 0xFF, 0xFF, 0xFD, 0xFF, 0xFF, 0xFF, 0xFF,
  0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x3F, 0x3F, 0x17, 0x6F, 0xA4, 0x80, 0xD2, 0xC4, 0xD0, 0xD2, 0xE4, 0xDA, 0xC0, 0x24, 0xD2, 0x00, 0x65, 0x92, 0x00, 0x4D, 0xB2, 0x00, 0x4D, 0x32, 0x00, 0x49, 0xB2, 0x20, 0x08, 0x40, 0x20, 0x00, 0x48, 0x20, 0x00, 0x48, 0xA0, 0x00, 0x48, 0xF0, 0x00, 0x48, 0x40, 0x00, 0x48, 0x00, 0x20, 0x40, 0x00, 0xB0, 0x48, 0x00, 0xB0, 0x48, 0x00, 0xA0, 0x48, 0x00, 0xB0, 0x48, 0x00, 0xB0, 0x48, 0x00, 0x40, 0x20, 0x00, 0x48, 0x20, 0x00, 0x48, 0xA0, 0x00, 0x48, 0xA0, 0x00, 0x48, 0x20, 0x00, 0x48, 0x00, 0x20, 0x48, 0x00, 0x20, 0x48, 0x00, 0x20, 0x48, 0x00, 0x22, 0x49, 0x00, 0x22, 0x4D, 0x00, 0x32, 0x45, 0x00, 0xD2, 0xE4, 0x80, 0xA2, 0xC4, 0xDA, 0xC0, 0xC4, 0xDA, 0xC0, 0xE4, 0x92, 0x63, 0x67, 0x1F, 0x7F, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
  0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFE, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFE, 0xFE, 0xFD, 0xF9, 0xF0, 0xF2, 0xED, 0x00, 0xD2, 0x6D, 0x01, 0xD2, 0x7D, 0x6D, 0xD2, 0xD2, 0xE9, 0xC0, 0xD2, 0xED, 0xC0, 0xD2, 0x6D, 0x00, 0xDB, 0xF6, 0x40, 0xD9, 0xB2, 0xE8, 0xDB, 0xB4, 0xED, 0xD2, 0xA9, 0xED, 0xD2, 0xA0, 0xAD, 0xD2, 0xA0, 0xA5, 0xDA, 0x00, 0x2D, 0xD2, 0x80, 0xAD, 0xDA, 0x80, 0xEE, 0xDB, 0x80, 0xB2, 0xC9, 0x80, 0xD2, 0x6D, 0xE8, 0x92, 0xCD, 0xC8, 0xD2, 0xD7, 0xD0, 0xFA, 0xC8, 0xC9, 0xF2, 0xE9, 0xE9, 0xD2, 0xC0, 0xED, 0x92, 0x80, 0x4D, 0xB2, 0x00, 0x49, 0xB2, 0xB0, 0xED, 0xFA, 0xFC, 0xFD, 0xFE, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFE, 0xFE, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
  0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFD, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x7F, 0x1F, 0x0F, 0x0F, 0x07, 0x03, 0x11, 0x11, 0x79, 0x79, 0x79, 0xF9, 0xFF, 0xFF, 0xFF, 0xFE, 0xFE, 0xFD, 0xF6, 0xFF, 0xED, 0xBE, 0xFF, 0xFD, 0xBF, 0xFF, 0xBD, 0xFF, 0xFE, 0xA3, 0xC1, 0x3C, 0xFE, 0xFE, 0xFC, 0xFC, 0x7D, 0x3C, 0x3D, 0x1D, 0x42, 0xBF, 0xFD, 0xFE, 0xB6, 0xFD, 0xFF, 0xF6, 0xFF, 0xFF, 0xFE, 0xFF, 0xFF, 0xFF, 0xFF, 0x0F, 0xFF, 0x87, 0x03, 0x03, 0x03, 0xFF, 0xFF, 0x07, 0x07, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x7E, 0x7E, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
  0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xDF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x0F, 0x03, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xFC, 0xFC, 0x00, 0x00, 0x00, 0xFD, 0x07, 0x07, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x7D, 0x9F, 0xE7, 0x7B, 0x7C, 0x9F, 0xEE, 0xE7, 0xFB, 0xF9, 0xF1, 0xC8, 0x98, 0x90, 0x00, 0x00, 0x87, 0x7F, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFE, 0xFC, 0x0C, 0xF8, 0xF8, 0xFC, 0x0C, 0x0D, 0xFC, 0xFC, 0xFC, 0xF8, 0xC0, 0x80, 0xE0, 0xF0, 0xF0, 0xF0, 0x00, 0x03, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xDF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
  0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x1F, 0x0F, 0x0F, 0x07, 0xC1, 0xC1, 0xC1, 0xC1, 0xC1, 0x01, 0x01, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x03, 0x03, 0x00, 0x00, 0x60, 0x63, 0xF0, 0xFC, 0xFF, 0xFF, 0x87, 0x07, 0x01, 0x00, 0x1E, 0x1E, 0x0E, 0x6E, 0x60, 0x6B, 0x6B, 0x79, 0x0E, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x3E, 0x08, 0x68, 0x68, 0x61, 0x6E, 0x0E, 0x0F, 0x1F, 0x1E, 0xC0, 0xC1, 0x07, 0x07, 0xFF, 0xFF, 0xF8, 0xF7, 0xEF, 0xEF, 0x9E, 0xDE, 0xDF, 0x1F, 0x9F, 0x8F, 0x8F, 0x83, 0xC1, 0xC1, 0x71, 0xF8, 0xFC, 0x39, 0x39, 0xB9, 0x39, 0xDD, 0xDD, 0xFD, 0xFD, 0x87, 0x07, 0x0F, 0x1F, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
  0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xC0, 0x00, 0x00, 0x00, 0x03, 0x03, 0x07, 0x3F, 0x3F, 0x87, 0x9E, 0x1E, 0x20, 0x38, 0x38, 0x38, 0x38, 0x38, 0x38, 0x38, 0x38, 0x38, 0x38, 0xF8, 0xF8, 0xE0, 0xC2, 0xC3, 0x83, 0x80, 0x80, 0xC0, 0xC0, 0xC0, 0xC0, 0xC0, 0xC0, 0xCF, 0xCF, 0xCF, 0xCF, 0xE7, 0xE7, 0xF8, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xF8, 0xF8, 0xE7, 0xEF, 0xCF, 0xCF, 0xCF, 0xC7, 0xC0, 0x30, 0x37, 0x87, 0x8C, 0x8C, 0x8E, 0x88, 0x8F, 0xC7, 0xC3, 0xE3, 0xF8, 0xF7, 0x67, 0x67, 0x66, 0x66, 0x66, 0x67, 0x67, 0x67, 0x66, 0x60, 0x60, 0x00, 0x9E, 0x07, 0xFF, 0x7F, 0x7F, 0x0F, 0x03, 0x01, 0x00, 0x00, 0xC0, 0xC1, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
  0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFE, 0x7E, 0x7E, 0x1E, 0x0F, 0x0F, 0x03, 0x01, 0x00, 0x80, 0xE0, 0xF0, 0xF0, 0xF8, 0xF8, 0xF8, 0xF8, 0xF8, 0xE0, 0x60, 0x00, 0x01, 0x01, 0x01, 0x07, 0x07, 0x7F, 0x7F, 0x7F, 0xFF, 0xFF, 0xF3, 0xF3, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x7F, 0xFF, 0xFF, 0x83, 0xFF, 0xFF, 0x87, 0x03, 0x01, 0x60, 0x60, 0xF8, 0xF8, 0xF8, 0xF8, 0xF8, 0xF8, 0xFC, 0xEC, 0xB8, 0x78, 0xE1, 0xE1, 0x83, 0x0F, 0x0E, 0x1E, 0x7E, 0xFE, 0xFE, 0xFF, 0xFF, 0xE7, 0xE7, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x7F, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
  0x07, 0x3F, 0x07, 0x1F, 0x5F, 0x47, 0x47, 0x07, 0x87, 0x07, 0x3F, 0x0F, 0x5F, 0x47, 0x47, 0x27, 0x24, 0x9C, 0x9C, 0x1C, 0x5C, 0x44, 0x24, 0x24, 0x3D, 0x0D, 0xFF, 0x1F, 0x07, 0x07, 0x07, 0x87, 0xBF, 0x07, 0x17, 0x5C, 0x5C, 0x44, 0x24, 0x24, 0x1C, 0x9C, 0x04, 0x04, 0x04, 0x04, 0x05, 0x25, 0x1F, 0x1F, 0x07, 0x07, 0x07, 0x07, 0x07, 0x57, 0x07, 0x1F, 0x1F, 0x07, 0x07, 0x07, 0x1F, 0x1F, 0x9F, 0xDF, 0x1F, 0x07, 0x07, 0x07, 0x07, 0x1F, 0x07, 0x07, 0x07, 0x07, 0x87, 0x1F, 0x07, 0x1F, 0x1C, 0x05, 0x25, 0x25, 0x25, 0x85, 0x84, 0x1C, 0x1C, 0x54, 0x44, 0x24, 0x24, 0x2F, 0x9F, 0x9F, 0x1F, 0x07, 0x07, 0x87, 0xFF, 0x05, 0xBD, 0x3D, 0x1D, 0x45, 0x44, 0x04, 0x04, 0x0C, 0x1C, 0x1C, 0x5F, 0x47, 0x27, 0x27, 0x27, 0x0F, 0x9F, 0x1F, 0x07, 0x07, 0x07, 0x07, 0x07, 0x3F, 0x47, 0x5F,
  0x00, 0x00, 0x00, 0x00, 0x04, 0x04, 0x04, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x01, 0x00, 0x00, 0x00, 0x02, 0x02, 0x00, 0x00, 0x00, 0x01, 0x13, 0x02, 0x0E, 0x01, 0x01, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x02, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0A, 0x01, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x01, 0x01, 0x00, 0x00, 0x00, 0x00, 0x02, 0x00, 0x01, 0x01, 0x02, 0x0E, 0x12, 0x11, 0x01, 0x01, 0x01, 0x02, 0x02, 0x02, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x02, 0x00, 0x01, 0x01, 0x04, 0x04, 0x04, 0x02, 0x00, 0x00, 0x00, 0x00
};


const byte score[] PROGMEM = {
  0x90, 64, 0, 115, 0x90, 52, 0, 115, 0x90, 50, 0, 116, 0x90, 52, 0, 115, 0x90, 64, 0, 115, 0x90, 52, 0, 116, 0x90, 50,
  0, 115, 0x90, 52, 0, 116, 0x90, 64, 0, 115, 0x90, 52, 0, 115, 0x90, 50, 0, 116, 0x90, 52, 0, 115, 0x90, 64, 0, 115,
  0x90, 52, 0, 116, 0x90, 50, 0, 115, 0x90, 52, 0, 116, 0x90, 64, 0x91, 76, 0, 115, 0x90, 52, 0x81, 0, 115, 0x90, 50, 0, 116,
  0x90, 52, 0, 115, 0x90, 64, 0x91, 79, 0, 116, 0x90, 52, 0x91, 76, 0, 115, 0x90, 50, 0x91, 74, 0, 115, 0x90, 52, 0x91, 71,
  0, 116, 0x90, 64, 0x91, 76, 0, 115, 0x90, 52, 0x91, 74, 0, 115, 0x90, 50, 0x91, 71, 0, 116, 0x90, 52, 0x91, 67, 0, 115,
  0x90, 64, 0x81, 0, 116, 0x90, 52, 0x91, 67, 0, 115, 0x90, 50, 0, 115, 0x90, 52, 0x91, 69, 0, 116, 0x90, 64, 0x91, 76, 0, 115,
  0x90, 52, 0x81, 0, 116, 0x90, 50, 0, 115, 0x90, 52, 0, 115, 0x90, 64, 0x91, 79, 0, 116, 0x90, 52, 0x91, 76, 0, 115, 0x90, 50,
  0x91, 74, 0, 115, 0x90, 52, 0x91, 71, 0, 116, 0x90, 64, 0x91, 76, 0, 115, 0x90, 52, 0x91, 74, 0, 116, 0x90, 50, 0x91, 71,
  0, 115, 0x90, 52, 0x91, 67, 0, 115, 0x90, 64, 0x81, 0, 116, 0x90, 52, 0x91, 67, 0, 115, 0x90, 50, 0, 116, 0x90, 52, 0x91, 69,
  0, 115, 0x90, 64, 0x91, 76, 0, 115, 0x90, 52, 0x81, 0, 116, 0x90, 50, 0, 115, 0x90, 52, 0, 115, 0x90, 67, 0x91, 79, 0, 116,
  0x90, 64, 0x91, 76, 0, 115, 0x90, 62, 0x91, 74, 0, 116, 0x90, 59, 0x91, 71, 0, 115, 0x90, 64, 0x91, 76, 0, 115, 0x90, 62,
  0x91, 74, 0, 116, 0x90, 59, 0x91, 71, 0, 115, 0x90, 55, 0x91, 67, 0, 116, 0x80, 0x81, 0, 115, 0x90, 55, 0x91, 67, 0, 115,
  0x80, 0, 116, 0x90, 57, 0x91, 69, 0, 115, 0x90, 64, 0x91, 76, 0, 115, 0x90, 52, 0x81, 0, 116, 0x90, 50, 0, 115, 0x90, 52,
  0, 116, 0x90, 67, 0x91, 79, 0, 115, 0x90, 64, 0x91, 76, 0, 115, 0x90, 62, 0x91, 74, 0, 116, 0x90, 59, 0x91, 71, 0, 115,
  0x90, 64, 0x91, 76, 0, 116, 0x90, 62, 0x91, 74, 0, 115, 0x90, 59, 0x91, 71, 0, 115, 0x90, 55, 0x91, 67, 0, 116, 0x80, 0x81,
  0, 115, 0x90, 55, 0x91, 67, 0, 115, 0x80, 0, 116, 0x90, 57, 0x91, 69, 0, 115, 0x90, 55, 0x91, 64, 0, 116, 0x90, 52, 0x91, 64,
  0, 115, 0x90, 50, 0x91, 62, 0, 115, 0x90, 52, 0x91, 64, 0, 116, 0x90, 55, 0x91, 67, 0, 115, 0x90, 52, 0x91, 64, 0, 116,
  0x90, 50, 0x91, 62, 0, 115, 0x90, 52, 0x91, 64, 0, 115, 0x90, 55, 0x91, 69, 0, 116, 0x90, 52, 0, 115, 0x90, 50, 0x91, 71,
  0, 115, 0x90, 52, 0x91, 67, 0, 116, 0x90, 55, 0, 115, 0x90, 52, 0x91, 64, 0, 116, 0x90, 50, 0x91, 62, 0, 115, 0x90, 52,
  0x91, 59, 0, 115, 0x90, 55, 0x91, 64, 0, 116, 0x90, 52, 0x91, 64, 0, 115, 0x90, 50, 0x91, 62, 0, 116, 0x90, 52, 0x91, 64,
  0, 115, 0x90, 55, 0x91, 67, 0, 115, 0x90, 52, 0x91, 64, 0, 116, 0x90, 50, 0x91, 62, 0, 115, 0x90, 52, 0x91, 64, 0, 115,
  0x90, 55, 0x91, 69, 0, 116, 0x90, 52, 0x91, 67, 0, 115, 0x90, 50, 0x91, 64, 0, 116, 0x90, 52, 0x91, 59, 0, 115, 0x90, 55,
  0x91, 62, 0, 115, 0x90, 52, 0, 116, 0x90, 50, 0x91, 64, 0, 115, 0x90, 52, 0, 116, 0x90, 55, 0x91, 76, 0, 115, 0x90, 52,
  0x91, 76, 0, 115, 0x90, 50, 0x91, 74, 0, 116, 0x90, 52, 0x91, 76, 0, 115, 0x90, 55, 0x91, 79, 0, 115, 0x90, 52, 0x91, 76,
  0, 116, 0x90, 50, 0x91, 74, 0, 115, 0x90, 52, 0x91, 76, 0, 116, 0x90, 55, 0x91, 81, 0, 115, 0x90, 52, 0, 115, 0x90, 50,
  0x91, 83, 0, 116, 0x90, 52, 0x91, 79, 0, 115, 0x90, 55, 0, 116, 0x90, 52, 0x91, 76, 0, 115, 0x90, 50, 0x91, 74, 0, 115,
  0x90, 52, 0x91, 71, 0, 116, 0x90, 55, 0x91, 76, 0, 115, 0x90, 52, 0x91, 76, 0, 115, 0x90, 50, 0x91, 74, 0, 116, 0x90, 52,
  0x91, 76, 0, 115, 0x90, 55, 0x91, 79, 0, 116, 0x90, 52, 0x91, 76, 0, 115, 0x90, 50, 0x91, 74, 0, 115, 0x90, 52, 0x91, 76,
  0, 116, 0x90, 55, 0x91, 81, 0, 115, 0x90, 52, 0x91, 79, 0, 115, 0x90, 50, 0x91, 76, 0, 116, 0x90, 52, 0x91, 71, 0, 115,
  0x90, 55, 0x91, 74, 0, 116, 0x90, 52, 0, 115, 0x90, 50, 0x91, 76, 0, 115, 0x90, 52, 0, 116, 0x90, 81, 0x81, 0, 115, 0x90, 76,
  0, 116, 0x91, 57, 0x90, 69, 0, 115, 0x90, 79, 0x81, 0, 115, 0x90, 74, 0, 116, 0x91, 55, 0x90, 67, 0, 115, 0x90, 81, 0x81,
  0, 115, 0x91, 57, 0x90, 74, 0, 116, 0x91, 52, 0x90, 76, 0, 115, 0x91, 52, 0x90, 71, 0, 116, 0x91, 50, 0x90, 78, 0, 115,
  0x91, 52, 0x90, 71, 0, 115, 0x91, 55, 0x90, 79, 0, 116, 0x91, 52, 0x90, 80, 0, 115, 0x91, 50, 0x90, 83, 0, 116, 0x91, 52,
  0x90, 85, 0, 115, 0x90, 81, 0x81, 0, 115, 0x90, 76, 0, 116, 0x91, 57, 0x90, 69, 0, 115, 0x90, 79, 0x81, 0, 115, 0x90, 74,
  0, 116, 0x91, 55, 0x90, 67, 0, 115, 0x90, 81, 0x81, 0, 116, 0x91, 57, 0x90, 74, 0, 115, 0x91, 52, 0x90, 76, 0, 115, 0x91, 52,
  0x90, 74, 0, 116, 0x91, 50, 0x90, 71, 0, 115, 0x91, 52, 0x90, 70, 0, 116, 0x91, 55, 0x90, 69, 0, 115, 0x91, 52, 0x90, 68,
  0, 115, 0x91, 50, 0x90, 64, 0, 116, 0x91, 52, 0, 115, 0x90, 81, 0x81, 0, 115, 0x90, 76, 0, 116, 0x91, 57, 0x90, 69, 0, 115,
  0x90, 79, 0x81, 0, 116, 0x90, 74, 0, 115, 0x91, 55, 0x90, 67, 0, 115, 0x90, 81, 0x81, 0, 116, 0x91, 57, 0x90, 74, 0, 115,
  0x91, 52, 0x90, 76, 0, 116, 0x91, 52, 0x90, 71, 0, 115, 0x91, 50, 0x90, 78, 0, 115, 0x91, 52, 0x90, 71, 0, 116, 0x91, 55,
  0x90, 79, 0, 115, 0x91, 52, 0x90, 80, 0, 115, 0x91, 50, 0x90, 83, 0, 116, 0x91, 52, 0x90, 85, 0, 115, 0x90, 74, 0x81, 0, 116,
  0x91, 50, 0x90, 74, 0, 115, 0x91, 49, 0x90, 74, 0, 115, 0x91, 50, 0x90, 74, 0, 116, 0x90, 76, 0x81, 0, 115, 0x91, 52, 0x90, 76,
  0, 116, 0x91, 50, 0x90, 76, 0, 115, 0x91, 52, 0x90, 76, 0, 115, 0x90, 79, 0x81, 0, 116, 0x91, 55, 0x90, 79, 0, 115, 0x91, 54,
  0x90, 79, 0, 115, 0x91, 55, 0x90, 79, 0, 116, 0x90, 81, 0x81, 0, 115, 0x90, 79, 0, 116, 0x90, 76, 0, 115, 0x90, 74, 0, 115,
  0x90, 76, 0, 231, 0x80, 0xF0
};


const unsigned char PROGMEM jump[] = {
  //
  0x90, 62, 0, 10, 0x90, 64, 0, 10, 0x90, 65, 0, 11, 0x90, 67, 0, 10, 0x90, 71, 0, 11, 0x90, 74, 0, 10, 0x80, 0xF0
};


const unsigned char PROGMEM miss[] = {
  //
  0x90, 94, 0, 15, 0x90, 92, 0, 16, 0x80, 0, 15, 0x90, 90, 0, 16, 0x80, 0xF0
};


const unsigned char PROGMEM hit[] = {
  //
  0x90, 48, 0, 31, 0x80, 0, 31, 0x90, 48, 0, 31, 0x80, 0, 32, 0x90, 44, 0, 62, 0x90, 41, 0, 63, 0x80, 0xF0
};


const unsigned char PROGMEM end[] = {
  0x90, 66, 0, 3, 0x91, 50, 0, 211, 0x90, 67, 0, 135, 0x90, 66, 0, 75, 0x80, 0, 4, 0x90, 67, 0, 107, 0x90, 66, 0, 100,
  0x92, 62, 0, 7, 0x80, 0, 215, 0x90, 64, 0x91, 48, 0x82, 0, 239, 0x80, 0, 3, 0x90, 62, 0, 175, 0x92, 60, 0, 4, 0x80,
  0, 221, 0x82, 0, 29, 0x90, 59, 0, 136, 0x81, 0, 3, 0x80, 0, 43, 0x90, 47, 0, 4, 0x91, 62, 0, 196, 0x81, 0, 14,
  0x91, 59, 0, 193, 0x81, 0, 25, 0x90, 60, 0x91, 45, 0, 190, 0x80, 0, 21, 0x90, 57, 0, 200, 0x80, 0, 14, 0x90, 59, 0, 4,
  0x91, 43, 0, 207, 0x80, 0, 4, 0x90, 55, 0, 132, 0x81, 0, 68, 0x80, 0, 14, 0x90, 57, 0x91, 42, 0, 218, 0x80, 0x81, 0, 28,
  0x90, 28, 0x91, 64, 1, 73, 0x81, 0, 68, 0x91, 64, 0, 57, 0x80, 0, 46, 0x81, 0, 22, 0x90, 28, 0, 60, 0x80, 0, 36,
  0x90, 66, 0, 4, 0x91, 30, 1, 22, 0x80, 0, 47, 0x81, 0, 3, 0x90, 28, 0, 57, 0x80, 0, 33, 0x90, 67, 0, 14, 0x91, 31,
  0, 225, 0x81, 0, 71, 0x91, 31, 0, 68, 0x80, 0, 18, 0x81, 0, 21, 0x90, 33, 0, 18, 0x91, 66, 0, 86, 0x80, 0, 11,
  0x90, 36, 0, 85, 0x80, 0, 15, 0x90, 38, 0, 110, 0x92, 40, 0, 8, 0x80, 0, 42, 0x81, 0, 36, 0x82, 0, 22, 0x90, 28,
  0, 14, 0x91, 67, 1, 12, 0x80, 0, 60, 0x81, 0, 4, 0x90, 28, 0, 68, 0x80, 0, 21, 0x90, 66, 0, 15, 0x91, 30, 0, 82,
  0x80, 0, 125, 0x90, 64, 0, 60, 0x81, 0, 47, 0x91, 28, 0, 71, 0x81, 0, 36, 0x91, 31, 1, 26, 0x81, 0, 39, 0x91, 31,
  0, 79, 0x81, 0, 21, 0x91, 33, 0, 100, 0x80, 0, 15, 0x90, 35, 0, 7, 0x81, 0, 78, 0x80, 0, 25, 0x90, 38, 0x91, 64,
  0, 104, 0x92, 40, 0, 32, 0x80, 0, 54, 0x82, 0, 28, 0x90, 28, 0, 122, 0x81, 0, 85, 0x91, 64, 0, 33, 0x80, 0, 82,
  0x90, 28, 0x81, 0, 68, 0x80, 0, 32, 0x90, 30, 0, 3, 0x91, 66, 1, 23, 0x80, 0, 39, 0x90, 28, 0, 11, 0x81, 0, 50,
  0x80, 0, 50, 0x90, 31, 0x91, 67, 1, 8, 0x80, 0, 43, 0x90, 31, 0, 25, 0x81, 0, 71, 0x80, 0, 15, 0x90, 33, 0, 7,
  0x91, 66, 0, 78, 0x80, 0, 22, 0x90, 35, 0, 11, 0x81, 0, 82, 0x80, 0, 21, 0x90, 38, 0x91, 64, 0, 107, 0x92, 40, 0, 32,
  0x80, 0, 61, 0x82, 0, 14, 0x90, 28, 0, 233, 0x80, 0, 89, 0x90, 28, 0, 71, 0x80, 0, 29, 0x90, 30, 1, 37, 0x80, 0, 28,
  0x90, 28, 0, 57, 0x80, 0, 50, 0x90, 31, 1, 177, 0x80, 0, 153, 0x81, 23, 80, 0xF0
};


ArduboyPlaytune tunes(arduboy.audio.enabled);


// Particle system parameters
const int MAX_PARTICLES = 10;
struct Particle {
  float x, y;
  float velX, velY;
  int size;
  bool active;
  int shrinkCounter;  // Counter to control how long each size lasts
};
Particle particles[MAX_PARTICLES];  // Array of particles


// Structure definitions
struct Platform {
  int x, y, width;
  bool moving;
  int direction;
  int deltaX;  // Added to track movement delta
};

struct Player {
  int16_t x, y;
  float velX, velY;
  int health;
  bool jumping;
  bool attacking;
  int attackTimer;
  bool chargedAttack;
  int chargeTime;
  bool idle;
  int attackDisplay;
};

// Platforms in the game
Platform platforms[PLATFORM_COUNT] = {
  { 20, 40, 40, false, 0 },
  { 60, 20, 30, false, 0 },
  { 90, 50, 30, false, 0 },
  { 40, 30, 30, true, 1 }
};

// Players
Player player1;
Player player2;

bool platformsInitialized = false;  // To check if platforms have been initialized

void setup() {
  // Arduboy setup
  arduboy.beginDoFirst();
  Serial.begin(9600);

  pinMode(5, OUTPUT);
  pinMode(13, OUTPUT);
  digitalWrite(13, LOW);

  tunes.initChannel(PIN_SPEAKER_1);
  tunes.initChannel(PIN_SPEAKER_2);
  if (!tunes.playing())
    tunes.playScore(score);

  // I2C initialization
  power_twi_enable();
  TWCR = _BV(TWEN) | _BV(TWIE) | _BV(TWEA);  // Enable I2C
  TWSR = 0;                                  // Clear prescaler bits
  TWBR = (F_CPU / 100000 - 16) / 2;          // Set I2C clock rate to 100kHz

  arduboy.clear();
  arduboy.setFrameRate(30);  // Ensure the game runs at a consistent 30 FPS

  while (arduboy.buttonsState()) {
    arduboy.pollButtons();  // Poll the buttons but do nothing
  }




  int titleY = 64;

  do {

    arduboy.pollButtons();
    arduboy.clear();
    if (titleY > -128) {
      titleY--;
    } else {
      arduboy.setCursor(32, 24);
      arduboy.setTextSize(2);
      arduboy.print("FIGHT!");
      arduboy.setTextSize(1);
    }



    arduboy.drawBitmap(0, titleY, bashTitle, 128, 128, WHITE);
    arduboy.display();
    delay(120);


    if (arduboy.justPressed(A_BUTTON)) {

      break;
    }
    if (arduboy.justPressed(B_BUTTON)) {

      break;
    }

  } while (true);




  arduboy.clear();
  arduboy.println("  BASH BUDDIES");
  arduboy.println("  Select Player:");
  arduboy.println("");
  arduboy.println("   [A] Player 1");
  arduboy.println("   [B] Player 2");
  arduboy.display();

  do {
    arduboy.pollButtons();
    if (arduboy.justPressed(A_BUTTON)) {
      playerRole = 1;
      break;
    }
    if (arduboy.justPressed(B_BUTTON)) {
      playerRole = 2;
      break;
    }

  } while (true);

  tunes.stopScore();

  // Initialize I2C based on player role
  if (playerRole == 1) {
    // Player 1 is the master
    Wire.begin(PLAYER_1_ADDRESS);  // Master
    Wire.onReceive(receiveData);
  } else {
    // Player 2 is the slave
    Wire.begin(PLAYER_2_ADDRESS);  // Slave
    Wire.onReceive(receiveData);
  }

  // Initialize deltaX for platforms
  for (int i = 0; i < PLATFORM_COUNT; i++) {
    platforms[i].deltaX = 0;
  }

  resetPlayers();
  initParticles();  // Initialize particles
}


// Initialize particle system
void initParticles() {
  for (int i = 0; i < MAX_PARTICLES; i++) {
    particles[i].active = false;
    particles[i].shrinkCounter = 0;  // Initialize shrink counter
  }
}

void resetPlayers() {
  // Initialize players
  player1 = { platforms[0].x + 5, platforms[0].y - PLAYER_HEIGHT, 0, 0,
              MAX_HEALTH, false, false, 0, false, 0, true, 0 };
  player2 = { platforms[1].x + 5, platforms[1].y - PLAYER_HEIGHT, 0, 0,
              MAX_HEALTH, false, false, 0, false, 0, true, 0 };
}

void loop() {
  if (!(arduboy.nextFrame())) {
    return;
  }

  arduboy.pollButtons();
  arduboy.clear();

  updatePlatforms();

  sendPlatformUpdate();  // Send platform positions from Player 1 to Player 2

  // Handle input and physics
  if (playerRole == 1) {
    handleInput(player1);
  } else {
    handleInput(player2);
  }

  applyPhysics(player1);
  applyPhysics(player2);

  // Send regular updates
  sendPlayerUpdate();

  // Player 1 checks for attacks
  if (player1.attacking && player1.attackTimer > 0) {
    if (checkAttackCollision(player1, player2)) {
      processAttack(player1, player2);
      sendAttackNotification(player1, player2);
    }
  }

  // Player 2 checks for attacks
  if (player2.attacking && player2.attackTimer > 0) {
    if (checkAttackCollision(player2, player1)) {
      processAttack(player2, player1);
      sendAttackNotification(player2, player1);
    }
  }

  updateParticles();  // Update particle system

  drawPlatforms();
  drawPlayer(player1);
  drawPlayer(player2);
  drawParticles();  // Draw particles
  drawHUD();

  if (checkVictory()) {
    resetPlayers();
  }

  arduboy.display();
}

// Function to handle player input
void handleInput(Player &player) {
  bool moving = false;

  if (arduboy.pressed(LEFT_BUTTON)) {
    player.velX = -PLAYER_SPEED;
    moving = true;
  } else if (arduboy.pressed(RIGHT_BUTTON)) {
    player.velX = PLAYER_SPEED;
    moving = true;
  } else {
    //player.velX = 0;  // Stop immediately when no button is pressed
  }

  if (arduboy.justPressed(A_BUTTON) && !player.jumping) {
    player.velY = JUMP_STRENGTH;
    player.jumping = true;
    moving = true;
    tunes.playScore(jump);
  }

  if (arduboy.pressed(B_BUTTON)) {
    player.chargeTime++;
    if (player.chargeTime > CHARGE_TIME) {
      player.chargedAttack = true;
    }
  }

  if (arduboy.justReleased(B_BUTTON)) {
    player.attacking = true;
    player.attackTimer = player.chargedAttack ? CHARGED_ATTACK_DURATION : ATTACK_DURATION;
    player.chargeTime = 0;
    tunes.playScore(miss);
  }

  if (player.attackTimer > 0) {
    player.attackTimer--;
  } else {
    player.attacking = false;
    player.chargedAttack = false;
  }

  player.idle = !moving && !player.attacking;
}

int roundFloat(float value) {
  if (value > 0) {
    return (int)(value + 0.5f);
  } else if (value < 0) {
    return (int)(value - 0.5f);
  } else {
    return 0;
  }
}


// Function to apply physics
void applyPhysics(Player &player) {
  player.velY += GRAVITY;


  // Apply friction
  if (player.velX != 0) {
    player.velX *= FRICTION;
    if (abs(player.velX) < 0.1) {
      player.velX = 0;
    }
  }


  // Store previous positions
  int prevX = player.x;
  int prevY = player.y;

  player.x += roundFloat(player.velX);
  player.y += roundFloat(player.velY);

  // Check for platform collisions
  bool onPlatform = false;
  Platform *currentPlatform = nullptr;

  int platformOnNum;

  for (int i = 0; i < PLATFORM_COUNT; i++) {
    Platform &platform = platforms[i];
    if (player.x + PLAYER_WIDTH > platform.x && player.x < platform.x + platform.width) {
      if (player.y + PLAYER_HEIGHT >= platform.y && player.y + PLAYER_HEIGHT - player.velY <= platform.y) {
        player.y = platform.y - PLAYER_HEIGHT;
        player.velY = 0;
        player.jumping = false;
        onPlatform = true;
        currentPlatform = &platform;
        platformOnNum = i;
      }
    }
  }

  if (onPlatform && platformOnNum == 3 && currentPlatform != nullptr) {
    // Adjust player's velX to match the platform's movement
    //if (player.velX == 0) {
    player.velX = currentPlatform->deltaX;

    //}
  }





  if (!onPlatform && player.y > SCREEN_HEIGHT - PLAYER_HEIGHT) {
    player.y = SCREEN_HEIGHT - PLAYER_HEIGHT;
    player.velY = 0;
    player.jumping = false;
  }

  if (player.x < 0) player.x = SCREEN_WIDTH;
  if (player.x > SCREEN_WIDTH) player.x = 0;
}

// Function to update platforms
void updatePlatforms() {
  if (playerRole == 1) {  // Only Player 1 updates the platforms
    for (int i = 0; i < PLATFORM_COUNT; i++) {
      if (platforms[i].moving) {
        int prevX = platforms[i].x;  // Save previous x position
        platforms[i].x += platforms[i].direction;
        platforms[i].deltaX = platforms[i].x - prevX;  // Calculate movement delta
        if (platforms[i].x < 20 || platforms[i].x + platforms[i].width > SCREEN_WIDTH - 20) {
          platforms[i].direction *= -1;
        }
      } else {
        platforms[i].deltaX = 0;  // No movement
      }
    }
  } else {
    // For Player 2, deltaX is already set in receiveData()
    for (int i = 0; i < PLATFORM_COUNT; i++) {
      if (!platforms[i].moving) {
        platforms[i].deltaX = 0;
      }
    }
  }
}

// Function to send platform updates from Player 1 to Player 2
void sendPlatformUpdate() {
  if (playerRole != 1) return;  // Only Player 1 sends platform updates

  Wire.beginTransmission(PLAYER_2_ADDRESS);
  Wire.write(CMD_PLATFORM_UPDATE);  // Command type

  // Send platform positions
  for (int i = 0; i < PLATFORM_COUNT; i++) {
    Platform &platform = platforms[i];
    // Send platform.x as 2 bytes
    Wire.write(platform.x >> 8);
    Wire.write(platform.x & 0xFF);
    // Send platform.y as 2 bytes
    Wire.write(platform.y >> 8);
    Wire.write(platform.y & 0xFF);
    // Send platform.width as 1 byte
    Wire.write(platform.width);
    // Send platform.moving as 1 byte
    Wire.write(platform.moving);
    // Send platform.direction as 1 byte (signed int)
    Wire.write((int8_t)platform.direction);
  }

  Wire.endTransmission();
}



// Particle functions
void spawnParticles(int x, int y, int velX, int velY) {
  for (int i = 0; i < MAX_PARTICLES; i++) {
    if (!particles[i].active) {
      particles[i].x = x;
      particles[i].y = y;
      particles[i].velX = (velX / 5) + (rand() % 5 - 2);  // Randomize velocity a bit
      particles[i].velY = (velY / 5) + (rand() % 5 - 2);
      particles[i].size = 3;
      particles[i].active = true;
      particles[i].shrinkCounter = 5;  // Set how many frames before shrinking
      //break;  // Spawn one particle per call
    }
  }
}

void updateParticles() {
  for (int i = 0; i < MAX_PARTICLES; i++) {
    if (particles[i].active) {
      particles[i].x += particles[i].velX;
      particles[i].y += particles[i].velY;
      // Decrease size only when shrinkCounter reaches 0
      if (particles[i].shrinkCounter > 0) {
        particles[i].shrinkCounter--;  // Count down for each frame
      } else {
        particles[i].size -= 1;          // Reduce size after shrinkCounter reaches 0
        particles[i].shrinkCounter = 5;  // Reset shrink counter
      }

      // Deactivate particle when size reaches 0
      if (particles[i].size <= 0) {
        particles[i].active = false;
      }
    }
  }
}

void drawParticles() {
  for (int i = 0; i < MAX_PARTICLES; i++) {
    if (particles[i].active) {
      arduboy.fillRect(particles[i].x, particles[i].y, particles[i].size, particles[i].size, WHITE);
    }
  }
}



// Function to check for attack collision
bool checkAttackCollision(Player &attacker, Player &defender) {
  return abs(attacker.x - defender.x) < PLAYER_WIDTH && abs(attacker.y - defender.y) < PLAYER_HEIGHT;
}

// Function to process an attack
void processAttack(Player &attacker, Player &defender) {
  int knockback = attacker.chargedAttack ? CHARGED_KNOCKBACK : BASE_KNOCKBACK;
  knockback += (MAX_HEALTH - defender.health) / 20;
  defender.velX = (attacker.x < defender.x) ? knockback * 4 : -knockback * 4;
  defender.velY = -knockback;

  defender.health -= attacker.chargedAttack ? CHARGED_DAMAGE : ATTACK_DAMAGE;

  attacker.attacking = false;
  attacker.attackTimer = 0;
  attacker.chargedAttack = false;

  attacker.attackDisplay = 5;

  tunes.playScore(hit);
  spawnParticles(defender.x + PLAYER_WIDTH / 2, defender.y + PLAYER_HEIGHT / 2, defender.velX, defender.velY);
}

// Function to send regular player updates
void sendPlayerUpdate() {
  Wire.beginTransmission((playerRole == 1) ? PLAYER_2_ADDRESS : PLAYER_1_ADDRESS);
  Wire.write(CMD_UPDATE);  // Command type

  Player &player = (playerRole == 1) ? player1 : player2;
  Wire.write(player.x >> 8);    // High byte
  Wire.write(player.x & 0xFF);  // Low byte
  Wire.write(player.y >> 8);
  Wire.write(player.y & 0xFF);
  Wire.write((int8_t)player.velX);
  Wire.write((int8_t)player.velY);
  Wire.write(player.attacking);
  Wire.write(player.chargedAttack);
  Wire.write(player.health);
  Wire.write(player.idle);
  Wire.endTransmission();
}

// Function to send attack notification
void sendAttackNotification(Player &attacker, Player &defender) {
  Wire.beginTransmission((playerRole == 1) ? PLAYER_2_ADDRESS : PLAYER_1_ADDRESS);
  Wire.write(CMD_ATTACK);  // Command type
  Wire.write(attacker.chargedAttack);
  Wire.write((int8_t)defender.velX);
  Wire.write((int8_t)defender.velY);
  Wire.write(defender.health);
  Wire.endTransmission();
}

// Function to receive data
void receiveData(int numBytes) {
  while (Wire.available()) {
    uint8_t commandType = Wire.read();
    if (commandType == CMD_UPDATE) {
      // Receive regular update
      Player &remotePlayer = (playerRole == 1) ? player2 : player1;
      remotePlayer.x = (Wire.read() << 8) | Wire.read();
      remotePlayer.y = (Wire.read() << 8) | Wire.read();
      remotePlayer.velX = (int8_t)Wire.read();
      remotePlayer.velY = (int8_t)Wire.read();
      remotePlayer.attacking = Wire.read();
      remotePlayer.chargedAttack = Wire.read();
      remotePlayer.health = Wire.read();
      remotePlayer.idle = Wire.read();
    } else if (commandType == CMD_ATTACK) {
      // Receive attack notification
      bool chargedAttack = Wire.read();
      int8_t velX = (int8_t)Wire.read();
      int8_t velY = (int8_t)Wire.read();
      int health = Wire.read();

      // Apply attack to local defender
      Player &defender = (playerRole == 1) ? player1 : player2;
      defender.velX = velX;
      defender.velY = velY;
      defender.health = health;
      Player &attacker = (playerRole == 2) ? player1 : player2;
      attacker.attackDisplay = 5;

    } else if (commandType == CMD_PLATFORM_UPDATE) {
      // Receive platform updates
      for (int i = 0; i < PLATFORM_COUNT; i++) {
        Platform &platform = platforms[i];
        int prevX = platform.x;  // Save previous x position
        platform.x = (Wire.read() << 8) | Wire.read();
        platform.y = (Wire.read() << 8) | Wire.read();
        platform.width = Wire.read();
        platform.moving = Wire.read();
        platform.direction = (int8_t)Wire.read();
        if (platformsInitialized) {
          platform.deltaX = platform.x - prevX;  // Calculate movement delta
        } else {
          platform.deltaX = 0;  // Initial frame
        }
      }
      platformsInitialized = true;
    }
  }
}

// Function to draw platforms
void drawPlatforms() {
  for (int i = 0; i < PLATFORM_COUNT; i++) {
    arduboy.fillRect(platforms[i].x, platforms[i].y, platforms[i].width, 2, WHITE);
  }
}

// Function to draw a player
void drawPlayer(Player &player) {
  if (player.idle) {
    if (arduboy.frameCount % 60 < 30) {
      // Draw a slightly shorter rectangle for 10 frames
      arduboy.fillRect(player.x, player.y + 1, PLAYER_WIDTH, PLAYER_HEIGHT - 1, WHITE);
    } else {
      // Draw the full rectangle for the next 10 frames
      arduboy.fillRect(player.x, player.y, PLAYER_WIDTH, PLAYER_HEIGHT, WHITE);
    }
  } else {
    arduboy.fillRect(player.x, player.y, PLAYER_WIDTH, PLAYER_HEIGHT, WHITE);
  }

  if (player.attacking) {
    arduboy.fillRect(player.x + 1, player.y + 1, PLAYER_WIDTH - 2, PLAYER_HEIGHT - 2, BLACK);
  }
  if (player.attackDisplay) {
    int expansion = 5 - player.attackDisplay;  // Calculate expansion
    arduboy.drawRect(
      player.x - 2 - expansion,
      player.y - 2 - expansion,
      PLAYER_WIDTH + 4 + 2 * expansion,
      PLAYER_HEIGHT + 4 + 2 * expansion,
      WHITE);
    player.attackDisplay--;
  }
}

// Function to draw HUD
void drawHUD() {
  arduboy.setCursor(0, 0);
  arduboy.print("P1: ");
  arduboy.print(player1.health);
  arduboy.setCursor(80, 0);
  arduboy.print("P2: ");
  arduboy.print(player2.health);
}

// Function to check victory condition
bool checkVictory() {
  if (player1.health <= 0 || player1.y > SCREEN_HEIGHT) {
    victoryAnimation(2);
    return true;
  }
  if (player2.health <= 0 || player2.y > SCREEN_HEIGHT) {
    victoryAnimation(1);
    return true;
  }
  return false;
}

// Function for victory animation
void victoryAnimation(int winner) {
  arduboy.clear();
  arduboy.setCursor(40, 30);
  arduboy.print((winner == 1) ? "P1 Wins!" : "P2 Wins!");
  arduboy.display();
  tunes.playScore(end);
  do {
    arduboy.pollButtons();
    if (arduboy.justPressed(A_BUTTON)) {

      break;
    }
    if (arduboy.justPressed(B_BUTTON)) {

      break;
    }

  } while (true);

  tunes.stopScore();
}