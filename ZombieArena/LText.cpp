#include "LText.h"

LText::LText(string aFontFile, sf::Color aColor, unsigned int aSize, float aPositionX, float aPositionY, string aString)
{
	m_Font.loadFromFile(aFontFile);
	setFont(m_Font);
	setCharacterSize(aSize);
	setFillColor(aColor);
	setPosition(aPositionX, aPositionY);
	setString(aString);
}
