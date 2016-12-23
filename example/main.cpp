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
#include <iomanip>
#include <sstream>

#include <SFML/Graphics.hpp>

#ifdef _WIN32
#include <windows.h>
#endif
#include <GL/gl.h>

#include <DMUtils/maths.hpp>
#include <DMUtils/sfml.hpp>
#include <LightSystem/LightSystem.hpp>
#include <LightSystem/SpotLight.hpp>
#include <LightSystem/FlashLight.hpp>
#include <LightSystem/LocalAmbiantLight.hpp>
#include <LightSystem/SpriteLight.hpp>

#define WIDTH   640
#define HEIGHT  600

//moved to bottom of file for easy reading of the main light handling code
void addWalls(dm::ls::LightSystem& ls);

//font taken from http://www.fontspace.com/melifonts/sweet-cheeks
int main(int argc, char** argv)
{
    /** SFML STUFF **/

    sf::RenderWindow window(sf::VideoMode(WIDTH, HEIGHT), "LightSystem test");

    bool debugLightMapOnly = false;
    bool aabb = false;
    bool debugUseShader = true;
    bool debugDrawLights = true;
    bool debugDrawWalls = true;
    //bg
    sf::Texture bg;
    if(!bg.loadFromFile("data/map.png"))
    {
        std::cerr << "Missing 'data/map.png'" << std::endl;
        exit(-1);
    }

    sf::Texture emissiveSpriteTexture;
    if(!emissiveSpriteTexture.loadFromFile("data/emissive.png"))
    {
        std::cerr << "Missing 'data/emissive.png'" << std::endl;
        exit(-2);
    }

    sf::Sprite bgSpr(bg,sf::IntRect(0,0,WIDTH,HEIGHT));
    bgSpr.setOrigin(sf::Vector2f(WIDTH/2,HEIGHT/2));

    sf::Sprite emissiveSprite(emissiveSpriteTexture);

    int fps = 0;
    int elapsedFrames = 0;
    sf::Clock clock, flickerClock;

    sf::Font font;

    if(!font.loadFromFile("data/Sweet Cheeks.ttf"))
    {
        std::cerr << "Missing 'data/Sweet Cheeks.ttf'" << std::endl;
        exit(-1); //because yes
    }

    sf::Text text;
    text.setFont(font);
    text.setCharacterSize(18);
    text.setPosition(560,10);
    text.setString("0");
    text.setColor(sf::Color::White);

    sf::View view;
    view.setSize(sf::Vector2f(WIDTH,HEIGHT));

    //Small rectangle representing player position
    sf::RectangleShape p(sf::Vector2f(10,10));
    p.setFillColor(sf::Color::Blue);
    p.setPosition(sf::Vector2f(1680,2090));
    p.setOrigin(5,5);

    int speed = 5;

    sf::Vector2i mouseInt = sf::Mouse::getPosition(window);
    sf::Vector2f mouse(window.mapPixelToCoords(mouseInt));

    sf::ConvexShape ambiantShape;
    ambiantShape.setPointCount(12);

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
    //you can create more than one if needed
    //for example you have several layers appearing (first floor and second floor for example)
    dm::ls::LightSystem ls;
    //change ambiant light
    //the ambiant light is the default light that will appear when no other light is added to the light system
    //an ambiant light set to White will show everything normally and an ambiant light set to black will hide everything
    //here we're trying to replicate night ambiant color
    ls.setAmbiantLight(sf::Color(15,0,60));
    //the lightSystem needs to be aware of the view you're using to properly draw the lights
    ls.setView(view);

    //Create walls that block the light
    addWalls(ls);

    //Let's create a bunch of lights now
    //the lights HAVE to be dynamically allocated. The LightSystem destroys them for you when it's destroyed
    //you can change that using LightSystem::setAutoDelete (in which case you don't need to have them manually allocated if you're careful about scopes)
    //if you do that you have to take care of the deletion yourself so be careful
    //do NOT destroy a light that hasn't been removed yet, it will cause a segfault

    //here we use the constructor with all the parameters but it's probably easier to read if you use the setters
    //the result is the same as long as you set all parameters before adding the light to your LightSystem

    //to ensure R + G + B = W
    dm::ls::SpotLight* spotRed =        new dm::ls::SpotLight(sf::Vector2f(1072,1678),200,sf::Color::Red,   0.0f, 180.0f*2.0f,  1.0f, 0.5f, 1.0f);
    dm::ls::SpotLight* spotBlue =       new dm::ls::SpotLight(sf::Vector2f(1272,1678),200,sf::Color::Blue,  0.0f, 180.0f*2.0f,  1.0f, 0.5f, 1.0f);
    dm::ls::SpotLight* spotGreen =      new dm::ls::SpotLight(sf::Vector2f(1172,1578),200,sf::Color::Green, 0.0f, 180.0f*2.0f,  1.0f, 0.5f, 1.0f);
    dm::ls::SpotLight* negativeColors = new dm::ls::SpotLight(sf::Vector2f(1172,1628),300,sf::Color::Red,   0.0f, 180.0f*2.0f, -1.0f, 5.0f, 5.0f);

    //looks at the player, shows that you don't need to update a light if you're just rotating it around
    dm::ls::SpotLight* eyeSpotLeft = new dm::ls::SpotLight(sf::Vector2f(1520,1871),300,sf::Color::White,-180.0f/4.0f ,180.0f/5.0f,0.5f,1.0f,1.5f);
    dm::ls::SpotLight* eyeSpotRight = new dm::ls::SpotLight(sf::Vector2f(1840,1871),300,sf::Color::White,180.0f/4.0f ,180.0f/5.0f,0.5f,1.0f,1.5f);

    dm::ls::SpotLight* sunRise = new dm::ls::SpotLight(sf::Vector2f(1679,2200),500,sf::Color(245,125,20),180.0f ,180.0f/3.0f,1.0f,0.0f,2.0f);

    //flickering light. Something for dynamic lights is planned for later, but for now the code later shows how to do it
    dm::ls::SpotLight* firePit1 = new dm::ls::SpotLight(sf::Vector2f(1584,1166),200,sf::Color(210,115,10),0.0f ,180.0f*2.0f,1.0f,0.1f,1.0f);
    dm::ls::SpotLight* firePit2 = new dm::ls::SpotLight(sf::Vector2f(1775,1166),200,sf::Color(210,115,10),0.0f ,180.0f*2.0f,1.0f,0.1f,1.0f);

    //a flashlight is a spotlight with a line base instead of a point
    dm::ls::SpotLight* flashLight = new dm::ls::FlashLight(sf::Vector2f(2845,1245),800,50,sf::Color::White,180.0f/2.0f,180.0f/10.0f,1.0f,0.0f,2.0f);

    //just some more lights to test a few things
    dm::ls::SpotLight* lamp = new dm::ls::SpotLight(sf::Vector2f(2160,1583),200,sf::Color::White,0.0f ,180.0f*2.0f,1.0f,0.0f,0.50f);

    //one negative spot in the room with the ceiling white light
    dm::ls::SpotLight* negativeSpot = new dm::ls::SpotLight(sf::Vector2f(2366,1440),300,sf::Color(127,127,127),180.0f/4.0f ,180.0f/4.0f,-1.0f,0.0f,2.0f);

    //template add example
    //also follows the player around, showing you don't need to update a light if you're just moving it around
    dm::ls::SpotLight* playerLight = ls.addLight<dm::ls::SpotLight>(p.getPosition(),200,sf::Color::Yellow);

    //local ambiant lights are useful for example to make a difference between day and night
    dm::ls::LocalAmbiantLight* localAmbiant = new dm::ls::LocalAmbiantLight(sf::Vector2f(1535,1439),ambiantShape,sf::Color::Red);
    //they can also be negative
    dm::ls::LocalAmbiantLight* negativeAmbiant = new dm::ls::LocalAmbiantLight(sf::Vector2f(991,1087),negativeShape,sf::Color::Green,true);

    //Example of emissive lights. Emissive lights are just a white sprite on a transparent background that are drawn with a different color above everything else
    //the sprite is copied and stored locally
    //emissive lights CANNOT be negative (this will need to change)
    //but they can be updated at any moment on any parameter without any cost
    //still need to call the LightSystem::update(Light*) on it
    dm::ls::SpriteLight* emissive = new dm::ls::SpriteLight(sf::Vector2f(2368,1592),sf::Color(100,255,255),180.0f/2.0f,emissiveSprite);
    emissive->setEmissive(true);

    //this parameter allows you to change the way textures are resized when a call to LightSystem::update(Light*) is done
    //with this set to true, the texture will only be resized when the new required size is greater than the current allocated size
    //with this set to false (default), the texture is always reallocated as long as the new light radius is different than the previous one
    //this loses some place on the graphic card but increases performance
    //especially useful set to true for lights that oscillate between two radius, like this flickering light
    //that goes from 180 to 200 in a wave
    firePit1->setResizeWhenIncrease(true);

    //add them all to the LightSystem
    //except the playerLight, since it's already been added by the template function
    ls.addLight(spotRed);
    ls.addLight(spotBlue);
    ls.addLight(spotGreen);
    ls.addLight(negativeColors);//you can add them anywhere, not just at the end. They are stored in a different list
    ls.addLight(eyeSpotLeft);
    ls.addLight(eyeSpotRight);
    ls.addLight(sunRise);
    ls.addLight(firePit1);
    ls.addLight(firePit2);
    ls.addLight(lamp);
    ls.addLight(flashLight);
    ls.addLight(negativeSpot);
    ls.addLight(localAmbiant);
    ls.addLight(negativeAmbiant);
    ls.addLight(emissive);

    //Modify a light
    //if you change its direcionAngle; its position or its color, it doesn't need to be updated
    playerLight->setDirectionAngle(180.0f);
    playerLight->setColor(sf::Color(255,175,0));
    //if you modify ANY of the parameters below, you have to update the light's texture using ls.update(Light*);
    //otherwise the update won't be taken into account and the behaviour is undefined
    //basically any parameter except for directionAngle and position
    playerLight->setLinearity(2.0f);
    playerLight->setBleed(0.0f);
    playerLight->setSpreadAngle(70.0f);
    playerLight->setIntensity(1.0f);
    playerLight->setRadius(250);
    ls.update(playerLight);

    clock_t oneRender, totalTime = 0;

    //the loop
    while (window.isOpen())
    {
        sf::Event event;
        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
                window.close();
            else if(event.type == sf::Event::KeyPressed)
            {
                switch(event.key.code)
                {
                    case sf::Keyboard::Escape:
                        {
                            window.close();
                        }
                        break;
                    case sf::Keyboard::Up :
                        {
                            p.move(0,-speed);
                        }
                        break;
                    case sf::Keyboard::Down :
                        {
                            p.move(0,speed);
                        }
                        break;
                    case sf::Keyboard::Left :
                        {
                            p.move(-speed,0);
                        }
                        break;
                    case sf::Keyboard::Right :
                        {
                            p.move(speed,0);
                        }
                        break;
                    case sf::Keyboard::F1 :
                        {
                            aabb = !aabb;
                        }
                        break;
                    case sf::Keyboard::F2 :
                        {
                            debugLightMapOnly = !debugLightMapOnly;
                        }
                        break;
                    case sf::Keyboard::F3 :
                        {
                            debugUseShader = !debugUseShader;
                        }
                        break;
                    case sf::Keyboard::F4 :
                        {
                            debugDrawLights = !debugDrawLights;
                        }
                        break;
                    case sf::Keyboard::F5 :
                        {
                            debugDrawWalls = !debugDrawWalls;
                        }
                        break;
                    case sf::Keyboard::F:
                        {
                            //see above, this parameter requires an update of the light's internal texture
                            //shows you how to turn a light ON and OFF easily
                            //in this particular case, you wouldn't need to update the texture
                            //since the lights aren't drawn if their intensity is 0
                            playerLight->setIntensity(1.0f - playerLight->getIntensity());
                            ls.update(playerLight);
                        }
                        break;
                    case sf::Keyboard::S:
                        {
                            //this parameter requires an update
                            if(playerLight->getSpreadAngle()==2.0f*180.0f)
                            {
                                playerLight->setSpreadAngle(180.0f/3.0f);
                                playerLight->setRadius(200);
                            }
                            else
                            {
                                playerLight->setSpreadAngle(2.0*180.0f);
                                playerLight->setRadius(100);
                            }
                            ls.update(playerLight);
                        }
                        break;
                    default:
                        break;
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
        oneRender = ::clock();
        window.setView(view);
        window.draw(bgSpr);
        window.draw(p);

        int flags = 0;
        if(debugLightMapOnly) flags |= dm::ls::LightSystem::DebugFlags::LIGHTMAP_ONLY;
        if(!debugUseShader) flags |= dm::ls::LightSystem::DebugFlags::SHADER_OFF;

        //use LightSystem::render if not using debug
        //if using debugRender, the flags allow you to modify the way the lights are drawn
        ls.debugRender(view,window,flags);
        if(debugDrawLights) ls.draw(view,window);
        //draws the light's AABB
        if(aabb) ls.drawAABB(view,window);
        if(debugDrawWalls) ls.drawWalls(view,window);

        {
            sf::ConvexShape shape = playerLight->getShape();
            sf::Transform t = shape.getTransform();

            sf::Vertex* vertexes = new sf::Vertex[shape.getPointCount()+1];
            for(size_t i = 0; i < shape.getPointCount(); ++i)
            {
                vertexes[i].position = t.transformPoint(shape.getPoint(i));
                vertexes[i].color = sf::Color::Green;
            }
            vertexes[shape.getPointCount()] = vertexes[0];

            window.draw(vertexes, shape.getPointCount()+1, sf::LineStrip);

            delete[] vertexes;
        }


        window.setView(baseView);

        window.draw(text);
        window.display();
        totalTime += ::clock() - oneRender;

        //sf::sleep(sf::milliseconds(16));
        ++elapsedFrames;
        if(clock.getElapsedTime().asMilliseconds() > 500)
        {
            fps = elapsedFrames;
            elapsedFrames = 0;
            clock.restart();
            mouse = window.mapPixelToCoords(mouseInt, view);
            sf::Color lightColor = ls.getLightColor(mouse.x, mouse.y);
            //for some reason my compiler doesn't find to_string so..
            std::ostringstream str;
            str << (fps*2) << "\n" << std::setprecision(3) << (float)totalTime/(float)fps << " ms"
            << "\n" << (int)lightColor.r << ", " << (int)lightColor.g << ", " << (int)lightColor.b
            << "\n" << (int)mouse.x << ", " << (int)mouse.y;
            text.setString(str.str());
            totalTime = 0;

            //also use this timer to alternate the negative light to a positive light
            //you can change the positivity of a light just by changing its intensity
            //this however requires you to remove the light and to add it again
            //the addLight takes care of updating the light, no need to readd it
            ls.removeLight(negativeSpot);
            if(negativeSpot->isNegative()) negativeSpot->setIntensity(1.0f);
            else negativeSpot->setIntensity(-1.0f);
            ls.addLight(negativeSpot);

            //you can easily switch a light on and off with this function
            //and it doesn't require an update of the light
            flashLight->setActive(!flashLight->isActive());

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
        if(flickerClock.getElapsedTime().asMilliseconds() > 100)
        {
            flickerClock.restart();
            if(firePit1->getRadius() == 200)
            {
                firePit1->setRadius(180);
                firePit2->setRadius(180);
            }
            else
            {
                firePit1->setRadius(200);
                firePit2->setRadius(200);
            }
            ls.update(firePit1);
            ls.update(firePit2);
        }
    }


    return 0;
}

void addWalls(dm::ls::LightSystem& ls)
{
    //first is screen
    sf::ConvexShape wallShape;
    wallShape.setPointCount(4);
    wallShape.setPoint(0,sf::Vector2f(0,0));
    wallShape.setPoint(1,sf::Vector2f(WIDTH,0));
    wallShape.setPoint(2,sf::Vector2f(WIDTH,HEIGHT));
    wallShape.setPoint(3,sf::Vector2f(0,HEIGHT));
    ls.addWall(wallShape);

    wallShape.setPoint(0,sf::Vector2f(1470,1728));
    wallShape.setPoint(1,sf::Vector2f(1664,1728));
    wallShape.setPoint(2,sf::Vector2f(1664,1791));
    wallShape.setPoint(3,sf::Vector2f(1470,1791));
    ls.addWall(wallShape);

    wallShape.setPoint(0,sf::Vector2f(1696,1728));
    wallShape.setPoint(1,sf::Vector2f(1888,1728));
    wallShape.setPoint(2,sf::Vector2f(1888,1791));
    wallShape.setPoint(3,sf::Vector2f(1696,1791));
    ls.addWall(wallShape);

    wallShape.setPoint(0,sf::Vector2f(1632,2064));
    wallShape.setPoint(1,sf::Vector2f(1663,2064));
    wallShape.setPoint(2,sf::Vector2f(1663,2114));
    wallShape.setPoint(3,sf::Vector2f(1632,2114));
    ls.addWall(wallShape);

    wallShape.setPoint(0,sf::Vector2f(1696,2064));
    wallShape.setPoint(1,sf::Vector2f(1727,2064));
    wallShape.setPoint(2,sf::Vector2f(1727,2114));
    wallShape.setPoint(3,sf::Vector2f(1696,2114));
    ls.addWall(wallShape);

    wallShape.setPoint(0,sf::Vector2f(1470,2080));
    wallShape.setPoint(1,sf::Vector2f(1632,2080));
    wallShape.setPoint(2,sf::Vector2f(1632,2114));
    wallShape.setPoint(3,sf::Vector2f(1470,2114));
    ls.addWall(wallShape);

    wallShape.setPoint(0,sf::Vector2f(1727,2080));
    wallShape.setPoint(1,sf::Vector2f(1888,2080));
    wallShape.setPoint(2,sf::Vector2f(1888,2114));
    wallShape.setPoint(3,sf::Vector2f(1727,2114));
    ls.addWall(wallShape);

    wallShape.setPoint(0,sf::Vector2f(1437,1728));
    wallShape.setPoint(1,sf::Vector2f(1470,1728));
    wallShape.setPoint(2,sf::Vector2f(1470,2114));
    wallShape.setPoint(3,sf::Vector2f(1437,2114));
    ls.addWall(wallShape);

    wallShape.setPoint(0,sf::Vector2f(1888,1728));
    wallShape.setPoint(1,sf::Vector2f(1922,1728));
    wallShape.setPoint(2,sf::Vector2f(1922,2114));
    wallShape.setPoint(3,sf::Vector2f(1888,2114));
    ls.addWall(wallShape);
}
