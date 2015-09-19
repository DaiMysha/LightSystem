# LightSystem
Implementation of a Light System in C++11 for SFML

VERSION 0.1

LightSystem is a simple Light Library meant to be used with 2D games. It allows the user to create lightning effects using a variety of different lights.
The version 0.1 is a beta and does not yet support light collisions (shadows)

[pic of the light system]

See the example/main.cpp file for an example on how to use the system and the different options.

Every light has the following attributes :
	* position : the position of the light on the whole world.
	* color : the color of the light.

The system allows you to use the following lights :

* SpotLight :
	Representing a point that shines with a given direction and spread angle.
	
	This type of light exposes the following parameters :
	* radius : the maximum distance at which the light emits.
	* directionAngle : the orientation of the light according the y-axis. Expressed in degrees.
	* spreadAngle : how wide the light cone is. A value of 360 creates a point light. Expressed in degrees.
	* intensity : the intensity at which the light is displayed. This value is between -1.0 and 1.0. If the intensity is nul, the light is not displayed.
	* bleed : how wide the bleed spot is (area around the center where the light is fully seen).
	* linearity : how linear the light attenuation is. A higher value means the light will be more intense over a greater distance.
	* resizeWhenIncrease : this parameter is used for optimisations. Set to true, the light does not reallocate its internal texture if the new size is lesser than the current size. Is mainly used when a light changes diameter in a cycle, for example a flickering candle light. Set to false, the light reallocates its internal texture everytime the radius changes.
	
	All parameters except for the directionAngle and the resizeWhenIncrease option require an update of the light.

* FlashLight :
	A FlashLight is a SpotLight with a linear base. It exposes the same parameters as the SpotLight.
	It also exposes the following parameter :
	* width : how wide the base line is. Expressed in pixels.
	
	Modifying a FlashLight's width requires an update of the light.

* LocalAmbiantLight :
	A LocalAmbiantLight represents an area where the basic lightning is different than the rest of the system.
	A LocalAmbiantLight has the following extra parameters :
	* shape : the shape of the area that has the specific lightning. Modifying this parameter requires an update of the light.

* SpriteLight :
	A SpriteLight is a light in shape of a specific picture given in parameter. It is best to use images that are in grey shades to have the best result on this light.
	Exposes the following parameter :
	* Angle : the orientation of the Sprite according to the y-axis.
	* Sprite : the sprite that the light means to print. Modifying this parameter requires an update of the light.

Others :

If you need a specific light that isn't covered by the given classes, you can implement your own lights by inheriting the Light class.

The lights can have the following properties :

* ACTIVE :
	An active Light is a light that can be drawn.
	
* NEGATIVE :
	A negative light is a light that absorbs its colors instead of adding them.
	
* EMISSIVE :
	An emissive light is draw above everything else. It does not blend with what's under it and does not generate shadows.
	
* ISOMETRIC :
	To be implemented later. This is for when the LightSystem has to work on an isometric map.
	
* POLYMORPH :
	A polymorph light is a light whose parameters can change (see the definition of each light to knows which parameters are influenced). This concerns every parameter that implies that the light has to be recalulcated. Used for optimisations (tbi).
	
* MOVABLE :
	A movable light is a light whose position and direction can change. This does not require a recalculation of the light. Used for optimisations (tbi).
	
* DYNAMIC :
	A dynamic light is a light that is both movable and polymorph. (this needs better name as a dynamic light actually is one that is either movable or polymorph.)
	
Modifying a parameter that requires an update and failing to update the light results in undefined behaviour.
	

Depends on DMUtils : https://github.com/DaiMysha/DMUtils

Depends on SFML