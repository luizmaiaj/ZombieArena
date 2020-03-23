#pragma once

#include "LSound.h"

constexpr auto CLIP_SIZE = 6; //
constexpr auto START_BULLETS = 24;
constexpr auto BULLET_SPRITES = 100;
constexpr auto FIRE_RATE = 1.0f;

typedef unsigned int uint;

class Weapon
{
public:
	Weapon();
	void Reset();
	void Reload();
	void AddClip(uint aBullets);
	bool Shoot();

private:
	uint m_spareBullets{ START_BULLETS };
	uint m_bulletsInClip{ CLIP_SIZE };
	uint m_clipSize{ CLIP_SIZE };
	float m_fireRate{ FIRE_RATE };
	LSound* m_shoot{ NULL };
	LSound* m_reload{ NULL };
	LSound* m_reloadFailed{ NULL };
};

