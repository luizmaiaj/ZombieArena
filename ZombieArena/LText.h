#pragma once
#include <SFML/Graphics.hpp>

using namespace sf;
using namespace std;

class LText :
	public Text
{
public:
	LText() {};
	LText(string aFontFile, sf::Color aColor, unsigned int aSize, float aPositionX, float aPositionY, string aString);

private:
	Font m_Font;
};

