#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>
#include <iostream>
#include <algorithm> // Für std::max und std::min
#include <vector>
#include <cmath> // Für sin()

// EGA Farbpalette (RGB)
struct Color {
    Uint8 r, g, b;
};

// Alien-Zustand
struct Alien {
    float x, y;
    float initial_x;
    int row;
    Color color;
};

// Zeichnet ein Space-Invaders-Alien an Position (x, y) mit gegebener Größe und Farbe
void DrawAlien(SDL_Renderer* renderer, int x, int y, int size, Color color) {
    // Einfache Pixel-Matrix für ein Alien (8x8)
    const int N = 8;
    const uint8_t alien[N] = {
        0b00111100,
        0b01111110,
        0b11111111,
        0b11011011,
        0b11111111,
        0b00100100,
        0b01011010,
        0b10100101
    };
    int pixel = size / N;
    SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, 255);
    for (int row = 0; row < N; ++row) {
        for (int col = 0; col < N; ++col) {
            if (alien[row] & (1 << (7 - col))) {
                SDL_Rect r = { x + col * pixel, y + row * pixel, pixel, pixel };
                SDL_RenderFillRect(renderer, &r);
            }
        }
    }
}
#include <SDL2/SDL.h>
#include <iostream>
#include <algorithm> // Für std::max und std::min
#include <vector>

struct Projectile {
    float x, y;
    float speed;
};

// Hilfsfunktion zum Zeichnen eines Kreises (Midpoint Circle Algorithmus)
void DrawCircle(SDL_Renderer* renderer, int32_t centreX, int32_t centreY, int32_t radius)
{
    int32_t x = radius;
    int32_t y = 0;
    int32_t radiusError = 1 - x;

    while (x >= y)
    {
        SDL_RenderDrawPoint(renderer, centreX + x, centreY + y);
        SDL_RenderDrawPoint(renderer, centreX + y, centreY + x);
        SDL_RenderDrawPoint(renderer, centreX - y, centreY + x);
        SDL_RenderDrawPoint(renderer, centreX - x, centreY + y);
        SDL_RenderDrawPoint(renderer, centreX - x, centreY - y);
        SDL_RenderDrawPoint(renderer, centreX - y, centreY - x);
        SDL_RenderDrawPoint(renderer, centreX + y, centreY - x);
        SDL_RenderDrawPoint(renderer, centreX + x, centreY - y);
        y++;
        if (radiusError < 0)
        {
            radiusError += 2 * y + 1;
        }
        else
        {
            x--;
            radiusError += 2 * (y - x + 1);
        }
    }
}

