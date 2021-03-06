#include "GameWindow.h"
#include "global.h"
#include <iostream>
#include <algorithm>

#define WHITE al_map_rgb(255, 255, 255)
#define BLACK al_map_rgb(0, 0, 0)
#define ORANGE_LIGHT al_map_rgb(255, 196, 87)
#define ORANGE_DARK al_map_rgb(255, 142, 71)
#define PURPLE al_map_rgb(149, 128, 255)
#define BLUE al_map_rgb(77, 129, 179)
#define min(a, b) ((a) < (b)? (a) : (b))
#define max(a, b) ((a) > (b)? (a) : (b))

// EDITED

#define MaxLevel 3
#define Acceleration 5
int background_width;
int background_height;
int Character_Health;
int camera_origin_x;
int camera_origin_y;

bool
GameWindow::mouse_hover(int startx, int starty, int width, int height)
{
    if(mouse_x >= startx && mouse_x <= startx + width)
        if(mouse_y >= starty && mouse_y <= starty + height)
            return true;

    return false;
}

Jacket*
GameWindow::spawnJacket(int x, int y)
{
    return new Jacket(x, y);
}

Enemy*
GameWindow::spawnEnemy(int arm, int x, int y)
{
    Enemy* e = new Enemy(x, y);
    Weapon* w = NULL;
    switch (arm)
    {
    case PISTOL:
        w = new Pistol();
        break;
    case SMG:
        w = new SubmachineGun();
        break;
    case AR:
        w = new AssaultRifle();
        break;
    }
    // printf("Spawning Enemy...\n");
    if (w) {
        // weapons.push_back(w);
        e->PickWeapon(w);
    }
    // printf("Spawning Enemy...\n");
    return e;
}

Weapon*
GameWindow::spawnWeapon(int type, int x, int y)
{
    int i = SMG;
    Weapon* w = NULL;
    switch (type)
    {
    case 1:
        w = new Pistol();
        printf("Dropping Pistol...\n");
        w->Drop(x, y);
        break;
    case 2:
        w = new SubmachineGun();
        printf("Dropping SMG...\n");
        w->Drop(x, y);
        break;
    case 3:
        w = new AssaultRifle();
        printf("Dropping AR...\n");
        w->Drop(x, y);
        break;
    default:
        break;
    }
    return w;
}

void
GameWindow::game_play()
{
    int msg = GAME_CONTINUE;
    printf("Using srand() for no reasons...\n");
    srand(time(NULL));
    printf("Init Menu...\n");
    game_menu();
    while (msg != GAME_EXIT)
    {
        msg = game_run();
    }
    show_err_msg(msg);
    game_destroy();
}

void
GameWindow::show_err_msg(int msg)
{
    if(msg == GAME_EXIT)
        fprintf(stderr, "Game Terminated...");
    else
        fprintf(stderr, "unexpected msg: %d", msg);

    game_destroy();
    exit(9);
}

