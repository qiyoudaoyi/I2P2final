#include "Jacket.h"

Jacket::Jacket(int spwan_x, int spwan_y) : Character(spwan_x, spwan_y)
{
    HP = 1000;
    sprites[UNARMED] = 7;
    sprites[PISTOL] = 8;
    sprites[SMG] = 8;
    sprites[AR] = 8;
    sprites[DEAD] = 7;

    hudFont = al_load_ttf_font("pirulen.ttf", font_size, 0); // load font
    strncpy(class_name, "Jacket", 20);
    Load_Img();
}

Jacket::~Jacket()
{
    for(unsigned int i = 0; i < moveImg.size(); i++) {
        ALLEGRO_BITMAP *img = moveImg[i];
        moveImg.erase(moveImg.begin() + i);
        i--;
        al_destroy_bitmap(img);
    }
    moveImg.clear();
    for(unsigned int i = 0; i < attackImg.size(); i++) {
        ALLEGRO_BITMAP *img = attackImg[i];
        attackImg.erase(attackImg.begin() + i);
        i--;
        al_destroy_bitmap(img);
    }
    attackImg.clear();
    printf("Images Cleared.\n");
    al_destroy_font(hudFont);
    al_destroy_bitmap(icon);
    printf("HUD Cleared.\n");
    for (int i = 0; i < bullets.size(); i++)
        delete bullets[i];
    // for (auto b : bullets)  delete b;
    bullets.clear();
    printf("Bullets Cleared.\n");
    if (wielding)   delete wielding;
    delete circle;
    printf("Circle Cleared.\n");
}

void
Jacket::Move(bool (&move_keys)[5], std::vector<Wall*> WallMap)
{

    if (move_keys[LSHIFT_KEY])  max_speed = 6;
    else                        max_speed = 4;

    if (move_keys[W_KEY])                    setVy(vy - Acceleration);
    else if (!move_keys[S_KEY] && vy < 0)    setVy(vy + 1);

    if (move_keys[A_KEY])                    setVx(vx - Acceleration);
    else if (!move_keys[D_KEY] && vx < 0)    setVx(vx + 1);

    if (move_keys[S_KEY])                    setVy(vy + Acceleration);
    else if (!move_keys[W_KEY] && vy > 0)    setVy(vy - 1);

    if (move_keys[D_KEY])                    setVx(vx + Acceleration);
    else if (!move_keys[A_KEY] && vx > 0)    setVx(vx - 1);

    counter = (counter + 1) % draw_frequency;
    if (vx == 0 && vy == 0) {
        sprite_count = 0;
    } else if (counter == 0) {
        sprite_count = (sprite_count + 1) % sprites[firearm];
    }

    // printf("vx: %d, vy: %d\n", vx, vy);
    bool move_x = true;
    bool move_y = true;
    bool move_both = true;
    for(auto wall: WallMap){
        if (wall->overlap(circle->x + vx, circle->y + vy)){
            move_both = false;
        } if (wall->overlap(circle->x + vx, circle->y)) {
            move_x = false;
        } if (wall->overlap(circle->x, circle->y + vy)) {
            move_y = false;
        }
    }
    if (move_both){
        circle->x += vx;
        circle->y += vy;
    }
    else if (move_x) circle->x += vx;
    else if (move_y) circle->y += vy;
}

void
Jacket::FireWeapon(int mouse_x, int mouse_y)
{
    Weapon* w = this->getWeapon();
    if (w != NULL && w->Fire()) {
        auto [x1, y1] = Transform();
        // printf("x1 = %d, y1 = %d\n", x1, y1);
        auto [ux, uy] = UnitVector(mouse_x - x1, mouse_y - y1);
        // printf("ux = %lf, uy = %lf\n", ux, uy);
        // Circle* shooter = new Circle(cam.first, cam.second, this->circle->r);
        // printf("Fire!\n");
        Bullet *b = new Bullet (
            this->getCircle(),
            ux, uy,
            w->getDamage(),
            w->getSpeed(),
            w->getBulletImg()
        );
        this->bullets.push_back(b);
    }
}

void
Jacket::Draw()
{
    int w, h, offset = 0;
    ALLEGRO_BITMAP* curImg;

    for (unsigned int i = 0; i < this->bullets.size(); i++)
        this->bullets[i]->Draw();

    if (HP) {
        for (int i = 0; i < getFirearm(); i++)       offset += sprites[i];
        offset += getSpriteCnt();
        if (!moveImg[offset + getSpriteCnt()])  return;
    } else {
        for (int i = 0; i < 4; i++) offset += sprites[i];
        offset += (int)(getRadianCCW() * 180 / PI) % sprites[4];
    }
    curImg = moveImg[offset];
    w = al_get_bitmap_width(curImg);
    h = al_get_bitmap_height(curImg);
    double cx = w / 2, cy = h / 2;
    double angle = 2 * PI - getRadianCCW();
    auto [dx, dy] = Transform();
    al_draw_scaled_rotated_bitmap(curImg, cx, cy, dx, dy, CharacterScale, CharacterScale, angle, 0);


    Weapon* weapon = getWeapon();
    int ammo = 0, mag_size = 0, reserved = 0;
    if (weapon) {
        ammo = weapon->getAmmo();
        mag_size = weapon->getMagSize();
        reserved = weapon->getReserved();
    }

    switch(getFirearm()){
        case 0:
            icon = al_load_bitmap("./unarmed_icon.png");
            break;
        case 1:
            icon = al_load_bitmap("./Pistol_icon.png");
            break;
        case 2:
            icon = al_load_bitmap("./SMG_icon.png");
            break;
        case 3:
            icon = al_load_bitmap("./AR_icon.png");
            break;
    }

    char buffer[50];
    sprintf(buffer, "%d", HP);
    al_draw_filled_rectangle(0, window_height - hud_height, hud_width, window_height, al_map_rgba(0,0,0,100));
    al_draw_text(hudFont, al_map_rgb(255, 255, 255), font_start, window_height - hud_height + font_start, 0, buffer);
    double length = (hud_width - font_start - 4 * font_size) * HP / 1000.0;
    al_draw_filled_rectangle(4 * font_size, window_height - hud_height + font_start, 4 * font_size + (int)length, window_height - hud_height + font_start + font_size, al_map_rgb(255, 0, 0));
    sprintf(buffer, "%d/%d   %d", ammo, mag_size, reserved);
    al_draw_text(hudFont, al_map_rgb(255, 255, 255), font_start, window_height - font_start - 3 * font_size / 2, 0, buffer);
    w = al_get_bitmap_width(icon);
    h = al_get_bitmap_height(icon);
    al_draw_bitmap(icon, hud_width - w - font_size, window_height - h - font_start, 0);
}

void Jacket::setRadianCCW(int mouse_x, int mouse_y){
    auto [x0, y0] = Transform();
    double vector_x = mouse_x - x0;
    double vector_y = -(mouse_y - y0);
    double radian = atan(vector_y / vector_x);

    if (vector_x > 0 && vector_y < 0)   radian += 2 * PI;
    if (vector_x < 0)                   radian += PI;
    // printf("vector_x = %lf, vector_y = %lf\n", vector_x, vector_y);
    // printf("degree = %lf\n", radian * 180 / PI);
    radian_ccw = radian;
}