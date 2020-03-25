#pragma once
#include <SFML/Graphics.hpp>
#include "TextureHolder.h"
#include "Weapon.h"
#include "ZombieManager.h"
#include "Pickup.h"

using namespace sf;

const float M_PI = (float)3.14159265358979323846f;
const float M_PI_180 = M_PI / 180.f;
const float M_180_PI = 180.f / M_PI;

const float START_SPEED = 200;
const int START_HEALTH = 100;
const float PLAYER_X_ORIGIN = 20.f;
const float PLAYER_Y_ORIGIN = 27.f;
const float PLAYER_ARM_ANGLE_SG = atan2(37 - PLAYER_Y_ORIGIN, 84 - PLAYER_X_ORIGIN) * M_180_PI;
const float PLAYER_ARM_ANGLE_HG = atan2(40 - PLAYER_Y_ORIGIN, 68 - PLAYER_X_ORIGIN) * M_180_PI;
const float PLAYER_ARM_DIST_SG = sqrt(powf(PLAYER_Y_ORIGIN - 37, 2) + powf(PLAYER_X_ORIGIN - 84, 2));
const float PLAYER_ARM_DIST_HG = sqrt(powf(PLAYER_Y_ORIGIN - 40, 2) + powf(PLAYER_X_ORIGIN - 68, 2));

class Player
{
public:
	Player();
	void resetPlayerStats(); // Call this at the end of every game
	void spawn(IntRect arena, Vector2u resolution);
	bool isMoving();
	bool hit(Time timeHit); // Handle the player getting hit by a zombie
	Time getLastHitTime(); // How long ago was the player last hit
	FloatRect getPosition(); // Where is the player
	Vector2f getCenter(); // Where is the center of the player
	float getRotation(); // Which angle is the player facing
	Sprite getSprite(); // Send a copy of the sprite to main
	int getHealth(); // How much health has the player currently got?
	void move(Keyboard::Key aKey, bool aPressed);
	void moveX(float aX);
	void moveY(float aY);
	void update(float aDT, Vector2i mousePosition); // We will call this function once every frame
	uint collisions(ZombieManager* apZombies);
	bool shoot(float xTarget, float yTarget);
	void upgradeSpeed(); // Give player a speed boost
	void upgradeHealth(); // Give the player some health
	void increaseHealthLevel(int amount); // Increase the maximum amount of health the player can have
	void setWeapon(WeaponType aWeapon);

	Weapon m_weapon;

private:
	void updatePosition(float aDT, Vector2i mousePosition);

	// Where is the player
	Vector2f m_Position;

	// Of course we will need a sprite
	Sprite m_handgun;
	Sprite m_shotgun;
	Sprite m_rifle;

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