GameWindow::GameWindow()
{
    if (!al_init())
        show_err_msg(-1);

    printf("Game Initializing...\n");

    display = al_create_display(window_width, window_height);
    event_queue = al_create_event_queue();

    timer = al_create_timer(1.0 / FPS);
    record_time = al_create_timer(1.0 / FPS);

    if (display == NULL || event_queue == NULL)
        show_err_msg(-1);

    // printf("Allegro Init...\n");
    al_init_primitives_addon();
    al_init_font_addon(); // initialize the font addon
    al_init_ttf_addon(); // initialize the ttf (True Type Font) addon
    al_init_image_addon(); // initialize the image addon
    al_init_acodec_addon(); // initialize acodec addon
    al_install_keyboard(); // install keyboard event
    al_install_mouse();    // install mouse event
    al_install_audio();    // install audio event

    // printf("Loading Founts...\n");
    font = al_load_ttf_font("Caviar_Dreams_Bold.ttf", 12,0);        // load small font
    Medium_font = al_load_ttf_font("Caviar_Dreams_Bold.ttf", 24,0); // load medium font
    Large_font = al_load_ttf_font("Caviar_Dreams_Bold.ttf", 36,0);  // load large font
    hudFont = al_load_ttf_font("pirulen.ttf", font_size, 0);

    // printf("Registering Events...\n");
    al_register_event_source(event_queue, al_get_display_event_source(display));
    al_register_event_source(event_queue, al_get_keyboard_event_source());
    al_register_event_source(event_queue, al_get_mouse_event_source());
    al_register_event_source(event_queue, al_get_timer_event_source(timer));


    board_x = 0; board_y = 0;

    char buffer[50];

    // printf("Loading Bitmaps...\n");
    icon = al_load_bitmap("./icon.jpg");
    startscene = al_load_bitmap("./StartScene.jpg");
    helpscene = al_load_bitmap("./HelpScene.jpg");
    stopscene = al_load_bitmap("./StopScene.jpg");
    clearscene = al_load_bitmap("./ClearScene.png");
    failscene = al_load_bitmap("./FailedScene.png");
    completescene = al_load_bitmap("./CompleteScene.jpg");

    al_set_display_icon(display, icon);
    al_reserve_samples(3);

    // printf("Loading Sounds...\n");
    sample = al_load_sample("ClickSound.ogg");
    clickSound = al_create_sample_instance(sample);
    al_set_sample_instance_playmode(clickSound, ALLEGRO_PLAYMODE_ONCE);
    al_attach_sample_instance_to_mixer(clickSound, al_get_default_mixer());

    sample = al_load_sample("BackgroundMusic.ogg");
    backgroundSound = al_create_sample_instance(sample);
    al_set_sample_instance_playmode(backgroundSound, ALLEGRO_PLAYMODE_LOOP);
    al_attach_sample_instance_to_mixer(backgroundSound, al_get_default_mixer());

    sample = al_load_sample("Paris.ogg");
    gameSound = al_create_sample_instance(sample);
    al_set_sample_instance_playmode(gameSound, ALLEGRO_PLAYMODE_LOOP);
    al_attach_sample_instance_to_mixer(gameSound, al_get_default_mixer());

    sample = al_load_sample("./SoundEffect/damage1.wav");
    for (int i = 0; i < 30; i++) {
        damageSound1.push_back(al_create_sample_instance(sample));
        al_set_sample_instance_playmode(damageSound1[i], ALLEGRO_PLAYMODE_ONCE);
        al_attach_sample_instance_to_mixer(damageSound1[i], al_get_default_mixer());
    }

    sample = al_load_sample("./SoundEffect/damage2.wav");
    for (int i = 0; i < 30; i++) {
        damageSound2.push_back(al_create_sample_instance(sample));
        al_set_sample_instance_playmode(damageSound2[i], ALLEGRO_PLAYMODE_ONCE);
        al_attach_sample_instance_to_mixer(damageSound2[i], al_get_default_mixer());
    }

    sample = al_load_sample("./SoundEffect/damage3.wav");
    for (int i = 0; i < 30; i++) {
        damageSound3.push_back(al_create_sample_instance(sample));
        al_set_sample_instance_playmode(damageSound3[i], ALLEGRO_PLAYMODE_ONCE);
        al_attach_sample_instance_to_mixer(damageSound3[i], al_get_default_mixer());
    }

    sample = al_load_sample("./SoundEffect/headshot2.wav");
    for (int i = 0; i < 30; i++) {
        deadSound.push_back(al_create_sample_instance(sample));
        al_set_sample_instance_playmode(deadSound[i], ALLEGRO_PLAYMODE_ONCE);
        al_attach_sample_instance_to_mixer(deadSound[i], al_get_default_mixer());
    }

    FILE *file;
    file = fopen("Record.txt", "r");
    fscanf(file, "%lld %lld %lld", &record[0], &record[1], &record[2]);
    fclose(file);
    // printf("Starting Timer...\n");
    al_start_timer(timer);
}

void
GameWindow::game_menu()
{
    game_reset();
    printf(">>> Menu...\n");
    al_play_sample_instance(backgroundSound);
    Draw();
}

