#pragma once
#include <iostream>
#include <windows.h>
#include "EnemyAI.h"
#include "ScreenMatrix.h"
#include "Controller.h"
#include "Renderer.h"
/// <summary>
/// The easiest AI in the game with basic strategy and ship placement
/// </summary>
class EasyAI : public EnemyAI
{
public:
	EasyAI() : EnemyAI(1) {};
	/// <summary>
	/// Shoots at a random position on the players board unless the AI is targeting in which it will shoot one by one through the players ship until the AI identifies it's been sunk
	/// </summary>
	void ShootShell() override
	{
		Renderer::Out("  The AI Is Thinking");
		Sleep(500); // Waits to place ship to make player believe the AI is "Thinking"
		if (targetingState != None)
		{
			TargetingShot();
		}
		else
			PickRandomPosition();
		system("cls");
		Renderer::GenerateGrid();
	}

	/// <summary>
	/// Picks a random location to shoot at
	/// </summary>
	void PickRandomPosition()
	{
		while (true) // Repeats until shot is valid
		{
			int randomX = GenerateRandomInt(0, 9);
			int randomY = GenerateRandomInt(0, 9);

			if (IsValidShot(randomX, randomY)) // If shot is valid it will continue
			{
				if (IsPlayerPresent(randomX, randomY))
				{
					targetingState = Searching;
					firstHitShotX = randomX;
					firstHitShotY = randomY;
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
		}
	}
	/// <summary>
	/// Places ships randomly ensuring they are not outside of bounds
	/// </summary>
	/// <param name="grid">The grid to place the ships on</param>
	void PlaceShips(int grid) override
	{
		Renderer::Out("  The AI Is Thinking");
		Sleep(1000); // Waits to place ship to make player believe the AI is "Thinking"
		bool allShipsInvalid = true;

		while (allShipsInvalid) // Continues until the placements are valid
		{
			if (timesTried > 100000)
				ResetGrid();

			bool failed = false;
			int randomX = GenerateRandomInt(0, 9);
			int randomY = GenerateRandomInt(0, 9);
			int direction = GenerateRandomInt(0, 3);

			for (int i = 0; i < shipLength + 1; i++) // Repeats for each ship
			{
				PlaceRandomShipPoints(direction, failed, randomX, randomY, i);

				if (i == shipLength && !failed) // Checks if ship has failed and if the ship is at ship length
				{
					StoreShip(direction, randomX, randomY);
				}
				else if (failed)
					break; // Exits the loop
			}

			NextShip(failed);

			if (shipLength == 5) // Finishes placement if shiplength is equal to 5
			{
				shipLength = 4;
				allShipsInvalid = false;
			}
		}
		FinishPlacements(grid);
	}

};

