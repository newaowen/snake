#include <time.h>
#include "util.h"
#include "sdl_util.h"   /* All SDL App's need this */
#include <stdio.h>

#define DEFAULT_LIFE 3

// 全局变量区域
SDL_Surface* screen = NULL;
TTF_Font* font = NULL;

// 正在运行的标记位
int is_running = 1;

// 蛇颜色
int snake_color[] = { 0xffffff, 0x666666 };
int color_green = 0x339933;
int head_color = 0xff0000;
float color_flash_vel = 0.008;
float snake_color_index = 0;

// 区域单元块像素大小
int box_size = 16;

// 窗口整体长高
int WIDTH = 700;
int HEIGHT = 480;

// game区域长宽
int board_width = 560, board_height = 480;
// 块行列数
int column = board_width / box_size;
int row = board_height / box_size;

// snake初始位置
float snake_posx = column / 2, snake_posy = row / 2;
float snake_new_posx, snake_new_posy;

// 蛇速度
float snake_velx = 0, snake_vely = 0;
float snake_vel_count = 0;
// 速度参数
float snake_vel_default = 0.008;

// 目标块位置
int target_posx = -1, target_posy = -1;
char score_str[50];

// 重新开始标记
int try_again = 0;

// snake节点
// 非循环链表
typedef struct snake_node {
	int x, y;
	struct snake_node* next;
	struct snake_node* prev;
} snake_node_t;

snake_node_t* snake_head = NULL;
snake_node_t* snake_tail = NULL;

int life = DEFAULT_LIFE;
// 玩家得分
int score = 0;

