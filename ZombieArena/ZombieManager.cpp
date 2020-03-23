#include "ZombieManager.h"

ZombieManager::ZombieManager()
{
	m_SM.pushback("sound/zombie_attack.wav");
	m_SM.pushback("sound/zombie_short.wav");
	m_SM.pushback("sound/zombie_short_low.wav");
	m_SM.pushback("sound/zombie_slow.wav");
}

Zombie* ZombieManager::createHorde(int numZombies, IntRect arena)
{
	delete[] m_zombies; // Delete the previously allocated memory (if it exists)

	m_zombies = new Zombie[numZombies];

	m_numZombiesAlive = m_numZombies = numZombies;

	int maxY = arena.height - ZOMBIE_GAP;
	int minY = arena.top + ZOMBIE_GAP;
	int maxX = arena.width - ZOMBIE_GAP;
	int minX = arena.left + ZOMBIE_GAP;

	for (int i = 0; i < numZombies; i++)
	{
		// Which side should the zombie spawn
		srand((int)time(0) * i);
		int side = (rand() % 4);
		float x = 0.f, y = 0.f;

		switch (side)
		{
		case 0:
			// left
			x = (float)minX;
			y = (float)(rand() % maxY) + minY;
			break;

		case 1:
			// right
			x = (float)maxX;
			y = (float)(rand() % maxY) + minY;
			break;

		case 2:
			// top
			x = (float)(rand() % maxX) + minX;
			y = (float)minY;
			break;

		case 3:
			// bottom
			x = (float)(rand() % maxX) + minX;
			y = (float)maxY;
			break;
		}

		// Bloater, crawler or runner
		srand((int)time(0) * i * 2);
		int type = (rand() % 3);

		// Spawn the new zombie into the array
		m_zombies[i].spawn(x, y, type, i);
	}
	return m_zombies;
}

void ZombieManager::update(float aDT, Vector2f playerPosition)
{
	for (uint i = 0; i < m_numZombies; i++)
	{
		if (m_zombies[i].isAlive())
			m_zombies[i].update(aDT, playerPosition);
	}

	m_SM.play(aDT);
}

uint ZombieManager::alive()
{
	return m_numZombiesAlive;
}

void ZombieManager::kill()
{
	m_numZombiesAlive--;
}

uint ZombieManager::count()
{
	return m_numZombies;
}
