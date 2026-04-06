#include "Renderer.h"
#include "Weapons.h"
#include <array>

std::string Renderer::currentEnemyFleets[4] = {""};
bool Renderer::debugMode;
/// <summary>
   /// Generates the full grid printing both your board and the enemies boards with a nice table surrounding it
   /// </summary>
   /// <param name="xPos">Cursors x position</param>
   /// <param name="yPos">Cursors y position</param>
void Renderer::GenerateGrid(int xPos, int yPos)
{
    char letter = 'A';
    char enemyLetter = 'A';
    for (int i = 0; i < 2; i++)
        for (int j = 0; j < (i == 0 ? yPos : Controller::GetLastEnemyShotPosition(true)); j++)
            (i == 0 ? letter : enemyLetter)++;

    std::string gridPos = letter + std::to_string(xPos + 1);
    std::string enemyGridPos = enemyLetter + std::to_string(Controller::GetLastEnemyShotPosition(false) + 1);

    Renderer::GenerateTableLine();
    Renderer::GenerateLogo();
    Renderer::GenerateTableLine();
    for (int i = 0; i < 2; i++)
    {
        if (i == 0)
            Renderer::GenerateFleetCompositions(true);
        else
            Renderer::GenerateFleetCompositions(false);
    }
    for (int i = 0; i < 10; i++)
    {
        if (Controller::GetDifficulty() > 1)
        {
            Out("  |   ", WHITE, false);
            DrawLine(*Controller::GetGrid(1), i, debugMode);
            if (Controller::GetDifficulty() > 2)
            {
                Out("  |   ", WHITE, false);
                DrawLine(*Controller::GetGrid(2), i, debugMode);
            }
        }
        Out("  |   ", WHITE, false);
        DrawLine(*Controller::GetGrid(0), i, debugMode);
        if (Controller::GetDifficulty() > 1)
        {
            Out("  |   ", WHITE, false);
            DrawLine(*Controller::GetGrid((Controller::GetDifficulty() > 2) ? 3 : 2), i, debugMode);
            if (Controller::GetDifficulty() > 2)
            {
                Out("  |   ", WHITE, false);
                DrawLine(*Controller::GetGrid(4), i, debugMode);
            }
        }
        else
        {
            Out("  |   ", WHITE, false);
            DrawLine(*Controller::GetGrid(1), i, debugMode);
        }
        Out("  |   ", WHITE);
    }

    Renderer::GenerateFleetCompositions(false);
    Renderer::GenerateTableLine();

    if (debugMode)
        Out(std::to_string(xPos) + "," + std::to_string(yPos), MAGENTA); // Prints out the coordinates of the currently selected tile
    else
        Out("  Selected Tile: " + gridPos); // Prints out the coordinates of the currently selected tile
    Out("");
    Out("  Selected Weapon Type Is: " + Weapons::DisplayWeaponType());
    Out("");
    Out("  Enemy's Last Shot Position: " + enemyGridPos);
    Out("");
    if (!(Controller::GetEnemyHits() % 5) && Controller::GetEnemyHits() != 0 && Controller::GetEnemyHits() <= 25)
    {
        Out("  New Weapon Unlocked");
        Out("");
    }
    if (Controller::GetDifficulty() > 1)
        Out("  Arrow Keys To Move, R To Rotate, ENTER To Select, 1 To Swap Shell Type, 2 To Change Fleet, ESCAPE To Quit");
    else
        Out("  Arrow Keys To Move, R To Rotate, ENTER To Select, 1 To Swap Shell Type, ESCAPE To Quit");
    Out("");
    if (Controller::GetDifficulty() > 1)
        Out("  The Enemy Fleets Will Take In Turns To Shoot Using Their Own Hit Maps As Reference For Places To Aim");
    Out("");
    if (Controller::GetDifficulty() == 3)
        Out("  On Hard You Get 2 Uses For Each Weapon Type Except Nuclear Bomb, You Also Have 2 Shots For Every Enemy Shot");
    Out("");
}