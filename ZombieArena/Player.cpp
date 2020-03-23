#define _USE_MATH_DEFINES
#include <cmath>
const float M_180_PI = 180.f / (float)M_PI; 

#include "Player.h"
#include "TextureHolder.h"

Player::Player()
{
	m_Speed = START_SPEED;
	m_Health = START_HEALTH;
	m_MaxHealth = START_HEALTH;

	// Associate a texture with the sprite
	// !!Watch this space!!
	m_Sprite = Sprite(TextureHolder::GetTexture("graphics/player.png"));

	// Set the origin of the sprite to the centre, 
	// for smooth rotation
	m_Sprite.setOrigin(m_Sprite.getLocalBounds().width / 2, m_Sprite.getLocalBounds().height / 2);
}

void Player::resetPlayerStats()
{
	m_weapon.Reset();

	m_Speed = START_SPEED;
	m_Health = START_HEALTH;
	m_MaxHealth = START_HEALTH;
}

void Player::spawn(IntRect arena, Vector2u resolution, int tileSize)
{
	// Place the player in the middle of the arena
	m_Position.x = (float) arena.width / 2;
	m_Position.y = (float) arena.height / 2;

	// Copy the details of the arena to the player's m_Arena
	m_Arena.left = arena.left;
	m_Arena.width = arena.width;
	m_Arena.top = arena.top;
	m_Arena.height = arena.height;

	// Remember how big the tiles are in this arena
	m_TileSize = tileSize;

	// Strore the resolution for future use
	m_Resolution.x = resolution.x;
	m_Resolution.y = resolution.y;

}

bool Player::isMoving()
{
	return (m_UpPressed || m_DownPressed || m_RightPressed || m_LeftPressed);
}

Time Player::getLastHitTime()
{
	return m_LastHit;
}

bool Player::hit(Time timeHit)
{
	if (timeHit.asMilliseconds() - m_LastHit.asMilliseconds() > 200)// 2 tenths of second
	{
		m_LastHit = timeHit;
		m_Health -= 10;
		return true;
	}
	else
	{
		return false;
	}

}

FloatRect Player::getPosition()
{
	return m_Sprite.getGlobalBounds();
}

Vector2f Player::getCenter()
{
	return m_Position;
}

float Player::getRotation()
{
	return m_Sprite.getRotation();
}

Sprite Player::getSprite()
{
	return m_Sprite;
}

int Player::getHealth()
{
	return m_Health;
}

void Player::move(Keyboard::Key aKey, bool aPressed)
{
	switch (aKey)
	{
	case Keyboard::Z:
		m_UpPressed = aPressed;
		break;
	case Keyboard::S:
		m_DownPressed = aPressed;
		break;
	case Keyboard::Q:
		m_LeftPressed = aPressed;
		break;
	case Keyboard::D:
		m_RightPressed = aPressed;
		break;
	}
}

void Player::moveX(float aX)
{
	m_LeftPressed = aX < 0;
	m_RightPressed = aX > 0;
}

void Player::moveY(float aY)
{
	m_UpPressed = aY > 0;
	m_DownPressed = aY < 0;
}

void Player::update(float aDT, Vector2i mousePosition)
{
	updatePosition(aDT, mousePosition);

	m_weapon.update(aDT);
}

uint Player::collisions(ZombieManager* apZombies)
{
	uint score = 0;

	// Collision detection: have any zombies been shot?
	for (uint j = 0; j < apZombies->count(); j++)
	{
		if (apZombies->m_zombies[j].isAlive())
		{
			Zombie* pZombie = &(apZombies->m_zombies[j]);

			if( m_weapon.collisions(pZombie) );
			{
				apZombies->kill();

				score += 10;
			}

			if (getPosition().intersects(apZombies->m_zombies[j].getPosition()))
			{
				//if (hit(gameTimeTotal)) hit.play();
			}
		}
	}

	return score;
}

bool Player::shoot(float xTarget, float yTarget)
{
	return m_weapon.Shoot(getCenter().x, getCenter().y, xTarget, yTarget);
}

void Player::upgradeSpeed()
{
	// 20% speed upgrade
	m_Speed += (float) (START_SPEED * .2);
}

void Player::upgradeHealth()
{
	// 20% max health upgrade
	m_MaxHealth += (int) (START_HEALTH * .2);

}

void Player::increaseHealthLevel(int amount)
{
	m_Health += amount;

	// But not beyond the maximum
	if (m_Health > m_MaxHealth)
	{
		m_Health = m_MaxHealth;
	}
}

void Player::updatePosition(float aDT, Vector2i mousePosition)
{
	// update position
	if (m_UpPressed) m_Position.y -= m_Speed * aDT;
	if (m_DownPressed) m_Position.y += m_Speed * aDT;
	if (m_RightPressed) m_Position.x += m_Speed * aDT;
	if (m_LeftPressed) m_Position.x -= m_Speed * aDT;

	// Keep the player in the arena
	if (m_Position.x > m_Arena.width - m_TileSize) m_Position.x = (float)m_Arena.width - m_TileSize;
	if (m_Position.x < m_Arena.left + m_TileSize) m_Position.x = (float)m_Arena.left + m_TileSize;
	if (m_Position.y > m_Arena.height - m_TileSize) m_Position.y = (float)m_Arena.height - m_TileSize;
	if (m_Position.y < m_Arena.top + m_TileSize) m_Position.y = (float)m_Arena.top + m_TileSize;

	m_Sprite.setPosition(m_Position);

	// Calculate the angle the player is facing
	float angle = (float)atan2(mousePosition.y - m_Resolution.y / 2, mousePosition.x - m_Resolution.x / 2) * M_180_PI;

	m_Sprite.setRotation(angle);
}
