#pragma once

#include "LSound.h"
#include "Bullet.h"
#include "Zombie.h"

typedef unsigned int uint;

constexpr uint CLIP_SIZE = 6;
constexpr uint START_BULLETS = 24;
constexpr uint BULLET_SPRITES = 100;
constexpr uint FIRE_RATE = 1000;
constexpr uint RELOAD_RATE = 1000;
constexpr float DAMAGE = 1.f;

enum class WeaponType { HANDGUN, SHOTGUN, RIFLE, KNIFE };

class Weapon
{
public:
	Weapon();
	void Reset();
	bool Reload();
	void AddClip(uint aBullets);
	bool Shoot(float startX, float startY, float xTarget, float yTarget);
	void IncreaseFireRate();
	void IncreaseClipSize();
	void update(float aDT);
	uint SpareBullets();
	uint ClipBullets();
	uint collisions(Zombie* apZombie);
	void drawBullets(RenderWindow& window);
	void setType(WeaponType aType);
	WeaponType getType();

private:
	void nextBullet();
	bool shootBullet(float startX, float startY, float xTarget, float yTarget, float aDeviation);

	uint m_spareBullets{ START_BULLETS };
	uint m_clipBullets{ CLIP_SIZE };
	uint m_clipSize{ CLIP_SIZE };
	uint m_fireRate{ FIRE_RATE };
	LSound* m_shotgun{ NULL };
	LSound* m_handgun{ NULL };
	LSound* m_rifle{ NULL };
	LSound* m_reload{ NULL };
	LSound* m_reloadFailed{ NULL };
	LSound* m_splat{ NULL };
	Time m_time;
	Time m_lastShot;
	Time m_lastReload;
	Clock m_clock;
	Bullet m_bullets[BULLET_SPRITES];
	int m_currentBullet{ 0 };
	WeaponType m_weaponType{ WeaponType::HANDGUN };
	float m_damage{ DAMAGE };
};

