/*
Copyright DaiMysha (c) 2015, All rights reserved.
DaiMysha@gmail.com
https://github.com/DaiMysha

Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.

Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation
and/or other materials provided with the distribution.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS
BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE
GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

*/

#include <iostream>
#include <sstream>

#include <SFML/Graphics.hpp>
#include <GL/gl.h>

#include <DMUtils/maths.hpp>
#include <DMUtils/sfml.hpp>
#include <LightSystem/LightSystem.hpp>
#include <LightSystem/SpotLight.hpp>
#include <LightSystem/LocalAmbiantLight.hpp>
#include <LightSystem/EmissiveLight.hpp>

#define WIDTH   640
#define HEIGHT  480

//font taken from http://www.fontspace.com/melifonts/sweet-cheeks
int main(int argc, char** argv) {

    /** SFML STUFF **/

    sf::RenderWindow window(sf::VideoMode(WIDTH, HEIGHT), "LightSystem test");

    bool debugLightMapOnly = false;
    bool aabb = false;
    bool debugUseShader = true;
    bool debugDrawLights = true;
    //bg
    sf::Texture bg;
    if(!bg.loadFromFile("data/map.png")) exit(-1);

    sf::Texture emissiveSpriteTexture;
    if(!emissiveSpriteTexture.loadFromFile("data/emissive.png")) exit(-2);

    sf::Sprite bgSpr(bg,sf::IntRect(0,0,WIDTH,HEIGHT));
    bgSpr.setOrigin(sf::Vector2f(WIDTH/2,HEIGHT/2));

    sf::Sprite emissiveSprite(emissiveSpriteTexture);

    int fps = 0;
    int elapsedFrames = 0;
    sf::Clock clock, flickerClock;

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
    p.setOrigin(5,5);

    int speed = 5;

    sf::Vector2i mouseInt = sf::Mouse::getPosition(window);
    sf::Vector2f mouse(window.mapPixelToCoords(mouseInt));

    sf::ConvexShape ambiantShape;
    ambiantShape.setPointCount(12);
    //position : 1440,1408
    //480,350
    ambiantShape.setPoint(0,sf::Vector2f(0.0f,0.0f));
    ambiantShape.setPoint(1,sf::Vector2f(289.0f,0.0f));
    ambiantShape.setPoint(2,sf::Vector2f(289.0f,63.0f));
    ambiantShape.setPoint(3,sf::Vector2f(352.0f,63.0f));
    ambiantShape.setPoint(4,sf::Vector2f(352.0f,225.0f));
    ambiantShape.setPoint(5,sf::Vector2f(289.0f,225.0f));
    ambiantShape.setPoint(6,sf::Vector2f(289.0f,288.0f));
    ambiantShape.setPoint(7,sf::Vector2f(0.0f,288.0f));
    ambiantShape.setPoint(8,sf::Vector2f(0.0f,225.0f));
    ambiantShape.setPoint(9,sf::Vector2f(-63.0f,225.0f));
    ambiantShape.setPoint(10,sf::Vector2f(-63.0f,63.0f));
    ambiantShape.setPoint(11,sf::Vector2f(0.0f,63.0f));

    sf::ConvexShape negativeShape;
    negativeShape.setPointCount(4);
    negativeShape.setPoint(0,sf::Vector2f(0.0f,0.0f));
    negativeShape.setPoint(1,sf::Vector2f(417.0f,0.0f));
    negativeShape.setPoint(2,sf::Vector2f(417.0f,290.0f));
    negativeShape.setPoint(3,sf::Vector2f(0.0f,290.0f));

    /** LIGHTSYSTEM EXAMPLE **/

    //create your LightSystem
    //one per game is usually enough
    DMGDVT::LS::LightSystem ls;
    //change ambiant light
    ls.setAmbiantLight(sf::Color(15,0,60));
    //the lightSystem needs to be aware of the view you're using to properly draw the lights
    ls.setView(view);

    //Let's create a bunch of lights now
    //the lights HAVE to be dynamically allocated. The LightSystem destroys them for you when it's destroyed
    //you can change that using LightSystem::setAutoDelete
    //if you do that you have to take care of the deletion yourself so be careful
    //do NOT destroy a light that hasn't been removed yet, it will cause a segfault

    //to ensure R + G + B = W
    DMGDVT::LS::SpotLight* spotRed =  new DMGDVT::LS::SpotLight(sf::Vector2f(1072,1678),200,sf::Color::Red, 0.0f       ,M_PIf*2.0f,1.0f,0.5f,1.0f);
    DMGDVT::LS::SpotLight* spotBlue = new DMGDVT::LS::SpotLight(sf::Vector2f(1272,1678),200,sf::Color::Blue,0.0f       ,M_PIf*2.0f,1.0f,0.5f,1.0f);
    DMGDVT::LS::SpotLight* spotGreen = new DMGDVT::LS::SpotLight(sf::Vector2f(1172,1578),200,sf::Color::Green,0.0f      ,M_PIf*2.0f,1.0f,0.5f,1.0f);
    DMGDVT::LS::SpotLight* negativeColors = new DMGDVT::LS::SpotLight(sf::Vector2f(1172,1628),300,sf::Color::Red,0.0f      ,M_PIf*2.0f,-1.0f,5.0f,5.0f);

    //looks at the player, shows that you don't need to update a light if you're just rotating it around
    DMGDVT::LS::SpotLight* eyeSpotLeft = new DMGDVT::LS::SpotLight(sf::Vector2f(1520,1871),300,sf::Color::White,-M_PIf/4.0f ,M_PIf/5.0f,0.5f,1.0f,1.5f);
    DMGDVT::LS::SpotLight* eyeSpotRight = new DMGDVT::LS::SpotLight(sf::Vector2f(1840,1871),300,sf::Color::White,M_PIf/4.0f ,M_PIf/5.0f,0.5f,1.0f,1.5f);

    DMGDVT::LS::SpotLight* sunRise = new DMGDVT::LS::SpotLight(sf::Vector2f(1679,2200),500,sf::Color(245,125,20),M_PIf ,M_PIf/3.0f,1.0f,0.0f,2.0f);

    //flickering light. Something for dynamic lights is planned for later, but for now the code later shows how to do it
    DMGDVT::LS::SpotLight* firePit1 = new DMGDVT::LS::SpotLight(sf::Vector2f(1584,1166),200,sf::Color(210,115,10),0.0f ,M_PIf*2.0f,1.0f,0.1f,1.0f);
    DMGDVT::LS::SpotLight* firePit2 = new DMGDVT::LS::SpotLight(sf::Vector2f(1775,1166),200,sf::Color(210,115,10),0.0f ,M_PIf*2.0f,1.0f,0.1f,1.0f);

    //just some more lights to test a few things
    DMGDVT::LS::SpotLight* lamp = new DMGDVT::LS::SpotLight(sf::Vector2f(2160,1583),200,sf::Color::White,0.0f ,M_PIf*2.0f,1.0f,0.0f,0.50f);
    DMGDVT::LS::SpotLight* hugeSpot = new DMGDVT::LS::SpotLight(sf::Vector2f(2845,1245),800,sf::Color::White,M_PIf/2.0f ,M_PIf/10.0f,1.0f,0.0f,2.0f);

    //one negative spot in the room with the ceiling white light
    DMGDVT::LS::SpotLight* negativeSpot = new DMGDVT::LS::SpotLight(sf::Vector2f(2366,1440),300,sf::Color(127,127,127),M_PIf/4.0f ,M_PIf/4.0f,-1.0f,0.0f,2.0f);

    //template add example
    //also follows the player around, showing you don't need to update a light if you're just moving it around
    DMGDVT::LS::SpotLight* playerLight = ls.addLight<DMGDVT::LS::SpotLight>(p.getPosition(),200,sf::Color::Yellow);

    //local ambiant lights are useful for example to make a difference between day and night
    DMGDVT::LS::LocalAmbiantLight* localAmbiant = new DMGDVT::LS::LocalAmbiantLight(sf::Vector2f(1535,1439),ambiantShape,sf::Color::Red);
    //they can also be negative
    DMGDVT::LS::LocalAmbiantLight* negativeAmbiant = new DMGDVT::LS::LocalAmbiantLight(sf::Vector2f(991,1087),negativeShape,sf::Color::Green,true);

    //Example of emissive lights. Emissive lights are just a white sprite on a transparent background that are drawn with a different color above everything else
    //the sprite is copied and stored locally
    //emissive lights CANNOT be negative
    //but they can be updated at any moment on any parameter without any cost
    //still need to call the LightSystem::update(Light*) on it
    DMGDVT::LS::EmissiveLight* emissive = new DMGDVT::LS::EmissiveLight(sf::Vector2f(2368,1592),sf::Color(100,255,255),M_PIf/2.0f,emissiveSprite);

    //this parameter allows you to change the way textures are resized when a call to LightSystem::update(Light*) is done
    //with this set to true, the texture will only be resized when the new required size is greater than the current allocated size
    //with this set to false (default), the texture is always reallocated as long as the new light radius is different than the previous one
    //this loses some place on the graphic card but increases performance
    //especially useful set to true for lights that oscillate between two radius, like this flickering light
    //that goes from 180 to 200 in a wave
    firePit1->setResizeWhenIncrease(true);

    //add them all to the LightSystem
    //except the playerLight, since it's been added by the template function
    ls.addLight(spotRed);
    ls.addLight(spotBlue);
    ls.addLight(spotGreen);
    ls.addLight(negativeColors);//you can add them anywhere, not just at the end
    ls.addLight(eyeSpotLeft);
    ls.addLight(eyeSpotRight);
    ls.addLight(sunRise);
    ls.addLight(firePit1);
    ls.addLight(firePit2);
    ls.addLight(lamp);
    ls.addLight(hugeSpot);
    ls.addLight(negativeSpot);
    ls.addLight(localAmbiant);
    ls.addLight(negativeAmbiant);
    ls.addLight(emissive);

    //Modify a light
    //if you change its direcionAngle or its position, it doesn't need to be updated
    playerLight->setDirectionAngle(M_PIf);
    //if you modify ANY of the parameters below, you have to update the light's texture using ls.update(Light*);
    //otherwise the update won't be taken into account
    //basically any parameter except for directionAngle and position
    playerLight->setLinearity(2.0f);
    playerLight->setBleed(0.0f);
    playerLight->setSpreadAngle(M_PIf/3.0f);
    playerLight->setColor(sf::Color::White);
    playerLight->setIntensity(1.0f);
    playerLight->setRadius(200);
    ls.update(playerLight);

    //the loop
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
                    case sf::Keyboard::F1 :
                    {
                        aabb = !aabb;
                    } break;
                    case sf::Keyboard::F2 :
                    {
                        debugLightMapOnly = !debugLightMapOnly;
                    } break;
                    case sf::Keyboard::F3 :
                    {
                        debugUseShader = !debugUseShader;
                    } break;
                    case sf::Keyboard::F4 :
                    {
                        debugDrawLights = !debugDrawLights;
                    } break;
                    case sf::Keyboard::F:
                    {
                        //see above, this parameter requires an update of the light's internal texture
                        //shows you how to turn a light ON and OFF easily
                        //in this particular case, you wouldn't need to update the texture
                        //since the lights aren't drawn if their intensity is 0
                        playerLight->setIntensity(1.0f - playerLight->getIntensity());
                        ls.update(playerLight);
                    } break;
                    case sf::Keyboard::S:
                    {
                        //this parameter requires an update
                        if(playerLight->getSpreadAngle()==2.0f*M_PIf) {
                            playerLight->setSpreadAngle(M_PIf/3.0f);
                            playerLight->setRadius(200);
                        } else {
                            playerLight->setSpreadAngle(2.0*M_PIf);
                            playerLight->setRadius(100);
                        }
                        ls.update(playerLight);
                    } break;
                    default: break;
                }
            }
        }

        mouseInt = sf::Mouse::getPosition(window);
        mouse = window.mapPixelToCoords(mouseInt);

        //example of code allowing you to make a light following your cursor centered on your character
        //my character is fixed on the screen at {w/2,h/2}
        //for something cleaner you might want to be using window.mapCoordsToPixel(p.getPosition()) instead
        playerLight->setDirectionAngle(DMUtils::sfml::getAngleBetweenVectors(sf::Vector2f(0.0f,1.0f),mouse-sf::Vector2f(WIDTH/2.0f,HEIGHT/2.0f)));
        playerLight->setPosition(p.getPosition());

        //shows you how to follow an object of the map
        eyeSpotLeft->setDirectionAngle(DMUtils::sfml::getAngleBetweenVectors(sf::Vector2f(0.0f,1.0f),p.getPosition() - eyeSpotLeft->getPosition()));
        eyeSpotRight->setDirectionAngle(DMUtils::sfml::getAngleBetweenVectors(sf::Vector2f(0.0f,1.0f),p.getPosition() - eyeSpotRight->getPosition()));

        //basic drawing stuff
        int x = p.getPosition().x-WIDTH/2;
        int y = p.getPosition().y-HEIGHT/2;

        bgSpr.setTextureRect(sf::IntRect(x,y,WIDTH,HEIGHT));
        bgSpr.setPosition(p.getPosition());

        window.clear();

        sf::View baseView = window.getView();

        view.setCenter(p.getPosition());

        //it is EXTREMELY IMPORTANT that you use the LightSystem::draw INSIDE your view
        window.setView(view);
            window.draw(bgSpr);
            window.draw(p);

            int flags = 0;
            if(debugLightMapOnly) flags |= DMGDVT::LS::LightSystem::DebugFlags::LightMap_only;
            if(!debugUseShader) flags |= DMGDVT::LS::LightSystem::DebugFlags::Shader_off;

            //use LightSystem::render if not using debug
            //if using debugRender, the flags allow you to modify the way the lights are drawn
            ls.debugRender(view,window,flags);
            if(debugDrawLights) ls.draw(view,window);
            //draws the light's AABB
            if(aabb) ls.drawAABB(view,window);

        window.setView(baseView);

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


            //also use this timer to alternate the negative light to a positive light
            //you can change the positivity of a light just by changing its intensity
            //this however requires an update of the light
            if(negativeSpot->isNegative()) negativeSpot->setIntensity(1.0f);
            else negativeSpot->setIntensity(-1.0f);
            ls.update(negativeSpot);

            //you can easily switch a light on and off with this function
            //and it doesn't require an update of the light
            hugeSpot->setActive(!hugeSpot->isActive());

            //this light alternates between a few colors to show the effect of a negative light
            sf::Color c = negativeColors->getColor();
            if(c==sf::Color::Red) negativeColors->setColor(sf::Color::Green);
            if(c==sf::Color::Green) negativeColors->setColor(sf::Color::Blue);
            if(c==sf::Color::Blue) negativeColors->setColor(sf::Color(127,127,127));
            if(c==sf::Color(127,127,127)) negativeColors->setColor(sf::Color::Black);
            if(c==sf::Color::Black) negativeColors->setColor(sf::Color::Red);
            ls.update(negativeColors);
            c = negativeColors->getColor();
        }
        //this is an example of how to make a light flicker
        if(flickerClock.getElapsedTime().asMilliseconds() > 100) {
            flickerClock.restart();
            if(firePit1->getRadius() == 200) {
                firePit1->setRadius(180);
                firePit2->setRadius(180);
            } else {
                firePit1->setRadius(200);
                firePit2->setRadius(200);
            }
            ls.update(firePit1);
            ls.update(firePit2);
        }
    }


    return 0;
}
