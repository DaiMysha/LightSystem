#include <iostream>
#include <sstream>

#include <SFML/Graphics.hpp>
#include <GL/gl.h>

#include <DMUtils/maths.hpp>
#include <LightSystem/LightSystem.hpp>
#include <LightSystem/SpotLight.hpp>

#define WIDTH   640
#define HEIGHT  480

//font taken from http://www.fontspace.com/melifonts/sweet-cheeks
int main(int argc, char** argv) {

    sf::RenderWindow window(sf::VideoMode(WIDTH, HEIGHT), "SFML testy");
    //bg
    sf::Texture bg;
    if(!bg.loadFromFile("data/map.png")) exit(-1);
    //if(!bg.loadFromFile("data/map2.jpg")) exit(-1);
    //bg.setRepeated(true);
    sf::Sprite bgSpr(bg,sf::IntRect(0,0,WIDTH,HEIGHT));
    bgSpr.setOrigin(sf::Vector2f(WIDTH/2,HEIGHT/2));

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

    sf::View view;
    view.setSize(sf::Vector2f(WIDTH,HEIGHT));

    sf::RectangleShape p(sf::Vector2f(10,10));
    p.setFillColor(sf::Color::Blue);
    p.setPosition(sf::Vector2f(1680,2090));

    int speed = 5;

    DMGDVT::LS::LightSystem ls;
    //1679,1583
    DMGDVT::LS::SpotLight* spot = new DMGDVT::LS::SpotLight(sf::Vector2f(1678,1582),400,sf::Color::Red,0.0f,M_PIf*2.0f,1.0f,5,10,1.0f);
    DMGDVT::LS::SpotLight* spot2 = new DMGDVT::LS::SpotLight(sf::Vector2f(1678,1582),400,sf::Color::Blue,M_PIf/4.0f,M_PIf/4.0f,1.0f,5,10,1.0f);

    ls.addLight(spot);
    ls.addLight(spot2);

    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed)
                window.close();
            else if(event.type == sf::Event::KeyPressed){
                switch(event.key.code) {
                    case sf::Keyboard::Escape:
                    {
                        window.close();
                    } break;
                    case sf::Keyboard::Up :
                    {
                        p.move(0,-speed);
                    } break;
                    case sf::Keyboard::Down :
                    {
                        p.move(0,speed);
                    } break;
                    case sf::Keyboard::Left :
                    {
                        p.move(-speed,0);
                    } break;
                    case sf::Keyboard::Right :
                    {
                        p.move(speed,0);
                    } break;

                    case sf::Keyboard::V :
                    {
                        view.rotate(10);
                    } break;
                    case sf::Keyboard::B :
                    {
                        view.rotate(-10);
                    } break;
                    default: break;
                }
            }
        }
        window.clear(sf::Color::Red);

        int x = p.getPosition().x-WIDTH/2;
        int y = p.getPosition().y-HEIGHT/2;

        bgSpr.setTextureRect(sf::IntRect(x,y,WIDTH,HEIGHT));
        bgSpr.setPosition(p.getPosition());

        sf::View baseView = window.getView();
        view.setCenter(p.getPosition());
        window.setView(view);
        window.draw(bgSpr);
        window.draw(p);
        window.setView(baseView);

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
