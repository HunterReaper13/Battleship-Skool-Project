#include "Controller.h"
#include "ScreenMatrix.h"
#include <vector>

std::vector<ScreenMatrix> Controller::grids;
bool Controller::exitGame = false;
int Controller::playerHits = 0;
int Controller::enemyHits = 0;
int Controller::difficulty = 0;
int Controller::lastEnemyXPos = 0;
int Controller::lastEnemyYPos = 0;
int Controller::enemySelected = 0;