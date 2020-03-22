#pragma once

#include "LSound.h"
#include <list>

using namespace std;

typedef list<LSound*>::iterator itSound;

class SoundManager :
	public list<LSound*>
{
public:
	SoundManager() {};
	void play(float aDT);
	void pushback(string aFile);

private:
	float m_Time{ 0.f };
};

