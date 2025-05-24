#include <allegro5/allegro.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_ttf.h>
#include <allegro5/allegro_primitives.h>
#include <allegro5/allegro_image.h>
#include <stdio.h>

#define ScreenWidth 800
#define ScreenHeight 600
#define Fps 60
#define MaxShapes 100
#define MoveStep 5
#define ResizeStep 2

typedef enum { StateRunning, StateMenu } AppState;
typedef enum { ShapeSquare, ShapeCircle, ShapeRectangle, ShapeTriangle } ShapeType;

typedef struct {
    ShapeType Type;
    float X, Y;
    float W, H, R;
    ALLEGRO_COLOR Color;
} Shape;

Shape shapes[MaxShapes];
int shapeCount = 0;

AppState state = StateRunning;
int menuSelection = 0;
int selectedShape = -1;
bool dragging = false;

bool moveUp = false, moveDown = false, moveLeft = false, moveRight = false;
bool resizeIncrease = false, resizeDecrease = false;

int screenshotCount = 0;

float elapsedTime = 0.0f;

bool InitializeAllegro();
ALLEGRO_FONT* LoadFont(const char* path, int size);
void RenderScene(ALLEGRO_FONT* font);
void DrawShape(const Shape* shape, bool selected);
void DrawMenu(ALLEGRO_FONT* font);
void AddShape(ShapeType type);
bool IsMouseOver(const Shape* shape, float mx, float my);
void SaveScreenshot();

