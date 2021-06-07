#include "pch.h"

#include "Chip8.h"

#include <SFML/Window.hpp>
#include <SFML/Graphics.hpp>

const int SCREEN_WIDTH = 640;
const int SCREEN_HEIGHT = 320;

void debugRender(const Chip8& sys);
void draw(const Chip8& sys, sf::RenderWindow& window);
void pressKey(Chip8& sys, sf::Keyboard::Key key);
void releaseKey(Chip8& sys, sf::Keyboard::Key key);

int main(int argc, char** argv)
{

  if (argc < 2)
  {
    std::cout << "ERROR: please provide the filepath to a valid ROM\n";
    return -1;
  }

  Chip8 sys;
  if (!sys.loadROM(argv[1]))
  {
    std::cout << "Failed to load ROM!\n";
    return -1;
  }

  sf::RenderWindow window(sf::VideoMode(SCREEN_WIDTH, SCREEN_HEIGHT), "Chip8");
  // window.setVerticalSyncEnabled(true);
  window.setFramerateLimit(60);

  while(window.isOpen())
  {
    // handle events
    sf::Event event;
    while(window.pollEvent(event))
    {
      switch(event.type)
      {
        case sf::Event::Closed:
          window.close();
        break;

        case sf::Event::KeyPressed:
          pressKey(sys, event.key.code);
        break;

        case sf::Event::KeyReleased:
          releaseKey(sys, event.key.code);
        break;
        
        default:
        break;

      }
    }

    // render
    if (sys.drawFlag)
    {
      draw(sys, window);
      // debugRender(sys);
      sys.drawFlag = false;
    }

    sys.executeCycle();
  }

  return 0;
}

void draw(const Chip8& sys, sf::RenderWindow& window)
{
  window.clear(sf::Color::Black);
  sf::RectangleShape pixel(sf::Vector2f(10.f, 10.f));
  pixel.setFillColor(sf::Color::White);
  
  for (int i = 0; i < 64; i++)
  {
    for (int j = 0; j < 32; j++)
    {
      if (!sys.display[i][j])
        continue;

      pixel.setPosition(sf::Vector2f(i*10.f, j*10.f));
      window.draw(pixel);
    }
  }

  window.display();
}

void debugRender(const Chip8& sys)
{
  for (int j = 0; j < 32; j++)
  {
    for (int i = 0; i < 64; i++)
    {
      std::cout << (int)sys.display[i][j] << ' ';
    }
    std::cout << '\n';
  }
  
}

void pressKey(Chip8& sys, sf::Keyboard::Key key)
{
  switch(key)
  {
    case sf::Keyboard::Num1: sys.keys[0x1] = 1; break; 
    case sf::Keyboard::Num2: sys.keys[0x2] = 1; break;
    case sf::Keyboard::Num3: sys.keys[0x3] = 1; break;
    case sf::Keyboard::Num4: sys.keys[0xC] = 1; break;
  
    case sf::Keyboard::Q: sys.keys[0x4] = 1; break; 
    case sf::Keyboard::W: sys.keys[0x5] = 1; break;
    case sf::Keyboard::E: sys.keys[0x6] = 1; break;
    case sf::Keyboard::R: sys.keys[0xD] = 1; break;
  
    case sf::Keyboard::A: sys.keys[0x7] = 1; break; 
    case sf::Keyboard::S: sys.keys[0x8] = 1; break;
    case sf::Keyboard::D: sys.keys[0x9] = 1; break;
    case sf::Keyboard::F: sys.keys[0xE] = 1; break;
  
    case sf::Keyboard::Z: sys.keys[0xA] = 1; break; 
    case sf::Keyboard::X: sys.keys[0x0] = 1; break;
    case sf::Keyboard::C: sys.keys[0xB] = 1; break;
    case sf::Keyboard::V: sys.keys[0xF] = 1; break;  
  }
}

void releaseKey(Chip8& sys, sf::Keyboard::Key key)
{
  switch(key)
  {
    case sf::Keyboard::Num1: sys.keys[0x1] = 0; break; 
    case sf::Keyboard::Num2: sys.keys[0x2] = 0; break;
    case sf::Keyboard::Num3: sys.keys[0x3] = 0; break;
    case sf::Keyboard::Num4: sys.keys[0xC] = 0; break;
  
    case sf::Keyboard::Q: sys.keys[0x4] = 0; break; 
    case sf::Keyboard::W: sys.keys[0x5] = 0; break;
    case sf::Keyboard::E: sys.keys[0x6] = 0; break;
    case sf::Keyboard::R: sys.keys[0xD] = 0; break;
  
    case sf::Keyboard::A: sys.keys[0x7] = 0; break; 
    case sf::Keyboard::S: sys.keys[0x8] = 0; break;
    case sf::Keyboard::D: sys.keys[0x9] = 0; break;
    case sf::Keyboard::F: sys.keys[0xE] = 0; break;
  
    case sf::Keyboard::Z: sys.keys[0xA] = 0; break; 
    case sf::Keyboard::X: sys.keys[0x0] = 0; break;
    case sf::Keyboard::C: sys.keys[0xB] = 0; break;
    case sf::Keyboard::V: sys.keys[0xF] = 0; break;  
  }
}