int main(int argc, char* argv[]) {
        // EGA Farbpalette
        std::vector<Color> ega_palette = {
            {0, 0, 0},       // 0: Schwarz
            {0, 0, 170},     // 1: Blau
            {0, 170, 0},     // 2: Grün
            {0, 170, 170},   // 3: Cyan
            {170, 0, 0},     // 4: Rot
            {170, 0, 170},   // 5: Magenta
            {170, 85, 0},    // 6: Braun
            {170, 170, 170}, // 7: Hellgrau
            {85, 85, 85},    // 8: Dunkelgrau
            {85, 85, 255},   // 9: Hellblau
            {85, 255, 85},   // 10: Hellgrün
            {85, 255, 255},  // 11: Hellcyan
            {255, 85, 85},   // 12: Hellrot
            {255, 85, 255},  // 13: Hellmagenta
            {255, 255, 85},  // 14: Gelb
            {255, 255, 255}  // 15: Weiß
        };
        srand(SDL_GetTicks());
    if (SDL_Init(SDL_INIT_VIDEO) != 0) {
        std::cerr << "SDL_Init Error: " << SDL_GetError() << std::endl;
        return 1;
    }
    if (TTF_Init() != 0) {
        std::cerr << "TTF_Init Error: " << TTF_GetError() << std::endl;
        SDL_Quit();
        return 1;
    }
    if (IMG_Init(IMG_INIT_PNG) == 0) {
        std::cerr << "IMG_Init Error: " << IMG_GetError() << std::endl;
        TTF_Quit();
        SDL_Quit();
        return 1;
    }

    // Konstanten für Fenster und Dreieck
    const int window_width = 800;
    const int window_height = 600;
    const int triangle_base = 60;
    const int triangle_height = 40;
    const float triangle_speed = 5.0f;
    float triangle_x = window_width / 2.0f;

    SDL_Window* window = SDL_CreateWindow("Space Invaders", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, window_width, window_height, SDL_WINDOW_SHOWN);
    if (!window) {
        std::cerr << "SDL_CreateWindow Error: " << SDL_GetError() << std::endl;
        SDL_Quit();
        return 1;
    }

    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if (!renderer) {
        std::cerr << "SDL_CreateRenderer Error: " << SDL_GetError() << std::endl;
        SDL_DestroyWindow(window);
        SDL_Quit();
        return 1;
    }
    // Load font for win message
    TTF_Font* font = TTF_OpenFont("/usr/share/fonts/truetype/dejavu/DejaVuSans-Bold.ttf", 36);
    if (!font) {
        std::cerr << "TTF_OpenFont Error: " << TTF_GetError() << std::endl;
        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(window);
        TTF_Quit();
        IMG_Quit();
        SDL_Quit();
        return 1;
    }
    // Load trophy PNG
    SDL_Texture* trophyTexture = nullptr;
    {
        SDL_Surface* trophySurface = IMG_Load("trophy.png");
        if (!trophySurface) {
            std::cerr << "IMG_Load Error: " << IMG_GetError() << std::endl;
        } else {
            trophyTexture = SDL_CreateTextureFromSurface(renderer, trophySurface);
            SDL_FreeSurface(trophySurface);
        }
    }
    // Load gorilla PNG
    SDL_Texture* gorillaTexture = nullptr;
    {
        SDL_Surface* gorillaSurface = IMG_Load("gorilla.png");
        if (!gorillaSurface) {
            std::cerr << "IMG_Load Error: " << IMG_GetError() << std::endl;
        } else {
            gorillaTexture = SDL_CreateTextureFromSurface(renderer, gorillaSurface);
            SDL_FreeSurface(gorillaSurface);
        }
    }
    // Load barrel PNG
    SDL_Texture* barrelTexture = nullptr;
    {
        SDL_Surface* barrelSurface = IMG_Load("barrel.png");
        if (!barrelSurface) {
            std::cerr << "IMG_Load Error: " << IMG_GetError() << std::endl;
        } else {
            barrelTexture = SDL_CreateTextureFromSurface(renderer, barrelSurface);
            SDL_FreeSurface(barrelSurface);
        }
    }
    // Load background PNG
    SDL_Texture* backgroundTexture = nullptr;
    {
        SDL_Surface* backgroundSurface = IMG_Load("background.png");
        if (!backgroundSurface) {
            std::cerr << "IMG_Load Error: " << IMG_GetError() << std::endl;
        } else {
            backgroundTexture = SDL_CreateTextureFromSurface(renderer, backgroundSurface);
            SDL_FreeSurface(backgroundSurface);
        }
    }

    // Geschossverwaltung
    std::vector<Projectile> projectiles;
    const int max_projectiles = 3;
    const float projectile_speed = 6.0f;

    bool running = true;
    bool win = false;
    bool game_over = false;
    // Alien-Array-Konstanten
    const int ALIEN_ROWS = 3;
    const int ALIEN_COLS = 10;
    int alien_size = 48;
    int alien_spacing_x = 20;
    int alien_spacing_y = 20;
    int start_x = 80;
    int start_y = 40;
    float alien_speed = 0.4f; // Aliens bewegen sich langsam nach unten
    // Funktion zum Initialisieren der Aliens mit Farbregeln
    auto init_aliens = [&]() {
        std::vector<Alien> new_aliens;
        // 2D color index array for adjacency checks
        std::vector<std::vector<int>> color_indices(ALIEN_ROWS, std::vector<int>(ALIEN_COLS, -1));
        int background_idx = 0; // EGA palette index for black
        int palette_size = ega_palette.size();
        for (int row = 0; row < ALIEN_ROWS; ++row) {
            for (int col = 0; col < ALIEN_COLS; ++col) {
                std::vector<int> forbidden;
                forbidden.push_back(background_idx); // No background color
                if (row > 0) forbidden.push_back(color_indices[row-1][col]); // No same as above
                if (col > 0) forbidden.push_back(color_indices[row][col-1]); // No same as left
                // Build list of allowed color indices
                std::vector<int> allowed;
                for (int i = 0; i < palette_size; ++i) {
                    if (std::find(forbidden.begin(), forbidden.end(), i) == forbidden.end()) {
                        allowed.push_back(i);
                    }
                }
                int color_idx = allowed.empty() ? 1 : allowed[rand() % allowed.size()];
                color_indices[row][col] = color_idx;
                Alien a;
                a.x = start_x + col * (alien_size + alien_spacing_x);
                a.y = start_y + row * (alien_size + alien_spacing_y);
                a.initial_x = a.x;
                a.row = row;
                a.color = ega_palette[color_idx];
                new_aliens.push_back(a);
            }
        }
        return new_aliens;
    };
    std::vector<Alien> aliens = init_aliens();
    float time_counter = 0.0f;
    const float wave_amplitude = 30.0f; // How far left/right aliens move
    const float wave_frequency = 0.02f; // Speed of the wave
    const float row_phase_delay = 0.5f; // Phase delay between rows
    SDL_Event event;
    while (running) {
        time_counter += 1.0f;
        // Event-Handling
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                running = false;
            }
        }

        // Win detection
        if (!win && !game_over && aliens.empty()) {
            win = true;
        }

        // Game Over detection - check if any alien reached the bottom
        if (!game_over && !win) {
            for (const auto& alien : aliens) {
                if (alien.y + alien_size >= window_height) {
                    game_over = true;
                    break;
                }
            }
        }

        // Tastenzustand abfragen
        const Uint8* state = SDL_GetKeyboardState(NULL);

        if (game_over) {
            SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0);
            SDL_RenderClear(renderer);
            SDL_Color textColor = {255, 0, 0, 255}; // Rot für Game Over
            std::string gameOverText = "Game Over";
            SDL_Surface* textSurface = TTF_RenderUTF8_Blended(font, gameOverText.c_str(), textColor);
            int textY = window_height/2 - 60;
            if (textSurface) {
                SDL_Texture* textTexture = SDL_CreateTextureFromSurface(renderer, textSurface);
                int tw = textSurface->w;
                int th = textSurface->h;
                SDL_Rect dstRect = {window_width/2 - tw/2, textY, tw, th};
                SDL_RenderCopy(renderer, textTexture, NULL, &dstRect);
                SDL_DestroyTexture(textTexture);
                SDL_FreeSurface(textSurface);
            }
            // Draw 'Press ENTER to start a new game' at the bottom
            SDL_Color bottomTextColor = {150, 150, 150, 255}; // gray
            std::string bottomText = "Press ENTER to start a new game";
            SDL_Surface* bottomSurface = TTF_RenderUTF8_Blended(font, bottomText.c_str(), bottomTextColor);
            if (bottomSurface) {
                SDL_Texture* bottomTexture = SDL_CreateTextureFromSurface(renderer, bottomSurface);
                int btw = bottomSurface->w;
                int bth = bottomSurface->h;
                SDL_Rect bottomRect = {window_width/2 - btw/2, window_height - bth - 40, btw, bth};
                SDL_RenderCopy(renderer, bottomTexture, NULL, &bottomRect);
                SDL_DestroyTexture(bottomTexture);
                SDL_FreeSurface(bottomSurface);
            }
            // Only restart on ENTER
            bool enter_pressed = state[SDL_SCANCODE_RETURN];
            if (enter_pressed) {
                aliens = init_aliens();
                projectiles.clear();
                triangle_x = window_width / 2.0f;
                time_counter = 0.0f;
                game_over = false;
            }
            SDL_RenderPresent(renderer);
            SDL_Delay(10);
            continue;
        }

        if (win) {
            SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0);
            SDL_RenderClear(renderer);
            SDL_Color textColor = {255, 255, 0, 255};
            std::string winText = "You're Winner!";
            SDL_Surface* textSurface = TTF_RenderUTF8_Blended(font, winText.c_str(), textColor);
            int textY = window_height/2 - 60;
            int trophyY = window_height/2 + 10;
            if (textSurface) {
                SDL_Texture* textTexture = SDL_CreateTextureFromSurface(renderer, textSurface);
                int tw = textSurface->w;
                int th = textSurface->h;
                SDL_Rect dstRect = {window_width/2 - tw/2, textY, tw, th};
                SDL_RenderCopy(renderer, textTexture, NULL, &dstRect);
                SDL_DestroyTexture(textTexture);
                SDL_FreeSurface(textSurface);
            }
            // Draw trophy PNG below the text
            if (trophyTexture) {
                int trophyW = 64, trophyH = 64;
                SDL_Rect trophyRect = {window_width/2 - trophyW/2, trophyY, trophyW, trophyH};
                SDL_RenderCopy(renderer, trophyTexture, NULL, &trophyRect);
            }
            // Draw 'Press ENTER to start a new game' at the bottom
            SDL_Color bottomTextColor = {50, 50, 50, 255}; // dark gray
            std::string bottomText = "Press ENTER to start a new game";
            SDL_Surface* bottomSurface = TTF_RenderUTF8_Blended(font, bottomText.c_str(), bottomTextColor);
            if (bottomSurface) {
                SDL_Texture* bottomTexture = SDL_CreateTextureFromSurface(renderer, bottomSurface);
                int btw = bottomSurface->w;
                int bth = bottomSurface->h;
                SDL_Rect bottomRect = {window_width/2 - btw/2, window_height - bth - 40, btw, bth};
                SDL_RenderCopy(renderer, bottomTexture, NULL, &bottomRect);
                SDL_DestroyTexture(bottomTexture);
                SDL_FreeSurface(bottomSurface);
            }
            // Only restart on ENTER
            bool enter_pressed = state[SDL_SCANCODE_RETURN];
            if (enter_pressed) {
                aliens = init_aliens();
                projectiles.clear();
                triangle_x = window_width / 2.0f;
                time_counter = 0.0f;
                win = false;
            }
            SDL_RenderPresent(renderer);
            SDL_Delay(10);
            continue;
        }

        // Hintergrund zeichnen
        if (backgroundTexture) {
            SDL_RenderCopy(renderer, backgroundTexture, NULL, NULL);
        } else {
            SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0);
            SDL_RenderClear(renderer);
        }

        // Gorilla zeichnen
        if (gorillaTexture) {
            int gorillaW = 64, gorillaH = 64;
            SDL_Rect gorillaRect = {static_cast<int>(triangle_x) - gorillaW/2, window_height - gorillaH - 10, gorillaW, gorillaH};
            SDL_RenderCopy(renderer, gorillaTexture, NULL, &gorillaRect);
        }

        if (state[SDL_SCANCODE_A]) {
            triangle_x -= triangle_speed;
        }
        if (state[SDL_SCANCODE_D]) {
            triangle_x += triangle_speed;
        }
        // Begrenzung, damit der Gorilla im Fenster bleibt
        triangle_x = std::max(triangle_x, 32.0f);
        triangle_x = std::min(triangle_x, window_width - 32.0f);
        // Geschoss abfeuern, wenn Leertaste gedrückt und weniger als 3 aktiv
        static bool space_was_pressed = false;
        if (state[SDL_SCANCODE_SPACE]) {
            if (!space_was_pressed && projectiles.size() < max_projectiles) {
                Projectile p;
                p.x = triangle_x;
                p.y = static_cast<float>(window_height - 32); // Unterkante Gorilla
                p.speed = projectile_speed;
                projectiles.push_back(p);
            }
            space_was_pressed = true;
        } else {
            space_was_pressed = false;
        }

        // Kollisionserkennung: Geschoss trifft Alien
        for (auto proj_it = projectiles.begin(); proj_it != projectiles.end(); ) {
            bool hit = false;
            for (auto alien_it = aliens.begin(); alien_it != aliens.end(); ++alien_it) {
                // Rechteck für Alien
                SDL_Rect alien_rect = { static_cast<int>(alien_it->x), static_cast<int>(alien_it->y), alien_size, alien_size };
                // Rechteck für Fass-Projektil
                int barrelW = 32, barrelH = 48;
                SDL_Rect proj_rect = { static_cast<int>(proj_it->x) - barrelW/2, static_cast<int>(proj_it->y) - barrelH, barrelW, barrelH };
                // SDL_HasIntersection prüft auf Überlappung
                if (SDL_HasIntersection(&alien_rect, &proj_rect)) {
                    // Alien entfernen
                    aliens.erase(alien_it);
                    hit = true;
                    break;
                }
            }
            if (hit) {
                proj_it = projectiles.erase(proj_it);
            } else {
                ++proj_it;
            }
        }

        // Aliens bewegen und zeichnen (individuelle Farbe)
        for (auto& alien : aliens) {
            alien.y += alien_speed; // Bewegung nach unten
            // Sine wave lateral movement with row-based phase delay
            float phase = time_counter * wave_frequency + alien.row * row_phase_delay;
            alien.x = alien.initial_x + wave_amplitude * sin(phase);
            DrawAlien(renderer, static_cast<int>(alien.x), static_cast<int>(alien.y), alien_size, alien.color);
        }

        // Geschosse bewegen und zeichnen
        for (auto& proj : projectiles) {
            proj.y -= proj.speed;
        }
        // Entferne Geschosse, die den oberen Rand verlassen haben
        projectiles.erase(
            std::remove_if(projectiles.begin(), projectiles.end(), [](const Projectile& p) {
                return p.y + 48 < 0;
            }),
            projectiles.end()
        );
        // Zeichne alle verbleibenden Geschosse
        for (const auto& proj : projectiles) {
            if (barrelTexture) {
                int barrelW = 32, barrelH = 48;
                SDL_Rect barrelRect = { static_cast<int>(proj.x) - barrelW/2, static_cast<int>(proj.y) - barrelH, barrelW, barrelH };
                SDL_RenderCopy(renderer, barrelTexture, NULL, &barrelRect);
            }
        }
        // ... entfernt: Dreieck-Zeichnung ...

        SDL_RenderPresent(renderer);
        SDL_Delay(10);
    }

    if (trophyTexture) SDL_DestroyTexture(trophyTexture);
    if (gorillaTexture) SDL_DestroyTexture(gorillaTexture);
    if (barrelTexture) SDL_DestroyTexture(barrelTexture);
    if (backgroundTexture) SDL_DestroyTexture(backgroundTexture);
    TTF_CloseFont(font);
    TTF_Quit();
    IMG_Quit();
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
    return 0;
}
