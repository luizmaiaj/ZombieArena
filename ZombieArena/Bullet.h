#pragma once
#include <SFML/Graphics.hpp>

using namespace sf;

const float BULLET_SPEED = 1500.f;
const float BULLET_RANGE = 500.f;

class Bullet
{
private:
	// Where is the bullet?
	Vector2f m_Position;

	// What each bullet looks like
	RectangleShape m_BulletShape;

	// Is this bullet currently whizzing through the air
	bool m_inFlight = false;

	// How fast does a bullet travel?
	float m_bulletSpeed{ BULLET_SPEED };
	float m_bulletRange{ BULLET_RANGE };

	// What fraction of 1 pixel does the bullet travel, 
	float m_bulletDistanceX{ 0.f };
	float m_bulletDistanceY{ 0.f };

	// Where is this bullet headed to?
	float m_XTarget{ 0.f };
	float m_YTarget{ 0.f };

	// Some boundaries so the bullet doesn't fly forever
	float m_MaxX{ 0.f };
	float m_MinX{ 0.f };
	float m_MaxY{ 0.f };
	float m_MinY{ 0.f };

public:
	Bullet(); // The constructor
	void stop(); // Stop the bullet
	bool isInFlight(); // Returns the value of m_InFlight
	void shoot(float startX, float startY, float xTarget, float yTarget, float aDeviation); // Launch a new bullet
	FloatRect getPosition(); // Tell the calling code where the bullet is in the world
	RectangleShape getShape(); // Return the actual shape (for drawing)
	void update(float elapsedTime); // Update the bullet each frame
	void setSpeed(float aSpeed);
	void setRange(float aRange);
};