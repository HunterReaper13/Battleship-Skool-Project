#pragma once
#include <vector>
#include "KeyboardControls.h"
#include "ScreenMatrix.h"
#include "Renderer.h"
#include "EnemyAI.h"
#include "Weapons.h"

//Defining the Directions In Which The Cursor Can Move
#define LEFT 0
#define RIGHT 1
#define UP 2
#define DOWN 3
#define NONE 4

// I Apologise if some of these comments aren't descriptive enough, this is very complex and some of this works by pure miracle but I'll do my best to explain
// whats going on here. This is also because Stephen complains too much about my lack of comments.

/// <summary>
/// Simple sturct that is used to store X and Y coordinates, mainly used to store the position of the curor
/// </summary>
struct xy
{
	int e[2];
	xy(int x, int y) : e{ x, y } {}
	int x() { return e[0]; }
	int y() { return e[1]; }
	xy() : e{ 0,0 } {}
};

/// <summary>
/// Cursor indicates where the "cursor" is on screen and can be moved with the arrow keys derived from the input class.
/// It also manages actions like placing ships and shooting shells.
/// </summary>
class Cursor : public Input
{
private:
	bool isTargeting = false; // Whether the cursor is set to targeting mode or is placing ships
	xy cursorPos; // The current position of the cursor on screen
	ScreenMatrix oldMatrixData = ScreenMatrix(); // The data of the grid from the previous cycle, used to keep the data values of the grid excluding the cursor markers in green
	int cursorLen = 1; // The length of the cursor, mainly used to determine the length of the ship
	int rotationState = 1; // Whether the ship is rotated vertically or horizontally. 1 == Horizontal and 0 == Vertical
	bool isExtraThirdShip = false; // This simply prints an extra 3 length ship
	int selectedGrid = 0;
public:
	/// <summary>
	/// Gets Ship Length
	/// </summary>
	/// <returns>Returns length of the ship</returns>
	int GetShipLength() { return cursorLen; }
	/// <summary>
	/// Returns the current position of the cursor on screen
	/// </summary>
	/// <returns></returns>
	xy GetCursorPosition() { return cursorPos; }
	/// <summary>
	/// Checks for an input then updates the cursor and either moves, rotates or peforms an action depending on the key pressed
	/// </summary>
	void CursorUpdate(EnemyAI* ai)
	{
		if (Key(PRESSEDTHISFRAME, R)) // Checks for R key and rotates
		{
			if (!isTargeting)
				RotateCursor(Controller::GetGrid(selectedGrid));
		}

		// Checks for a direction key, using PRESSED so you can hold the key
		if (Key(PRESSED, KEY_UP))
		{
			MoveCursor(UP, Controller::GetGrid(selectedGrid));
		}
		if (Key(PRESSED, KEY_DOWN))
		{
			MoveCursor(DOWN, Controller::GetGrid(selectedGrid));
		}
		if (Key(PRESSED, KEY_LEFT))
		{
			MoveCursor(LEFT, Controller::GetGrid(selectedGrid));
		}
		if (Key(PRESSED, KEY_RIGHT))
		{
			MoveCursor(RIGHT, Controller::GetGrid(selectedGrid));
		}
		if (Key(PRESSEDTHISFRAME, ESCAPE))
		{
			Controller::SetEndGame(true);
		}
		if (Key(PRESSEDTHISFRAME, ONE) && isTargeting)
		{
			Weapons::ChangeShell();
			if (Weapons::GetWeaponType() == 4) // Sets the entire grid to cursor
			{
				for (int y = 0; y < 10; y++)
					for (int x = 0; x < 10; x++)
						Controller::EditGrid(selectedGrid, x, y, 4);
			}
			else // Resets the grid back to normal
			{
				for (int y = 0; y < 10; y++)
					for (int x = 0; x < 10; x++)
						Controller::EditGrid(selectedGrid, x, y, oldMatrixData.data[x][y]);

				Controller::EditGrid(selectedGrid, cursorPos.x(), cursorPos.y(), 4);
			}
			system("cls"); //Clears & Regenerates the Grid
			Renderer::GenerateGrid(cursorPos.y(), cursorPos.x());
		}
		if (Key(PRESSEDTHISFRAME, TWO) && isTargeting)
		{
			if (isTargeting && Controller::GetDifficulty() > 1)
			{
				if (Weapons::GetWeaponType() < 2)
					Controller::GetGrid(selectedGrid)->data[cursorPos.x()][cursorPos.y()] = oldMatrixData.data[cursorPos.x()][cursorPos.y()]; // Sets the cursor value back to the original value
				else // Resets the whole grid back to the original matrix
				{
					for (int y = 0; y < 10; y++)
						for (int x = 0; x < 10; x++)
							Controller::EditGrid(selectedGrid, x, y, oldMatrixData.data[x][y]);
				}

				if (Controller::GetDifficulty() == 2) // Changes grid and resets back to the first grid depending on difficultys
					if (selectedGrid >= 2)
						selectedGrid = 1;
					else
						selectedGrid++;
				else
					if (selectedGrid >= 4)
						selectedGrid = 1;
					else
						selectedGrid++;
				oldMatrixData = *Controller::GetGrid(selectedGrid); // Sets old matrix data to the new grid
				if (Weapons::GetWeaponType() != 4)
					Controller::GetGrid(selectedGrid)->data[cursorPos.x()][cursorPos.y()] = 4; // Applies a cursor
				else // Changes entire grid to cursor
				{
					for (int y = 0; y < 10; y++)
						for (int x = 0; x < 10; x++)
							Controller::EditGrid(selectedGrid, x, y, 4);
				}
				system("cls"); //Clears & Regenerates the Grid
				Renderer::GenerateGrid(cursorPos.y(), cursorPos.x());
			}
		}

		//Peforms an action depending on if you're in targeting mode or not
		if (Key(PRESSEDTHISFRAME, ENTER))
		{
			if (!isTargeting)
				InsertShip(Controller::GetGrid(selectedGrid));
			else
			{
				Weapons::ShootCoordinates(cursorPos.x(), cursorPos.y()); //Sets the coordinates of the fire location in Weapons
				switch (Weapons::GetWeaponType()) // Shoots associated weapon depending on the selected weapon
				{
				case 0:
					Weapons::ShootDefaultShell(Controller::GetGrid(selectedGrid), &oldMatrixData, ai);
					cursorLen = 0;
					break;
				case 1:
					Weapons::ShootMissile(Controller::GetGrid(selectedGrid), &oldMatrixData, ai);
					cursorLen = 0;
					break;
				case 2:
					Weapons::ShootRadarShell(Controller::GetGrid(selectedGrid), &oldMatrixData, ai);
					cursorLen = 0;
					break;
				case 3:
					Weapons::AirStrike(Controller::GetGrid(selectedGrid), &oldMatrixData, ai);
					cursorLen = 0;
					break;
				case 4:
					Weapons::NuclearBomb(Controller::GetGrid(selectedGrid), &oldMatrixData, ai);
					cursorLen = 100;
					break;
				}
			}
		}
	}
	/// <summary>
	/// Moves the cursor by taking a direction and moving the cursor by 1 in that direction before printing the grid again with the new data values.
	/// </summary>
	/// <param name="direction">Whether to move the cursor up, down, left or right</param>
	/// <param name="matrix">The screen you want the cursor on, left for the allied ships or right for enemy ships</param>
	/// <param name="overrideEdgeDetection">Whether to ignore if the ship will be off the edge (mostly used to allow ships to rotate)</param>
	void MoveCursor(int direction, ScreenMatrix* matrix, bool overrideEdgeDetection = false)
	{
		xy newPos; // Creates a new position
		switch (direction) // Sets the new direction by using the current position and either adding or subtracting one to move it up, down, left or right
		{
		case UP:
			newPos = xy(cursorPos.x() - 1, cursorPos.y());
			break;
		case DOWN:
			newPos = xy(cursorPos.x() + 1, cursorPos.y());
			break;
		case LEFT:
			newPos = xy(cursorPos.x(), cursorPos.y() - 1);
			break;
		case RIGHT:
			newPos = xy(cursorPos.x(), cursorPos.y() + 1);
			break;
		case NONE:
			newPos = xy(cursorPos.x(), cursorPos.y());
			break;
		}

		if (IsTouchingEdge(newPos) && !overrideEdgeDetection) // If the player tries to move the ship off the grid it will simply return and prevent the move
			return;
		else if (overrideEdgeDetection) // If override edge detection is on it will move the ships away from the grid, normally performed if a ship is rotated against an edge
			for (int i = 0; i < cursorLen + 1; i++)
			{
				if (rotationState == 0)
					matrix->data[cursorPos.x()][cursorPos.y() + i] = oldMatrixData.data[cursorPos.x()][cursorPos.y() + i];
				else
					matrix->data[cursorPos.x() + i][cursorPos.y()] = oldMatrixData.data[cursorPos.x() + i][cursorPos.y()];
			}

		// Complex stuff

		for (int i = 0; i < cursorLen + 1; i++) // If the ship is horizontal it will set the data for the matrix to the data of the previously cycled matrix data before the cursor markers were drawn on
			if (rotationState != 0)
				matrix->data[cursorPos.x()][cursorPos.y() + i] = oldMatrixData.data[cursorPos.x()][cursorPos.y() + i];
			else if (rotationState == 0) // Same as above but for vertically drawn ships
				matrix->data[cursorPos.x() + i][cursorPos.y()] = oldMatrixData.data[cursorPos.x() + i][cursorPos.y()];

		for (int i = 0; i < cursorLen + 1; i++) // If the ship is horizontal it will set the data of the current screen to the previously cycled matrix data then print the green cursor markers over the screen
		{
			if (rotationState == 0)
			{
				if (matrix->data[newPos.x() + i][newPos.y()] != 4)
					oldMatrixData.data[newPos.x() + i][newPos.y()] = matrix->data[newPos.x() + i][newPos.y()];
				else
					oldMatrixData.data[newPos.x() + i][newPos.y()] = 0;
				matrix->data[newPos.x() + i][newPos.y()] = 4;
			}
			else // Same as above but for vertically drawn ships
			{
				if (matrix->data[newPos.x()][newPos.y() + i] != 4)
					oldMatrixData.data[newPos.x()][newPos.y() + i] = matrix->data[newPos.x()][newPos.y() + i];
				else
					oldMatrixData.data[newPos.x()][newPos.y() + i] = 0;
				matrix->data[newPos.x()][newPos.y() + i] = 4;
			}
		}
		//Sets the current position to the new position
		cursorPos = newPos;

		system("cls"); // Clears the screen and regenerates the grid
		Renderer::GenerateGrid(cursorPos.y(), cursorPos.x());
	}