int main() {
    if (!InitializeAllegro()) return -1;

    ALLEGRO_DISPLAY* display = al_create_display(ScreenWidth, ScreenHeight);
    al_set_window_title(display, "Selecionar e Redimensionar Formas - Allegro");
    ALLEGRO_FONT* font = LoadFont("arial.ttf", 16);
    ALLEGRO_EVENT_QUEUE* queue = al_create_event_queue();
    ALLEGRO_TIMER* timer = al_create_timer(1.0 / Fps);

    al_register_event_source(queue, al_get_display_event_source(display));
    al_register_event_source(queue, al_get_keyboard_event_source());
    al_register_event_source(queue, al_get_mouse_event_source());
    al_register_event_source(queue, al_get_timer_event_source(timer));

    al_init_image_addon();

    al_start_timer(timer);

    bool redraw = true, running = true;
    float dragOffsetX = 0, dragOffsetY = 0;

    while (running) {
        ALLEGRO_EVENT ev;
        al_wait_for_event(queue, &ev);

        if (ev.type == ALLEGRO_EVENT_DISPLAY_CLOSE) running = false;

        if (ev.type == ALLEGRO_EVENT_KEY_DOWN) {
            if (state == StateRunning) {
                if (ev.keyboard.keycode == ALLEGRO_KEY_M) {
                    state = StateMenu;
                } else if (selectedShape != -1) {
                    if (ev.keyboard.keycode == ALLEGRO_KEY_UP) moveUp = true;
                    if (ev.keyboard.keycode == ALLEGRO_KEY_DOWN) moveDown = true;
                    if (ev.keyboard.keycode == ALLEGRO_KEY_LEFT) moveLeft = true;
                    if (ev.keyboard.keycode == ALLEGRO_KEY_RIGHT) moveRight = true;
                    if (ev.keyboard.keycode == ALLEGRO_KEY_EQUALS || ev.keyboard.keycode == ALLEGRO_KEY_PAD_PLUS) resizeIncrease = true;
                    if (ev.keyboard.keycode == ALLEGRO_KEY_MINUS || ev.keyboard.keycode == ALLEGRO_KEY_PAD_MINUS) resizeDecrease = true;
                    if (ev.keyboard.keycode == ALLEGRO_KEY_C) shapes[selectedShape].Color = al_map_rgb(rand() % 256, rand() % 256, rand() % 256);
                    if (ev.keyboard.keycode == ALLEGRO_KEY_DELETE) {
                        for (int i = selectedShape; i < shapeCount - 1; i++) {
                            shapes[i] = shapes[i + 1];
                        }
                        shapeCount--;

                        if (shapeCount == 0) {
                            selectedShape = -1;
                        } else {
                            selectedShape = selectedShape % shapeCount;
                        }
                    }
                }

                if (ev.keyboard.keycode == ALLEGRO_KEY_S && (ev.keyboard.modifiers & ALLEGRO_KEYMOD_CTRL)) {
                    SaveScreenshot();
                }

            } else if (state == StateMenu) {
                if (ev.keyboard.keycode == ALLEGRO_KEY_DOWN) {
                    menuSelection = (menuSelection + 1) % 4;
                } else if (ev.keyboard.keycode == ALLEGRO_KEY_UP) {
                    menuSelection = (menuSelection - 1 + 4) % 4;
                } else if (ev.keyboard.keycode == ALLEGRO_KEY_ENTER) {
                    AddShape((ShapeType)menuSelection);
                    state = StateRunning;
                } else if (ev.keyboard.keycode == ALLEGRO_KEY_ESCAPE) {
                    state = StateRunning;
                }
            }
        }

        if (ev.type == ALLEGRO_EVENT_KEY_UP) {
            if (selectedShape != -1) {
                if (ev.keyboard.keycode == ALLEGRO_KEY_UP) moveUp = false;
                if (ev.keyboard.keycode == ALLEGRO_KEY_DOWN) moveDown = false;
                if (ev.keyboard.keycode == ALLEGRO_KEY_LEFT) moveLeft = false;
                if (ev.keyboard.keycode == ALLEGRO_KEY_RIGHT) moveRight = false;
                if (ev.keyboard.keycode == ALLEGRO_KEY_EQUALS || ev.keyboard.keycode == ALLEGRO_KEY_PAD_PLUS) resizeIncrease = false;
                if (ev.keyboard.keycode == ALLEGRO_KEY_MINUS || ev.keyboard.keycode == ALLEGRO_KEY_PAD_MINUS) resizeDecrease = false;
            }
        }

        if (ev.type == ALLEGRO_EVENT_MOUSE_BUTTON_DOWN) {
            if (state == StateRunning) {
                float mx = ev.mouse.x;
                float my = ev.mouse.y;

                selectedShape = -1;

                for (int i = shapeCount - 1; i >= 0; i--) {
                    if (IsMouseOver(&shapes[i], mx, my)) {
                        selectedShape = i;
                        dragging = true;
                        dragOffsetX = mx - shapes[i].X;
                        dragOffsetY = my - shapes[i].Y;
                        break;
                    }
                }
            }
        }

        if (ev.type == ALLEGRO_EVENT_MOUSE_BUTTON_UP) {
            dragging = false;
        }

        if (ev.type == ALLEGRO_EVENT_MOUSE_AXES && dragging && selectedShape != -1) {
            shapes[selectedShape].X = ev.mouse.x - dragOffsetX;
            shapes[selectedShape].Y = ev.mouse.y - dragOffsetY;
        }

        if (ev.type == ALLEGRO_EVENT_TIMER) {
            elapsedTime += 1.0f / Fps;

            if (selectedShape != -1) {
                Shape* s = &shapes[selectedShape];
                if (moveUp) s->Y -= MoveStep;
                if (moveDown) s->Y += MoveStep;
                if (moveLeft) s->X -= MoveStep;
                if (moveRight) s->X += MoveStep;

                if (resizeIncrease) {
                    if (s->Type == ShapeCircle) s->R += ResizeStep;
                    else {
                        s->W += ResizeStep;
                        if (s->Type != ShapeSquare) s->H += ResizeStep;
                    }
                }
                if (resizeDecrease) {
                    if (s->Type == ShapeCircle && s->R > ResizeStep) s->R -= ResizeStep;
                    else {
                        if (s->W > ResizeStep) s->W -= ResizeStep;
                        if (s->Type != ShapeSquare && s->H > ResizeStep) s->H -= ResizeStep;
                    }
                }
            }
            redraw = true;
        }

        if (redraw) {
            al_clear_to_color(al_map_rgb(0, 0, 0));

            if (state == StateRunning) {
                RenderScene(font);
            } else if (state == StateMenu) {
                DrawMenu(font);
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

bool InitializeAllegro() {
    if (!al_init()) return false;
    if (!al_install_keyboard()) return false;
    if (!al_install_mouse()) return false;
    if (!al_init_primitives_addon()) return false;
    if (!al_init_image_addon()) return false;
    al_init_font_addon();
    al_init_ttf_addon();
    return true;
}

ALLEGRO_FONT* LoadFont(const char* path, int size) {
    return al_load_font(path, size, 0);
}

void RenderScene(ALLEGRO_FONT* font) {
    for (int i = 0; i < shapeCount; i++) {
        DrawShape(&shapes[i], i == selectedShape);
    }

    al_draw_text(font, al_map_rgb(255, 255, 255), 10, 10, 0, "M → Menu");
    al_draw_text(font, al_map_rgb(255, 255, 255), 10, 30, 0, "Clique → Selecionar");
    al_draw_text(font, al_map_rgb(255, 255, 255), 10, 50, 0, "Setas/Mouse → Mover");
    al_draw_text(font, al_map_rgb(255, 255, 255), 10, 70, 0, "+ / - → Redimensionar");
    al_draw_text(font, al_map_rgb(255, 255, 255), 10, 90, 0, "Ctrl+S → Screenshot");
    al_draw_text(font, al_map_rgb(255, 255, 255), 10, 110, 0, "Delete → Deletar forma");
    al_draw_text(font, al_map_rgb(255, 255, 255), 10, 130, 0, "C → Mudar a cor");

    char timerText[32];
    sprintf(timerText, "Timer: %.1f s", elapsedTime);

    al_draw_text(font, al_map_rgb(255, 255, 255), ScreenWidth - 120, 10, 0, timerText);
}

void DrawShape(const Shape* shape, bool selected) {
    ALLEGRO_COLOR border = selected ? al_map_rgb(255, 255, 0) : al_map_rgb(0, 0, 0);

    switch (shape->Type) {
        case ShapeSquare:
            al_draw_filled_rectangle(shape->X, shape->Y, shape->X + shape->W, shape->Y + shape->W, shape->Color);
            al_draw_rectangle(shape->X, shape->Y, shape->X + shape->W, shape->Y + shape->W, border, 2);
            break;
        case ShapeRectangle:
            al_draw_filled_rectangle(shape->X, shape->Y, shape->X + shape->W, shape->Y + shape->H, shape->Color);
            al_draw_rectangle(shape->X, shape->Y, shape->X + shape->W, shape->Y + shape->H, border, 2);
            break;
        case ShapeCircle:
            al_draw_filled_circle(shape->X, shape->Y, shape->R, shape->Color);
            al_draw_circle(shape->X, shape->Y, shape->R + 2, border, 2);
            break;
        case ShapeTriangle:
            al_draw_filled_triangle(shape->X, shape->Y, shape->X + shape->W, shape->Y, shape->X + shape->W / 2, shape->Y - shape->H, shape->Color);
            al_draw_triangle(shape->X, shape->Y, shape->X + shape->W, shape->Y, shape->X + shape->W / 2, shape->Y - shape->H, border, 2);
            break;
    }
}

void DrawMenu(ALLEGRO_FONT* font) {
    const char* options[] = { "Quadrado", "Círculo", "Retângulo", "Triângulo" };
    for (int i = 0; i < 4; i++) {
        ALLEGRO_COLOR color = (i == menuSelection) ? al_map_rgb(255, 255, 0) : al_map_rgb(255, 255, 255);
        al_draw_textf(font, color, ScreenWidth / 2 - 100, 150 + i * 50, 0, "%s", options[i]);
    }
    al_draw_text(font, al_map_rgb(255, 255, 255), ScreenWidth / 2 - 100, 100, 0, "Selecione com ↑ ↓ e ENTER");
}

void AddShape(ShapeType type) {
    if (shapeCount >= MaxShapes) return;
    Shape s;
    s.Type = type;
    s.X = rand() % (ScreenWidth - 100) + 50;
    s.Y = rand() % (ScreenHeight - 100) + 50;

    switch (type) {
        case ShapeSquare:
            s.W = 50;
            s.H = 50;
            s.R = 0;
            break;
        case ShapeRectangle:
            s.W = 100;
            s.H = 50;
            s.R = 0;
            break;
        case ShapeCircle:
            s.R = 30;
            s.W = s.H = 0;
            break;
        case ShapeTriangle:
            s.W = 60;
            s.H = 80;
            s.R = 0;
            break;
    }

    s.Color = al_map_rgb(rand() % 256, rand() % 256, rand() % 256);
    shapes[shapeCount++] = s;
}

bool IsMouseOver(const Shape* shape, float mx, float my) {
    switch (shape->Type) {
        case ShapeSquare:
        case ShapeRectangle:
            return (mx >= shape->X && mx <= shape->X + shape->W && my >= shape->Y && my <= shape->Y + (shape->Type == ShapeSquare ? shape->W : shape->H));
        case ShapeCircle:
            return ((mx - shape->X) * (mx - shape->X) + (my - shape->Y) * (my - shape->Y)) <= (shape->R * shape->R);
        case ShapeTriangle:
            return (mx >= shape->X && mx <= shape->X + shape->W && my <= shape->Y && my >= shape->Y - shape->H);
    }
    return false;
}

void SaveScreenshot() {
    char filename[64];
    snprintf(filename, sizeof(filename), "screenshot_%d.png", screenshotCount++);
    ALLEGRO_BITMAP* screenshot = al_clone_bitmap(al_get_target_bitmap());
    if (al_save_bitmap(filename, screenshot)) {
        printf("Screenshot salva: %s\n", filename);
    } else {
        printf("Erro ao salvar screenshot!\n");
    }
    al_destroy_bitmap(screenshot);
}
