#include <iostream>
#include "Renderer.h"
#include "KeyboardControls.h"
#include "Cursor.h"
#include "ASCIIGenerator.h"
#include "EnemyAI.h"
#include "EasyAI.h"
#include "MediumAI.h"
#include "HardAI.h"
#include "Controller.h"
#include "Weapons.h"
#include "Manual.h"

EnemyAI* ai[4];
Cursor mainCursor;
int Input::keys[256]{ 0 };
int Input::old_keys[256]{ 0 };
bool isPlaying = true;
int refreshRate = 75;
int selectedOption = 0;
bool inDebugMode = false;
bool isPickingShipsPositions = true;
bool clearCursorOnce = false;
std::string debugCode = "77HIMp77--CSDMMD650"; // 77 Hunter is mr programmer -- cooler stephen don't mark me down

/// <summary>
/// Generates a random integer
/// </summary>
/// <param name="min">Min inclusive</param>
/// <param name="max">Max inclusive</param>
/// <returns></returns>
int GenerateRandomInt(int min, int max)
{
	std::random_device dev;
	std::mt19937 rng(dev());
	std::uniform_int_distribution<std::mt19937::result_type> randInt(min, max);
	return randInt(rng);
}
/// <summary>
/// Runs during playtime, manages which AI shoots, player shooting and ship picking at the start of the game
/// </summary>
void PlayingGame()
{
	if (isPickingShipsPositions)
	{
		//Allows the player to pick their ship positions
		mainCursor.CursorUpdate(ai[0]);
		if (mainCursor.GetShipLength() >= 5) // Places last ship
		{
			isPickingShipsPositions = false;
		}
	}
	else
	{
		if (!clearCursorOnce) // Generates AI's
		{
			bool doOnce = true;
			if (Controller::GetDifficulty() == 1)
			{
				ai[0] = new EasyAI();
				ai[0]->PlaceShips(1);
				Renderer::SetEnemyFleets(ai[0]->GetName(), 0);
			}
			else if (Controller::GetDifficulty() == 2)
			{
				while (ai[0] == ai[1] || doOnce) // Ensures there aren't two medium difficulty AI's or two easy difficulty AI
				{
					for (int i = 0; i < 2; i++) // Picks a random difficulty from easy to medium
					{
						if (GenerateRandomInt(0, 1) == 0)
							ai[i] = new EasyAI();
						else
							ai[i] = new MediumAI();
						ai[i]->PlaceShips(i + 1);
						Renderer::SetEnemyFleets(ai[i]->GetName(), i);
						doOnce = false;
					}
				}
			}
			else if (Controller::GetDifficulty() == 3)
			{
				while (ai[0] == ai[1] && ai[0] == ai[2] && ai[1] == ai[2] || doOnce) // Ensures all the ai's aren't the same
				{
					for (int i = 0; i < 4; i++) // Picks a random difficulty for each ai from easy to hard
					{
						int randInt = GenerateRandomInt(0, 2);
						if (randInt == 0)
							ai[i] = new EasyAI();
						else if (randInt == 1)
							ai[i] = new MediumAI();
						else
							ai[i] = new HardAI();
						ai[i]->PlaceShips(i + 1);
						Renderer::SetEnemyFleets(ai[i]->GetName(), i);
						doOnce = false;
					}
				}				
			}
			mainCursor.CursorClear(true, 1); // Swaps to targeting
			clearCursorOnce = true;
		}
		// Allows player to shoot their shot then each ai will shoot back one after another
		mainCursor.CursorUpdate(ai[Controller::GetNextEnemyFiring()]);
	}
}

/// <summary>
/// Runs the main menu for the game allowing the player to scroll through each option
/// </summary>
/// <param name="isInMainMenu"></param>
void MainMenu(bool& isInMainMenu)
{
	bool doOnce = true;
	selectedOption = 0; // Resets selected option
	while (isInMainMenu)
	{
		if (doOnce) // Prints ascii pictures and the options
		{
			system("cls");
			ASCIIGenerator::PrintGameTitle();
			Renderer::Out("                                                                                  Press Your Arrow Keys To Move And Enter To Select");
			Renderer::Out("");
			Renderer::Out("                                                                                                  > Begin Game");
			Renderer::Out("                                                                                                      Manual");
			Renderer::Out("                                                                                                       Quit");
			doOnce = false;
		}
		int tempOption = 0;
		tempOption = selectedOption;
		if (Input::MenuScroller(3, selectedOption) == -1) // Exits main menu
		{
			isInMainMenu = false;
		}
		else if (selectedOption != tempOption) // Changes the arrow placements depending on option inputted
		{
			system("cls");
			ASCIIGenerator::PrintGameTitle();
			Renderer::Out("                                                                                  Press Your Arrow Keys To Move And Enter To Select");
			Renderer::Out("");
			if (selectedOption == 0)
				Renderer::Out("                                                                                                  > Begin Game");
			else
				Renderer::Out("                                                                                                    Begin Game");

			if (selectedOption == 1)
				Renderer::Out("                                                                                                  >   Manual");
			else
				Renderer::Out("                                                                                                      Manual");
			if (selectedOption == 2)
				Renderer::Out("                                                                                                  >    Quit");
			else
				Renderer::Out("                                                                                                       Quit");
		}
		Sleep(refreshRate);
	}
	system("cls");
	Input::SwapKeyBuffer();
}

