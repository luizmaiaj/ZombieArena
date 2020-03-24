// ZombieArena.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <sstream>
#include <fstream>
#include "ZombieArena.h"

int main()
{
	// Here is the instabce of TextureHolder
	TextureHolder holder;

	Joystick js;
	uint uijsButtons = 0;

	// Start with the GAME_OVER state
	State state = State::GAME_OVER;

	// Get the screen resolution and create an SFML window
	Vector2u resolution;
	resolution.x = VideoMode::getDesktopMode().width;
	resolution.y = VideoMode::getDesktopMode().height;

	RenderWindow window(VideoMode(resolution.x, resolution.y), "Zombie Arena", Style::Default);

	// Create a an SFML View for the main action
	View mainView(sf::FloatRect(0, 0, (float) resolution.x, (float) resolution.y));

	Clock clock; // Here is our clock for timing everything
	Time gameTimeTotal; // How long has the PLAYING state been active

	Vector2f mouseWorldPosition; // Where is the mouse in relation to world coordinates
	Vector2i mouseScreenPosition; // Where is the mouse in relation to screen coordinates

	Player player; // Create an instance of the Player class
	ZombieManager Zombies;

	IntRect arena; // The boundaries of the arena

	VertexArray background; // Create the background
	Texture textureBackground = TextureHolder::GetTexture("graphics/background_sheet.png"); // Load the texture for our background vertex array

	// Update game HUD text
	stringstream ssAmmo;
	stringstream ssScore;
	stringstream ssHiScore;
	stringstream ssWave;
	stringstream ssZombiesAlive;

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
	int score = 0, hiScore = 0;

	// For the home/game over screen
	Sprite spriteGameOver;
	Texture textureGameOver = TextureHolder::GetTexture("graphics/background.png");
	spriteGameOver.setTexture(textureGameOver);
	spriteGameOver.setPosition(0, 0);

	// Create a view for the HUD
	View hudView(sf::FloatRect(0.f, 0.f, (float) resolution.x, (float) resolution.y));

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

	LSound hit("sound/hit.wav"); // hit sound
	LSound footsteps("sound/footsteps.wav"); // footsteps sound
	LSound powerup("sound/powerup.wav"); // Prepare the powerup sound
	LSound pickup("sound/pickup.wav"); // Prepare the pickup sound
	LSound relief("sound/relief.wav"); // relief sound when getting extra health

	// The main game loop
	while (window.isOpen())
	{
		if (Keyboard::isKeyPressed(Keyboard::Escape)) window.close(); // Handle the player quitting

		Time dt = clock.restart(); // Update the delta time
		gameTimeTotal += dt; // Update the total game time

		Event event;
		while (window.pollEvent(event)) // Handle events
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
						player.m_weapon.IncreaseFireRate();
						state = State::PLAYING;
						break;
					case Keyboard::Num2:
						player.m_weapon.IncreaseClipSize();
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
					createBackground(background, arena);

					player.spawn(arena, resolution); // Spawn the player in the middle of the arena

					// Configure the pick-ups
					healthPickup.setArena(arena);
					ammoPickup.setArena(arena);

					Zombies.createHorde(5 * wave, arena);
					
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
					else if (event.key.code == Keyboard::Num1)
					{
						player.setWeapon(WeaponType::HANDGUN);
					}
					else if (event.key.code == Keyboard::Num2)
					{
						player.setWeapon(WeaponType::SHOTGUN);
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
						player.shoot(mouseWorldPosition.x, mouseWorldPosition.y);
					}
					else if (Mouse::isButtonPressed(sf::Mouse::Right))
					{
						player.m_weapon.Reload();
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
					for (uint jsb = 0; jsb < uijsButtons; jsb++)
					{
						if (js.isButtonPressed(0, jsb))
							cout << "button: " << jsb << "\n";
					}
					break;
				}
			}
		}// End event polling

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

			// Loop through each Zombie and update them
			Zombies.update(dtAsSeconds, playerPosition);

			// Make the view centre around the player				
			mainView.setCenter(player.getCenter());

			// Update the pickups
			healthPickup.update(dtAsSeconds);
			ammoPickup.update(dtAsSeconds);

			score += player.collisions(&Zombies);

			if (score >= hiScore) hiScore = score;

			if (player.getHealth() <= 0)
			{
				state = State::GAME_OVER;

				std::ofstream outputFile("gamedata/scores.txt");
				outputFile << hiScore;
				outputFile.close();

				footsteps.stop();
			}

			if (!Zombies.alive()) state = State::LEVELING_UP; // when all zombies die

			if (player.getPosition().intersects(healthPickup.getPosition()) && healthPickup.isSpawned()) // Has the player touched health pickup
			{
				player.increaseHealthLevel(healthPickup.gotIt());
				relief.play(); // Play a sound
			}

			if (player.getPosition().intersects(ammoPickup.getPosition()) && ammoPickup.isSpawned()) // Has the player touched ammo pickup
			{
				player.m_weapon.AddClip(ammoPickup.gotIt());
			}

			healthBar.setSize(Vector2f((float) player.getHealth() * 3, 70.f)); // size up the health bar
			
			timeSinceLastUpdate += dt; // Increment the amount of time since the last HUD update
			framesSinceLastHUDUpdate++; // Increment the number of frames since the last HUD calculation

			// Calculate FPS every fpsMeasurementFrameInterval frames
			if (framesSinceLastHUDUpdate > UPDATE_HUD_INTERVAL)
			{
				framesSinceLastHUDUpdate = 0;
				timeSinceLastUpdate = Time::Zero;

				// Update the ammo text
				ssAmmo.str(std::string());
				ssAmmo << player.m_weapon.ClipBullets() << "/" << player.m_weapon.SpareBullets();
				ammoText.setString(ssAmmo.str());

				// Update the score text
				ssScore.str(std::string());
				ssScore << "Score:" << score;
				scoreText.setString(ssScore.str());

				// Update the high score text
				ssHiScore.str(std::string());
				ssHiScore << "Hi Score:" << hiScore;
				hiScoreText.setString(ssHiScore.str());

				// Update the wave
				ssWave.str(std::string());
				ssWave << "Wave:" << wave;
				waveNumberText.setString(ssWave.str());

				// Update the high score text
				ssZombiesAlive.str(std::string());
				ssZombiesAlive << "Zombies:" << Zombies.alive();
				zombiesRemainingText.setString(ssZombiesAlive.str());
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
			for (uint i = 0; i < Zombies.count(); i++) window.draw(Zombies.m_zombies[i].getSprite());

			player.m_weapon.drawBullets(window);

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

		if (state == State::PAUSED) window.draw(pausedText);

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
