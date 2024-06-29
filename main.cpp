#include <SDL.h>
#include <SDL_ttf.h>
#include <iostream>
#include <vector>
#include <ctime>
#include <string>

const int HEADER_HEIGHT = 100;
const int SCREEN_WIDTH = 800;
const int SCREEN_HEIGHT = 800 + HEADER_HEIGHT;
const int NUM_UNITS_X = 20;
const int NUM_UNITS_Y = (SCREEN_HEIGHT - HEADER_HEIGHT) / ((SCREEN_WIDTH / NUM_UNITS_X));
const int UNIT_SIZE = SCREEN_WIDTH / NUM_UNITS_X;
const int FONT_SIZE = 32;
const SDL_Color black = { 0, 0, 0, 255 };
const SDL_Color red = { 255, 0, 0, 255 };
const SDL_Color green = { 0, 100, 0, 255 };


enum Direction { UP, DOWN, LEFT, RIGHT };
enum GameState { PLAYING, GAME_OVER };

struct Position {
    int x, y;
};

class Snake {
public:
    Snake() {
        reset();
    }

    void reset() {
        segments.clear();
        segments.push_back({ 0, HEADER_HEIGHT + (SCREEN_HEIGHT - HEADER_HEIGHT) / 2 });
        direction = RIGHT;
        growing = false;
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
        if (head.x < 0 || head.x >= SCREEN_WIDTH || head.y < HEADER_HEIGHT || head.y >= SCREEN_HEIGHT) {
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
        for (size_t i = 0; i < segments.size(); ++i) {
            if (i == 0) {
                SDL_SetRenderDrawColor(renderer, 0, 100, 0, 255);
            } else {
                SDL_SetRenderDrawColor(renderer, 0, 255, 0, 255);
            }
            drawRoundedRect(renderer, segments[i].x, segments[i].y, UNIT_SIZE, UNIT_SIZE, 5);
        }
    }

    Position getHeadPosition() const {
        return segments[0];
    }

private:
    std::vector<Position> segments;
    Direction direction;
    bool growing = false;

    void drawRoundedRect(SDL_Renderer* renderer, int x, int y, int w, int h, int r) {
        SDL_Rect rects[3] = {
            { x + r, y, w - 2 * r, h },
            { x, y + r, r, h - 2 * r },
            { x + w - r, y + r, r, h - 2 * r }
        };
        SDL_RenderFillRects(renderer, rects, 3);

        for (int i = 0; i < r; ++i) {
            for (int j = 0; j < r; ++j) {
                if ((i * i + j * j) <= (r * r)) {
                    SDL_RenderDrawPoint(renderer, x + r - i, y + r - j);
                    SDL_RenderDrawPoint(renderer, x + w - r + i, y + r - j);
                    SDL_RenderDrawPoint(renderer, x + r - i, y + h - r + j);
                    SDL_RenderDrawPoint(renderer, x + w - r + i, y + h - r + j);
                }
            }
        }
    }
};

class Food {
public:
    Food() {
        respawn();
    }

    void respawn() {
        position.x = (rand() % (SCREEN_WIDTH / UNIT_SIZE)) * UNIT_SIZE;
        position.y = HEADER_HEIGHT + (rand() % ((SCREEN_HEIGHT - HEADER_HEIGHT) / UNIT_SIZE)) * UNIT_SIZE;
    }

    Position getPosition() const {
        return position;
    }

    void render(SDL_Renderer* renderer) {
        drawCircle(renderer, position.x + UNIT_SIZE / 2, position.y + UNIT_SIZE / 2, UNIT_SIZE / 2);
    }

private:
    Position position;

