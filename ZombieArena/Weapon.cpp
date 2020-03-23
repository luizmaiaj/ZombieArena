#include "Weapon.h"

Weapon::Weapon()
{
	Reset();

	m_shoot = new LSound("sound/shoot.wav"); // Prepare the shoot sound
	m_reload = new LSound("sound/reload.wav"); // Prepare the reload sound
	m_reloadFailed = new LSound("sound/reload_failed.wav"); // Prepare the failed sound
}

void Weapon::Reset()
{
	m_spareBullets = START_BULLETS;
	m_bulletsInClip = CLIP_SIZE;
	m_clipSize = CLIP_SIZE;
	m_fireRate = FIRE_RATE;
}

void Weapon::Reload()
{
	if (m_bulletsInClip < m_clipSize)
	{
		bool bReloaded = false;
		uint iToReload = m_clipSize - m_bulletsInClip;

		if (iToReload <=  m_spareBullets)
		{
			bReloaded = true;
			m_bulletsInClip += iToReload;
			m_spareBullets -= iToReload;
		}
		else
		{
			bReloaded = true;
			m_bulletsInClip += m_spareBullets;
			m_spareBullets = 0;
		}

		if (bReloaded)
		{
			m_reload->play();
		}
	}
	else
		m_reloadFailed->play();
}

void Weapon::AddClip(uint aBullets)
{
	m_reload->play();

	m_spareBullets += aBullets;
}

bool Weapon::Shoot()
{
	if (m_bulletsInClip == 0) return false;

	m_shoot->play();

	m_bulletsInClip--;

	return true;
}