void
GameWindow::level_init(int l)
{
    printf("Initilizing Level %d...\n", l);
    char buffer[50];
    FILE *file;
    int n, x, y, arm;

    sprintf(buffer, "LEVEL%d.txt", l);
    file = fopen(buffer, "r");
    level = l;
    // JACKET
    printf("%s\n", buffer);
    sprintf(buffer, "./Map%d.png", level);
    printf("%s\n", buffer);
    background = al_load_bitmap(buffer);
    background_width = al_get_bitmap_width(background);
    background_height = al_get_bitmap_height(background);
    fscanf(file, "%s", buffer);
    x = atoi(buffer);
    fscanf(file, "%s", buffer);
    y = atoi(buffer);
    printf("x = %d, y = %d\n", x, y);
    jacket = spawnJacket(x, y);
    // ENEMIES
    fscanf(file, "%s", buffer);
    n = atoi(buffer);
    printf("n = %d\n", n);
    enemy_count = n;
    for (int i = 0; i < n; i++) {
        Enemy* e = NULL;
        fscanf(file, "%s", buffer);
        arm = atoi(buffer);
        fscanf(file, "%s", buffer);
        x = atoi(buffer);
        fscanf(file, "%s", buffer);
        y = atoi(buffer);
        printf("arm = %d, x = %d, y = %d\n", arm, x, y);
        e = spawnEnemy(arm, x, y);
        enemies.push_back(e);
    }
    // WEAPONS
    fscanf(file, "%s", buffer);
    n = atoi(buffer);
    printf("n = %d\n", n);
    for (int i = 0; i < n; i++) {
        Weapon* w = NULL;
        fscanf(file, "%s", buffer);
        arm = atoi(buffer);
        fscanf(file, "%s", buffer);
        x = atoi(buffer);
        fscanf(file, "%s", buffer);
        y = atoi(buffer);
        printf("arm = %d, x = %d, y = %d\n", arm, x, y);
        w = spawnWeapon(arm, x, y);
        weapons.push_back(w);
    }

    fscanf(file, "%s", buffer);
    Node_Amount = atoi(buffer);
    for(int i=0; i < Node_Amount; i++)
    {
        int x, y;
        fscanf(file, "%d %d", &x, &y);
        LevelMap.push_back({x, y});
    }

    fscanf(file, "%s", buffer);
    Wall_Amount = atoi(buffer);

    for(int i = 0; i < Wall_Amount; i++){
        int index_1, index_2;
        fscanf(file, "%d %d", &index_1, &index_2);
        Wall* new_wall = new Wall(LevelMap[index_1].first, LevelMap[index_1].second, LevelMap[index_2].first, LevelMap[index_2].second);
        WallMap.push_back(new_wall);
    }

    /*fscanf(file, "%s", buffer);
    Door_Amount = atoi(buffer);

    for(int i = 0; i < Door_Amount; i++){
        int index_1, index_2;
        fscanf(file, "%d %d", &index_1, &index_2);
        Door* new_door = new Door(LevelMap[index_1].first, LevelMap[index_1].second, LevelMap[index_2].first, LevelMap[index_2].second);
        DoorMap.push_back(new_door);
    }*/

    fclose(file);
    al_start_timer(record_time);
}

void
GameWindow::game_begin(int l)
{
    game_reset();
    printf(">>> In Game <<<\n");
    printf("Clearing Jacket...\n");
    if (jacket) delete jacket;
    printf("Clearing Enemies...\n");
    for (int i = 0; i < enemies.size(); i++) {
        delete enemies[i];
        printf("One Enemy Deleted.\n");
    }
    printf("All Enemies Deleted.\n");
    enemies.clear();
    printf("Clearing Walls...\n");
    for (int i = 0; i < WallMap.size(); i++)    delete WallMap[i];
    WallMap.clear();
    printf("Clearing Nodes...\n");
    LevelMap.clear();
    printf("Clearing Weapons...\n");
    for (int i = 0; i < weapons.size(); i++)    delete weapons[i];
    weapons.clear();
    printf("All Cleared.\n");

    if (l == 0) jacket = spawnJacket(0, 0);
    else        level_init(l);

    Draw();

    /*al_play_sample_instance(startSound);
    while(al_get_sample_instance_playing(startSound));*/
    al_play_sample_instance(gameSound);
}

