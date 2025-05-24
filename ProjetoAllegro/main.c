#include <allegro5/allegro.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_ttf.h>
#include <allegro5/allegro_primitives.h>
#include <stdio.h>

#define SCREEN_WIDTH 800
#define SCREEN_HEIGHT 600
#define FPS 60
#define MAX_SHAPES 100
#define MOVE_STEP 5
#define RESIZE_STEP 2

typedef enum { STATE_RUNNING, STATE_MENU } AppState;
typedef enum { SHAPE_SQUARE, SHAPE_CIRCLE, SHAPE_RECTANGLE, SHAPE_TRIANGLE } ShapeType;

typedef struct {
    ShapeType type;
    float x, y;
    float w, h, r;
    ALLEGRO_COLOR color;
} Shape;

Shape shapes[MAX_SHAPES];
int shape_count = 0;

AppState state = STATE_RUNNING;
int menu_selection = 0;
int selected_shape = -1;
bool dragging = false;

bool move_up = false, move_down = false, move_left = false, move_right = false;
bool resize_increase = false, resize_decrease = false;

bool initialize_allegro();
ALLEGRO_FONT* load_font(const char* path, int size);
void render_scene(ALLEGRO_FONT* font);
void draw_shape(const Shape* s, bool selected);
void draw_menu(ALLEGRO_FONT* font);
void add_shape(ShapeType type);
bool is_mouse_over(const Shape* s, float mx, float my);

int main() {
    if (!initialize_allegro()) return -1;

    ALLEGRO_DISPLAY* display = al_create_display(SCREEN_WIDTH, SCREEN_HEIGHT);
    al_set_window_title(display, "Selecionar e Redimensionar Formas - Allegro");
    ALLEGRO_FONT* font = load_font("arial.ttf", 24);
    ALLEGRO_EVENT_QUEUE* queue = al_create_event_queue();
    ALLEGRO_TIMER* timer = al_create_timer(1.0 / FPS);

    al_register_event_source(queue, al_get_display_event_source(display));
    al_register_event_source(queue, al_get_keyboard_event_source());
    al_register_event_source(queue, al_get_mouse_event_source());
    al_register_event_source(queue, al_get_timer_event_source(timer));

    al_start_timer(timer);

    bool redraw = true, running = true;
    float drag_offset_x = 0, drag_offset_y = 0;

    while (running) {
        ALLEGRO_EVENT ev;
        al_wait_for_event(queue, &ev);

        if (ev.type == ALLEGRO_EVENT_DISPLAY_CLOSE) running = false;

        if (ev.type == ALLEGRO_EVENT_KEY_DOWN) {
            if (state == STATE_RUNNING) {
                if (ev.keyboard.keycode == ALLEGRO_KEY_M) {
                    state = STATE_MENU;
                } else if (selected_shape != -1) {
                    if (ev.keyboard.keycode == ALLEGRO_KEY_UP) move_up = true;
                    if (ev.keyboard.keycode == ALLEGRO_KEY_DOWN) move_down = true;
                    if (ev.keyboard.keycode == ALLEGRO_KEY_LEFT) move_left = true;
                    if (ev.keyboard.keycode == ALLEGRO_KEY_RIGHT) move_right = true;
                    if (ev.keyboard.keycode == ALLEGRO_KEY_EQUALS || ev.keyboard.keycode == ALLEGRO_KEY_PAD_PLUS) resize_increase = true;
                    if (ev.keyboard.keycode == ALLEGRO_KEY_MINUS || ev.keyboard.keycode == ALLEGRO_KEY_PAD_MINUS) resize_decrease = true;
                }
            } else if (state == STATE_MENU) {
                if (ev.keyboard.keycode == ALLEGRO_KEY_DOWN) {
                    menu_selection = (menu_selection + 1) % 4;
                } else if (ev.keyboard.keycode == ALLEGRO_KEY_UP) {
                    menu_selection = (menu_selection - 1 + 4) % 4;
                } else if (ev.keyboard.keycode == ALLEGRO_KEY_ENTER) {
                    add_shape((ShapeType)menu_selection);
                    state = STATE_RUNNING;
                } else if (ev.keyboard.keycode == ALLEGRO_KEY_ESCAPE) {
                    state = STATE_RUNNING;
                }
            }
        }

        if (ev.type == ALLEGRO_EVENT_KEY_UP) {
            if (selected_shape != -1) {
                if (ev.keyboard.keycode == ALLEGRO_KEY_UP) move_up = false;
                if (ev.keyboard.keycode == ALLEGRO_KEY_DOWN) move_down = false;
                if (ev.keyboard.keycode == ALLEGRO_KEY_LEFT) move_left = false;
                if (ev.keyboard.keycode == ALLEGRO_KEY_RIGHT) move_right = false;
                if (ev.keyboard.keycode == ALLEGRO_KEY_EQUALS || ev.keyboard.keycode == ALLEGRO_KEY_PAD_PLUS) resize_increase = false;
                if (ev.keyboard.keycode == ALLEGRO_KEY_MINUS || ev.keyboard.keycode == ALLEGRO_KEY_PAD_MINUS) resize_decrease = false;
            }
        }

        if (ev.type == ALLEGRO_EVENT_MOUSE_BUTTON_DOWN) {
            if (state == STATE_RUNNING) {
                float mx = ev.mouse.x;
                float my = ev.mouse.y;

                selected_shape = -1;

                for (int i = shape_count - 1; i >= 0; i--) {
                    if (is_mouse_over(&shapes[i], mx, my)) {
                        selected_shape = i;
                        dragging = true;
                        drag_offset_x = mx - shapes[i].x;
                        drag_offset_y = my - shapes[i].y;
                        break;
                    }
                }
            }
        }

        if (ev.type == ALLEGRO_EVENT_MOUSE_BUTTON_UP) {
            dragging = false;
        }

        if (ev.type == ALLEGRO_EVENT_MOUSE_AXES && dragging && selected_shape != -1) {
            shapes[selected_shape].x = ev.mouse.x - drag_offset_x;
            shapes[selected_shape].y = ev.mouse.y - drag_offset_y;
        }

        if (ev.type == ALLEGRO_EVENT_TIMER) {
            if (selected_shape != -1) {
                Shape* s = &shapes[selected_shape];
                if (move_up) s->y -= MOVE_STEP;
                if (move_down) s->y += MOVE_STEP;
                if (move_left) s->x -= MOVE_STEP;
                if (move_right) s->x += MOVE_STEP;

                if (resize_increase) {
                    if (s->type == SHAPE_CIRCLE) s->r += RESIZE_STEP;
                    else {
                        s->w += RESIZE_STEP;
                        if (s->type != SHAPE_SQUARE) s->h += RESIZE_STEP;
                    }
                }
                if (resize_decrease) {
                    if (s->type == SHAPE_CIRCLE && s->r > RESIZE_STEP) s->r -= RESIZE_STEP;
                    else {
                        if (s->w > RESIZE_STEP) s->w -= RESIZE_STEP;
                        if (s->type != SHAPE_SQUARE && s->h > RESIZE_STEP) s->h -= RESIZE_STEP;
                    }
                }
            }
            redraw = true;
        }

        if (redraw) {
            al_clear_to_color(al_map_rgb(0, 0, 0));

            if (state == STATE_RUNNING) {
                render_scene(font);
            } else if (state == STATE_MENU) {
                draw_menu(font);
            }

            al_flip_display();
            redraw = false;
        }
    }

    al_destroy_font(font);
    al_destroy_timer(timer);
    al_destroy_event_queue(queue);
    al_destroy_display(display);
    return 0;
}

