// ZombieArena.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <sstream>
#include <fstream>
#include "ZombieArena.h"
#include "Player.h"
#include "TextureHolder.h"
#include "Bullet.h"
#include "Pickup.h"

int main()
{
	// Here is the instabce of TextureHolder
	TextureHolder holder;

	Joystick js;
	unsigned int uijsButtons = 0;

	// The game will always be in one of four states
	enum class State { PAUSED, LEVELING_UP, GAME_OVER, PLAYING };
	// Start with the GAME_OVER state
	State state = State::GAME_OVER;

	// Get the screen resolution and create an SFML window
	Vector2f resolution;
	resolution.x = (float) VideoMode::getDesktopMode().width;
	resolution.y = (float) VideoMode::getDesktopMode().height;

	RenderWindow window(VideoMode((unsigned int) resolution.x, (unsigned int) resolution.y), "Zombie Arena", Style::Default);

	// Create a an SFML View for the main action
	View mainView(sf::FloatRect(0, 0, resolution.x, resolution.y));

	// Here is our clock for timing everything
	Clock clock;
	// How long has the PLAYING state been active
	Time gameTimeTotal;

	// Where is the mouse in relation to world coordinates
	Vector2f mouseWorldPosition;
	// Where is the mouse in relation to screen coordinates
	Vector2i mouseScreenPosition;

	// Create an instance of the Player class
	Player player;

	// The boundaries of the arena
	IntRect arena;

	// Create the background
	VertexArray background;
	// Load the texture for our background vertex array
	Texture textureBackground = TextureHolder::GetTexture("graphics/background_sheet.png");

	// Prepare for a horde of zombies
	int numZombies;
	int numZombiesAlive;
	Zombie* zombies = NULL;

	// 100 bullets should do
	Bullet bullets[100];
	int currentBullet = 0;
	int bulletsSpare = 24;
	int bulletsInClip = 6;
	int clipSize = 6;
	float fireRate = 1;
	// When was the fire button last pressed?
	Time leftLastPressed;
	Time rightLastPressed;

	// Hide the mouse pointer and replace it with crosshair
	window.setMouseCursorVisible(true);
	Sprite spriteCrosshair;
	Texture textureCrosshair = TextureHolder::GetTexture("graphics/crosshair.png");
	spriteCrosshair.setTexture(textureCrosshair);
	spriteCrosshair.setOrigin(25, 25);

	// Create a couple of pickups
	Pickup healthPickup(1);
	Pickup ammoPickup(2);

	// About the game
	int score = 0;
	int hiScore = 0;

	// For the home/game over screen
	Sprite spriteGameOver;
	Texture textureGameOver = TextureHolder::GetTexture("graphics/background.png");
	spriteGameOver.setTexture(textureGameOver);
	spriteGameOver.setPosition(0, 0);

	// Create a view for the HUD
	View hudView(sf::FloatRect(0, 0, resolution.x, resolution.y));

	// Create a sprite for the ammo icon
	Sprite spriteAmmoIcon;
	Texture textureAmmoIcon = TextureHolder::GetTexture("graphics/ammo_icon.png");
	spriteAmmoIcon.setTexture(textureAmmoIcon);
	spriteAmmoIcon.setPosition(20, 980);

	// Load the font
	string strFontFile = "fonts/zombiecontrol.ttf";

	LText pausedText(strFontFile, Color::White, 155, 400, 400, "Press Enter \nto continue"); // Paused
	LText gameOverText(strFontFile, Color::White, 125, 250, 650, "Play <ENTER>\nEsc <exit>"); // Paused
	LText levelUpText(strFontFile, Color::White, 80, 150, 250, "1- Increased rate of fire\n2- Increased clip size\n3- Increased max health\n4- Increased run speed\n5- More and better health pickups\n6- More and better ammo pickups"); // Levelling up
	LText ammoText(strFontFile, Color::White, 55, 200, 980, "Press Enter \nto continue"); // Ammo
	LText scoreText(strFontFile, Color::White, 55, 20, 0, "Press Enter \nto continue"); // Score

	// Load the high score from a text file/
	std::ifstream inputFile("gamedata/scores.txt");
	if (inputFile.is_open())
	{
		inputFile >> hiScore;
		inputFile.close();
	}

	std::stringstream s;
	s << "Hi Score:" << hiScore;
	LText hiScoreText(strFontFile, Color::White, 55, 1400, 0, s.str()); // Hi Score
	LText zombiesRemainingText(strFontFile, Color::White, 55, 1500, 980, "Zombies: 100"); // Zombies remaining

	int wave = 0;
	LText waveNumberText(strFontFile, Color::White, 55, 1250, 980, "Wave: 0"); // Wave number

	// Health bar
	RectangleShape healthBar;
	healthBar.setFillColor(Color::Red);
	healthBar.setPosition(450, 980);

	// When did we last update the HUD?
	int framesSinceLastHUDUpdate = 0;
	// What time was the last update
	Time timeSinceLastUpdate;
	// How often (in frames) should we update the HUD
	int fpsMeasurementFrameInterval = 100;

	LSound hit("sound/hit.wav"); // hit sound
	LSound footsteps("sound/footsteps.wav"); // footsteps sound
	LSound splat("sound/splat.wav"); // Prepare the splat sound
	LSound shoot("sound/shoot.wav"); // Prepare the shoot sound
	LSound reload("sound/reload.wav"); // Prepare the reload sound
	LSound reloadFailed("sound/reload_failed.wav"); // Prepare the failed sound
	LSound powerup("sound/powerup.wav"); // Prepare the powerup sound
	LSound pickup("sound/pickup.wav"); // Prepare the pickup sound
	LSound relief("sound/relief.wav"); // relief sound when getting extra health

	SoundManager smZombie;
	smZombie.pushback("sound/zombie_attack.wav");
	smZombie.pushback("sound/zombie_short.wav");
	smZombie.pushback("sound/zombie_short_low.wav");
	smZombie.pushback("sound/zombie_slow.wav");

	// The main game loop
	while (window.isOpen())
	{
		// Update the delta time
		Time dt = clock.restart();
		// Update the total game time
		gameTimeTotal += dt;

		// Handle events
		Event event;
		while (window.pollEvent(event))
		{
			if (state == State::PAUSED)
			{
				switch (event.type)
				{
				case Event::KeyPressed:
					if (event.key.code == Keyboard::Return) // Restart while paused
					{
						state = State::PLAYING;
						clock.restart(); // Reset the clock so there isn't a frame jump
					}
					break;
				}
			}
			else if (state == State::GAME_OVER)
			{
				switch (event.type)
				{
				case Event::KeyPressed:
					if (event.key.code == Keyboard::Return) // Start a new game
					{
						state = State::LEVELING_UP;
						wave = 0;
						score = 0;

						// Prepare the gun and ammo for next game
						currentBullet = 0;
						bulletsSpare = 24;
						bulletsInClip = 6;
						clipSize = 6;
						fireRate = 1;

						player.resetPlayerStats(); // Reset the player's stats
					}
					break;
				}
			}
			else if (state == State::LEVELING_UP) // Handle the levelling up state
			{
				if (event.type == Event::KeyPressed)
				{
					switch (event.key.code) // Handle the player levelling up
					{
					case Keyboard::Num1:
						fireRate++;
						state = State::PLAYING;
						break;
					case Keyboard::Num2:
						clipSize += clipSize;
						state = State::PLAYING;
						break;
					case Keyboard::Num3:
						player.upgradeHealth();
						state = State::PLAYING;
						break;
					case Keyboard::Num4:
						player.upgradeSpeed();
						state = State::PLAYING;
						break;
					case Keyboard::Num5:
						healthPickup.upgrade();
						state = State::PLAYING;
						break;
					case Keyboard::Num6:
						ammoPickup.upgrade();
						state = State::PLAYING;
						break;
					}
				}

				if (state == State::PLAYING)
				{
					wave++; // Increase the wave number

					// Prepare the level
					arena.width = 500 * wave;
					arena.height = 500 * wave;
					arena.left = 0;
					arena.top = 0;

					// Pass the vertex array by reference to the createBackground function
					int tileSize = createBackground(background, arena);

					player.spawn(arena, resolution, tileSize); // Spawn the player in the middle of the arena

					// Configure the pick-ups
					healthPickup.setArena(arena);
					ammoPickup.setArena(arena);

					numZombies = 5 * wave; // Create a horde of zombies

					delete[] zombies; // Delete the previously allocated memory (if it exists)
					zombies = createHorde(numZombies, arena);
					numZombiesAlive = numZombies;

					powerup.play(); // Play the powerup sound

					clock.restart(); // Reset the clock so there isn't a frame jump
				}
			}
			else if (state == State::PLAYING)
			{
				switch (event.type)
				{
				case Event::KeyPressed:
				case Event::KeyReleased:
					if (event.key.code == Keyboard::Return) // pause game
					{
						state = State::PAUSED;
						footsteps.stop();
					}
					else // move player with keyboard
					{
						player.move(event.key.code, (event.type == Event::KeyPressed)?true:false);
					}
					break;
				case Event::MouseButtonPressed:
				case Event::MouseButtonReleased:
					if (Mouse::isButtonPressed(sf::Mouse::Left)) // Fire a bullet
					{
						if (gameTimeTotal.asMilliseconds() - leftLastPressed.asMilliseconds() > 1000 / fireRate && bulletsInClip > 0)
						{
							bullets[currentBullet].shoot(player.getCenter().x, player.getCenter().y, mouseWorldPosition.x, mouseWorldPosition.y);

							currentBullet++;
							if (currentBullet > 99) currentBullet = 0;

							leftLastPressed = gameTimeTotal;
							shoot.play();
							bulletsInClip--;
						}
					}
					else if (Mouse::isButtonPressed(sf::Mouse::Right))
					{
						if (gameTimeTotal.asMilliseconds() - rightLastPressed.asMilliseconds() > 1000)
						{
							if (bulletsInClip < clipSize)
							{
								bool bReloaded = false;
								int iToReload = clipSize - bulletsInClip;

								if (iToReload <= bulletsSpare)
								{
									bReloaded = true;
									bulletsInClip += iToReload;
									bulletsSpare -= iToReload;
								}
								else
								{
									bReloaded = true;
									bulletsInClip += bulletsSpare;
									bulletsSpare = 0;
								}

								if (bReloaded)
								{
									rightLastPressed = gameTimeTotal;
									reload.play();
								}
							}
						}
						else
							reloadFailed.play();
					}
					break;
				case Event::JoystickConnected:
					js.update();
					uijsButtons = js.getButtonCount(0);
					break;
				case Event::JoystickDisconnected:
					uijsButtons = 0;
					break;
				case Event::JoystickMoved:
					if (event.joystickMove.axis == sf::Joystick::Axis::PovX)
					{
						player.moveX(event.joystickMove.position);
					}
					else if (event.joystickMove.axis == sf::Joystick::Axis::PovY)
					{
						player.moveY(event.joystickMove.position);
					}
					break;
				case Event::JoystickButtonPressed:
					for (unsigned int jsb = 0; jsb < uijsButtons; jsb++)
					{
						if (js.isButtonPressed(0, jsb))
							cout << "button: " << jsb << "\n";
					}
					break;
				}
			}
		}// End event polling

		 // Handle the player quitting
		if (Keyboard::isKeyPressed(Keyboard::Escape)) window.close();

		if (player.isMoving())
		{
			if (footsteps.getStatus() != Sound::Playing) footsteps.play();
		}
		else
		{
			footsteps.stop();
		}

		 // UPDATE THE FRAME
		if (state == State::PLAYING)
		{
			// Make a decimal fraction of 1 from the delta time
			float dtAsSeconds = dt.asSeconds();

			smZombie.play(dtAsSeconds);

			// Where is the mouse pointer
			mouseScreenPosition = Mouse::getPosition();

			// Convert mouse position to world coordinates of mainView
			mouseWorldPosition = window.mapPixelToCoords(Mouse::getPosition(), mainView);

			// Set the crosshair to the mouse world location
			spriteCrosshair.setPosition(mouseWorldPosition);

			// Update the player
			player.update(dtAsSeconds, Mouse::getPosition());

			// Make a note of the players new position
			Vector2f playerPosition(player.getCenter());

			// Make the view centre around the player				
			mainView.setCenter(player.getCenter());

			// Loop through each Zombie and update them
			for (int i = 0; i < numZombies; i++)
			{
				if (zombies[i].isAlive()) zombies[i].update(dt.asSeconds(), playerPosition);
			}

			// Update any bullets that are in-flight
			for (int i = 0; i < 100; i++)
			{
				if (bullets[i].isInFlight()) bullets[i].update(dtAsSeconds);
			}

			// Update the pickups
			healthPickup.update(dtAsSeconds);
			ammoPickup.update(dtAsSeconds);

			// Collision detection
			// Have any zombies been shot?
			for (int i = 0; i < 100; i++)
			{
				for (int j = 0; j < numZombies; j++)
				{
					if (bullets[i].isInFlight() && zombies[j].isAlive())
					{
						if (bullets[i].getPosition().intersects(zombies[j].getPosition()))
						{
							// Stop the bullet
							bullets[i].stop();

							// Register the hit and see if it was a kill
							if (zombies[j].hit())
							{
								// Not just a hit but a kill too
								score += 10;
								if (score >= hiScore)
								{
									hiScore = score;
								}

								numZombiesAlive--;

								// When all the zombies are dead (again)
								if (numZombiesAlive == 0)
								{
									state = State::LEVELING_UP;
								}
							}

							// Make a splat sound
							splat.play();
						}
					}
				}
			}// End zombie being shot

			// Have any zombies touched the player			
			for (int i = 0; i < numZombies; i++)
			{
				if (player.getPosition().intersects(zombies[i].getPosition()) && zombies[i].isAlive())
				{
					//if (player.hit(gameTimeTotal)) hit.play();

					if (player.getHealth() <= 0)
					{
						state = State::GAME_OVER;

						std::ofstream outputFile("gamedata/scores.txt");
						outputFile << hiScore;
						outputFile.close();

						footsteps.stop();
					}
				}
			}// End player touched

			// Has the player touched health pickup
			if (player.getPosition().intersects(healthPickup.getPosition()) && healthPickup.isSpawned())
			{
				player.increaseHealthLevel(healthPickup.gotIt());
				relief.play(); // Play a sound
			}

			// Has the player touched ammo pickup
			if (player.getPosition().intersects(ammoPickup.getPosition()) && ammoPickup.isSpawned())
			{
				bulletsSpare += ammoPickup.gotIt();
				reload.play(); // Play a sound
			}

			healthBar.setSize(Vector2f((float) player.getHealth() * 3, 70.f)); // size up the health bar
			
			timeSinceLastUpdate += dt; // Increment the amount of time since the last HUD update
			framesSinceLastHUDUpdate++; // Increment the number of frames since the last HUD calculation

			// Calculate FPS every fpsMeasurementFrameInterval frames
			if (framesSinceLastHUDUpdate > fpsMeasurementFrameInterval)
			{
				// Update game HUD text
				stringstream ssAmmo;
				stringstream ssScore;
				stringstream ssHiScore;
				stringstream ssWave;
				stringstream ssZombiesAlive;

				// Update the ammo text
				ssAmmo << bulletsInClip << "/" << bulletsSpare;
				ammoText.setString(ssAmmo.str());

				// Update the score text
				ssScore << "Score:" << score;
				scoreText.setString(ssScore.str());

				// Update the high score text
				ssHiScore << "Hi Score:" << hiScore;
				hiScoreText.setString(ssHiScore.str());

				// Update the wave
				ssWave << "Wave:" << wave;
				waveNumberText.setString(ssWave.str());

				// Update the high score text
				ssZombiesAlive << "Zombies:" << numZombiesAlive;
				zombiesRemainingText.setString(ssZombiesAlive.str());

				framesSinceLastHUDUpdate = 0;
				timeSinceLastUpdate = Time::Zero;
			}// End HUD update
		}// End updating the scene

		 // DRAW THE SCENE
		if (state == State::PLAYING)
		{
			window.clear();

			// set the mainView to be displayed in the window and draw everything related to it
			window.setView(mainView);

			// Draw the background
			window.draw(background, &textureBackground);

			// Draw the zombies
			for (int i = 0; i < numZombies; i++) window.draw(zombies[i].getSprite());

			for (int i = 0; i < 100; i++)
			{
				if (bullets[i].isInFlight()) window.draw(bullets[i].getShape());
			}

			// Draw the player
			window.draw(player.getSprite());

			// Draw the pickups is currently spawned
			if (ammoPickup.isSpawned()) window.draw(ammoPickup.getSprite());
			
			if (healthPickup.isSpawned()) window.draw(healthPickup.getSprite());

			//Draw the crosshair
			window.draw(spriteCrosshair);

			// Switch to the HUD view
			window.setView(hudView);

			// Draw all the HUD elements
			window.draw(spriteAmmoIcon);
			window.draw(ammoText);
			window.draw(scoreText);
			window.draw(hiScoreText);
			window.draw(healthBar);
			window.draw(waveNumberText);
			window.draw(zombiesRemainingText);
		}

		if (state == State::LEVELING_UP)
		{
			window.draw(spriteGameOver);
			window.draw(levelUpText);
		}

		if (state == State::PAUSED)
		{
			window.draw(pausedText);
		}

		if (state == State::GAME_OVER)
		{
			window.draw(spriteGameOver);
			window.draw(gameOverText);
			window.draw(scoreText);
			window.draw(hiScoreText);
		}

		window.display();

	}// End game loop

	return 0;
}

