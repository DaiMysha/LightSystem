#include <iostream>
#include <sstream>

#include <SFML/Graphics.hpp>

#include <DMUtils/maths.hpp>

#include "Player.hpp"
#include "LightSystem/LightSystem.hpp"
#include "LightSystem/SpotLight.hpp"

#define WIDTH   640
#define HEIGHT  480

/***************** TODO LIST ********************
* move the sfUtils to an appropriate project
* same for math
* see if you can use viewports instead of mass -screen
* move center and drawAABB to Light
************************************************/

int main(int argc, char** argv) {

    sf::RenderWindow window(sf::VideoMode(WIDTH, HEIGHT), "SFML testy");
    sf::CircleShape shape(12.f);
    shape.setFillColor(sf::Color::Blue);

    //bg
    sf::Texture bg;
    if(!bg.loadFromFile("data/map.png")) exit(-1);
    //bg.setRepeated(true);
    sf::Sprite bgSpr(bg);
    bgSpr.setTextureRect(sf::IntRect(0,0,WIDTH,HEIGHT));

    Player p(1670,2088,2);
    shape.setPosition(WIDTH/2-2,HEIGHT/2-2);

    int fps = 0;
    int elapsedFrames = 0;
    sf::Clock clock, pclock;

    sf::Font font;

    if(!font.loadFromFile("data/Sweet Cheeks.ttf")) exit(-1); //because yes

    sf::Text text;
    text.setFont(font);
    text.setCharacterSize(18);
    text.setPosition(580,10);
    text.setString("0");
    text.setColor(sf::Color::White);

    DMGDVT::LS::LightSystem ls;
    //1679,1583
    DMGDVT::LS::SpotLight* spot = new DMGDVT::LS::SpotLight(sf::Vector2f(1678,1582),400,sf::Color::Red,0.0f,M_PIf*2.0f,1.0f,5,10,1.0f);
    DMGDVT::LS::SpotLight* spot2 = new DMGDVT::LS::SpotLight(sf::Vector2f(1678,1582),400,sf::Color::Blue,M_PIf/4.0f,M_PIf/4.0f,1.0f,5,10,1.0f);

    ls.addLight(spot);
    ls.addLight(spot2);

    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) window.close();
            else if(event.type == sf::Event::KeyPressed || event.type == sf::Event::KeyReleased) {
                bool down = (event.type == sf::Event::KeyPressed);
                switch(event.key.code) {
                    case sf::Keyboard::Up: p.set_keyDown(Player::KEY_UP, down); break;
                    case sf::Keyboard::Down: p.set_keyDown(Player::KEY_DOWN, down); break;
                    case sf::Keyboard::Left: p.set_keyDown(Player::KEY_LEFT, down); break;
                    case sf::Keyboard::Right: p.set_keyDown(Player::KEY_RIGHT, down); break;
                    case sf::Keyboard::Escape: window.close(); break;
                    default: break;
                }
            }
        }

        if(pclock.getElapsedTime().asMilliseconds() > 16) {
            p.move();
            pclock.restart();
        }
        //calculates where the sprite has to be blit
        int x = p.get_x() - WIDTH/2;
        int y = p.get_y() - HEIGHT/2;
        bgSpr.setTextureRect(sf::IntRect(x,y,x+WIDTH,y+WIDTH));
        //draw
        window.clear();
        window.draw(bgSpr);
        window.draw(shape);

        ls.render(x,y,WIDTH,HEIGHT,window);
        ls.drawAABB(x,y,WIDTH,HEIGHT,window);

        window.draw(text);

        window.display();

        //sf::sleep(sf::milliseconds(16));
        ++elapsedFrames;
        if(clock.getElapsedTime().asMilliseconds() > 500) {
            fps = elapsedFrames;
            elapsedFrames = 0;
            clock.restart();
            //for some reason my compiler doesn't find to_string so..
            std::ostringstream str;
            str << (fps*2);
            text.setString(str.str());
        }
    }

    return 0;
}
