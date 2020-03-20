#include "LSound.h"

LSound::LSound(string aSoundFile)
{
	m_SBuffer.loadFromFile(aSoundFile);
	setBuffer(m_SBuffer);
}
