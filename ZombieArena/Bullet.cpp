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
	m_inFlight = true;
	m_Position.x = startX;
	m_Position.y = startY;

	float dx = startX - targetX;
	float dy = startY - targetY;

	float angle = atan2(dy, dx) * M_180_PI;
	
	if (aDeviation != 0)
	{ // correct the angles to deviate the bullet
		angle += aDeviation;

		if (angle > 180) angle -= 360;
		if (angle < -180) angle += 360;

		dx = cos(angle * M_PI_180);
		dy = sin(angle * M_PI_180);
	}

	// Set the "speed" horizontally and vertically
	m_bulletDistanceY = m_bulletSpeed / (1 + abs(dx / dy));
	m_bulletDistanceX = m_bulletSpeed - m_bulletDistanceY;

	// Point the bullet in the right direction
	if (targetX < startX) m_bulletDistanceX = -m_bulletDistanceX;
	if (targetY < startY) m_bulletDistanceY = -m_bulletDistanceY;

	// Set a max range of 1000 pixels
	m_MinX = startX - m_bulletRange;
	m_MaxX = startX + m_bulletRange;
	m_MinY = startY - m_bulletRange;
	m_MaxY = startY + m_bulletRange;

	// Position the bullet ready to be drawn
	m_BulletShape.setPosition(m_Position);
}

void Bullet::stop()
{
	m_inFlight = false;
}

bool Bullet::isInFlight()
{
	return m_inFlight;
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
	m_Position.x += m_bulletDistanceX * elapsedTime;
	m_Position.y += m_bulletDistanceY * elapsedTime;

	// Move the bullet
	m_BulletShape.setPosition(m_Position);

	// Has the bullet gone out of range?
	if (m_Position.x < m_MinX || m_Position.x > m_MaxX || m_Position.y < m_MinY || m_Position.y > m_MaxY)
		m_inFlight = false;
}

void Bullet::setSpeed(float aSpeed)
{
	if(!isInFlight()) m_bulletSpeed = aSpeed; // only change speed for bullets not yet shot
}

void Bullet::setRange(float aRange)
{
	if (!isInFlight()) m_bulletRange = aRange; // only change speed for bullets not yet shot
}