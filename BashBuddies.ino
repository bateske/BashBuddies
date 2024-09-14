
#include <Arduboy2.h>
#include <Wire.h>

#include <ArduboyPlaytune.h>

#include "data.h"

Arduboy2 arduboy;

// Define player role (1 for Player 1, 2 for Player 2)
int playerRole = 0; // 1 for Player 1, 2 for Player 2

bool isSinglePlayer = false; // Add a flag to determine if it's single-player mode


// I2C Addresses
const int PLAYER_1_ADDRESS = 8;
const int PLAYER_2_ADDRESS = 9;

const int BROADCAST_ADDRESS = 0x00;

// I2C command to detect player
const uint8_t CMD_CHECK_PLAYER = 3;

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

const int KNOCKBACK_DURATION = 20;


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
  int playerNum;
  int knockbackTimer;
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

  int titleTick = 0;

  do {
    arduboy.clear();
    updateParticles();
    drawParticles();  // Draw particles


    if (titleY >= 20 && titleY % 6 == 1) {
      int randomX = random(20, 108);  // Random x between 20 and 108
      int randomY = random(10, 40);   // Random y between 10 and 40
      spawnParticles(randomX, randomY, 0, 0);
    }

    arduboy.drawBitmap(0, titleY, bashTitle, 128, 128, WHITE);

    if (titleY > -58) {
      delay(10);
    };
    if (titleY < -58 && titleY > -68) {
      arduboy.drawBitmap(79, -1, darken, 50, 50, BLACK);
      arduboy.drawBitmap(80, 0, punch1, 48, 48, WHITE);
    }
    if (titleY < -62 && titleY > -68) {
      arduboy.drawBitmap(39, 7, darken, 50, 50, BLACK);
      arduboy.drawBitmap(40, 8, punch2, 48, 48, WHITE);
    }
    if (titleY < -64 && titleY > -68) {
      arduboy.drawBitmap(-1, 15, darken, 50, 50, BLACK);
      arduboy.drawBitmap(0, 16, punch3, 48, 48, WHITE);
    }


    if (titleY < -76 && titleY > -92) {
      arduboy.drawBitmap(-1, -1, darken, 50, 50, BLACK);
      arduboy.drawBitmap(0, 0, punch4, 48, 48, WHITE);
    }
    if (titleY < -81 && titleY > -92) {
      arduboy.drawBitmap(39, 7, darken, 50, 50, BLACK);
      arduboy.drawBitmap(40, 8, punch5, 48, 48, WHITE);
    }
    if (titleY < -84 && titleY > -92) {
      arduboy.drawBitmap(79, 15, darken, 50, 50, BLACK);
      arduboy.drawBitmap(80, 16, punch6, 48, 48, WHITE);
    }


    if (titleY < -92 && titleY > -108) {
      arduboy.drawBitmap(79, -1, darken, 50, 50, BLACK);
      arduboy.drawBitmap(80, 0, punch7, 48, 48, WHITE);
    }
    if (titleY < -96 && titleY > -108) {
      arduboy.drawBitmap(39, 7, darken, 50, 50, BLACK);
      arduboy.drawBitmap(40, 8, punch8, 48, 48, WHITE);
    }
    if (titleY < -99 && titleY > -108) {
      arduboy.drawBitmap(-1, 15, darken, 50, 50, BLACK);
      arduboy.drawBitmap(0, 16, punch9, 48, 48, WHITE);
    }


    if (titleY < -108) {
      arduboy.drawBitmap(79, -1, darken, 50, 50, BLACK);
      arduboy.drawBitmap(80, 0, punch10, 48, 48, WHITE);
    }
    if (titleY < -118) {
      arduboy.drawBitmap(-1,-1, darken, 50, 50, BLACK);
      arduboy.drawBitmap(0, 0, punch11, 48, 48, WHITE);
    }
    if (titleY < -134) {
      arduboy.drawBitmap(39, 15, darken, 50, 50, BLACK);
      arduboy.drawBitmap(40, 16, punch12, 48, 48, WHITE);
    }






    arduboy.pollButtons();

    if (titleY > -135) {
      if (titleTick < 7) {
        titleTick++;
      } else {
        titleY--;
        titleTick = 0;
      }

    } else {
    }





    //arduboy.fillRect(0,0,20,20,BLACK);
    //arduboy.setCursor(0,0);
    //arduboy.print(titleY);



    arduboy.display();




    if (arduboy.justPressed(A_BUTTON)) {

      break;
    }
    if (arduboy.justPressed(B_BUTTON)) {

      break;
    }

  } while (true);


  while (arduboy.buttonsState()) {
    arduboy.pollButtons();  // Poll the buttons but do nothing
  }

  arduboy.clear();
  arduboy.drawBitmap(0, -6, bashTitle, 128, 128, WHITE);
  arduboy.setCursor(0,48);
  arduboy.println("  [A] Single Player   ");
  arduboy.println("  [B] Multiplayer     ");
  arduboy.display();
// Inside setup()
do {
  arduboy.pollButtons();
  if (arduboy.justPressed(A_BUTTON)) {
    isSinglePlayer = true; // Single-player mode
    break;
  }
  if (arduboy.justPressed(B_BUTTON)) {
    isSinglePlayer = false; // Multiplayer mode
    break;
  }
} while (true);

  tunes.stopScore();