int createBackground(VertexArray& rVA, IntRect arena)
{
	// How big is each tile/texture
	const int TILE_TYPES = 3;
	const int VERTS_IN_QUAD = 4;

	int worldWidth = arena.width / TILE_SIZE;
	int worldHeight = arena.height / TILE_SIZE;

	// What type of primitive are we using?
	rVA.setPrimitiveType(Quads);

	// Set the size of the vertex array
	rVA.resize(worldWidth * worldHeight * VERTS_IN_QUAD);

	// Start at the beginning of the vertex array
	int currentVertex = 0;

	for (int w = 0; w < worldWidth; w++)
	{
		for (int h = 0; h < worldHeight; h++)
		{
			// Position each vertex in the current quad
			rVA[currentVertex + 0].position = Vector2f((float) (w * TILE_SIZE), (float) h * TILE_SIZE);
			rVA[currentVertex + 1].position = Vector2f((float) (w * TILE_SIZE) + TILE_SIZE, (float) h * TILE_SIZE);
			rVA[currentVertex + 2].position = Vector2f((float) (w * TILE_SIZE) + TILE_SIZE, (float) (h * TILE_SIZE) + TILE_SIZE);
			rVA[currentVertex + 3].position = Vector2f((float) (w * TILE_SIZE), (float) (h * TILE_SIZE) + TILE_SIZE);

			// Define the position in the Texture to draw for current quad
			// Either mud, stone, grass or wall
			if (h == 0 || h == worldHeight - 1 || w == 0 || w == worldWidth - 1)
			{
				// Use the wall texture
				rVA[currentVertex + 0].texCoords = Vector2f(0, 0 + TILE_TYPES * TILE_SIZE);
				rVA[currentVertex + 1].texCoords = Vector2f(TILE_SIZE, 0 + TILE_TYPES * TILE_SIZE);
				rVA[currentVertex + 2].texCoords = Vector2f(TILE_SIZE, TILE_SIZE + TILE_TYPES * TILE_SIZE);
				rVA[currentVertex + 3].texCoords = Vector2f(0, TILE_SIZE + TILE_TYPES * TILE_SIZE);
			}
			else
			{
				// Use a random floor texture
				srand((int)time(0) + h * w - h);
				int mOrG = (rand() % TILE_TYPES);
				int verticalOffset = mOrG * TILE_SIZE;

				rVA[currentVertex + 0].texCoords = Vector2f((float) 0, (float) 0 + verticalOffset);
				rVA[currentVertex + 1].texCoords = Vector2f((float) TILE_SIZE, (float) 0 + verticalOffset);
				rVA[currentVertex + 2].texCoords = Vector2f((float) TILE_SIZE, (float) TILE_SIZE + verticalOffset);
				rVA[currentVertex + 3].texCoords = Vector2f((float) 0, (float) TILE_SIZE + verticalOffset);
			}

			// Position ready for the next for vertices
			currentVertex = currentVertex + VERTS_IN_QUAD;
		}
	}

	return TILE_SIZE;
}

Zombie* createHorde(int numZombies, IntRect arena)
{
	Zombie* zombies = new Zombie[numZombies];

	int maxY = arena.height - 20;
	int minY = arena.top + 20;
	int maxX = arena.width - 20;
	int minX = arena.left + 20;

	for (int i = 0; i < numZombies; i++)
	{
		// Which side should the zombie spawn
		srand((int)time(0) * i);
		int side = (rand() % 4);
		float x = 0.f, y = 0.f;

		switch (side)
		{
		case 0:
			// left
			x = (float) minX;
			y = (float) (rand() % maxY) + minY;
			break;

		case 1:
			// right
			x = (float) maxX;
			y = (float) (rand() % maxY) + minY;
			break;

		case 2:
			// top
			x = (float) (rand() % maxX) + minX;
			y = (float) minY;
			break;

		case 3:
			// bottom
			x = (float) (rand() % maxX) + minX;
			y = (float) maxY;
			break;
		}

		// Bloater, crawler or runner
		srand((int)time(0) * i * 2);
		int type = (rand() % 3);

		// Spawn the new zombie into the array
		zombies[i].spawn(x, y, type, i);
	}
	return zombies;
}