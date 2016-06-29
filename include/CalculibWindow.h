#ifndef _CALCULIB_WINDOW_H_
#define _CALCULIB_WINDOW_H_

#include <SFML/Graphics.hpp>

class CalculibWindow
{
#ifdef CALCULIB
public:
    static void initWindow();
    static void drawVramOnScreen();
    static void clearScreen();
    static void setScreenZoom(int newScreenZoom);
    static void handleEvent();
    static bool isFocused();

private:
    static int screenZoom;
    static sf::RenderWindow * screenWindow;
    static sf::Texture * screenTexture;
    static sf::Sprite * screenSprite;
    static void updateScreenZoom();
    static bool focused;
#endif
};


#endif //_CALCULIB_WINDOW_H_