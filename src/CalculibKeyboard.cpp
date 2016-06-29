#include "CalculibKeyboard.h"
#include "CalculibWindow.h"

bool IsKeyUp(int keyCode)
{
    return !IsKeyDown(keyCode);
}

bool IsKeyDown(int keyCode)
{
    if(!CalculibWindow::isFocused())
    {
        return false;
    }
    
    for(int i = 0 ; i < CalculibKeyboard::keymap.getSize() ; i++)
    {
        if(CalculibKeyboard::keymap.get(i).casioKeyId == keyCode)
        {
            return sf::Keyboard::isKeyPressed(CalculibKeyboard::keymap.get(i).SFMLKey);
        }
    }
    printf("WARNING: Key %d is not mapped !\n",keyCode);
    return false;
}

CalculibList<CalculibKeyUnit> CalculibKeyboard::keymap;

void CalculibKeyboard::calculibMapKey(int newKeyCode, sf::Keyboard::Key newSFMLKey)
{
    keymap.add(CalculibKeyUnit(newKeyCode,newSFMLKey));
}