	/// <summary>
	/// Places a ship at the current cursor coordinates given there isn't already a ship there
	/// </summary>
	/// <param name="matrix">Your grid</param>
	void InsertShip(ScreenMatrix* matrix)
	{
		// Checks if there is a ship underneath the ship you're trying to place
		for (int i = 0; i < cursorLen + 1; i++)
		{
			if (rotationState == 0 && oldMatrixData.data[cursorPos.x() + i][cursorPos.y()] == 1)
				return;
			else if (rotationState == 1 && oldMatrixData.data[cursorPos.x()][cursorPos.y() + i] == 1)
				return;
		}
		//Places a ship tile for the entire length of your ship
		for (int i = 0; i < cursorLen + 1; i++)
		{
			if (rotationState == 0)
			{
				matrix->data[cursorPos.x() + i][cursorPos.y()] = 1;
				oldMatrixData.data[cursorPos.x() + i][cursorPos.y()] = 1;
			}
			else if (rotationState == 1)
			{
				matrix->data[cursorPos.x()][cursorPos.y() + i] = 1;
				oldMatrixData.data[cursorPos.x()][cursorPos.y() + i] = 1;
			}
		}
		//Checks if the next ship is a third ship allowing for two 3 long ship placements
		if (cursorLen != 2 || isExtraThirdShip)
			cursorLen++; // Next ship
		else
			isExtraThirdShip = true;

		system("cls"); //Clears & Regenerates the Grid
		Renderer::GenerateGrid(cursorPos.y(), cursorPos.x());
	}