bool initialize_allegro() {
    if (!al_init()) return false;
    if (!al_install_keyboard()) return false;
    if (!al_install_mouse()) return false;
    if (!al_init_primitives_addon()) return false;
    al_init_font_addon();
    al_init_ttf_addon();
    return true;
}

ALLEGRO_FONT* load_font(const char* path, int size) {
    return al_load_font(path, size, 0);
}

void render_scene(ALLEGRO_FONT* font) {
    for (int i = 0; i < shape_count; i++) {
        draw_shape(&shapes[i], i == selected_shape);
    }
    al_draw_text(font, al_map_rgb(255, 255, 255), 10, 10, 0, "M - Menu | Clique - Selecionar | Setas/Mouse - Mover");
    al_draw_text(font, al_map_rgb(255, 255, 255), 10, 30, 0, "+ / - → Redimensionar");
}

void draw_shape(const Shape* s, bool selected) {
    ALLEGRO_COLOR border = selected ? al_map_rgb(255, 255, 0) : al_map_rgb(0, 0, 0);

    switch (s->type) {
        case SHAPE_SQUARE:
            al_draw_filled_rectangle(s->x, s->y, s->x + s->w, s->y + s->w, s->color);
            al_draw_rectangle(s->x, s->y, s->x + s->w, s->y + s->w, border, 2);
            break;
        case SHAPE_RECTANGLE:
            al_draw_filled_rectangle(s->x, s->y, s->x + s->w, s->y + s->h, s->color);
            al_draw_rectangle(s->x, s->y, s->x + s->w, s->y + s->h, border, 2);
            break;
        case SHAPE_CIRCLE:
            al_draw_filled_circle(s->x, s->y, s->r, s->color);
            al_draw_circle(s->x, s->y, s->r + 2, border, 2);
            break;
        case SHAPE_TRIANGLE:
            al_draw_filled_triangle(s->x, s->y, s->x + s->w, s->y, s->x + s->w / 2, s->y - s->h, s->color);
            al_draw_triangle(s->x, s->y, s->x + s->w, s->y, s->x + s->w / 2, s->y - s->h, border, 2);
            break;
    }
}

void draw_menu(ALLEGRO_FONT* font) {
    const char* options[] = { "Quadrado", "Círculo", "Retângulo", "Triângulo" };
    for (int i = 0; i < 4; i++) {
        ALLEGRO_COLOR color = (i == menu_selection) ? al_map_rgb(255, 255, 0) : al_map_rgb(255, 255, 255);
        al_draw_textf(font, color, SCREEN_WIDTH / 2 - 100, 150 + i * 50, 0, "%s", options[i]);
    }
    al_draw_text(font, al_map_rgb(255, 255, 255), SCREEN_WIDTH / 2 - 100, 100, 0, "Selecione com ↑ ↓ e ENTER");
}

void add_shape(ShapeType type) {
    if (shape_count >= MAX_SHAPES) return;
    Shape s;
    s.type = type;
    s.x = rand() % (SCREEN_WIDTH - 100) + 50;
    s.y = rand() % (SCREEN_HEIGHT - 100) + 50;
    s.w = 50; s.h = 50; s.r = 30;
    s.color = al_map_rgb(rand() % 256, rand() % 256, rand() % 256);
    shapes[shape_count++] = s;
}

bool is_mouse_over(const Shape* s, float mx, float my) {
    switch (s->type) {
        case SHAPE_SQUARE:
        case SHAPE_RECTANGLE:
            return (mx >= s->x && mx <= s->x + s->w && my >= s->y && my <= s->y + (s->type == SHAPE_SQUARE ? s->w : s->h));
        case SHAPE_CIRCLE:
            return ((mx - s->x) * (mx - s->x) + (my - s->y) * (my - s->y)) <= (s->r * s->r);
        case SHAPE_TRIANGLE:
            return true; // simplificação
    }
    return false;
}