/// <summary>
/// Runs when the game ends and either you or the enemy winds
/// </summary>
void GameEnd()
{
	system("cls");
	while (!Input::Key(PRESSED, ENTER)) // Waits for player to press enter
	{
		ASCIIGenerator::PrintEndScreen();
		Renderer::Out("                                                                                                  > Main Menu", (Controller::GetPlayerHits() >= 17 ? RED : CYAN /*Changes color depending on if player wins or enemy wins*/));
		system("pause >nul");
		system("cls");
	}
	//Resets all of the values for when the player plays again
	Weapons::ResetAllWeapons();
	Controller::ResetEnemyHits();
	Controller::ResetPlayerHits();
	Controller::SetEndGame(false);
	Controller::DeleteGrids(true);
	mainCursor.CursorClear(false, 0);
	for (int i = 0; i < 4; i++)
	{
		Renderer::SetEnemyFleets("", i);
	}
	isPickingShipsPositions = true;
	clearCursorOnce = false;
	Input::SwapKeyBuffer();
	Sleep(refreshRate);
}
/// <summary>
/// Puts the game into full screen
/// </summary>
void FullScreen()
{
	HWND Hwnd = GetForegroundWindow();
	int x = GetSystemMetrics(SM_CXSCREEN);
	int y = GetSystemMetrics(SM_CYSCREEN);
	LONG winstyle = GetWindowLong(Hwnd, GWL_STYLE);
	SetWindowLong(Hwnd, GWL_STYLE, (winstyle | WS_POPUP | WS_MAXIMIZE) & ~WS_CAPTION & ~WS_THICKFRAME & ~WS_BORDER);
	SetWindowPos(Hwnd, HWND_TOP, 0, 0, x, y, 0);
}

int main()
{
	FullScreen(); // Puts game into full screen
	bool isInMainMenu = true;
	bool isInSettingsMenu = false;
	bool doOnce = true;
	while (true) // Waits for player to press enter
	{
		Renderer::Out("Please Ensure The Game Is In Full Screen Before You Begin. Press Enter To Continue");
		if (Input::Key(PRESSEDTHISFRAME, ENTER))
		{
			break;
		}
		system("pause >nul");
		system("cls");
	}
	Input::SwapKeyBuffer();
	MainMenu(isInMainMenu); // Plays main menu
	while (isPlaying) // Game starts
	{
		Sleep(refreshRate);
		if (selectedOption == 0) // Plays game
		{
			while (true) // Wait for game to end
			{
				if (doOnce)
				{
					bool doOnceAgain = true;
					Sleep(refreshRate);
					while (true)
					{
						if (doOnceAgain) // Allows player to change difficulty
						{
							system("cls");
							ASCIIGenerator::PrintGameTitle();
							Renderer::Out("                                                                                                 Pick A Difficulty");
							Renderer::Out("                                                                                                     > Easy");
							Renderer::Out("                                                                                                      Medium");
							Renderer::Out("                                                                                                       Hard");
							doOnceAgain = false;
						}
						int tempOption = 0;
						tempOption = selectedOption;
						if (Input::MenuScroller(3, selectedOption) == -1) // Player picks a difficulty
						{
							Controller::SetDifficulty(selectedOption + 1);
							break;
						}
						else if (selectedOption != tempOption) // Changes option highlighted
						{
							system("cls");
							ASCIIGenerator::PrintGameTitle();
							Renderer::Out("                                                                                                 Pick A Difficulty");
							if (selectedOption == 0)
								Renderer::Out("                                                                                                     > Easy");
							else
								Renderer::Out("                                                                                                       Easy");

							if (selectedOption == 1)
								Renderer::Out("                                                                                                    > Medium");
							else
								Renderer::Out("                                                                                                      Medium");
							if (selectedOption == 2)
								Renderer::Out("                                                                                                     > Hard");
							else
								Renderer::Out("                                                                                                       Hard");
						}
						Sleep(refreshRate);
					}
					Sleep(refreshRate);
					system("cls");
					Controller::CreateNewGrid(Controller::GetDifficulty() + (Controller::GetDifficulty() == 3 ? 1 : 0)); // Creates player and enemy grids
					Renderer::debugMode = inDebugMode; // Puts game into debug mode
					Renderer::GenerateGrid(); // Generates grid
					doOnce = false; // Exits difficulty selector
				}
				PlayingGame(); // Plays game
				Input::SwapKeyBuffer();
				Sleep(refreshRate);

				bool gameHasEnded = Controller::GetEnemyHits() == (17 * Controller::GetDifficulty()) + (Controller::GetDifficulty() == 3 ? 17 : 0) || Controller::GetPlayerHits() == 17 || Controller::GetEndGame();

				// Checks if game has ended
				if (gameHasEnded)
					break;
			}

			if (Controller::GetEndGame()) // Player quits game prematurely
				Controller::IncrementPlayerHits(100);

			GameEnd(); // End game
			isInMainMenu = true;
			MainMenu(isInMainMenu); // Returns to main menu
			doOnce = true;
		}
		else if (selectedOption == 1)
		{
			Manual::OpenManual();
			isInMainMenu = true;
			MainMenu(isInMainMenu); // Returns to main menu

		}
		else if (selectedOption == 2)
		{
			return 0;
		}
	}
}