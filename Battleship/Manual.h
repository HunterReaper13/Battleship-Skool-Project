#pragma once
#include <iostream>
#include "Renderer.h"
#include "KeyboardControls.h"
#include "ASCIIGenerator.h"
class Manual
{
private:
	static int currentPage;
	static int maxPageCount;
public:
	static void OpenManual()
	{
		system("cls");
		currentPage = 0;
		ChangePage(false);

		while (true)
		{
			if (Input::Key(PRESSEDTHISFRAME, KEY_RIGHT))
			{
				ChangePage(false);
			}
			if (Input::Key(PRESSEDTHISFRAME, KEY_LEFT))
			{
				ChangePage(true);
			}
			if (Input::Key(PRESSEDTHISFRAME, ESCAPE))
			{
				break;
			}
			Input::SwapKeyBuffer();
		}
	}
	static void ChangePage(bool negative)
	{
		system("cls");
		currentPage += negative ? -1 : 1;
		if (currentPage > maxPageCount)
		{
			currentPage = 1;
		}
		else if (currentPage < 1)
		{
			currentPage = maxPageCount;
		}
		DisplayHeader(currentPage);
		DisplayBody(currentPage);
	}
	static void DisplayHeader(int page)
	{
		switch (page)
		{
		case 1:
			ASCIIGenerator::PrintPageOneHeader();
			break;
		case 2:
			ASCIIGenerator::PrintPageTwoHeader();
			break;
		case 3:
			ASCIIGenerator::PrintPageThreeHeader();
			break;
		}
		Renderer::Out("");
	}
	static void DisplayBody(int page)
	{
		switch (page)
		{
		case 1:
			Renderer::Out("Battleship is a two-player, turn-based strategy game where players attempt to sink their opponent’s hidden fleet.");
			Renderer::Out("The player secretly places five ships a carrier, battleship, cruiser, submarine, and destroyer on a grid, then takes turns with the AI to HIT or MISS ships, aiming to sink all opposing ships first");
			Renderer::Out("In this version of battleships the rules work a little differently with you having a unique set of weapons you can use against the enemy fleets that you can acquire from hitting enemy ships.");
			Renderer::Out("As well, on difficulties above easy you will fight several fleets at once so utilizing your weapons correctly is key to success.");
			Renderer::Out("");
			Renderer::Out("x : means an empty square or a square an enemy ship can be hiding behind");
			Renderer::Out("x", YELLOW, false);
			Renderer::Out(" : means a missed shot");
			Renderer::Out("x", RED, false);
			Renderer::Out(" : means a hit shot");
			Renderer::Out("x", CYAN, false);
			Renderer::Out(" : means an ally ship");
			Renderer::Out("x", MAGENTA, false);
			Renderer::Out(" : means an enemy ship is present but not hit");
			Renderer::Out("x", GREEN, false);
			Renderer::Out(" : means your cursor to show where you're currently aiming at");
			break;
		case 2:
			Renderer::Out("The List Of Controls For The Game");
			Renderer::Out("");
			Renderer::Out("Move Cursor -> Arrow Keys");
			Renderer::Out("");
			Renderer::Out("Select Option -> ENTER");
			Renderer::Out("");
			Renderer::Out("Exit Menus/Game -> ESCAPE");
			Renderer::Out("");
			Renderer::Out("Switch Weapons -> 1 Key");
			Renderer::Out("");
			Renderer::Out("Swap Grids -> 2 Key");
			Renderer::Out("");
			Renderer::Out("Rotate Cursor -> R Key");
			break;
		case 3:
			Renderer::Out("The List Of Special Weapons In The Game");
			Renderer::Out("");
			Renderer::Out("Tomahawk Missile - Shoots A 3x3 Grid Around The Cursor Tile, Taking Out All Tiles In The Grid");
			Renderer::Out("");
			Renderer::Out("Radar Shell - Shoots A 5x5 Grid Around The Cursor Tile, Displaying All Ship Tiles As Purple Displaying Where They Are But Not Hitting Them");
			Renderer::Out("");
			Renderer::Out("Airstrike - Shoots A Full Line Across The Board");
			Renderer::Out("");
			Renderer::Out("Nuclear Bomb - Kills A Whole Grid");
			break;
		}

		Renderer::Out("");
		Renderer::Out("");
		if (currentPage + 1 <= maxPageCount)
			Renderer::Out("Press Right Arrow To Go To The Next Page, ", WHITE, false);
		else
			Renderer::Out("Press Right Arrow To Go To The First Page, ", WHITE, false);
		if (currentPage - 1 >= 1)
			Renderer::Out("Press Left Arrow To Go To The Previous Page, ", WHITE, false);
		else
			Renderer::Out("Press Left Arrow To Go To The Last Page, ", WHITE, false);
		Renderer::Out("And Escape To Return To Main Menu");
	}
};