int
GameWindow::game_run()
{
    int message = GAME_CONTINUE;

    if (!al_is_event_queue_empty(event_queue))
        message = process_event();

    return message;
}

int
GameWindow::game_update()
{
    // printf("In game_update\n");
    if (GameState == GAMESTATE_MENU && preGameState != GAMESTATE_MENU) {
        game_menu();
    }

    bool success = false;
    if (GameState == GAMESTATE_PLAYING) {
        if (preGameState == GAMESTATE_MENU)
            game_begin(1);
        if (preGameState == GAMESTATE_SUCCESS)
            game_begin(level + 1);
        if (preGameState == GAMESTATE_FAILURE)
            game_begin(level);

        // printf("weapons.size() = %d\n", weapons.size());

        // UPDATE JACKET
        // printf("Update Jacket\n");
        Weapon* w = jacket->getWeapon();
        if (game_keys[E_KEY]) {
            game_keys[E_KEY] = false;
            std::vector<Weapon*> near_weapons;
            std::vector<int> near_weapons_idx;
            Weapon* pick_target;
            Circle* jacket_cirle = jacket->getCircle();

            for (int i = 0; i < weapons.size(); i++)
                if (weapons[i]->isDropped() && Circle::isOverlap(jacket_cirle, weapons[i]->getCircle()))
                    near_weapons_idx.push_back(i);

            if (!near_weapons_idx.empty()) {
                printf("Picking Weapon...\n");
                int pick_idx = near_weapons_idx[0];
                double min_distance = Circle::Distance(jacket_cirle, weapons[pick_idx]->getCircle());
                for (int i = 1; i < near_weapons_idx.size(); i++) {
                    double distance = Circle::Distance(jacket_cirle, weapons[pick_idx]->getCircle());
                    if (distance < min_distance) {
                        min_distance = distance;
                        pick_idx = near_weapons_idx[i];
                    }
                }
                if (w)  weapons.push_back(jacket->DropWeapon());
                jacket->PickWeapon(weapons[pick_idx]);
                weapons.erase(weapons.begin() + pick_idx);
                weapons.shrink_to_fit();
            }
        }

        if (game_keys[R_KEY]) {
            game_keys[R_KEY] = false;
            if (w)  w->StartReload();
        }

        if (func_keys[H_KEY]) {
            func_keys[H_KEY] = false;
            if (botmode)    botmode = false;
            else            botmode = true;
        }

        if (game_keys[G_KEY]) {
            game_keys[G_KEY] = false;
            if (w)  weapons.push_back(jacket->DropWeapon());
        }

        if (w) {
            if (w->isReloading()) {
                w->Reload();
            } else {
                w->CoolDown();
                if (mouse_hold)
                    if (w->getAmmo() == 0)  w->StartReload();
                    else                    jacket->FireWeapon(mouse_x, mouse_y);
            }
        }

        jacket->Move(move_keys, WallMap);
        std::vector<Bullet*>    bullets = jacket->getBullets();
        std::set<int>           erase_indices;
        erase_indices.clear();
        for (int i = 0; i < bullets.size(); i++) {
            bullets[i]->Move();
            bool in_air = true;
            int bx = bullets[i]->getX(), by = bullets[i]->getY();
            if (bx < 0 || bx > background_width || by < 0 || by > background_height) {
                erase_indices.insert(i);
                in_air = false;
                continue;
            }
            for (auto wall : WallMap) if (in_air && wall->overlap(bx, by)) {
                erase_indices.insert(i);
                continue;
            }
            for (int j = 0; j < enemies.size() && in_air; j++) {
                if (enemies[j]->getHP() == 0)   continue;
                if (Circle::isOverlap(enemies[j]->getCircle(), bullets[i]->getCircle())) {
                    // BULLET HIT ENEMY SOUND EFFECT
                    play_damage_sound();
                    printf("Hit!\n");
                    if (enemies[j]->TakeDamage(bullets[i]->getDamage())) {
                        // ENEMY DEAD EFFECT
                        play_dead_sound();
                        printf("Enemy Dead!\n");
                        enemy_count--;
                        if (enemies[j]->getWeapon())
                            weapons.push_back(enemies[j]->DropWeapon());
                        printf("Enemy Weapon Dropped!\n");
                    }
                    erase_indices.insert(i);
                    in_air = false;
                    continue;
                }
            }
        }
        // printf("Test\n");
        // if (!bullets.empty())   printf("%d bullets in air.\n", bullets.size());
        for (auto idx : erase_indices)   jacket->EraseBullet(idx);
        erase_indices.clear();

        // UPDATE ENEMIES
        if (!botmode) {
            // printf("Update Enemis\n");
            int jx = jacket->getX(), jy = jacket->getY();
            for (auto e : enemies) {
                if (e->getHP()) {
                    int ex = e->getX(), ey = e->getY();
                    Weapon* enemy_w = e->getWeapon();
                    if (enemy_w) {
                        if (enemy_w->isReloading()) enemy_w->Reload();
                        else                        enemy_w->CoolDown();
                    }
                    bool visible = true;
                    for (auto w : WallMap) {
                        if (wallBetween(w, ex, ey, jx, jy)) {
                            visible = false;
                            break;
                        }
                    }
                    if (visible)    e->Assault(jx, jy, WallMap);
                }

                bullets = e->getBullets();
                for (int i = 0; i < bullets.size(); i++) {
                    bullets[i]->Move();
                    int bx = bullets[i]->getX(), by = bullets[i]->getY();
                    bool in_air = true;
                    if (bx < 0 || bx > background_width || by < 0 || by > background_height) {
                        erase_indices.insert(i);
                        continue;
                    }
                    for (auto wall : WallMap) if (in_air && wall->overlap(bx, by)) {
                        erase_indices.insert(i);
                        in_air = false;
                        continue;
                    }
                    if (in_air && Circle::isOverlap(jacket->getCircle(), bullets[i]->getCircle())) {
                        // BULLET HIT JACKET SOUND EFFECT
                        play_damage_sound();
                        jacket->TakeDamage(bullets[i]->getDamage());
                        erase_indices.insert(i);
                    }
                }
                for (auto idx : erase_indices)   e->EraseBullet(idx);
                erase_indices.clear();
            }
        }

        success = true;
        for (auto e : enemies)  if (e->getHP() != 0)    success = false;
        // printf("Update cam origin\n");
        camera_origin_x = jacket->getCircle()->x - window_width / 2;
        camera_origin_y = jacket->getCircle()->y - window_height / 2;
        if(camera_origin_x < 0) camera_origin_x = 0;
        if(camera_origin_x > background_width - window_width) camera_origin_x = background_width - window_width;
        if(camera_origin_y > background_height - window_height) camera_origin_y = background_height - window_height;
        if(camera_origin_y < 0) camera_origin_y = 0;
        board_x = camera_origin_x;
        board_y = camera_origin_y;
    }

    // printf("Update GameState\n");
    int msg = GAME_CONTINUE, nxtState = GameState;
    switch (GameState)
    {
    case GAMESTATE_MENU:
        if (func_keys[ENTER_KEY])       nxtState = GAMESTATE_PLAYING;
        else if (func_keys[H_KEY])      nxtState = GAMESTATE_TUTORIAL;
        else if (func_keys[ESCAPE_KEY]) msg = GAME_EXIT;
        break;
    case GAMESTATE_TUTORIAL:
        for (int i = 0; i < FuncKeysUsed; i++) {
            if (func_keys[i]) {
                nxtState = GAMESTATE_MENU;
                break;
            }
        }
        break;
    case GAMESTATE_PLAYING:
        if (func_keys[P_KEY])           nxtState = GAMESTATE_PAUSE;
        else if (success)               nxtState = GAMESTATE_SUCCESS;
        else if (jacket->getHP() == 0)  nxtState = GAMESTATE_FAILURE;
        break;
    case GAMESTATE_PAUSE:
        if (func_keys[P_KEY] || func_keys[ENTER_KEY])
            nxtState = GAMESTATE_PLAYING;
        else if (func_keys[ESCAPE_KEY])
            nxtState = GAMESTATE_MENU;
        break;
    case GAMESTATE_SUCCESS:
        if (func_keys[ENTER_KEY]) {
            if (level == MaxLevel)  nxtState = GAMESTATE_END;
            else                    nxtState = GAMESTATE_PLAYING;
        }
        break;
    case GAMESTATE_FAILURE:
        if (func_keys[ENTER_KEY])       nxtState = GAMESTATE_PLAYING;
        else if (func_keys[ESCAPE_KEY]) nxtState = GAMESTATE_MENU;
        break;
    case GAMESTATE_END:
        if (func_keys[ENTER_KEY])       nxtState = GAMESTATE_MENU;
        else if (func_keys[ESCAPE_KEY]) msg = GAME_EXIT;
        break;
    }
    for (int i = 0; i < FuncKeysUsed; i++)
        if (func_keys[i])   func_keys[i] = false;

    Draw();
    preGameState = GameState;
    GameState = nxtState;

    return msg;
}

