#pragma once
#include "ScreenMatrix.h"
#include "Controller.h"
#include "Renderer.h"
#include <array>
#include <random>
#include <windows.h>
class EnemyAI
{
private:
	/// <summary>
	/// Possible fleet names that can be picked by an AI depending on their difficulty
	/// </summary>
	std::string possibleFleetNames[16] = { "USA", "UK", "Russia", "China", "France", "India", "Japan", "Germany", "Canada", "Indonesia", "South Africa", "Switzerland", "Mongolia", "Kazhakstan", "Afghanistan", "Hungary" };
	/// <summary>
	/// Picks a random ship name depending on the difficulty of the AI
	/// </summary>
	/// <param name="difficulty"></param>
	void ChooseShipName(int difficulty)
	{
		if (difficulty == 3)
		{
			name = possibleFleetNames[GenerateRandomInt(0, 4)];
		}
		else if (difficulty == 2)
		{
			name = possibleFleetNames[GenerateRandomInt(5, 10)];
		}
		else
		{
			name = possibleFleetNames[GenerateRandomInt(10, 15)];
		}
	}
protected:
	std::string name = "";
	int shipLength = 1;
	int lastShotY = 0;
	int lastShotX = 0;
	int firstHitShotX = 0;
	int firstHitShotY = 0;
	bool isExtraThirdShip = false;
	ScreenMatrix startingGrid = ScreenMatrix(); // The finalized grid that the AI places their ships on
	ScreenMatrix tempStartingGrid = ScreenMatrix(); // A temporary grid that AI places ships on to test if it's valid and/or smart
	int lastShotDirection = 1;
	int shipPointsHit = 0;
	int failedAttempts = 0;
	int timesTried = 0;
	int longestShipLength = 5;
	/// <summary>
	/// Targeting state for the enemy
	/// </summary>
	enum TargetingState
	{
		Searching,
		Hunting,
		LostTrack,
		None,
	};

	TargetingState targetingState = None;

