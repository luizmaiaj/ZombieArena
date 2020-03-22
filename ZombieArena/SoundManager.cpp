#include "SoundManager.h"

const int MIN_TIME = 3;

void SoundManager::play(float aDT)
{
	m_Time += aDT;

	if (m_Time >= MIN_TIME)
	{
		m_Time = 0.f;

		srand((int) time(0));
		int selected = (rand() % size());
		int i = 0;

		for (itSound it = begin(); it != end(); it++)
		{
			if (i == selected)
			{
				LSound* pSound = *it;

				pSound->play();

				break;
			}

			i++;
		}
	}
}

void SoundManager::pushback(string aFile)
{
	LSound* pZ = new LSound(aFile); // zombie sound
	push_back(pZ);
}
