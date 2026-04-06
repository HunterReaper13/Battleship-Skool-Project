#pragma once
#include <iostream>
#include <fstream>
#include <string>
#include <array>
#include "Main.h"
#include "Controller.h"
/// <summary>
/// Basic Rendering Class which renders the game grid and assists with outputting data
/// </summary>
class Renderer {
private:
    static std::string currentEnemyFleets[4];
public:
    static void SetEnemyFleets(std::string fleetName, int index)
    {
        currentEnemyFleets[index] = fleetName;
    }
    static bool debugMode;
    /// <summary>
    /// Takes an input and outputs it in the console similar to std::cout but you can put in colors as an input and false if you don't want to generate a new line
    /// </summary>
    /// <typeparam name="T">A generic type that allows inputting integers, strings and etc</typeparam>
    /// <param name="obj">The thing you want to output</param>
    /// <param name="col">The color of the text</param>
    /// <param name="newLine">Whether you want a new line after you input the text</param>
    /// <param name="buffer">The console buffer, pointer to where the console logs outputs</param>
    template <typename T>
    static void Out(T obj, const char* col = WHITE, bool newLine = true, std::streambuf* buffer = std::cout.rdbuf()) {

        std::streambuf* oldBuf = std::cout.rdbuf(); // Gets the last buffer
        std::cout.rdbuf(buffer); // Reads the current buffer

        if (newLine)
            std::cout << col << obj << RESET << std::endl; // Prints out whatever is inserted to obj and colors it, then resets the color with RESET
        else
            std::cout << col << obj << RESET; // Same as above but without a new line
        std::cout.rdbuf(oldBuf); // Reads the old buffer
    }
    /// <summary>
    /// Draws the first line of a battleship board
    /// </summary>
    /// <param name="matrix">The board you want to print</param>
    /// <param name="currentLine">The row of the line you're printing</param>
    /// <param name="debug">If true it prints the int value of the grid point instead of X</param>
    static void DrawLine(ScreenMatrix matrix, int currentLine, bool debug = false)
    {
        for (int i = 0; i < 10; i++)
        {
            const char* colour = WHITE;
            int* shipStatus = &matrix.data[currentLine][i];
            switch (*shipStatus)
            {
                case 1:
                    colour = CYAN; // Allied ships
                    break;
                case 2:
                    colour = RED; // Hit
                    break;
                case 3:
                    colour = YELLOW; // Miss
                    break;
                case 4:
                    colour = GREEN; // Cursor for pointing
                    break;
                case 5:
                    if (!debugMode)
                        colour = WHITE; // Hidden enemy ships
                    else
                        colour = GREEN;
                    break;
                case 6:
                    colour = MAGENTA;
                    break;
            }
            std::string output; // This is for debugging, when debug is active it will swap the x's with the value that each tile is, this displays hidden enemy positions
            if (debug)
                output = std::to_string(*shipStatus);
            else
                output = "x";
            Out(output + " ", colour, false);
        }
    }
    /// <summary>
    /// Generates the full grid printing both your board and the enemies board with a nice table surrounding it
    /// </summary>
    /// <param name="debug">Replaces the x's as data values</param>
    static void GenerateGrid(int xPos = 0, int yPos = 0);

    /// <summary>
    /// Generates the top line of the table "+--------------------------+"
    /// </summary>
    static void GenerateTableLine()
    {
        std::string table = "  +";

        for (int i = 0; i < 51 + (Controller::GetDifficulty() < 2 ? 0 : (Controller::GetDifficulty() > 2 ? 78 : 26)); i++) // Repeats depending on the size of the grid via difficulty
        {
            table += "-";
        }
        table += "+";
        Out(table, WHITE);
    }
    /// <summary>
    /// Generates the battleships title of the game "|          Battleships           |"
    /// </summary>
    static void GenerateLogo()
    {
        std::string logo = "  |";
        for (int i = 0; i < 2; i++)
        {
            for (int j = 0; j < (Controller::GetDifficulty() < 2 ? 20 : (Controller::GetDifficulty() > 2 ? 59 : 33)); j++) // Repeats depending on the size of the grid via difficulty
                logo += " ";

            if (i == 0)
                logo += "BattleShips";
            else
                logo += "|";
        }
        Out(logo, WHITE);
    }
    /// <summary>
    /// Generates The titles of the grids whether it is an enemy fleet or player fleet, this can also generate empty spaces through the DisplayFleetNames bool
    /// </summary>
    /// <param name="DisplayFleetNames"></param>
    static void GenerateFleetCompositions(bool displayFleetNames)
    {
        std::string fleetComposition = "  |";
        for (int i = 0; i < (Controller::GetDifficulty() > 2 ? 5 : (Controller::GetDifficulty() + 1)); i++) // Repeats for each grid depending on the difficulty
        {
            for (int c = 0; c < 2; c++)
            {
                for (int j = 0; j < (displayFleetNames ? CalculateSpaceAmount(i, c) : 7); j++)
                    fleetComposition += " ";
                if (c == 0 && displayFleetNames)
                    if (i == 0 && Controller::GetDifficulty() == 1 || i == 1 && Controller::GetDifficulty() == 2 || i == 2 && Controller::GetDifficulty() == 3) // Checks for the player fleet
                        fleetComposition += "Your Fleet ";
                    else if (currentEnemyFleets[i - 1 > 0 ? i - 1 : 0] == "")
                        fleetComposition += "           ";
                    else
                    {
                        int fleetToGet = 0;
                        fleetComposition += currentEnemyFleets[(i - 1) > 0 ? (i - 1) : 0] + "'s Fleet";
                    }
                else if (c == 0)
                    fleetComposition += "           ";
                else
                    fleetComposition += "|";
            }
        }
        Out(fleetComposition, WHITE);
    }

    static int CalculateSpaceAmount(int gridBeingBuilt, int timesBuilt)
    {
        if (gridBeingBuilt == 0 && Controller::GetDifficulty() == 1 || gridBeingBuilt == 1 && Controller::GetDifficulty() == 2 || gridBeingBuilt == 2 && Controller::GetDifficulty() == 3 || currentEnemyFleets[gridBeingBuilt - 1 > 0 ? gridBeingBuilt - 1 : 0] == "")
        {
            return 7;
        }
        else
        {
            int amountOfSpaces = 0;

            for (int i = 8; i > (currentEnemyFleets[gridBeingBuilt - 1 > 0 ? gridBeingBuilt - 1 : 0].length() / 2); i--) // Adds spaces based upon how long a name is to ensure the name is always centered
                amountOfSpaces++;

            if (currentEnemyFleets[gridBeingBuilt - 1 > 0 ? gridBeingBuilt - 1 : 0].length() % 2 == 0 && timesBuilt == 0)
                amountOfSpaces++;
            
            return amountOfSpaces;
        }
    }
};

