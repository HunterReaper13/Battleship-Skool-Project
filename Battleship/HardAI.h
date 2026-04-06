#pragma once
#include "EnemyAI.h"
class HardAI : public EnemyAI
{
private:
	int score = 0;
	bool shipRotationStatus[5] = { 0 }; // Vertical
	int checkShip = 5;
	ScreenMatrix weightMap = ScreenMatrix();
public:
	HardAI() : EnemyAI(3) {};
	/// <summary>
	/// Shoots a shell deciding whether to shoot a targeting shot or a guessing shot using the AI's current targeting state
	/// </summary>
	void ShootShell() override
	{
		if (targetingState == None)
		{
			ShootOnWeightMap();
		}
		else
		{
			if (TargetingShot())
				ShootOnWeightMap();
			else
				UpdateWeightMap();
		}
	}
	/// <summary>
	/// Shoots a "random" shot using a weight map to decide the best possible guess position
	/// </summary>
	void ShootOnWeightMap()
	{
		int highestWeight = 0;
		int bestPositionX = 0;
		int bestPositionY = 0;
		for (int x = 0; x < 10; x++)
		{
			for (int y = 0; y < 10; y++) // Checks grid for the highest weight
			{
				if (weightMap.data[x][y] > highestWeight && IsValidShot(x, y))
				{
					highestWeight = weightMap.data[x][y];
					bestPositionX = x;
					bestPositionY = y;
				}
			}
		}

		if (IsPlayerPresent(bestPositionX, bestPositionY)) // Fires shot
		{
			targetingState = Searching;
			firstHitShotX = bestPositionX;
			firstHitShotY = bestPositionY;
			AnnounceHit(true);
		}
		else
		{
			AnnounceHit(false);
		}

		UpdateWeightMap(); // Changes weight map to adjust based on misses and/or hits
		system("cls");
		Renderer::GenerateGrid();
	}
	/// <summary>
	/// Updates the weight map by going through each position on the grid and adjusting the weight for that tile by calculating if a ship can be placed there
	/// </summary>
	void UpdateWeightMap()
	{
		weightMap = ScreenMatrix(); // Resets weight map
		bool extraThreeShip = false;
		for (int x = 0; x < 10; x++)
		{
			for (int y = 0; y < 10; y++)
			{
				for (int eachShip = 0; eachShip < 5; eachShip++)
				{
					if (eachShip > 0)
					{
						MapPossibleShipPositions(x, y, eachShip);

						if (eachShip == 2)
						{
							MapPossibleShipPositions(x, y, eachShip);
						}
					}
				}
			}
		}
	}
	/// <summary>
	/// Maps each possible ship placement from a coordinate accounting for all directions, hits and misses currently on the grid (https://cliambrown.com/battleship/methodology.php) <- Reference
	/// </summary>
	/// <param name="x"></param>
	/// <param name="y"></param>
	/// <param name="eachShip">The current ship being checked</param>
	void MapPossibleShipPositions(int x, int y, int eachShip)
	{
		for (int ship = 0; ship <= eachShip; ship++)
		{
			for (int direction = 0; direction < 4; direction++)
			{
				switch (direction)
				{
				case 0:
					if (IsValidShot(x + 1, y) && eachShip > 0)
					{
						if (IsValidShot(x + ship, y))
						{
							weightMap.data[x + ship][y] += 1;
						}
					}
				case 1:
					if (IsValidShot(x - 1, y) && eachShip > 0)
					{
						if (IsValidShot(x - ship, y))
						{
							weightMap.data[x - ship][y] += 1;
						}
					}
				case 2:
					if (IsValidShot(x, y + 1) && eachShip > 0)
					{
						if (IsValidShot(x, y + ship))
						{
							weightMap.data[x][y + ship] += 1;
						}
					}
				case 3:
					if (IsValidShot(x, y - 1) && eachShip > 0)
					{
						if (IsValidShot(x, y - ship))
						{
							weightMap.data[x][y - ship] += 1;
						}
					}
					break;
				}
			}
		}
	}
	/// <summary>
	/// Places all of the AI's ships ensuring they're valid and/or intelligent choices using score to decide
	/// </summary>
	/// <param name="grid"></param>
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

				if (!IsValidPlacement(randomX, randomY - i, 3)) // Checks surrounding ship placements
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
				//ScoreCheck(grid); // Checks the placement score
				if (score < 15) // Finishes placement
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
		UpdateWeightMap();
	}

	/// <summary>
	/// Checks to see if the ships placed are intelligent choices
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