    void drawCircle(SDL_Renderer* renderer, int centerX, int centerY, int radius) {
        SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
        for (int w = 0; w < radius * 2; w++) {
            for (int h = 0; h < radius * 2; h++) {
                int dx = radius - w;
                int dy = radius - h;
                if ((dx * dx + dy * dy) <= (radius * radius)) {
                    SDL_RenderDrawPoint(renderer, centerX + dx, centerY + dy);
                }
            }
        }
    }
};

void renderChessboard(SDL_Renderer* renderer) {
    bool white = true;
    for (int y = HEADER_HEIGHT; y < SCREEN_HEIGHT; y += UNIT_SIZE) {
        for (int x = 0; x < SCREEN_WIDTH; x += UNIT_SIZE) {
            if (white) {
                SDL_SetRenderDrawColor(renderer, 210, 210, 210, 255);
            } else {
                SDL_SetRenderDrawColor(renderer, 180, 180, 180, 255);
            }
            SDL_Rect fillRect = { x, y, UNIT_SIZE, UNIT_SIZE };
            SDL_RenderFillRect(renderer, &fillRect);
            white = !white;
        }
        white = !white;
    }
}

void renderHeader(SDL_Renderer* renderer, TTF_Font* font, int score) {
    SDL_Surface* surfaceMessage = TTF_RenderText_Solid(font, "Snake", black);
    SDL_Texture* message = SDL_CreateTextureFromSurface(renderer, surfaceMessage);
    SDL_Rect messageRect = { 40, 10, surfaceMessage->w, surfaceMessage->h };
    SDL_RenderCopy(renderer, message, NULL, &messageRect);
    SDL_FreeSurface(surfaceMessage);
    SDL_DestroyTexture(message);

    std::string scoreText = "Score: " + std::to_string(score);
    surfaceMessage = TTF_RenderText_Solid(font, scoreText.c_str(), black);
    message = SDL_CreateTextureFromSurface(renderer, surfaceMessage);
    messageRect = { SCREEN_WIDTH / 2, 10, surfaceMessage->w, surfaceMessage->h };
    SDL_RenderCopy(renderer, message, NULL, &messageRect);
    SDL_FreeSurface(surfaceMessage);
    SDL_DestroyTexture(message);
}

void renderGameOver(SDL_Renderer* renderer, TTF_Font* font) {
    SDL_Surface* surfaceMessage = TTF_RenderText_Solid(font, "You lost!", red);
    SDL_Texture* message = SDL_CreateTextureFromSurface(renderer, surfaceMessage);
    SDL_Rect messageRect = { SCREEN_WIDTH / 2 - surfaceMessage->w / 2, SCREEN_HEIGHT / 2 - surfaceMessage->h / 2, surfaceMessage->w, surfaceMessage->h };
    SDL_RenderCopy(renderer, message, NULL, &messageRect);
    SDL_FreeSurface(surfaceMessage);
    SDL_DestroyTexture(message);

    surfaceMessage = TTF_RenderText_Solid(font, "Play again", green);
    message = SDL_CreateTextureFromSurface(renderer, surfaceMessage);
    SDL_Rect playAgainRect = { SCREEN_WIDTH / 2 - surfaceMessage->w / 2, SCREEN_HEIGHT / 2 + 50, surfaceMessage->w, surfaceMessage->h };
    SDL_RenderCopy(renderer, message, NULL, &playAgainRect);
    SDL_FreeSurface(surfaceMessage);
    SDL_DestroyTexture(message);

    surfaceMessage = TTF_RenderText_Solid(font, "Close", red);
    message = SDL_CreateTextureFromSurface(renderer, surfaceMessage);
    SDL_Rect closeRect = { SCREEN_WIDTH / 2 - surfaceMessage->w / 2, SCREEN_HEIGHT / 2 + 100, surfaceMessage->w, surfaceMessage->h };
    SDL_RenderCopy(renderer, message, NULL, &closeRect);
    SDL_FreeSurface(surfaceMessage);
    SDL_DestroyTexture(message);
}

bool isMouseOverRect(int mouseX, int mouseY, SDL_Rect rect) {
    return mouseX > rect.x && mouseX < rect.x + rect.w && mouseY > rect.y && mouseY < rect.y + rect.h;
}

int main(int argc, char* args[]) {
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        std::cerr << "SDL could not initialize! SDL_Error: " << SDL_GetError() << std::endl;
        return 1;
    }

    if (TTF_Init() == -1) {
        std::cerr << "TTF_Init: " << TTF_GetError() << std::endl;
        SDL_Quit();
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

    TTF_Font* font = TTF_OpenFont("C:\\Users\\Jedrzej\\Desktop\\sgd_project\\assets\\arial.ttf", FONT_SIZE);
    if (!font) {
        std::cerr << "Failed to load font: " << TTF_GetError() << std::endl;
        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(window);
        SDL_Quit();
        return 1;
    }

    srand(static_cast<unsigned>(time(0)));

    bool quit = false;
    SDL_Event e;
    Snake snake;
    Food food;
    int score = 0;
    GameState gameState = PLAYING;

    while (!quit) {
        while (SDL_PollEvent(&e) != 0) {
            if (e.type == SDL_QUIT) {
                quit = true;
            } else if (e.type == SDL_KEYDOWN) {
                if (gameState == PLAYING) {
                    switch (e.key.keysym.sym) {
                    case SDLK_UP: snake.setDirection(UP); break;
                    case SDLK_DOWN: snake.setDirection(DOWN); break;
                    case SDLK_LEFT: snake.setDirection(LEFT); break;
                    case SDLK_RIGHT: snake.setDirection(RIGHT); break;
                    }
                }
            } else if (e.type == SDL_MOUSEBUTTONDOWN && gameState == GAME_OVER) {
                int mouseX, mouseY;
                SDL_GetMouseState(&mouseX, &mouseY);
                SDL_Rect playAgainRect = { SCREEN_WIDTH / 2 - 100, SCREEN_HEIGHT / 2 + 50, 200, 50 };
                SDL_Rect closeRect = { SCREEN_WIDTH / 2 - 100, SCREEN_HEIGHT / 2 + 100, 200, 50 };

                if (isMouseOverRect(mouseX, mouseY, playAgainRect)) {
                    snake.reset();
                    food.respawn();
                    score = 0;
                    gameState = PLAYING;
                } else if (isMouseOverRect(mouseX, mouseY, closeRect)) {
                    quit = true;
                }
            }
        }

        if (gameState == PLAYING) {
            snake.move();

            if (snake.getHeadPosition().x == food.getPosition().x && snake.getHeadPosition().y == food.getPosition().y) {
                snake.grow();
                food.respawn();
                score += 10;
            }

            if (snake.checkCollision()) {
                gameState = GAME_OVER;
            }
        }

        SDL_SetRenderDrawColor(renderer, 0, 100, 0, 255);
        SDL_Rect headerRect = { 0, 0, SCREEN_WIDTH, HEADER_HEIGHT };
        SDL_RenderFillRect(renderer, &headerRect);

        renderHeader(renderer, font, score);

        renderChessboard(renderer);

        if (gameState == PLAYING) {
            snake.render(renderer);
            food.render(renderer);
        } else if (gameState == GAME_OVER) {
            renderGameOver(renderer, font);
        }

        SDL_RenderPresent(renderer);

        SDL_Delay(150);
    }

    TTF_CloseFont(font);
    TTF_Quit();
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}
