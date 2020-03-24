#include "Bullet.h"
const float M_PI = (float)3.14159265358979323846f;
const float M_PI_180 = M_PI / 180.f;
const float M_180_PI = 180.f / M_PI;

// The constructor
Bullet::Bullet()
{
	m_BulletShape.setSize(sf::Vector2f(2, 2));
}

void Bullet::shoot(float startX, float startY, float targetX, float targetY, float aDeviation)
{
	m_InFlight = true;
	m_Position.x = startX;
	m_Position.y = startY;

	float dx = startX - targetX;
	float dy = startY - targetY;

	float angle = atan2(dy, dx) * M_180_PI;
	
	if (aDeviation != 0)
	{ // correct the angles to deviate the bullet
		angle += aDeviation;
		dx = cos(angle * M_PI_180);
		dy = sin(angle * M_PI_180);
	}

	// Set the "speed" horizontally and vertically
	m_BulletDistanceY = m_BulletSpeed / (1 + abs(dx / dy));
	m_BulletDistanceX = BULLET_SPEED - m_BulletDistanceY;

	// Point the bullet in the right direction
	if (targetX < startX) m_BulletDistanceX = -m_BulletDistanceX;
	if (targetY < startY) m_BulletDistanceY = -m_BulletDistanceY;

	// Set a max range of 1000 pixels
	m_MinX = startX - BULLET_RANGE;
	m_MaxX = startX + BULLET_RANGE;
	m_MinY = startY - BULLET_RANGE;
	m_MaxY = startY + BULLET_RANGE;

	// Position the bullet ready to be drawn
	m_BulletShape.setPosition(m_Position);
}

void Bullet::stop()
{
	m_InFlight = false;
}

bool Bullet::isInFlight()
{
	return m_InFlight;
}

FloatRect Bullet::getPosition()
{
	return m_BulletShape.getGlobalBounds();
}

RectangleShape Bullet::getShape()
{
	return m_BulletShape;
}


void Bullet::update(float elapsedTime)
{
	// Update the bullet position variables
	m_Position.x += m_BulletDistanceX * elapsedTime;
	m_Position.y += m_BulletDistanceY * elapsedTime;

	// Move the bullet
	m_BulletShape.setPosition(m_Position);

	// Has the bullet gone out of range?
	if (m_Position.x < m_MinX || m_Position.x > m_MaxX || m_Position.y < m_MinY || m_Position.y > m_MaxY)
		m_InFlight = false;
}

void Bullet::setSpeed(float aSpeed)
{
	m_BulletSpeed = aSpeed;
}
