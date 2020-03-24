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
	m_handgun = Sprite(TextureHolder::GetTexture("graphics/player_handgun.png"));
	m_shotgun = Sprite(TextureHolder::GetTexture("graphics/player_shotgun.png"));

	setWeapon(WeaponType::HANDGUN);
}

void Player::resetPlayerStats()
{
	m_weapon.Reset();

	m_Speed = START_SPEED;
	m_Health = START_HEALTH;
	m_MaxHealth = START_HEALTH;
}

void Player::spawn(IntRect arena, Vector2u resolution)
{
	// Place the player in the middle of the arena
	m_Position.x = (float) arena.width / 2;
	m_Position.y = (float) arena.height / 2;

	// Copy the details of the arena to the player's m_Arena
	m_playerArena.left = arena.left + m_gapSize;
	m_playerArena.width = arena.width - m_gapSize;
	m_playerArena.top = arena.top + m_gapSize;
	m_playerArena.height = arena.height - m_gapSize;

	// Strore the resolution for future use
	m_res_2.x = (float) resolution.x / 2;
	m_res_2.y = (float) resolution.y / 2;
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
	return m_handgun.getGlobalBounds();
}

Vector2f Player::getCenter()
{
	return m_Position;
}

float Player::getRotation()
{
	return m_handgun.getRotation();
}

Sprite Player::getSprite()
{
	switch (m_weapon.getType())
	{
	case WeaponType::HANDGUN:
		return m_handgun;
	case WeaponType::SHOTGUN:
		return m_shotgun;
	}

	return m_handgun;
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

			if( m_weapon.collisions(pZombie) )
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
	m_Speed += (float) (START_SPEED * .2); // 20% speed upgrade
}

void Player::upgradeHealth()
{
	m_MaxHealth += (int) (START_HEALTH * .2); // 20% max health upgrade
}

void Player::increaseHealthLevel(int amount)
{
	m_Health += amount;

	if (m_Health > m_MaxHealth) m_Health = m_MaxHealth;
}

void Player::setWeapon(WeaponType aWeapon)
{
	m_weapon.setType(aWeapon);

	float fWidth_2 = m_handgun.getLocalBounds().width / 2;
	float fHeight_2 = m_handgun.getLocalBounds().height / 2;

	if (fWidth_2 < m_shotgun.getLocalBounds().width / 2) fWidth_2 = m_shotgun.getLocalBounds().width / 2;
	if (fHeight_2 < m_shotgun.getLocalBounds().height / 2) fHeight_2 = m_shotgun.getLocalBounds().height / 2;

	switch(aWeapon)
	{
	case WeaponType::HANDGUN:
		m_handgun.setOrigin(21.f, 27.f); // position determined by the size of the sprite
		break;
	case WeaponType::SHOTGUN:
		m_shotgun.setOrigin(21.f, 27.f);  // position determined by the size of the sprite
		break;
	}

	m_gapSize = (fWidth_2 > fHeight_2) ? fWidth_2 : fHeight_2;

	m_gapSize = TILE_SIZE + m_gapSize;
}

void Player::updatePosition(float aDT, Vector2i mousePosition)
{
	// update position
	if (m_UpPressed) m_Position.y -= m_Speed * aDT;
	if (m_DownPressed) m_Position.y += m_Speed * aDT;
	if (m_RightPressed) m_Position.x += m_Speed * aDT;
	if (m_LeftPressed) m_Position.x -= m_Speed * aDT;

	// Keep the player in the arena
	if (m_Position.x > m_playerArena.width) m_Position.x = (float) m_playerArena.width;
	if (m_Position.x < m_playerArena.left) m_Position.x = (float) m_playerArena.left;
	if (m_Position.y > m_playerArena.height) m_Position.y = (float) m_playerArena.height;
	if (m_Position.y < m_playerArena.top) m_Position.y = (float) m_playerArena.top;

	m_handgun.setPosition(m_Position);
	m_shotgun.setPosition(m_Position);

	// Calculate the angle the player is facing
	float angle = atan2(mousePosition.y - m_res_2.y, mousePosition.x - m_res_2.x) * M_180_PI;

	m_handgun.setRotation(angle);
	m_shotgun.setRotation(angle);
}
