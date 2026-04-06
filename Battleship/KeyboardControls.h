#pragma once
#include <Windows.h>
#include <iostream>
// These Conditions Define The Arrow Keys And Enter Key For The Selection Options, Mostly So I Don't Have To Remember The Numbers For Different Keys
#define KEY_LEFT 0x25
#define KEY_RIGHT 0x27
#define KEY_UP 0x26
#define KEY_DOWN 0x28
#define ENTER 0x0D
#define R 0x52
#define ONE 0x31
#define TWO 0x32
#define ESCAPE 0x1B
#define PRESSED 0
#define DEPRESSED 1
#define PRESSEDTHISFRAME 2
#define DEPRESSEDTHISFRAME 3

/// <summary>
/// Checks for keyboard presses and outputs true or false if a key is pressed if the key inputted is pressed or not
/// </summary>
class Input
{
private:
    static int keys[256]; // An array that stores the current key state of each key
    static int old_keys[256]; // An array that stores the key state last cycle
    /// <summary>
    /// Gets the key pressed during the cycle using a keycode as an input
    /// </summary>
    /// <param name="KeyCode">The Key You Want Pressed</param>
    /// <returns>The Key Pressed</returns>
    static bool KeyPressed(int KeyCode) 
    {
        int KeyState = GetKeyState(KeyCode); // Don't ask me what get key state is, i couldn't even figure it out but it works
        if (KeyState == -127 || KeyState == -128) // Registers if a key is pressed
            return true;
        return false;
    }
public:
    /// <summary>
    /// Gets a key pressed statement using a state to see if the key is held, unheld, pressed or depressed and compares the input with the input last cycle
    /// </summary>
    /// <param name="state">Whether the key is held, unheld, pressed or depressed</param>
    /// <param name="keyCode">The key you want to compare against</param>
    /// <returns>True if the key pressed is equal to the keycode and false if not</returns>
    static bool Key(int state, int keyCode) 
    {

        keys[keyCode] = GetKeyState(keyCode);
        switch (state) 
        {
        case PRESSED:
            if (KeyPressed(keyCode))
                return true;
            break;
        case DEPRESSED:
            if (!KeyPressed(keyCode))
                return true;
            break;
        case PRESSEDTHISFRAME:
            if (KeyPressed(keyCode) && keys[keyCode] != old_keys[keyCode])
                return true;
            break;
        case DEPRESSEDTHISFRAME:
            if (!KeyPressed(keyCode) && keys[keyCode] != old_keys[keyCode])
                return true;
            break;
        }
        return false;
    }
    /// <summary>
    /// Updates old_keys
    /// </summary>
    static void SwapKeyBuffer() {
        for (int i = 0; i < std::size(keys); i++) {
            old_keys[i] = keys[i];
        }
    }
    /// <summary>
    /// Scrolls through a menu by using key inputs to detect for up or down keys then enter to return -1
    /// </summary>
    /// <param name="maxOptions"></param>
    /// <param name="selectedOption"></param>
    /// <returns></returns>
    static int MenuScroller(int maxOptions, int &selectedOption)
    {
        if (Key(PRESSEDTHISFRAME, KEY_UP))
        {
            if (selectedOption <= 0)
                selectedOption = maxOptions - 1;
            else
                selectedOption--;
        }
        if (Key(PRESSEDTHISFRAME, KEY_DOWN))
        {
            if (selectedOption >= maxOptions - 1)
                selectedOption = 0;
            else
                selectedOption++;
        }
        if (Key(PRESSEDTHISFRAME, ENTER))
        {
            return -1;
        }
    }
};