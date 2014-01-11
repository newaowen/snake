#include <time.h>
#include "sdl_util.h"   /* All SDL App's need this */
#include <stdio.h>

// 全局变量区域
SDL_Surface* screen = NULL;
TTF_Font* font = NULL;

bool is_running = true;

int snake_color[] = {0xffffff, 0x666666};
int color_green = 0x339933;
float color_flash_vel = 0.008;
float snake_color_index = 0;

int box_size = 16;

int WIDTH = 700;
int HEIGHT = 480;

int board_width = 560, board_height = 480;

int column = board_width / box_size;
int row = board_height / box_size;

float snake_posx = column/2, snake_posy = row/2;
float snake_new_posx, snake_new_posy;

float snake_velx = 0, snake_vely = 0;
float snake_vel_count = 0;
float snake_vel_default = 0.015;

// 目标块位置
int target_posx = -1, target_posy = -1;
char score_str[50];

bool try_again = false;

// snake array 
// 非循环链表
struct snake_node {
    int x, y;
    struct snake_node* next;
    struct snake_node* prev;
} ;

snake_node* snake_head = NULL;
snake_node* snake_tail = NULL;

int score = 0;

void gen_target_pos() {
    int x = rand() % column; 
    int y = rand() % row;

    // 判断是否与snake当前位置重复
    bool flag = false;
    snake_node* p = snake_head;
    while (p != NULL) {
        if (x == p->x && y == p->y) {
            flag = true;
            break;
        }
        p = p->next;
    }
    if (flag) {
        gen_target_pos();
    } else {
        target_posx = x;
        target_posy = y;
        printf("gen target pos: %d, %d\n", target_posx, target_posy);
    }
}

snake_node* create_snake_node(int x, int y) {
    snake_node* p = (snake_node*)malloc(sizeof(snake_node));
    if (p != NULL) {
        p->x = x;
        p->y = y;
        p->next = NULL;
        p->prev = NULL;
    }
    return p;
}

snake_node* insert_snake_head(int x, int y) {
    snake_node* p = create_snake_node(x, y);
    if (p != NULL) {
        p->next = snake_head;
        snake_head->prev = p;
        snake_head = p;
    }
    return p; 
}

void init_snake_head() {
    snake_head = create_snake_node(snake_posx, snake_posy);
    snake_head->next = NULL;
    snake_head->prev = NULL;
}

/* Initialize all game, set screen mode, load images etc */
void game_init() { 
	srand((int)time(NULL));
    screen = sdl_init(WIDTH, HEIGHT);
    font = sdl_init_font("res/FreeSans.ttf");
    sdl_set_title("snake");
    init_snake_head();
}



//snake_node* new_node(snake_node* head

bool game_is_running() {
    return is_running;
}

void draw_text(char* s, int x, int y) {
     SDL_Surface* ts = sdl_draw_text(font, s, 0);
    if (ts != NULL) {
       SDL_Rect rect;
       rect.x = x; 
       rect.y = y;
       rect.w = 100; rect.h = 80;
       SDL_BlitSurface(ts, NULL, screen, &rect);
    }
}

void draw_hud() {
    draw_text("level 1", board_width + 20, 50);
    sprintf(score_str, "score: %d", score); // puts string into buffer
    draw_text(score_str, board_width + 20, 120);
}

void draw_board() {
   // draw横线
   for (int i = 0; i <= row; i++) {
        sdl_draw_line(screen, 0, i*box_size, board_width, i*box_size, color_green);
   } 
   // draw竖线
   for (int i = 0; i <= column; i++) {
        sdl_draw_line(screen, i*box_size, 0, i*box_size, board_height, color_green);
   }
}

/** 
 * 吃到
 */ 
void snake_eat() {
    //吃到
    target_posx = -1;
    target_posy = -1;
    score += 10;
}

bool try_move_snake(float* new_posy, float* new_posx) {
    // 速度控制
    snake_vel_count += snake_vel_default;
    if (snake_vel_count < 1) {
        return false;
    }
    snake_vel_count = 0;
    
    snake_new_posy = snake_head->y + snake_vely;
    snake_new_posx = snake_head->x + snake_velx;
    if (snake_new_posy < 0 || snake_new_posy >= row || snake_new_posx < 0 || snake_new_posx >= column) {
        return false;
    }

    return true;
}

void move_snake(int x, int y) {
    // 判断新位置是否触发snake位置变动
    printf("snake moved\n");
    snake_node* old_head = snake_head;
    insert_snake_head(x, y);
    if (old_head->next == NULL) {
        snake_tail = old_head;
    }

    if (target_posx == x && target_posy == y) {
        snake_eat();
    } else {
        // snake自动运动
        // 去掉尾部
        if (snake_tail != NULL) {
            snake_node* t = snake_tail->prev;
            t->next = NULL;
            free(snake_tail);
            snake_tail = t;
        }
    }
}

