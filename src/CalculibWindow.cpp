#include "CalculibWindow.h"
#include "CalculibGraphics.h"


int CalculibWindow::screenZoom;
sf::RenderWindow * CalculibWindow::screenWindow;
sf::Texture * CalculibWindow::screenTexture;
sf::Sprite * CalculibWindow::screenSprite;
bool CalculibWindow::focused;

void CalculibWindow::initWindow()
{
    screenZoom = 3;
    
    screenWindow = new sf::RenderWindow(sf::VideoMode((unsigned int)(128*screenZoom),(unsigned int)(64*screenZoom)), "Screen");
    
    screenTexture = new sf::Texture();
    screenTexture->create(128,64);
    
    screenSprite = new sf::Sprite(*screenTexture);
    screenSprite->setScale(screenZoom,screenZoom);
    
    focused = true;
}
void CalculibWindow::drawVramOnScreen()
{
    sf::Uint8 rawTextureData[128*64*4]; // RGBA buffer
    for(int i = 0 ; i < 128*64 ; i++) // each pixel
    {
        int byteId = i/8;
        int bitShift = 7-(i&7); //invert byte order
        rawTextureData[i*4+0] = ~(255 * ((CalculibGraphics::VRAM[byteId]>>bitShift) & 1)); //R
        rawTextureData[i*4+1] = ~(255 * ((CalculibGraphics::VRAM[byteId]>>bitShift) & 1)); //G
        rawTextureData[i*4+2] = ~(255 * ((CalculibGraphics::VRAM[byteId]>>bitShift) & 1)); //B
        rawTextureData[i*4+3] = 255; //5
    }
    screenTexture->update(rawTextureData);
    screenWindow->draw(*screenSprite);
    screenWindow->display();
}
void CalculibWindow::clearScreen()
{
    screenWindow->clear(sf::Color(255, 255, 255, 255));
}
void CalculibWindow::setScreenZoom(int newScreenZoom)
{
    int oldZoom = screenZoom;
    screenZoom = newScreenZoom;
    if(oldZoom != screenZoom)
    {
        updateScreenZoom();
    }
}

void CalculibWindow::updateScreenZoom()
{
    screenWindow->setSize(sf::Vector2u((unsigned int)(128*screenZoom),(unsigned int)(64*screenZoom)));
    screenSprite->setScale(screenZoom,screenZoom);
}

void CalculibWindow::handleEvent()
{
    sf::Event event;
    while (screenWindow->pollEvent(event))
    {
        if(event.type == sf::Event::Closed)
        {
            //TODO: kill network thread !
            exit(0);
        }
        else if(event.type == sf::Event::LostFocus)
        {
            focused = false;
        }
        else if(event.type == sf::Event::GainedFocus)
        {
            focused = true;
        }
    }
}

bool CalculibWindow::isFocused()
{
    return focused;
}