	/// <summary>
	/// Checks if the ship is outside the grid
	/// </summary>
	/// <param name="newPos">Position you want to check</param>
	/// <returns></returns>
	bool IsTouchingEdge(xy newPos) {
		bool xLocked, yLocked; // Locks the y or x direction and prevents moving if either or true
		if (rotationState == 0)
		{
			xLocked = (newPos.x() > (9 - cursorLen) || newPos.x() < 0); // Checks if the x direction is less than 0 or greated than 9 sutracting how long your ship is
			yLocked = (newPos.y() > 9 || newPos.y() < 0); // Checks if the y direction is more than 9 or less than 0

		}
		else // Same as above but the conditions are swapped
		{
			xLocked = (newPos.x() > 9 || newPos.x() < 0);
			yLocked = (newPos.y() > (9 - cursorLen) || newPos.y() < 0);
		}
		if (xLocked || yLocked)
			return true;
		else
			return false;
	}

	

	/// <summary>
	/// Clears the cursor by clearing old matrix data grid and resetting cursor length to 0 as well as changing to targeting mode if inputted
	/// </summary>
	/// <param name="changeToTargeting">Whether to swap to targeting mode</param>
	void CursorClear(bool changeToTargeting, int gridToClear)
	{
		if (gridToClear > 0)
			oldMatrixData = *Controller::GetGrid(gridToClear);
		else
			oldMatrixData = ScreenMatrix();
		cursorPos = xy(0, 0);
		isTargeting = changeToTargeting;
		rotationState == 1;
		if (changeToTargeting)
		{
			cursorLen = 0;
			selectedGrid++;
			Controller::EditGrid(selectedGrid, cursorPos.x(), cursorPos.y(), 4);
			isExtraThirdShip = false;
			system("cls");
			Renderer::GenerateGrid();
		}
		else
		{
			cursorLen = 1;
			selectedGrid = 0;
		}
	}

	/// <summary>
	/// Rotates the ship by changing the rotation state then moves the cursor overriding the edge detection to move the ship away from the grid when rotating
	/// </summary>
	/// <param name="mat">The grid to rotate on</param>
	void RotateCursor(ScreenMatrix* mat)
	{
		rotationState = rotationState == 0 ? 1 : 0; // Flips the rotation state

		if (IsTouchingEdge(cursorPos)) // Checks if touching edge and moves the ship
		{
			for (int i = 0; i < cursorLen; i++)
			{
				MoveCursor((rotationState == 0) ? UP : LEFT, mat, true);
			}
		}
		else
		{
			MoveCursor(NONE, mat, true); // Rotates the Cursor statically
		}
	}
};