	/// <summary>
	/// Announces to the player that the enemy has shot
	/// </summary>
	/// <param name="hasHit">Whether the AI hit the plaer</param>
	void AnnounceHit(bool hasHit)
	{
		system("cls");
		Renderer::GenerateGrid();
		std::string announcement = "  " + name + "'s Fleet Has" + (hasHit ? " Hit" : " Missed");
		Renderer::Out(announcement, hasHit ? RED : YELLOW);
		Renderer::Out("");
		Sleep(500);
	}
	/// <summary>
	/// Announces to the player that the enemy has placed their fleet
	/// </summary>
	void AnnounceShipPlacements()
	{
		system("cls");
		Renderer::GenerateGrid();
		std::string announcement = "  " + name + "'s Fleet Has Been Deployed";
		Renderer::Out(announcement);
		Renderer::Out("");
		Sleep(1500);
	}
	/// <summary>
/// Fires the enemys shot and returns whether it was a hit or not
/// </summary>
/// <param name="x">X position to shoot at</param>
/// <param name="y">Y position to shoot at</param>
/// <returns>If the shot was a hit</returns>
	bool IsPlayerPresent(int x, int y)
	{
		if (Controller::GetGridPosition(0, x, y) == 1)
		{
			Controller::EditGrid(0, x, y, 2);
			shipPointsHit++;
			failedAttempts = 0;
			Controller::IncrementPlayerHits(1);
		}
		else
		{
			Controller::EditGrid(0, x, y, 3);
		}

		lastShotX = x;
		lastShotY = y;
		SetLastShotPosition(x, y); // Sets the last shot position in the controller
		return Controller::GetGridPosition(0, x, y) == 2;
	}
	/// <summary>
	/// Resets the grid
	/// </summary>
	void ResetGrid()
	{
		startingGrid = ScreenMatrix(); // Resets grid
		tempStartingGrid = ScreenMatrix(); // Resets grid
		shipLength = 1;
		isExtraThirdShip = false;
	}
	/// <summary>
	/// Checks if the shot is invalid by checking if it is outside of the bounds of the grid or has already been targeted
	/// </summary>
	/// <param name="x"></param>
	/// <param name="y"></param>
	/// <returns>Whether shot is valid</returns>
	bool IsValidShot(int x, int y)
	{
		if (Controller::GetGridPosition(0, x, y) == 2 || Controller::GetGridPosition(0, x, y) == 3 || x > 9 || x < 0 || y > 9 || y < 0)
			return false;
		else
			return true;
	}
	/// <summary>
	/// Targets player ships by heading in a direction and searches for a hit, then returns if it was a hit or not and changes to hunting mode in which it continues to shoot in the same direction until the ship is destroyed
	/// or the enemy has lost tracking
	/// </summary>
	/// <returns></returns>
	bool TargetingShot()
	{
		bool shotUnfired = true;
		int newShotX = lastShotX; // Gets last shot X position
		int newShotY = lastShotY; // Gets last shot Y position

		while (true) // Repeats until a shot has been fired
		{
			if (targetingState == Searching)
			{
				newShotX = firstHitShotX; // Gets the first hit that was shot on X
				newShotY = firstHitShotY; // Gets the first hit that was shot on Y
				while (true) // Repeats until a direction being shot at is valid
				{
					switch (lastShotDirection) // Picks a direction
					{
					case 1:
						newShotY = firstHitShotY;
						newShotX = firstHitShotX - 1;
						break;
					case 2:
						newShotX = firstHitShotX;
						newShotY = firstHitShotY + 1;
						break;
					case 3:
						newShotY = firstHitShotY;
						newShotX = firstHitShotX + 1;
						break;
					case 4:
						newShotX = firstHitShotX;
						newShotY = firstHitShotY - 1;
						break;
					}

					if (IsValidShot(newShotX, newShotY)) // Shoots shot
					{
						if (IsPlayerPresent(newShotX, newShotY))
						{
							targetingState = Hunting; // If hit it changes the targeting state
							AnnounceHit(true);
						}
						else
						{
							lastShotDirection++; // Changes direction
							AnnounceHit(false);
						}

						shotUnfired = false; // Shot fired
						return shotUnfired;
					}
					else
						lastShotDirection++; // Changes direction


					if (lastShotDirection > 4) // If all directions are checked it shoots randomly
					{
						lastShotDirection = 1;
						failedAttempts++;
						if (failedAttempts > 2)
						{
							targetingState = None;
							break;
						}
					}
				}
			}
			else if (targetingState == Hunting) // Hunts down ship
			{
				while (true)
				{
					switch (lastShotDirection) // Continues to shoot one forward depending on the direction currently heading in
					{
					case 1:
						newShotY = lastShotY;
						newShotX = lastShotX - 1;
						break;
					case 2:
						newShotX = lastShotX;
						newShotY = lastShotY + 1;
						break;
					case 3:
						newShotY = lastShotY;
						newShotX = lastShotX + 1;
						break;
					case 4:
						newShotX = lastShotX;
						newShotY = lastShotY - 1;
						break;
					}

					if (IsValidShot(newShotX, newShotY)) // Checks for a valid hit
					{
						bool playerPresent = IsPlayerPresent(newShotX, newShotY);
						if (playerPresent && shipPointsHit < longestShipLength) // Shoots shot and if its a hit and not a last hit it will continue
						{
							targetingState = Hunting;
							AnnounceHit(true);
						}
						else if (playerPresent) // Resets the targeting state because the largest ship currently possible has been shot
						{
							shipPointsHit = 0;
							targetingState = None;
							longestShipLength--;
							AnnounceHit(true);
						}
						else
						{
							targetingState = LostTrack; // Loses track
							AnnounceHit(false);
						}

						shotUnfired = false;
						return shotUnfired;
					}
					else
					{
						targetingState = LostTrack; // Loses track
						break;
					}
				}
			}
			else if (targetingState == LostTrack)
			{
				if (lastShotDirection == 1 || lastShotDirection == 3) // Checks otherside of ship to see if ship continues
				{
					lastShotDirection = lastShotDirection == 1 ? 3 : 1;
				}
				else if (lastShotDirection == 2 || lastShotDirection == 4)
				{
					lastShotDirection = lastShotDirection == 2 ? 4 : 2;
				}

				switch (lastShotDirection) // Continues in new directions
				{
				case 1:
					newShotY = firstHitShotY;
					newShotX = firstHitShotX - 1;
					break;
				case 2:
					newShotX = firstHitShotX;
					newShotY = firstHitShotY + 1;
					break;
				case 3:
					newShotY = firstHitShotY;
					newShotX = firstHitShotX + 1;
					break;
				case 4:
					newShotX = firstHitShotX;
					newShotY = firstHitShotY - 1;
					break;
				}

				if (IsValidShot(newShotX, newShotY)) // Checks if shot is valid
				{
					if (IsPlayerPresent(newShotX, newShotY))
					{
						targetingState = Hunting; // Swaps back to hunting
						AnnounceHit(true);
					}
					else
					{
						shipPointsHit = 0;
						targetingState = None; // Exits targeting
						AnnounceHit(false);
					}

					shotUnfired = false;
					return shotUnfired;
				}
				else // Exits targeting
				{
					targetingState = None;
					shipPointsHit = 0;
					return shotUnfired;
				}
			}
			else
			{
				shipPointsHit = 0;
				return shotUnfired;
			}
		}
	}
	/// <summary>
	/// Whether the placement of the ship is valid by checking for nearby ships and if the ship is in the grid
	/// </summary>
	/// <param name="x"></param>
	/// <param name="y"></param>
	/// <param name="radius">Radius of all nearby tiles to check for ship placements</param>
	/// <returns></returns>
	bool IsValidPlacement(int x, int y, int radius)
	{
		for (int tempX = -radius; tempX <= radius; tempX++) // Simple equation for a circle
		{
			for (int tempY = -radius; tempY <= radius; tempY++)
			{
				int drawX = tempX + x;
				int drawY = tempY + y;
				if (startingGrid.data[drawX][drawY] == 5)
				{
					if (Renderer::debugMode)
						Renderer::Out("Failure State: Tried to place on current ship");
					return false; // Ship already placed within range
				}
			}
		}

		bool isInBounds = x >= 0 && x <= 9 && y >= 0 && y <= 9; // Inbounds

		if (Renderer::debugMode)
		{
			if (!isInBounds)
				Renderer::Out("Failure State: Out Of Bounds");

			Sleep(100);
		}
		return isInBounds;
	}
	/// <summary>
	/// Places a random point for a ship to start from
	/// </summary>
	/// <param name="direction">direction to move in</param>
	/// <param name="failed">Whether the ship placement failed</param>
	/// <param name="randomX"></param>
	/// <param name="randomY"></param>
	/// <param name="i"></param>
	void PlaceRandomShipPoints(int direction, bool& failed, int randomX, int randomY, int i)
	{
		switch (direction)
		{
		case 0:
			if (IsValidPlacement(randomX + i, randomY, 1)) // Checks if ship placement is valid
				tempStartingGrid.data[randomX + i][randomY] = 5;
			else
				failed = true;
			break;
		case 1:
			if (IsValidPlacement(randomX - i, randomY, 1))
				tempStartingGrid.data[randomX - i][randomY] = 5;
			else
				failed = true;
			break;
		case 2:
			if (IsValidPlacement(randomX, randomY + i, 1))
				tempStartingGrid.data[randomX][randomY + i] = 5;
			else
				failed = true;
			break;
		case 3:
			if (IsValidPlacement(randomX, randomY - i, 1))
				tempStartingGrid.data[randomX][randomY - i] = 5;
			else
				failed = true;
			break;
		}
	}
	/// <summary>
	/// Continues onto the next ship for placement
	/// </summary>
	/// <param name="failed"></param>
	void NextShip(bool& failed)
	{
		if (!failed)
			if (shipLength != 2 || isExtraThirdShip)
				shipLength++; // Continues onto the next ship
			else
				isExtraThirdShip = true; // Continues onto the next ship but without increasing the ship length
		else
		{
			timesTried++;
			failed = false; // Resets failed status
		}
	}
	/// <summary>
	/// Stores the ship in the starting grid and locks in the ship position
	/// </summary>
	/// <param name="direction">Direction to place ship in</param>
	/// <param name="randomX"></param>
	/// <param name="randomY"></param>
	void StoreShip(int direction, int randomX, int randomY)
	{
		for (int i = 0; i < shipLength + 1; i++) // For each ship tile it locks the tile into the starting grid
		{
			if (direction == 0)
				startingGrid.data[randomX + i][randomY] = tempStartingGrid.data[randomX + i][randomY];
			else if (direction == 1)
				startingGrid.data[randomX - i][randomY] = tempStartingGrid.data[randomX - i][randomY];
			else if (direction == 2)
				startingGrid.data[randomX][randomY + i] = tempStartingGrid.data[randomX][randomY + i];
			else if (direction == 3)
				startingGrid.data[randomX][randomY - i] = tempStartingGrid.data[randomX][randomY - i];
		}
	}
	/// <summary>
	/// Finish the grid and announce ship placements
	/// </summary>
	/// <param name="grid"></param>
	void FinishPlacements(int grid)
	{
		*Controller::GetGrid(grid) = startingGrid; // Sets the grid to the new grid
		startingGrid = ScreenMatrix(); // Resets grid
		tempStartingGrid = ScreenMatrix(); // Resets grid
		shipLength = 1; // Resets ship length
		isExtraThirdShip = false; // Resets third ship status
		AnnounceShipPlacements();
	}
public:
	EnemyAI(int difficulty)
	{
		ChooseShipName(difficulty);
	};
	virtual void ShootShell() = 0;
	virtual void PlaceShips(int grid) = 0;
	/// <summary>
	/// Generates a random integar
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
	void SetLastShotPosition(int x, int y) { Controller::SetLastEnemyShotPosition(x, y); }
	std::string GetName() { return name; }
	void SetName(std::string newName) { name = newName; }
};

