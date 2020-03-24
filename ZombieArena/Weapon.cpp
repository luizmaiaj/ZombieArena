#include "Weapon.h"

Weapon::Weapon()
{
	Reset();

	m_shotgun = new LSound("sound/shotgun.wav"); // Prepare the shoot sound
	m_handgun = new LSound("sound/handgun.wav"); // Prepare the shoot sound
	m_reload = new LSound("sound/reload.wav"); // Prepare the reload sound
	m_reloadFailed = new LSound("sound/reload_failed.wav"); // Prepare the failed sound
	m_splat = new LSound("sound/splat.wav"); // Prepare the splat sound
}

void Weapon::Reset()
{
	m_spareBullets = START_BULLETS;
	m_clipBullets = CLIP_SIZE;
	m_clipSize = CLIP_SIZE;
	m_fireRate = FIRE_RATE;
}

bool Weapon::Reload()
{
	bool bReloaded = false;

	m_time += m_clock.restart();

	uint dt = (uint)m_time.asMilliseconds() - m_lastReload.asMilliseconds();

	if (dt < RELOAD_RATE) return bReloaded;

	if (m_clipBullets < m_clipSize)
	{
		uint iToReload = m_clipSize - m_clipBullets;

		if (iToReload <=  m_spareBullets)
		{
			bReloaded = true;
			m_clipBullets += iToReload;
			m_spareBullets -= iToReload;
		}
		else
		{
			bReloaded = true;
			m_clipBullets += m_spareBullets;
			m_spareBullets = 0;
		}

		if (bReloaded)
		{
			m_reload->play();
		}
	}
	else
		m_reloadFailed->play();

	return bReloaded;
}

void Weapon::AddClip(uint aBullets)
{
	m_reload->play();

	m_spareBullets += aBullets;
}

bool Weapon::Shoot(float startX, float startY, float xTarget, float yTarget)
{
	m_time += m_clock.restart();

	uint dt = (uint) m_time.asMilliseconds() - m_lastShot.asMilliseconds();

	if ((dt > m_fireRate))
	{
		uint count{ 0 };

		switch (m_weaponType)
		{
		case WeaponType::SHOTGUN:
			if (shootBullet(startX, startY, xTarget, yTarget, -7.f)) count++;
			if (shootBullet(startX, startY, xTarget, yTarget,  7.f)) count++;
			if (count > 0) m_shotgun->play();
			break;
		case WeaponType::HANDGUN:
			if (shootBullet(startX, startY, xTarget, yTarget, 0)) m_handgun->play();
			break;
		}

		return true;
	}

	return false;
}

bool Weapon::shootBullet(float startX, float startY, float xTarget, float yTarget, float aDeviation)
{
	if (m_clipBullets <= 0) return false;

	m_bullets[m_currentBullet].shoot(startX, startY, xTarget, yTarget, aDeviation);

	nextBullet();

	m_clipBullets--;

	return true;
}

void Weapon::nextBullet()
{
	if (++m_currentBullet > (BULLET_SPRITES - 1)) m_currentBullet = 0;
}

void Weapon::IncreaseFireRate()
{
	m_fireRate /= 2;
}

void Weapon::IncreaseClipSize()
{
	m_clipSize++;
}

void Weapon::update(float aDT)
{	
	for (int i = 0; i < BULLET_SPRITES; i++) // Update any bullets that are in-flight
	{
		if (m_bullets[i].isInFlight())
			m_bullets[i].update(aDT);
	}
}

uint Weapon::SpareBullets()
{
	return m_spareBullets;
}

uint Weapon::ClipBullets()
{
	return m_clipBullets;
}

uint Weapon::collisions(Zombie* apZombie)
{
	uint score = 0;

	for (uint i = 0; i < BULLET_SPRITES; i++)
	{
		if (m_bullets[i].isInFlight())
		{
			if (m_bullets[i].getPosition().intersects(apZombie->getPosition()))
			{
				m_bullets[i].stop(); // Stop the bullet

				if (apZombie->hit()) // Register the hit and see if it was a kill
				{
					score += 10; // Not just a hit but a kill too
				}

				m_splat->play();
			}
		}
	}

	return score;
}

void Weapon::drawBullets(RenderWindow& window)
{
	for (uint i = 0; i < BULLET_SPRITES; i++)
		if (m_bullets[i].isInFlight())
			window.draw(m_bullets[i].getShape());
}

void Weapon::setType(WeaponType aType)
{
	m_weaponType = aType;

	float fSpeed = 0.f;

	switch (aType)
	{
	case WeaponType::HANDGUN:
		fSpeed = BULLET_SPEED;
		break;
	case WeaponType::SHOTGUN:
		fSpeed = BULLET_SPEED * .75f;
		break;
	}

	for (uint i = 0; i < BULLET_SPRITES; i++)
		m_bullets[i].setSpeed(fSpeed);
}

WeaponType Weapon::getType()
{
	return m_weaponType;
}