void
GameWindow::game_reset()
{
    printf("Resetting...\n");
    mute = false;
    update = false;

    printf("Stop All Sounds...\n");
    // stop sample instance
    al_stop_sample_instance(backgroundSound);
    al_stop_sample_instance(gameSound);
    al_stop_sample_instance(clickSound);

    printf("Stop All Timers...\n");
    // stop timer
    // al_stop_timer(timer);
    al_set_timer_count(record_time, 0);
}

void
GameWindow::game_destroy()
{

    FILE* file;
    file = fopen("Record.txt", "w");
    fprintf(file, "%lld %lld %lld", record[0], record[1], record[2]);
    fclose(file);
    game_reset();

    al_destroy_display(display);
    al_destroy_event_queue(event_queue);
    al_destroy_font(font);
    al_destroy_font(Medium_font);
    al_destroy_font(Large_font);

    al_destroy_timer(timer);

    al_destroy_bitmap(icon);
    al_destroy_bitmap(background);
    al_destroy_bitmap(helpscene);
    al_destroy_bitmap(startscene);

    al_destroy_sample(sample);
    al_destroy_sample_instance(clickSound);
    al_destroy_sample_instance(gameSound);
    al_destroy_sample_instance(backgroundSound);
}

int
GameWindow::process_event()
{
    // printf("In process_event\n");
    int message = GAME_CONTINUE;

    al_wait_for_event(event_queue, &event);
    // printf("Event Processing...\n");

    update = false;

    if (event.type == ALLEGRO_EVENT_TIMER) {
        // printf(">>> Timer Event\n");
        if (event.timer.source == timer) {
            update = true;
        }
    }
    else {
        // printf(">>> Not Timer Event\n");
        if (event.type == ALLEGRO_EVENT_DISPLAY_CLOSE) {
            // printf(">>> Display Closed\n");
            return GAME_EXIT;
        } else if (event.type == ALLEGRO_EVENT_KEY_DOWN) {
            switch (event.keyboard.keycode) {
                // GAMEPLAY KEYS
                case ALLEGRO_KEY_W:
                    // printf("W is pressed!\n");
                    if (!move_keys[W_KEY]) move_keys[W_KEY] = true;
                    break;
                case ALLEGRO_KEY_A:
                    // printf("A is pressed!\n");
                    if (!move_keys[A_KEY]) move_keys[A_KEY] = true;
                    break;
                case ALLEGRO_KEY_S:
                    // printf("S is pressed!\n");
                    if (!move_keys[S_KEY]) move_keys[S_KEY] = true;
                    break;
                case ALLEGRO_KEY_D:
                    // printf("D is pressed!\n");
                    if (!move_keys[D_KEY]) move_keys[D_KEY] = true;
                    break;
                case ALLEGRO_KEY_LSHIFT:
                    // printf("LSHIFT is pressed!\n");
                    if (!move_keys[LSHIFT_KEY]) move_keys[LSHIFT_KEY] = true;
                    break;
                case ALLEGRO_KEY_E:
                    // printf("E is pressed!\n");
                    game_keys[E_KEY] = true;
                    break;
                case ALLEGRO_KEY_R:
                    // printf("R is pressed!\n");
                    game_keys[R_KEY] = true;
                    break;
                case ALLEGRO_KEY_G:
                    // printf("E is pressed!\n");
                    game_keys[G_KEY] = true;
                    break;

                // FUNCTION KEYS
                case ALLEGRO_KEY_ENTER:
                    func_keys[ENTER_KEY] = true;
                    break;
                case ALLEGRO_KEY_P:
                    func_keys[P_KEY] = true;
                    break;
                case ALLEGRO_KEY_H:
                    func_keys[H_KEY] = true;
                    break;
                case ALLEGRO_KEY_ESCAPE:
                    func_keys[ESCAPE_KEY] = true;
                    break;
            }
        } else if (event.type == ALLEGRO_EVENT_KEY_UP) {
            switch (event.keyboard.keycode) {
                case ALLEGRO_KEY_W:
                    // printf("W is released!\n");
                    if (move_keys[W_KEY])    move_keys[W_KEY] = false;
                    break;
                case ALLEGRO_KEY_A:
                    // printf("A is released!\n");
                    if (move_keys[A_KEY])    move_keys[A_KEY] = false;
                    break;
                case ALLEGRO_KEY_S:
                    // printf("S is released!\n");
                    if (move_keys[S_KEY])    move_keys[S_KEY] = false;
                    break;
                case ALLEGRO_KEY_D:
                    // printf("D is released!\n");
                    if (move_keys[D_KEY])    move_keys[D_KEY] = false;
                    break;
                case ALLEGRO_KEY_LSHIFT:
                    // printf("LSHIFT is released!\n");
                    if (move_keys[LSHIFT_KEY])  move_keys[LSHIFT_KEY] = false;
                    break;
            }
        } else if (event.type == ALLEGRO_EVENT_MOUSE_BUTTON_DOWN) {
            if(event.mouse.button == 1) {
                if (!mouse_hold)    mouse_hold = true;
            }
        } else if (event.type == ALLEGRO_EVENT_MOUSE_BUTTON_UP) {
            if (event.mouse.button == 1) {
                if (mouse_hold)     mouse_hold = false;
            }
        } else if (event.type == ALLEGRO_EVENT_MOUSE_AXES) {
            if (GameState == GAMESTATE_PLAYING) {
                mouse_x = event.mouse.x;
                mouse_y = event.mouse.y;
                // printf("mouse_x = %d, mouse_y = %d\n", mouse_x, mouse_y);
                jacket->setRadianCCW(mouse_x, mouse_y);
                //menu->MouseIn(mouse_x, mouse_y);
            }
        }

    }

    // printf("Event Processed.\n");

    if (update) {
        message = game_update();
        update = false;
    }

    return message;
}

