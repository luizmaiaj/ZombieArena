#pragma once

#include <SFML/Graphics.hpp>
#include "TextureHolder.h"
#include "Weapon.h"
#include "ZombieManager.h"
#include "Pickup.h"

using namespace sf;

const float START_SPEED = 200;
const int START_HEALTH = 100;

class Player
{
public:
	Player();

	// Call this at the end of every game
	void resetPlayerStats();

	void spawn(IntRect arena, Vector2u resolution);

	bool isMoving();

	// Handle the player getting hit by a zombie
	bool hit(Time timeHit);

	// How long ago was the player last hit
	Time getLastHitTime();

	// Where is the player
	FloatRect getPosition();

	// Where is the center of the player
	Vector2f getCenter();

	// Which angle is the player facing
	float getRotation();

	// Send a copy of the sprite to main
	Sprite getSprite();

	// How much health has the player currently got?
	int getHealth();

	// The next four functions move the player
	void move(Keyboard::Key aKey, bool aPressed);
	void moveX(float aX);
	void moveY(float aY);

	// We will call this function once every frame
	void update(float aDT, Vector2i mousePosition);

	uint collisions(ZombieManager* apZombies);

	bool shoot(float xTarget, float yTarget);

	// Give player a speed boost
	void upgradeSpeed();

	// Give the player some health
	void upgradeHealth();

	// Increase the maximum amount of health the player can have
	void increaseHealthLevel(int amount);

	Weapon m_weapon;

private:
	void updatePosition(float aDT, Vector2i mousePosition);

	// Where is the player
	Vector2f m_Position;

	// Of course we will need a sprite
	Sprite m_Sprite;

	// And a texture
	// !!Watch this space!!
	Texture m_Texture;

	// What is the screen resolution
	Vector2f m_res_2;

	// What size is the current arena
	FloatRect m_playerArena;

	// How big is each tile of the arena
	float m_gapSize{ 0.f };

	// Which directions is the player currently moving in
	bool m_UpPressed{ false };
	bool m_DownPressed{ false };
	bool m_LeftPressed{ false };
	bool m_RightPressed{ false };

	// How much health has the player got?
	int m_Health{ START_HEALTH };
	// What is the maximum health the player can have
	int m_MaxHealth{ START_HEALTH };

	// When was the player last hit
	Time m_LastHit;

	// Speed in pixels per second
	float m_Speed{ START_SPEED };
};

