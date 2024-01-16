#include <bits/stdc++.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>

using namespace std;

const int SCREEN_WIDTH = 640;
const int SCREEN_HEIGHT = 480;
const int GRID_SIZE = 20;

SDL_Window* window = nullptr;
SDL_Renderer* renderer = nullptr;
TTF_Font* font = nullptr;

struct Snake
{
    vector<pair<int, int>> body;
    char dir;
};


struct Food
{
    int x, y;
};


Snake snake;
Food food;

int score = 0;

enum class Check
{
    Running,
    GameOver,
};

Check gameState = Check::Running;

void render();
void Game_Over();
bool foodOnBorder();
bool foodOnSnake();
void generateFood();

void generateFood()
{
    do
    {
        food.x = rand() % (SCREEN_WIDTH / GRID_SIZE) * GRID_SIZE;
        food.y = rand() % (SCREEN_HEIGHT / GRID_SIZE) * GRID_SIZE;
    }
    while (foodOnSnake() || foodOnBorder());
}

void initialize()
{
    SDL_Init(SDL_INIT_VIDEO);
    window = SDL_CreateWindow("Snake Game by Hridoy", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

    if (TTF_Init() != 0)
    {
        cout << "Error:" << TTF_GetError() << endl;
        exit(0);
    }

    font = TTF_OpenFont("hridoy.ttf", 24);
    if (!font)
    {
        cout << "Error:" << TTF_GetError() << endl;
        exit(0);
    }

    snake.body.push_back({SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2});
    snake.dir = 'R';

    generateFood();
}


bool foodOnSnake()
{
    for (const auto& i : snake.body)
    {
        if (food.x == i.first && food.y == i.second)
        {
            return true;
        }
    }
    return false;
}


bool foodOnBorder()
{
    return (food.x == 0 || food.x == SCREEN_WIDTH - GRID_SIZE || food.y == 0 || food.y == SCREEN_HEIGHT - GRID_SIZE);
}


bool checkCollision()
{
    pair<int, int> head = snake.body.front();
    for (size_t i = 1; i < snake.body.size(); ++i)
    {
        if (head.first == snake.body[i].first && head.second == snake.body[i].second)
        {
            return true;
        }
    }
    return false;
}


void close()
{
    TTF_CloseFont(font);
    TTF_Quit();
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
}


void update()
{
    pair<int, int> newHead = snake.body.front();

    if (snake.dir == 'U')
    {
        newHead.second -= GRID_SIZE;
    }
    else if (snake.dir == 'D')
    {
        newHead.second += GRID_SIZE;
    }
    else if (snake.dir == 'L')
    {
        newHead.first -= GRID_SIZE;
    }
    else if (snake.dir == 'R')
    {
        newHead.first += GRID_SIZE;
    }


    newHead.first = (newHead.first + SCREEN_WIDTH) % SCREEN_WIDTH;
    newHead.second = (newHead.second + SCREEN_HEIGHT) % SCREEN_HEIGHT;
    snake.body.insert(snake.body.begin(), newHead);


    if (newHead.first == food.x && newHead.second == food.y)
    {
        score += 1;
        generateFood();
    }
    else
    {
        snake.body.pop_back();
    }


    if (checkCollision() || newHead.first == 0 || newHead.first == SCREEN_WIDTH - GRID_SIZE || newHead.second == 0 || newHead.second == SCREEN_HEIGHT - GRID_SIZE)
    {
        gameState = Check::GameOver;
    }


}

int main(int argc, char* argv[])
{
    initialize();

    SDL_Event event;
    bool quit = false;

    while (!quit)
    {
        while (SDL_PollEvent(&event) != 0)
        {
            if (event.type == SDL_QUIT)
            {
                quit = true;
            }
            else if (event.type == SDL_KEYDOWN)
            {
                if (event.key.keysym.sym == SDLK_UP)
                {
                    if (gameState == Check::GameOver)
                    {
                        gameState = Check::Running;
                        score = 0;
                        snake.body.clear();
                        snake.body.push_back({SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2});
                        snake.dir = 'R';
                        generateFood();
                    }
                    else if (snake.dir != 'D')
                    {
                        snake.dir = 'U';
                    }
                }
                else if (event.key.keysym.sym == SDLK_DOWN)
                {
                    if (gameState == Check::Running && snake.dir != 'U')
                    {
                        snake.dir = 'D';
                    }
                }
                else if (event.key.keysym.sym == SDLK_LEFT)
                {
                    if (gameState == Check::Running && snake.dir != 'R')
                    {
                        snake.dir = 'L';
                    }
                }
                else if (event.key.keysym.sym == SDLK_RIGHT)
                {
                    if (gameState == Check::Running && snake.dir != 'L')
                    {
                        snake.dir = 'R';
                    }
                }
            }
        }

        if (gameState == Check::Running)
        {
            update();
            render();
        }
        else if (gameState == Check::GameOver)
        {
            Game_Over();
        }

        SDL_Delay(120);
    }


    close();
    return 0;
}


void render()
{


    SDL_SetRenderDrawColor(renderer, 255, 255, 224, 255);
    SDL_RenderClear(renderer);


    SDL_Color textColor = {255, 255, 150,255 };
    TTF_Font* textFont = TTF_OpenFont("hridoy.ttf", 70);


    if (textFont)
    {

        string text = "SWE-21";
        SDL_Surface* textSurface = TTF_RenderText_Solid(textFont, text.c_str(), textColor);
        SDL_Texture* textTexture = SDL_CreateTextureFromSurface(renderer, textSurface);


        int textX = (SCREEN_WIDTH - textSurface->w) / 2;
        int textY = (SCREEN_HEIGHT - textSurface->h) / 2;


        SDL_Rect textRect = {textX, textY, textSurface->w, textSurface->h};
        SDL_RenderCopy(renderer, textTexture, NULL, &textRect);


        SDL_FreeSurface(textSurface);
        SDL_DestroyTexture(textTexture);
        TTF_CloseFont(textFont);

    }


    for (size_t i = 0; i < snake.body.size(); ++i)
    {

        SDL_Rect rect = {snake.body[i].first, snake.body[i].second, GRID_SIZE, GRID_SIZE};


        if (i % 2 == 0)
        {
            SDL_SetRenderDrawColor(renderer, 128, 0, 0, 255);
            SDL_RenderFillRect(renderer, &rect);
        }
        else
        {
            SDL_SetRenderDrawColor(renderer, 0, 0, 255, 255);
            SDL_RenderFillRect(renderer, &rect);
        }
    }



    SDL_SetRenderDrawColor(renderer, 75, 0, 130, 255);
    SDL_Rect foodRect = {food.x, food.y, GRID_SIZE, GRID_SIZE};
    SDL_RenderFillRect(renderer, &foodRect);


    SDL_SetRenderDrawColor(renderer, 220, 20, 60, 255);
    SDL_Rect borderRect = {0, 0, SCREEN_WIDTH+2, GRID_SIZE+2};
    SDL_RenderFillRect(renderer, &borderRect);


    borderRect = {0, SCREEN_HEIGHT - GRID_SIZE, SCREEN_WIDTH, GRID_SIZE};
    SDL_RenderFillRect(renderer, &borderRect);


    borderRect = {0, 0, GRID_SIZE, SCREEN_HEIGHT};
    SDL_RenderFillRect(renderer, &borderRect);


    borderRect = {SCREEN_WIDTH - GRID_SIZE, 0, GRID_SIZE, SCREEN_HEIGHT};
    SDL_RenderFillRect(renderer, &borderRect);


    SDL_Color textc = {255, 255, 255, 255};
    string scoreText = "Score  #  " + to_string(score);
    SDL_Surface* textSurface = TTF_RenderText_Solid(font, scoreText.c_str(), textc);
    SDL_Texture* textTexture = SDL_CreateTextureFromSurface(renderer, textSurface);


    SDL_Rect textRect = {(SCREEN_WIDTH - textSurface->w) / 2, GRID_SIZE / 2 - textSurface->h / 2, textSurface->w, textSurface->h};
    SDL_RenderCopy(renderer, textTexture, NULL, &textRect);


    SDL_FreeSurface(textSurface);
    SDL_DestroyTexture(textTexture);
    SDL_RenderPresent(renderer);
}


void Game_Over()
{
    SDL_SetRenderDrawColor(renderer, 255, 255, 224, 255);
    SDL_RenderClear(renderer);

    SDL_Color textColor = {255, 0, 0, 255};
    string gameOverText = "!!! Game Over !!!";
    SDL_Surface* gameOverSurface = TTF_RenderText_Solid(font, gameOverText.c_str(), textColor);
    SDL_Texture* gameOverTexture = SDL_CreateTextureFromSurface(renderer, gameOverSurface);
    SDL_Rect gameOverRect = {(SCREEN_WIDTH - gameOverSurface->w) / 2, SCREEN_HEIGHT / 3, gameOverSurface->w, gameOverSurface->h};
    SDL_RenderCopy(renderer, gameOverTexture, NULL, &gameOverRect);


    SDL_FreeSurface(gameOverSurface);
    SDL_DestroyTexture(gameOverTexture);


    string scoreMessage = " Better luck next time ! Final score #  " + to_string(score);
    SDL_Surface* scoreSurface = TTF_RenderText_Solid(font, scoreMessage.c_str(), textColor);
    SDL_Texture* scoreTexture = SDL_CreateTextureFromSurface(renderer, scoreSurface);
    SDL_Rect scoreRect = {(SCREEN_WIDTH - scoreSurface->w) / 2, SCREEN_HEIGHT / 2, scoreSurface->w, scoreSurface->h};
    SDL_RenderCopy(renderer, scoreTexture, NULL, &scoreRect);


    SDL_FreeSurface(scoreSurface);
    SDL_DestroyTexture(scoreTexture);

    SDL_RenderPresent(renderer);
}
