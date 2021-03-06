#include "Bullet.h"

Bullet::Bullet(Circle *shooter, double ux, double uy, int damage, int speed, ALLEGRO_BITMAP* bulletImg)
: unit_x(ux), unit_y(uy), damage(damage), speed(speed), bulletImg(bulletImg)
{
    this->pos_x = shooter->x;
    this->pos_y = shooter->y;
    this->circle = new Circle(shooter->x, shooter->y, 10);

    double radian = atan(-uy / ux);
    if (ux > 0 && uy < 0)   radian += 2 * PI;
    if (ux < 0)             radian += PI;
    radian_ccw = radian;
    // initialize the effect sound of attack
}

Bullet::~Bullet()
{
    // printf("Deleteing Circle...\n");
    delete circle;
//     printf("Destorying image...\n");
//     al_destroy_bitmap(bulletImg);
//     printf("Destorying arrpw...\n");
//     al_destroy_sample_instance(arrow);
//     printf("Destorying sample...\n");
//     al_destroy_sample(sample);
}

void
Bullet::Move()
{
    // printf("speed = %d, ux = %lf, uy = %lf\n", speed, unit_x, unit_y);
    pos_x += speed * unit_x;
    pos_y += speed * unit_y;
    // printf("x = %d, x = %lf\n", pos_x, pos_y);
    circle->x = (int)pos_x;
    circle->y = (int)pos_y;
}

void
Bullet::Draw()
{
    // get height and width of sprite bitmap
    int w = al_get_bitmap_width(bulletImg);
    int h = al_get_bitmap_height(bulletImg);
    double cx = w / 2, cy = h / 2;
    double angle = 2 * PI - getRadianCCW();
    // printf("angle = %lf\n", angle);
    // printf("%d %d\n", dx, dy);
    // printf("Jacket: %d %d %d %d\n", circle->x, circle->y, dx, dy);
    auto [dx, dy] = Transform();
    al_draw_scaled_rotated_bitmap(bulletImg, cx, cy, dx, dy, 1, 1, angle, 0);
    //al_draw_filled_circle(pos_x, pos_y, circle->r, al_map_rgba(196, 79, 79, 200));
}