void
GameWindow::Draw()
{
    // printf("In Draw\n");
    if (GameState == GAMESTATE_MENU) {
        // printf("Drawing Menu...\n");
        al_draw_bitmap(startscene, 0, 0, 0);
    } else if (GameState == GAMESTATE_TUTORIAL) {
        al_draw_bitmap(helpscene, 0, 0, 0);
    } else if (GameState == GAMESTATE_PLAYING) {
        // printf("Drawing Gamescene...\n");
        // printf("Drawing Map...\n");
        al_draw_bitmap(background, -board_x, -board_y, 0);
        // printf("Drawing Weapon...\n");
        for (auto weapon : weapons) weapon->Draw();
        // printf("Drawing Enemies...\n");
        for (auto enemy : enemies)  enemy->Draw();
        // printf("Drawing Jacket...\n");
        char buffer[50];
        sprintf(buffer, "Enemies left: %d", enemy_count);
        al_draw_filled_rectangle(0, 0, hud_width, hud_height, al_map_rgba(0,0,0,100));
        al_draw_text(hudFont, al_map_rgb(255, 255, 255), font_start, font_start, 0, buffer);
        sprintf(buffer, "Time: %.2lf", al_get_timer_count(record_time) / 60.0);
        al_draw_text(hudFont, al_map_rgb(255, 255, 255), font_start, 2 * font_start + font_size, 0, buffer);
        jacket->Draw();
        //for(auto wall: WallMap) wall->Draw();
        // printf("Done!!!...\n\n");
        // menu->Draw();
    } else if (GameState == GAMESTATE_PAUSE) {
        al_draw_bitmap(stopscene, 0, 0, 0);
    } else if (GameState == GAMESTATE_SUCCESS) {
        al_draw_bitmap(clearscene, 0, 0, 0);
        long long int recent = al_get_timer_count(record_time);
        al_stop_timer(record_time);
        char buffer[50];
        record[level - 1] = min(recent, record[level - 1]);
        sprintf(buffer, "Best: %.2lf    Recent: %.2lf", record[level - 1] / 60.0, recent / 60.0);
        //printf("Time: %d\n", al_get_timer_count(record_time));
        al_draw_text(hudFont, al_map_rgb(0, 0, 0), 15 * font_size, window_height - 2 * font_size, 0, buffer);
    } else if (GameState == GAMESTATE_FAILURE) {
        al_draw_bitmap(failscene, 0, 0, 0);
    } else if (GameState == GAMESTATE_END) {
        al_draw_bitmap(completescene, 0, 0, 0);
    }
    al_flip_display();
}

