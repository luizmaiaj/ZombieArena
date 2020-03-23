#pragma once

#include "Zombie.h"
#include "SoundManager.h"
#include "LText.h"
#include "Player.h"
#include "TextureHolder.h"
#include "Bullet.h"
#include "Pickup.h"

enum class State { PAUSED, LEVELING_UP, GAME_OVER, PLAYING }; // The game will always be in one of four states

constexpr auto UPDATE_HUD_INTERVAL = 100; // How often (in frames) should we update the HUD;

using namespace sf;

int createBackground(VertexArray& rVA, IntRect arena);

Zombie* createHorde(int numZombies, IntRect arena);
