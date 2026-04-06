#pragma once
#include <iostream>
#include <vector>
#include "ScreenMatrix.h"
/// <summary>
/// A singleton class which manages all of the game data and manages all of the grids within the game
/// </summary>
class Controller
{
private:
	static Controller* gameController;
    static std::vector<ScreenMatrix> grids;
    static bool exitGame;
    static int playerHits;
    static int enemyHits;
    static int difficulty;
    static int lastEnemyYPos;
    static int lastEnemyXPos;
    static int enemySelected;
    Controller()
    {
        grids.push_back(ScreenMatrix());
    }
public:
    /// <summary>
    /// Gets the int value for the next enemy due to fire a shot
    /// </summary>
    /// <returns></returns>
    static int GetNextEnemyFiring() { return enemySelected; }
    /// <summary>
    /// Changes enemy firing to the next one in the array
    /// </summary>
    static void SetNextEnemyFiring() 
    {
        enemySelected++;

        if (enemySelected >= difficulty)
        {
            enemySelected = 0;
        }
    }
    static int GetLastEnemyShotPosition(bool isX) { return isX ? lastEnemyXPos : lastEnemyYPos; } // Gets Last Shot Position
    static void SetLastEnemyShotPosition(int x, int y) { lastEnemyXPos = x; lastEnemyYPos = y; } // Sets Enemy Last Shot Position
    static int GetPlayerHits() { return playerHits; } // Gets Player Hits
    static int GetEnemyHits() { return enemyHits; } // Gets Enemy Hits
    /// <summary>
    /// Increments player hits by whatever is passed through
    /// </summary>
    /// <param name="hits">Amount of hits to increment by</param>
    static void IncrementPlayerHits(int hits) { playerHits += hits; }
    /// <summary>
    /// Increments enemy hits by whatever is passed through
    /// </summary>
    /// <param name="hits">Amount of hits to increment by</param>
    static void IncrementEnemyHits(int hits) { enemyHits += hits; }
    /// <summary>
    /// Sets player hits to 0
    /// </summary>
    static void ResetPlayerHits() { playerHits = 0; }
    /// <summary>
    /// Sets enemy hits to 0
    /// </summary>
    static void ResetEnemyHits() { enemyHits = 0; }
    /// <summary>
    /// Gets the exit status of the game
    /// </summary>
    /// <returns>The exit status</returns>
    static bool GetEndGame() { return exitGame; }
    /// <summary>
    /// Sets the status of whether the game has been quit
    /// </summary>
    /// <param name="exit">If to exit or not</param>
    static void SetEndGame(bool exit) { exitGame = exit; }
    /// <summary>
    /// Gets the difficulty value
    /// </summary>
    /// <returns>The difficulty value</returns>
    static int GetDifficulty() { return difficulty; }
    /// <summary>
    /// Sets the difficulty
    /// </summary>
    /// <param name="newDiff">Difficulty value</param>
    static void SetDifficulty(int newDiff) {difficulty = newDiff;}
	/// <summary>
	/// Deletes the constructor for the singleton class
	/// </summary>
	/// <param name="obj"></param>
	Controller(const Controller& obj) = delete;
    static Controller* GetController() {
        if (gameController == nullptr) {
            if (gameController == nullptr) {
                gameController = new Controller();
            }
        }
        return gameController;
    }
    /// <summary>
    /// 
    /// </summary>
    /// <param name="index">What Grid To Return</param>
    /// <returns>Returns a grid</returns>
    static ScreenMatrix* GetGrid(int index) { return &grids[index]; }
    /// <summary>
    /// Returns the data value for a selected grid position using index to determine which grid to access from
    /// </summary>
    /// <param name="index">Which grid to access from</param>
    /// <param name="x">X position of the grid</param>
    /// <param name="y">Y position of the grid</param>
    /// <returns>Data value at grid position</returns>
    static int GetGridPosition(int index,int x, int y) { return grids[index].data[x][y]; }

    /// <summary>
    /// Sets the data value for a selected grid position using an index to determine which grid to edit
    /// </summary>
    /// <param name="index">Which grid to access</param>
    /// <param name="x">X position of the grid</param>
    /// <param name="y">Y position of the grid</param>
    /// <param name="editType">The data value to change to</param>
    static void EditGrid(int index, int x, int y, int editType) { grids[index].data[x][y] = editType; }
    /// <summary>
    /// Generates new grids for the game including player grids
    /// </summary>
    /// <param name="gridsToCreate">The amount of grids to create</param>
    static void CreateNewGrid(int gridsToCreate)
    {
        ScreenMatrix tempGrid = ScreenMatrix();
        for (int i = -1; i < gridsToCreate; i++)
        {
            grids.push_back(tempGrid);
        }
    }
    /// <summary>
    /// Deletes all of the enemy grids unless deletePlayerGrid is true
    /// </summary>
    /// <param name="deletePlayerGrid">Whether to delete the players grid or not</param>
    static void DeleteGrids(bool deletePlayerGrid)
    {
        if (!deletePlayerGrid)
            for (int i = -1; i < grids.size(); i++)
                grids.pop_back();
        else
            grids.clear();
    }
};

