#pragma once

#include "Zombie.h"

typedef unsigned int uint;

class ZombieManager
{
public:
	Zombie* createHorde(int numZombies, IntRect arena);
	void update(float aDT, Vector2f playerPosition);
	uint alive();
	void kill();
	uint count();

	Zombie* m_zombies{ NULL };

private:
	uint m_numZombies{ 0 };
	uint m_numZombiesAlive{ 0 };
};

