#ifndef _CALCULIB_KEYBOARD_H_
#define _CALCULIB_KEYBOARD_H_

#include "../../CasioLib/keybios.h"
#include "CalculibList.h"

#include <SFML/Window.hpp>

struct CalculibKeyUnit
{
    int casioKeyId;
    sf::Keyboard::Key SFMLKey;
    CalculibKeyUnit(int newCasioKeyId, sf::Keyboard::Key newSFMLKey)
    {
        casioKeyId = newCasioKeyId;
        SFMLKey = newSFMLKey;
    }
};


class CalculibKeyboard
{
public:

    static void calculibMapKey(int newKeyCode, sf::Keyboard::Key newSFMLKey);
    static CalculibList<CalculibKeyUnit> keymap;
};


//casio implementations
bool IsKeyDown(int keyCode);
bool IsKeyUp(int keyCode);


#endif