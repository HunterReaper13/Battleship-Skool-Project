#pragma once

/// <summary>
/// Contains a 2 Dimensional Array of integers to represent board states (No Ship, Allied Ship, Hit, Miss)
/// </summary>
class ScreenMatrix
{
public:
	int data[10][10]; // 2D Array Storing Integers
	/// <summary>
	/// Constructor that fills the array with 0 across the board
	/// </summary>
	ScreenMatrix() : data{ 0 } {}
};