bool
GameWindow::wallBetween(Wall* wall, int x1, int y1, int x2, int y2){
    int xl = wall->get_xl(); int xr = wall->get_xr(); int yl = wall->get_yl(); int yr = wall->get_yr();
    //printf("two point: %d %d %d %d\n", x1, y1, x2, y2);
    if(x1 == x2){
        if(x1 <= xr && x1 >= xl){
            if(max(y1, y2) <= yl) return false;
            else if(min(y1, y2) >= yr) return false;
            else return true;
        }
        else return false;
    }
    if(y1 == y2){
        if(y1 <= yr && y1 >= yl){
            if(max(x1, x2) <= xl) return false;
            else if(min(x1, x2) >= xr) return false;
            else return true;
        }
        else return false;
    }
    double slope = (y1 - y2) * 1.0 / ((x1 - x2) * 1.0); double con = y1 - slope * x1; int thick = 30;
    double intersect_y1 = slope * xl + con; if(intersect_y1 >= min(y1, y2) && intersect_y1 <= max(y1, y2) && intersect_y1 >= yl && intersect_y1 <= yr) return true;
    double intersect_y2 = slope * xr + con; if(intersect_y2 >= min(y1, y2) && intersect_y1 <= max(y1, y2) && intersect_y2 >= yl && intersect_y2 <= yr) return true;
    double intersect_x1 = (yl - con) / slope; if(intersect_x1 >= min(x1, x2) && intersect_x1 <= max(x1, x2) && intersect_x1 >= xl && intersect_x1 <= xr) return true;
    double intersect_x2 = (yr - con) / slope; if(intersect_x2 >= min(x1, x2) && intersect_x2 <= max(x1, x2) && intersect_x2 >= xl && intersect_x2 <= xr) return true;
    return false;
}

void
GameWindow::play_damage_sound()
{
    static int cnt = 0;
    if (cnt++ == 29) cnt = 0;
    al_play_sample_instance(damageSound1[cnt]);
}

void
GameWindow::play_dead_sound()
{
    static int cnt = 0;
    if (cnt++ == 29) cnt = 0;
    al_play_sample_instance(deadSound[cnt]);
}