void draw_snake() {
    float color_index = 0;
   
    // 绘制snake list
    snake_node* p = snake_head;
    while (p != NULL) {
        // fill rectangle
        int x = p->x;//snake_posx;
        int y = p->y;//(int)snake_posy;
        sdl_draw_box(screen, x*box_size + 1, y*box_size + 1, 
                (x + 1)*box_size - 1, (y + 1)* box_size - 1, snake_color[(int)color_index]);
        p = p->next;
    }
}

void draw_target() {
    int x = target_posx;
    int y = target_posy;
    if (x >=0 && y >= 0) {
        float color_index = 0;
        snake_color_index += color_flash_vel;
        if (snake_color_index >= sizeof(snake_color)/sizeof(snake_color[0])) {
            snake_color_index = 0;
        }
        color_index = snake_color_index;
        sdl_draw_box(screen, x*box_size + 1, y*box_size + 1, 
            (x + 1)*box_size - 1, (y + 1)* box_size - 1, snake_color[(int)color_index]);
    } else {
        gen_target_pos();
    }
}

void println(char* s) {
    printf("%s\n", s);
}

void sdl_handle_event(SDL_Event* event) {
	char keypressed;
    switch (event->type) {
        case SDL_KEYDOWN:
            keypressed = event->key.keysym.sym;
            //printf("keypressed:%c\n", keypressed);
            if (keypressed == SDLK_UP || keypressed == SDLK_w) {
                printf("up keypressed:%c\n", keypressed);
                snake_vely = -1;//-snake_vel_default;
                snake_velx = 0;
            } else if (keypressed == SDLK_DOWN || keypressed == SDLK_s) {
                snake_vely = 1;//snake_vel_default;
                snake_velx = 0;
            } else if (keypressed == SDLK_LEFT || keypressed == SDLK_a) {
                snake_velx = -1;//-snake_vel_default;
                snake_vely = 0;
            } else if (keypressed == SDLK_RIGHT || keypressed == SDLK_d) {
                snake_velx = 1;//snake_vel_default;
                snake_vely = 0;
            }
            break;

            /*  
        case SDL_KEYUP:
            keypressed = event->key.keysym.sym;
            if (keypressed == SDLK_UP || keypressed == SDLK_w) {
                snake_vely = 0;
            } else if (keypressed == SDLK_DOWN || keypressed == SDLK_s) {
                snake_vely = 0;
            } else if (keypressed == SDLK_LEFT || keypressed == SDLK_a) {
                snake_velx = 0;
            } else if (keypressed == SDLK_RIGHT || keypressed == SDLK_d) {
                snake_velx = 0;
            }
            break;
            */
    } 
}

void sdl_handle_quit_event() {
    is_running = false; 
}

void render_frame(float tick) {
    // clear
    SDL_FillRect(screen,NULL, 0x000000); 

    draw_board();

    draw_target();

    draw_snake();

    draw_hud();

    SDL_Flip(screen);
}

void clear_snake() {
    snake_node* p = snake_head;
    while (p != NULL) {
        snake_node* t = p;
        p = p->next;
        p->prev = NULL;
        free(p); 
    }
    init_snake_head();
}

void collision_test_border() {
    snake_node* p = snake_head;
    int x = p->x;
    int y = p->y;
    // 边界碰撞
    if (x < 0 || x >= column || y < 0 || y >= row) {
        return 0;
        clear_snake(); 
        score = 0;
    }
    // 蛇身碰撞
}

void collision_test_snake() {

}

void game_logic() {
    bool flag = try_move_snake();
    // 碰撞检测
    //collistion_test();
    // snake自动运动
    if (flag) { 
        move_snake(snake_new_posx, snake_new_posy);
    }
}

void sdl_event_loop() {
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        // 退出处理(关闭窗口和ESC键)
        if (event.type == SDL_QUIT || (event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_ESCAPE)) {
            sdl_handle_quit_event(); 
        } else {
            sdl_handle_event(&event);
        }
    } 
}

void render_game_over() {

}


/* Main function with game loop */
int main(int argc, char *argv[]) {
    game_init();

    float tick = 0.0f;
    // loop
    while (game_is_running()) {
        tick = SDL_GetTicks();
        // game逻辑
        game_logic();
        render_frame(tick);
        sdl_event_loop();
    }

    //render_game_over();
}
