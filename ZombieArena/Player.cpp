#include "Player.h"
#include "TextureHolder.h"

Player::Player()
{
	m_Speed = START_SPEED;
	m_Health = START_HEALTH;
	m_MaxHealth = START_HEALTH;

	// Associate a texture with the sprite
	m_handgun = Sprite(TextureHolder::GetTexture("graphics/player_handgun.png"));
	m_handgun.setOrigin(PLAYER_X_ORIGIN, PLAYER_Y_ORIGIN);

	m_shotgun = Sprite(TextureHolder::GetTexture("graphics/player_shotgun.png"));
	m_shotgun.setOrigin(PLAYER_X_ORIGIN, PLAYER_Y_ORIGIN);

	m_rifle   = Sprite(TextureHolder::GetTexture("graphics/player_rifle.png"));
	m_rifle.setOrigin(PLAYER_X_ORIGIN, PLAYER_Y_ORIGIN);

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
	switch (m_weapon.getType())
	{
	case WeaponType::HANDGUN:
		return m_handgun.getGlobalBounds();
		break;
	case WeaponType::SHOTGUN:
		return m_shotgun.getGlobalBounds();
		break;
	case WeaponType::RIFLE:
		return m_rifle.getGlobalBounds();
		break;
	}

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
	case WeaponType::RIFLE:
		return m_rifle;
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
	float xWeapon{ getCenter().x };
	float yWeapon{ getCenter().y };
	float rotation{ getRotation() };
	float correction{ 0.f };

	switch (m_weapon.getType())
	{
	case WeaponType::HANDGUN:
		correction = (rotation + PLAYER_ARM_ANGLE_HG) * M_PI_180;
		xWeapon += cos(correction) * PLAYER_ARM_DIST_HG;
		yWeapon += sin(correction) * PLAYER_ARM_DIST_HG;
		break;
	case WeaponType::SHOTGUN:
	case WeaponType::RIFLE:
		correction = (rotation + PLAYER_ARM_ANGLE_SG) * M_PI_180;
		xWeapon += cos(correction) * PLAYER_ARM_DIST_SG;
		yWeapon += sin(correction) * PLAYER_ARM_DIST_SG;
		break;
	}

	return m_weapon.Shoot(xWeapon, yWeapon, xTarget, yTarget);
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

	switch (aWeapon)
	{
	case WeaponType::HANDGUN:
		m_gapSize = m_handgun.getLocalBounds().width / 2;
		break;
	case WeaponType::SHOTGUN:
		m_gapSize = m_shotgun.getLocalBounds().width / 2;
		break;
	case WeaponType::RIFLE:
		m_gapSize = m_rifle.getLocalBounds().width / 2;
		break;
	}

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
	m_rifle.setPosition(m_Position);

	// Calculate the angle the player is facing
	float angle = atan2(mousePosition.y - m_res_2.y, mousePosition.x - m_res_2.x) * M_180_PI;

	m_handgun.setRotation(angle);
	m_shotgun.setRotation(angle);
	m_rifle.setRotation(angle);
}