// 生成目标位置点
void gen_target_pos() {
	int x = rand() % column;
	int y = rand() % row;

	// 判断是否与snake当前位置重复
	int flag = 0;
	snake_node_t* p = snake_head;
	while (p != NULL) {
		if (x == p->x && y == p->y) {
			flag = 1;
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

// 创建蛇节点
snake_node_t* snake_create_node(int x, int y) {
	snake_node_t* p = (snake_node_t*) malloc(sizeof(snake_node_t));
	if (p != NULL) {
		p->x = x;
		p->y = y;
		p->next = NULL;
		p->prev = NULL;
	}
	return p;
}

// 插入蛇头
snake_node_t* snake_insert_head(int x, int y) {
	snake_node_t* p = snake_create_node(x, y);
	if (p != NULL) {
		p->next = snake_head;
		snake_head->prev = p;
		snake_head = p;
	}
	return p;
}

// 初始化蛇头
void snake_init_head() {
	snake_head = snake_create_node(snake_posx, snake_posy);
	snake_head->next = NULL;
	snake_head->prev = NULL;
}

void snake_free() {
	//释放原对象
	snake_node_t* p = snake_head;
	while (p != NULL) {
		snake_node_t* t = p;
		p = p->next;
		p->prev = NULL;
		free(p);
	}
	snake_init_head();
}

// 重置蛇
void snake_reset() {
	snake_free();
	snake_init_head();
}

/* Initialize all game, set screen mode, load images etc */
void game_init() {
	srand((int) time(NULL));
	screen = sdl_init(WIDTH, HEIGHT);
	font = sdl_init_font("res/FreeSans.ttf");
	sdl_set_title("snake");
	snake_init_head();
}

int game_is_running() {
	return is_running;
}

// 绘制右侧文本工具方法
void draw_text(char* s, int x, int y) {
	SDL_Surface* ts = sdl_draw_text(font, s, 0);
	if (ts != NULL) {
		SDL_Rect rect;
		rect.x = x;
		rect.y = y;
		rect.w = 100;
		rect.h = 80;
		SDL_BlitSurface(ts, NULL, screen, &rect);
	}
}

// 绘制右侧文本
void draw_hud() {
	draw_text("level 1", board_width + 20, 50);
	sprintf(score_str, "score: %d", score); // puts string into buffer
	draw_text(score_str, board_width + 20, 120);
}

// 绘制左侧游戏区域
void draw_board() {
	// draw横线
	for (int i = 0; i <= row; i++) {
		sdl_draw_line(screen, 0, i * box_size, board_width, i * box_size,
				color_green);
	}
	// draw竖线
	for (int i = 0; i <= column; i++) {
		sdl_draw_line(screen, i * box_size, 0, i * box_size, board_height,
				color_green);
	}
}

/** 
 * 吃到
 */
void snake_eat_target() {
	target_posx = -1;
	target_posy = -1;
	score += 10;
}

// 尝试移动snake, 并进行碰撞检测
int snake_try_move() {
	// 速度控制
	snake_vel_count += snake_vel_default;
	if (snake_vel_count < 1) {
		return 0;
	}
	snake_vel_count = 0;

	snake_new_posy = snake_head->y + snake_vely;
	snake_new_posx = snake_head->x + snake_velx;

	return 1;
}

int collision_test() {
	snake_node_t* p = snake_head;
	int x = p->x;
	int y = p->y;

	//边界碰撞
	if (snake_new_posy < 0 || snake_new_posy >= row || snake_new_posx < 0
			|| snake_new_posx >= column) {
		return 1;
	}

	return 0;
}

// 真正移动蛇
void snake_move(int x, int y) {
	// 判断新位置是否触发snake位置变动
	snake_node_t* old_head = snake_head;
	snake_insert_head(x, y);
	if (old_head->next == NULL) {
		snake_tail = old_head;
	}

	if (target_posx == x && target_posy == y) {
		snake_eat_target();
	} else {
		// snake自动运动
		// 去掉尾部
		if (snake_tail != NULL) {
			snake_node_t* t = snake_tail->prev;
			t->next = NULL;
			free(snake_tail);
			snake_tail = t;
		}
	}
}

void draw_snake() {
	float color_index = 0;

	// 绘制snake list
	snake_node_t* p = snake_head;
	// 绘制蛇头
	int x = p->x;
	int y = p->y;
	sdl_draw_box(screen, x * box_size + 1, y * box_size + 1,
			(x + 1) * box_size - 1, (y + 1) * box_size - 1, head_color);

	// 绘制蛇身体
	p = p->next;
	while (p != NULL) {
		// fill rectangle
		int x = p->x;
		int y = p->y;
		//printf("draw snake: %d,%d\n", x, y);
		sdl_draw_box(screen, x * box_size + 1, y * box_size + 1,
				(x + 1) * box_size - 1, (y + 1) * box_size - 1,
				snake_color[(int) color_index]);
		p = p->next;
	}
}

void draw_target() {
	int x = target_posx;
	int y = target_posy;
	if (x >= 0 && y >= 0) {
		float color_index = 0;
		snake_color_index += color_flash_vel;
		if (snake_color_index >= sizeof(snake_color) / sizeof(snake_color[0])) {
			snake_color_index = 0;
		}
		color_index = snake_color_index;
		sdl_draw_box(screen, x * box_size + 1, y * box_size + 1,
				(x + 1) * box_size - 1, (y + 1) * box_size - 1,
				snake_color[(int) color_index]);
	} else {
		gen_target_pos();
	}
}

void sdl_handle_event(SDL_Event* event) {
	char keypressed;
	switch (event->type) {
	case SDL_KEYDOWN:
		keypressed = event->key.keysym.sym;
		//printf("keypressed:%c\n", keypressed);
		if (keypressed == SDLK_UP || keypressed == SDLK_w) {
			printf("up keypressed:%c\n", keypressed);
			snake_vely = -1; //-snake_vel_default;
			snake_velx = 0;
		} else if (keypressed == SDLK_DOWN || keypressed == SDLK_s) {
			snake_vely = 1; //snake_vel_default;
			snake_velx = 0;
		} else if (keypressed == SDLK_LEFT || keypressed == SDLK_a) {
			snake_velx = -1; //-snake_vel_default;
			snake_vely = 0;
		} else if (keypressed == SDLK_RIGHT || keypressed == SDLK_d) {
			snake_velx = 1; //snake_vel_default;
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
	is_running = 0;
}

void render_frame(float tick) {
	// clear
	SDL_FillRect(screen, NULL, 0x000000);

	draw_board();

	draw_target();

	draw_snake();

	draw_hud();

	SDL_Flip(screen);
}

void move_logic() {
	int flag = snake_try_move();
	if (flag) {
		if (collision_test()) { //碰撞检测
			// 发生碰撞，死命逻辑
			life--;
			// 如果生命已使用完，则game over进入欢迎界面
			if (life > 0) {
				snake_reset();
				// 重新生成点
				gen_target_pos();
			} else {
				// game over

			}
		}

		// snake自动运动
		snake_move(snake_new_posx, snake_new_posy);
	}
}

void sdl_event_loop() {
	SDL_Event event;
	while (SDL_PollEvent(&event)) {
		// 退出处理(关闭窗口和ESC键)
		if (event.type == SDL_QUIT
				|| (event.type == SDL_KEYDOWN
						&& event.key.keysym.sym == SDLK_ESCAPE)) {
			sdl_handle_quit_event();
		} else {
			sdl_handle_event(&event);
		}
	}
}

// 游戏结束提示
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
		move_logic();
		render_frame(tick);
		sdl_event_loop();
	}
	//render_game_over();
}
