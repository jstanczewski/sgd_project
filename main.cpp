#include <SDL.h>
#include <iostream>
#include <vector>
#include <ctime>

// Screen dimension constants
const int SCREEN_WIDTH = 800;
const int SCREEN_HEIGHT = 800;
const int UNIT_SIZE = 20;

enum Direction { UP, DOWN, LEFT, RIGHT };

struct Position {
    int x, y;
};

class Snake {
public:
    Snake() {
        segments.push_back({ 0, SCREEN_HEIGHT / 2 });
        direction = RIGHT;
    }

    void move() {
        Position newHead = getHeadPosition();

        switch (direction) {
        case UP: newHead.y -= UNIT_SIZE; break;
        case DOWN: newHead.y += UNIT_SIZE; break;
        case LEFT: newHead.x -= UNIT_SIZE; break;
        case RIGHT: newHead.x += UNIT_SIZE; break;
        }

        segments.insert(segments.begin(), newHead);

        if (!growing) {
            segments.pop_back();
        }
        growing = false;
    }

    void grow() {
        growing = true;
    }

    void setDirection(Direction newDirection) {
        if ((direction == UP && newDirection != DOWN) ||
            (direction == DOWN && newDirection != UP) ||
            (direction == LEFT && newDirection != RIGHT) ||
            (direction == RIGHT && newDirection != LEFT)) {
            direction = newDirection;
        }
    }

    bool checkCollision() {
        Position head = getHeadPosition();
        if (head.x < 0 || head.x >= SCREEN_WIDTH || head.y < 0 || head.y >= SCREEN_HEIGHT) {
            return true;
        }

        for (size_t i = 1; i < segments.size(); ++i) {
            if (head.x == segments[i].x && head.y == segments[i].y) {
                return true;
            }
        }
        return false;
    }

    void render(SDL_Renderer* renderer) {
        SDL_SetRenderDrawColor(renderer, 0, 255, 0, 255);
        for (const Position& pos : segments) {
            SDL_Rect fillRect = { pos.x, pos.y, UNIT_SIZE, UNIT_SIZE };
            SDL_RenderFillRect(renderer, &fillRect);
        }
    }

    Position getHeadPosition() const {
        return segments[0];
    }

private:
    std::vector<Position> segments;
    Direction direction;
    bool growing = false;
};

class Food {
public:
    Food() {
        respawn();
    }

    void respawn() {
        position.x = (rand() % (SCREEN_WIDTH / UNIT_SIZE)) * UNIT_SIZE;
        position.y = (rand() % (SCREEN_HEIGHT / UNIT_SIZE)) * UNIT_SIZE;
    }

    Position getPosition() const {
        return position;
    }

    void render(SDL_Renderer* renderer) {
        SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
        SDL_Rect fillRect = { position.x, position.y, UNIT_SIZE, UNIT_SIZE };
        SDL_RenderFillRect(renderer, &fillRect);
    }

private:
    Position position;
};

int main(int argc, char* args[]) {
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        std::cerr << "SDL could not initialize! SDL_Error: " << SDL_GetError() << std::endl;
        return 1;
    }

    SDL_Window* window = SDL_CreateWindow("Snake Game", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
    if (!window) {
        std::cerr << "Window could not be created! SDL_Error: " << SDL_GetError() << std::endl;
        SDL_Quit();
        return 1;
    }

    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if (!renderer) {
        std::cerr << "Renderer could not be created! SDL_Error: " << SDL_GetError() << std::endl;
        SDL_DestroyWindow(window);
        SDL_Quit();
        return 1;
    }

    srand(static_cast<unsigned>(time(0)));

    bool quit = false;
    SDL_Event e;
    Snake snake;
    Food food;

    while (!quit) {
        while (SDL_PollEvent(&e) != 0) {
            if (e.type == SDL_QUIT) {
                quit = true;
            } else if (e.type == SDL_KEYDOWN) {
                switch (e.key.keysym.sym) {
                case SDLK_UP: snake.setDirection(UP); break;
                case SDLK_DOWN: snake.setDirection(DOWN); break;
                case SDLK_LEFT: snake.setDirection(LEFT); break;
                case SDLK_RIGHT: snake.setDirection(RIGHT); break;
                }
            }
        }

        snake.move();

        if (snake.getHeadPosition().x == food.getPosition().x && snake.getHeadPosition().y == food.getPosition().y) {
            snake.grow();
            food.respawn();
        }

        if (snake.checkCollision()) {
            quit = true;
        }

        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderClear(renderer);

        snake.render(renderer);
        food.render(renderer);

        SDL_RenderPresent(renderer);

        SDL_Delay(150);
    }

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}
