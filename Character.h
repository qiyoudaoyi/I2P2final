#ifndef CHARACTER_H_INCLUDED
#define CHARACTER_H_INCLUDED

#include <stdio.h>
#include <string.h>
#include <vector>
#include <allegro5/allegro.h>
#include <allegro5/allegro_image.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_ttf.h>
#include <allegro5/allegro_primitives.h>
#include "Circle.h"
#include "Object.h"
#include "Weapon.h"
#include "Bullet.h"
#include "global.h"
#include "Wall.h"

#define Acceleration 1

enum {UNARMED = 0, PISTOL, SMG, AR, DEAD};
const char firearm_names[][10] = {"UNARMED", "PISTOL", "SMG", "AR", "DEAD"};

class Character : public Object
{
public:
    Character(int, int);
    virtual ~Character() {};

    // override virtual function "Object::Draw"
    virtual void Draw();
    // Load bitmaps of animation image into container "moveImg"
    void Load_Img();

    // virtual void Move(bool(&)[5], std::vector<Wall*>);
    Weapon* DropWeapon();
    void PickWeapon(Weapon*);
    // void FireWeapon(int, int);
    bool TakeDamage(int);
    void EraseBullet(int);

    // functions that return informations of monster
    Weapon* getWeapon() { return wielding; }
    std::vector<Bullet*>& getBullets() { return bullets; }
    double getRadianCCW() { return radian_ccw; }
    int getVx() { return vx; }
    int getVy() { return vy; }
    int getHP() { return HP; }
    std::pair<std::vector<ALLEGRO_BITMAP*>, std::vector<ALLEGRO_BITMAP*>> getImg() { return {moveImg, attackImg};}
    int getFirearm() {return firearm; }
    int getSpriteCnt() {return sprite_count; }

    virtual void setRadianCCW(int, int);
    void setVx(int);
    void setVy(int);

protected:
    int HP = 20;
    char class_name[20];
    int sprites[5];
    std::vector<Bullet*> bullets;
    double radian_ccw;
    // animation counter
    int firearm = 0, counter = 0;
    // animation image of current direction
    int sprite_count;
    // VELOCITY
    int vx = 0, vy = 0, max_speed;
    bool sprint;
    // set of animation images
    std::vector<ALLEGRO_BITMAP*>    moveImg;
    std::vector<ALLEGRO_BITMAP*>    attackImg;
    Weapon *wielding = NULL;
private:

};


#endif // CHARACTER_H_INCLUDED
