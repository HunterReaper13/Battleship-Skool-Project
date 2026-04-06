#include "Weapons.h"

int Weapons::weaponType = 0;
int Weapons::maxWeapons = 4;
int Weapons::cursorPosX = 0;
int Weapons::cursorPosY = 0;
bool Weapons::hasUsedMissile = false;
bool Weapons::hasUsedAirStrike = false;
bool Weapons::hasUsedNuclearBomb = false;
bool Weapons::hasUsedRadar = false;
bool Weapons::validHit = false;
int Weapons::missileUses = 0;
int Weapons::radarUses = 0;
int Weapons::airStrikeUses = 0;
int Weapons::playerShots = 0;