#pragma once
#include <iostream>
#include <windows.h>
#include "EnemyAI.h"
#include "ScreenMatrix.h"
#include "Controller.h"
#include "Renderer.h"
class MediumAI : public EnemyAI
{
private:
	int score = 0;
	bool shipRotationStatus[5] = { 0 }; // Vertical
	int shotsFired = 0;
public:
	MediumAI() : EnemyAI(2) {};
	/// <summary>
	/// Medium AI Shoots a shell, intially targeting the middle squares then targeting randomly but checking to ensure shots aren't too cosez
	/// </summary>
	void ShootShell() override
	{
		Renderer::Out("  The AI Is Thinking");
		Sleep(1500); // Waits to place ship to make player believe the AI is "Thinking"
		if (shotsFired < 5 && targetingState == None)
		{
			int newShotX = 0;
			int newShotY = 0;
			bool doOnce = true;
			while (!IsValidShot(newShotX, newShotY) || doOnce) // Checks middle squares and picks a random one until valid
			{
				if (shotsFired <= 1)
				{
					newShotX = GenerateRandomInt(4, 5);
					newShotY = GenerateRandomInt(4, 5);
				}
				else
				{
					newShotY = GenerateRandomInt(2, 6);
					newShotX = GenerateRandomInt(2, 6);
				}

				doOnce = false;
			}

			if (IsPlayerPresent(newShotX, newShotY)) // Shoots shot
			{
				firstHitShotX = newShotX;
				firstHitShotY = newShotY;
				targetingState = Searching;
				AnnounceHit(true);
			}
			else
			{
				targetingState = None;
				AnnounceHit(false);
			}
			shotsFired++;;
		}
		else if (targetingState != None) // Targeting shots
		{
			if (TargetingShot())
				SmartRandomShot();
		}
		else
			SmartRandomShot();
	}
	/// <summary>
	/// Shoots a shot randomly, checking if the choice is intelligent by checking for nearby misses & hits
	/// </summary>
	void SmartRandomShot() // Shoots a random shot
	{
		int fails = 0;
		while (true) // Repeats until shot is valid
		{
			int randomX = GenerateRandomInt(0, 9);
			int randomY = GenerateRandomInt(0, 9);

			if (IsValidShot(randomX, randomY) && IsIntelligentChoice(randomX, randomY,  (100 - fails) > 0 ? (100 - fails) : 1) || IsValidShot(randomX, randomY) && fails > 100) // If shot is valid it will continue
			{
				if (IsPlayerPresent(randomX, randomY)) // Shoots shot
				{
					firstHitShotX = randomX;
					firstHitShotY = randomY;
					targetingState = Searching;
					AnnounceHit(true);
				}
				else
				{
					targetingState = None;
					AnnounceHit(false);
				}

				startingGrid.data[randomX][randomY] = Controller::GetGridPosition(0, randomX, randomY); // Changes grid data
				break;
			}
			else
				fails++;
		}
	}
	// Picks an intelligent shot by getting the x and y coordinates and drawing a circle aroudn the coord and seeing if there is any other shots made in that region
	bool IsIntelligentChoice(int xCoord, int yCoord, int r)
	{
		for (int x = -r; x <= r; x++) // Simple equation for a circle
		{
			for (int y = -r; y <= r; y++)
			{
				if (x * x + y * y <= r * r)
				{
					int circleX = xCoord + x;
					int circleY = yCoord + y;
					if (Controller::GetGridPosition(0, circleX, circleY) == 3 || Controller::GetGridPosition(0, circleX, circleY) == 2)
					{
						return false;
					}
				}
			}
		}
		return true;
	}
	/// <summary>
	/// The Medium AI Picks ship placements
	/// </summary>
	/// <param name="grid">Grid that the enemy is placing on</param>
	void PlaceShips(int grid) override
	{
		Renderer::Out("  The AI Is Thinking");
		Sleep(1000); // Waits to place ship to make player believe the AI is "Thinking"
		bool allShipsInvalid = true;
		while (allShipsInvalid)
		{
			if (timesTried > 100000)
			{
				ResetGrid();
				score = 0;

				for (int i = 0; i < sizeof(shipRotationStatus) / sizeof(shipRotationStatus[0]); i++)
					shipRotationStatus[i] = false;
			}


			bool failed = false;
			int randomX = GenerateRandomInt(0, 9); // Random Coordinate
			int randomY = GenerateRandomInt(0, 9);
			int direction = GenerateRandomInt(0, 3); // Random direction

			for (int i = 0; i < shipLength + 1; i++) // Goes through each ship unit
			{
				PlaceRandomShipPoints(direction, failed, randomX, randomY, i);

				if (direction < 2) // Sets the rotation status of each ship
					shipRotationStatus[i] = false;
				else
					shipRotationStatus[i] = true;

				if (!IsValidPlacement(randomX, randomY - i, 2))
					score++;

				if (i == shipLength && !failed) // Locks in each ship
				{
					StoreShip(direction, randomX, randomY);
				}
				else if (failed)
					break;
			}

			NextShip(failed);



			if (shipLength == 5)
			{
				ScoreCheck(grid); // Checks the placement score
				if (score < 10) // Finishes placement
				{
					shipLength = 4;
					allShipsInvalid = false;
				}
				else // Resets all ship placements
				{
					if (Renderer::debugMode)
					{
						Renderer::Out("Failure State: Failed Score Check");
						Sleep(100);
					}
					startingGrid = ScreenMatrix(); // Resets grid
					tempStartingGrid = ScreenMatrix(); // Resets grid
					shipLength = 1;
					isExtraThirdShip = false;
					score = 0;

					for (int i = 0; i < sizeof(shipRotationStatus) / sizeof(shipRotationStatus[0]); i++)
						shipRotationStatus[i] = false;
				}
			}
			if (Renderer::debugMode)
			{
				Renderer::Out(timesTried);
				Renderer::Out(score);
				Sleep(1);
				system("cls");
				Renderer::GenerateGrid();
			}
		}
		FinishPlacements(grid);
	}

	/// <summary>
	/// Checks to see if the ships placed are
	/// </summary>
	/// <param name="grid"></param>
	void ScoreCheck(int grid)
	{
		for (int y = 0; y < 10; y++)
			for (int x = 0; x < 10; x++)
				if (x > 3 && x < 6 && y > 3 && y < 6)
				{
					if (startingGrid.data[x][y] == 5)
						score++;
				}
		int shipsRotated = 0;
		for (int i = 0; i < sizeof(shipRotationStatus) / sizeof(shipRotationStatus[0]); i++)
		{
			if (shipRotationStatus[i] == true)
				shipsRotated++;
		}
		if (shipsRotated < 2)
			score++;
	}
};

