#pragma once

#include "Zombie.h"
#include "SoundManager.h"

typedef unsigned int uint;

const uint ZOMBIE_GAP = 75;

class ZombieManager
{
public:
	ZombieManager();
	Zombie* createHorde(int numZombies, IntRect arena);
	void update(float aDT, Vector2f playerPosition);
	uint alive();
	void kill();
	uint count();

	Zombie* m_zombies{ NULL };

private:
	SoundManager m_SM;
	uint m_numZombies{ 0 };
	uint m_numZombiesAlive{ 0 };
};

