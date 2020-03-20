#pragma once

#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>

using namespace sf;
using namespace std;

class LSound :
	public Sound
{
public:
	LSound() {};
	LSound(string aSoundFile);

private:
	SoundBuffer m_SBuffer;
};

