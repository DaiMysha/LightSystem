
#include "Player.hpp"


Player::Player(int x, int y, int s) : _x(x), _y(y), _speed(s) {
    for(int i=0;i<NOT_A_KEY;++i) _keyDown[i] = false;
}

void Player::blit() {
}

int Player::get_x() {
    return _x;
}

int Player::get_y() {
    return _y;
}

void Player::set_keyDown(KeyEnum key, bool val) {
    if(key!=NOT_A_KEY) _keyDown[key]=val;
}

void Player::move() {
    if(_keyDown[KEY_UP]) _y-=_speed;
    if(_keyDown[KEY_DOWN]) _y+=_speed;
    if(_keyDown[KEY_LEFT]) _x-=_speed;
    if(_keyDown[KEY_RIGHT]) _x+=_speed;
}
