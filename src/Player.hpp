#ifndef HEADER_PLAYER
#define HEADER_PLAYER

class Player {
    public:
    enum KeyEnum { KEY_UP, KEY_DOWN, KEY_LEFT, KEY_RIGHT, NOT_A_KEY};
    Player(int x=0, int y=0, int s=0);

    void blit();
    int get_x();
    int get_y();

    void set_keyDown(KeyEnum key, bool val);

    void move();

    private:
    int _x;
    int _y;
    bool _keyDown[NOT_A_KEY];
    int _speed;
};


#endif // HEADER_PLAYER