if (!isSinglePlayer) {
  // Multiplayer: Initialize I2C
  Wire.begin(PLAYER_2_ADDRESS); 
  Wire.onReceive(receiveData);

  Wire.beginTransmission(PLAYER_1_ADDRESS); 
  Wire.write(CMD_CHECK_PLAYER);
  if (Wire.endTransmission() == 0) {
    playerRole = 2;
    Wire.begin(PLAYER_2_ADDRESS); 
  } else {
    playerRole = 1;
    Wire.begin(PLAYER_1_ADDRESS);
  }
} else {
  // Single-player: No I2C initialization
  playerRole = 1; // Player 1 will always be controlled by the user in single-player
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
              MAX_HEALTH, false, false, 0, false, 0, true, 0 , 1};
  player2 = { platforms[1].x + 5, platforms[1].y - PLAYER_HEIGHT, 0, 0,
              MAX_HEALTH, false, false, 0, false, 0, true, 0 , 2};
}

void loop() {
  if (!(arduboy.nextFrame())) {
    return;
  }

  arduboy.pollButtons();
  arduboy.clear();

  updatePlatforms();

  if (!isSinglePlayer) {
    sendPlatformUpdate(); // Only in multiplayer mode
  }
  // Handle input and physics
  if (playerRole == 1) {
    handleInput(player1);
  } else {
    handleInput(player2);
  }


  if (isSinglePlayer) {
    // Simple AI for Player 2 in single-player mode
    controlAI(player2);
  } else {
    // Multiplayer mode: Handle Player 2 as remote player
    sendPlayerUpdate(); // Only in multiplayer mode
  }

  applyPhysics(player1);
  applyPhysics(player2);




if (!isSinglePlayer) {
  // Send regular updates
  sendPlayerUpdate();
}

  // Player 1 checks for attacks
  if (player1.attacking && player1.attackTimer > 0) {
    if (checkAttackCollision(player1, player2)) {
      processAttack(player1, player2);
      if (!isSinglePlayer) {
      sendAttackNotification(player1, player2);
      }
    }
  }

  // Player 2 checks for attacks
  if (player2.attacking && player2.attackTimer > 0) {
    if (checkAttackCollision(player2, player1)) {
      processAttack(player2, player1);
      if (!isSinglePlayer) {
      sendAttackNotification(player2, player1);
      }
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
// Global attack cooldown for AI
int aiAttackCooldown = 0;

// Global variables for random jumping
unsigned long nextJumpTime = 0;  // When the AI should jump next

void controlAI(Player &player) {
  unsigned long currentTime = millis();

    if (player.knockbackTimer > 0) {
    player.knockbackTimer--;
    return;  // Skip other movement while knockback is active
  }
  player.idle = false;
  // Define idle chance (30% of the time)
  if (random(0, 100) < 70) {
    player.velX = 0;  // Stay idle
  } else {
    // Move towards Player 1 when not idle
    if (player1.x > player.x) {
      player.velX = PLAYER_SPEED;
    } else if (player1.x < player.x) {
      player.velX = -PLAYER_SPEED;
    } else {
      player.velX = 0;
    }
  }

  // Handle random jumping at random intervals
  if (!player.jumping && currentTime >= nextJumpTime) {
    player.velY = JUMP_STRENGTH-1;
    player.jumping = true;
    
    // Set the next jump to a random time in the future (2 to 5 seconds)
    nextJumpTime = currentTime + random(1000, 4000);
  }

  // Attack when close and attack cooldown is over
  if (abs(player1.x - player.x) < PLAYER_WIDTH && aiAttackCooldown == 0) {
    player.attacking = true;
    player.attackTimer = 2;
    aiAttackCooldown = 30;  // Start global attack cooldown
  }

  // Reduce attack timer
  if (player.attackTimer > 0) {
    player.attackTimer--;
  } else {
    player.attacking = false;
  }

  // Handle global attack cooldown
  if (aiAttackCooldown > 0) {
    aiAttackCooldown--;
  }
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
  defender.velX = (attacker.x < defender.x) ? knockback * 8 : -knockback * 8;
  defender.velY = -knockback;

  defender.health -= attacker.chargedAttack ? CHARGED_DAMAGE : ATTACK_DAMAGE;

  defender.knockbackTimer = KNOCKBACK_DURATION;

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
    } else if (commandType == CMD_CHECK_PLAYER) {
    // Send response confirming Player 1's presence
    //Wire.beginTransmission(PLAYER_2_ADDRESS);
    //Wire.write(CMD_CHECK_PLAYER);
    //Wire.endTransmission();
        
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
      //arduboy.fillRect(player.x, player.y + 1, PLAYER_WIDTH, PLAYER_HEIGHT - 1, WHITE);
      arduboy.fillRect(player.x, player.y, PLAYER_WIDTH, PLAYER_HEIGHT, WHITE);
    } else {
      // Draw the full rectangle for the next 10 frames
      arduboy.fillRect(player.x, player.y, PLAYER_WIDTH, PLAYER_HEIGHT, WHITE);
    }
  } else {
    arduboy.fillRect(player.x, player.y, PLAYER_WIDTH, PLAYER_HEIGHT, WHITE);
  }

  arduboy.setCursor(player.x+1,player.y+1);
  arduboy.setTextColor(BLACK);
    

  if(player.playerNum==1){
    arduboy.write(34);
  }else{
    arduboy.write(19);
  }
arduboy.setTextColor(WHITE);

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

    while (arduboy.buttonsState()) {
    arduboy.pollButtons();  // Poll the buttons but do nothing
  }
  delay(500);
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