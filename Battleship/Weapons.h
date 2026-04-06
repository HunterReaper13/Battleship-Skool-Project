#pragma once
#include "ScreenMatrix.h"
#include "EnemyAI.h"
#include "Renderer.h"
#include "Controller.h"
class Weapons
{
private:
	static int weaponType;
	static int maxWeapons;
	static int cursorPosX;
	static int cursorPosY;
	static bool hasUsedMissile;
	static bool hasUsedRadar;
	static bool hasUsedAirStrike;
	static bool hasUsedNuclearBomb;
	static bool validHit;
	static int missileUses;
	static int radarUses;
	static int airStrikeUses;
	static int playerShots;
public:
	static void ResetAllWeapons() { hasUsedMissile = false; hasUsedAirStrike = false; hasUsedNuclearBomb = false; hasUsedRadar = false; }
	static void ShootCoordinates(int x, int y) { cursorPosX = x; cursorPosY = y; }
	static int GetWeaponType() { return weaponType; }
	/// <summary>
	/// Shoots a shell, checks for an enemy ship and if it's a hit it will change the x to a red. If it's a miss it will set the x to yellow.
	/// </summary>
	/// <param name="grid">The current grid, this will always be the enemy grid but this is good for if for later there are multiple enemy grids (Update I added multiple grids and this is very useful)</param>
	/// <param name="tempGrid">The temporary grid that displays all of the shots and misses without the cursor</param>
	/// <param name="ai">The enemy ai that will retaliate a shot after you shoot yours</param>
	static void ShootDefaultShell(ScreenMatrix* grid, ScreenMatrix* tempGrid, EnemyAI* ai)
	{
		ShootShot(grid, tempGrid, cursorPosX, cursorPosY, false);
		system("cls"); //Clears & Regenerates the Grid
		Renderer::GenerateGrid(cursorPosY, cursorPosX);
		EnemyFire(ai);
		validHit = false;
	}
	/// <summary>
	/// Shoots a shot at the determied x and y position and sets the data value to a 2 for a hit or a 3 for a miss
	/// </summary>
	/// <param name="grid">The grid you're shooting at</param>
	/// <param name="tempGrid">The temporary grid to change the cursor to red for a hit</param>
	/// <param name="x">The x position to fire at</param>
	/// <param name="y">The y position to fire at</param>
	/// <param name="isRadar">Whether the shot is a radar shot or not</param>
	static void ShootShot(ScreenMatrix* grid, ScreenMatrix* tempGrid, int x, int y, bool isRadar)
	{
		if (tempGrid->data[x][y] >= 5 && tempGrid->data[x][y] != 2 && tempGrid->data[x][y] != 3)
		{
			grid->data[x][y] = isRadar ? 6 : 2;
			tempGrid->data[x][y] = isRadar ? 6 : 2; // Sets the data of the old matrix and the current data to change the green cursor marker to yellow
			validHit = true;
			Controller::IncrementEnemyHits(isRadar ? 0 : 1); // Increments enemy hits for a valid hit
		}
		// Missed if not an enemy ship
		else if (tempGrid->data[x][y] != 2 && tempGrid->data[x][y] != 3)
		{
			grid->data[x][y] = 3;
			tempGrid->data[x][y] = 3; // Sets the data of the old matrix and the current data to change the green cursor marker to red
			validHit = true;
		}
		else if (isRadar)
			validHit = true;
		system("cls"); //Clears & Regenerates the Grid
		Renderer::GenerateGrid(x, y);

	}
	/// <summary>
	/// Triggers the enemy to shoot back
	/// </summary>
	/// <param name="ai">What enemy is shooting back</param>
	static void EnemyFire(EnemyAI* ai)
	{
		if (Controller::GetDifficulty() < 3 || playerShots > 0)
		{
			if (validHit)
			{
				ai->ShootShell(); // Enemy Shoots A Shot
				Controller::SetNextEnemyFiring();
				playerShots = 0;
			}
		}
		else
			playerShots++;
	}
	/// <summary>
	/// Fires 9 shots in a 3 by 3 grid
	/// </summary>
	/// <param name="grid">Grid to shoot at</param>
	/// <param name="tempGrid">Temporary grid to change cursor</param>
	/// <param name="ai">The Ai that will shoot back</param>
	static void ShootMissile(ScreenMatrix* grid, ScreenMatrix* tempGrid, EnemyAI* ai)
	{
		if (!hasUsedMissile && Controller::GetEnemyHits() >= 0)
		{
			int tempCursorPosX = cursorPosX - 1;
			int tempCursorPosY = cursorPosY - 2;

			for (int x = 0; x < 3; x++) // Shoots in a 3 by 3 grid
			{
				for (int y = 0; y < 3; y++)
				{
					tempCursorPosY++;
					ShootShot(grid, tempGrid, tempCursorPosX, tempCursorPosY, false);
				}
				tempCursorPosY = cursorPosY - 2;
				tempCursorPosX++;
			}

			if (missileUses > 0 || Controller::GetDifficulty() < 3) // Allows the missile to be used twice if in hard
				hasUsedMissile = true;
			validHit = true;
			weaponType = 0;
			system("cls"); //Clears & Regenerates the Grid
			Renderer::GenerateGrid(cursorPosX, cursorPosY);
			missileUses++;

			EnemyFire(ai);
		}
		else
		{
			ShootDefaultShell(grid, tempGrid, ai);
		}
	}
	/// <summary>
	/// Shoots 25 shots in a 5 by 5 grid but instead of hitting the ships it makes them purple for the player to see
	/// </summary>
	/// <param name="grid">The grid to shoot at</param>
	/// <param name="tempGrid">The temp grid to change the cursor/param>
	/// <param name="ai">The ai that will shoot back</param>
	static void ShootRadarShell(ScreenMatrix* grid, ScreenMatrix* tempGrid, EnemyAI* ai)
	{
		if (!hasUsedRadar && Controller::GetEnemyHits() >= 10)
		{
			int tempCursorPosX = cursorPosX - 2;
			int tempCursorPosY = cursorPosY - 2;

			for (int x = 0; x < 5; x++) // Shoots in a 5 by 5 grid
			{
				for (int y = 0; y < 5; y++)
				{
					tempCursorPosY++;
					ShootShot(grid, tempGrid, tempCursorPosX, tempCursorPosY, true);
				}
				tempCursorPosY = cursorPosY - 2;
				tempCursorPosX++;
			}

			if (radarUses > 0 || Controller::GetDifficulty() < 3)
				hasUsedRadar = true;
			validHit = true;
			weaponType = 0;
			system("cls"); //Clears & Regenerates the Grid
			Renderer::GenerateGrid(cursorPosX, cursorPosY);
			radarUses++;

			EnemyFire(ai);
		}
		else
		{
			ShootDefaultShell(grid, tempGrid, ai);
		}
	}
	/// <summary>
	/// Shoots in a full line choosing whether to shoot vertical or horizontal depending on the cursor rotation state
	/// </summary>
	/// <param name="grid">The grid to shoot at</param>
	/// <param name="tempGrid">The temp grid to remove the cursor</param>
	/// <param name="ai">The ai that shoots back</param>
	static void AirStrike(ScreenMatrix* grid, ScreenMatrix* tempGrid, EnemyAI* ai)
	{
		if (!hasUsedAirStrike && Controller::GetEnemyHits() >= 17)
		{
			for (int x = 0; x < 10; x++) // Shoots out a whole row
				ShootShot(grid, tempGrid, x, cursorPosY, false);


			if (airStrikeUses > 0 || Controller::GetDifficulty() < 3)
				hasUsedAirStrike = true;
			validHit = true;
			weaponType = 0;
			system("cls"); //Clears & Regenerates the Grid
			Renderer::GenerateGrid(cursorPosX, cursorPosY);
			EnemyFire(ai);
		}
	}
	/// <summary>
	/// Deletes all entities in a grid changing them all to 2 and incrementing enemy hits by 17 for a whole grid
	/// </summary>
	/// <param name="grid">The grid to shoot at</param>
	/// <param name="tempGrid">The temp grid to remove the cursor</param>
	/// <param name="ai">The ai to shoot back</param>
	static void NuclearBomb(ScreenMatrix* grid, ScreenMatrix* tempGrid, EnemyAI* ai)
	{
		if (!hasUsedNuclearBomb && Controller::GetEnemyHits() >= 25)
		{
			for (int y = 0; y < 10; y++)
			{
				for (int x = 0; x < 10; x++)
				{
					if (grid->data[x][y] == 5)
						Controller::IncrementEnemyHits(1);
					grid->data[y][x] = 2;
					tempGrid->data[y][x] = 2; // Sets the data of the old matrix and the current data to change the green cursor marker to yellow
				}
				system("cls"); //Clears & Regenerates the Grid
				Renderer::GenerateGrid(cursorPosY, cursorPosX);
			}
			validHit = true;
			hasUsedNuclearBomb = true;
			validHit = true;
			weaponType = 0;
			system("cls"); //Clears & Regenerates the Grid
			Renderer::GenerateGrid(cursorPosX, cursorPosY);
			EnemyFire(ai);
		}
	}
	/// <summary>
	/// Changes the weapon type that the player is using
	/// </summary>
	static void ChangeShell()
	{
		if (weaponType < maxWeapons)
			weaponType++;
		else
			weaponType = 0;
	}
	/// <summary>
	/// Displays which weapon the player is used and changing the end to (Used) or (Locked) depending on if they have been used or if they're not available yet
	/// </summary>
	/// <returns>"Weapon Type" (Used)/(Locked)</returns>
	static std::string DisplayWeaponType()
	{
		std::string weapon = "";
		switch (weaponType)
		{
		case 0:
			weapon = "Default Shell";
			break;
		case 1:
			weapon = "Tomahawk Missile";
			break;
		case 2:
			weapon = "Radar Shell";
			break;
		case 3:
			weapon = "Airstrike";
			break;
		case 4:
			weapon = "Nuclear Bomb";
			break;
		}

		if (weaponType * 5 > Controller::GetEnemyHits())
			weapon += " (Locked)";
		else
			if ((weaponType == 1 && hasUsedMissile) || (weaponType == 2 && hasUsedRadar) || (weaponType == 3 && hasUsedAirStrike) || (weaponType == 4 && hasUsedNuclearBomb))
				weapon += " (Used)";
		return weapon;
	}
};

