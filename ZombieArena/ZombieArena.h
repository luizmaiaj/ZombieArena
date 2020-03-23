#pragma once

#include "Zombie.h"
#include "LText.h"
#include "Player.h"
#include "TextureHolder.h"
#include "Bullet.h"
#include "Pickup.h"
#include "ZombieManager.h"

enum class State { PAUSED, LEVELING_UP, GAME_OVER, PLAYING }; // The game will always be in one of four states

constexpr auto UPDATE_HUD_INTERVAL = 100; // How often (in frames) should we update the HUD;

using namespace sf;

int createBackground(VertexArray& rVA, IntRect arena);
