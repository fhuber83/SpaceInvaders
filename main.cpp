#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>
#include <iostream>
#include <algorithm> // Für std::max und std::min
#include <vector>
#include <cmath> // Für sin()

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

static const int initial_window_width = 800;//1280;
static const int initial_window_height = 600;//720;

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

// Boss-Zustand
struct Boss {
    float x, y;
    float initial_x;
    int hp;
    int max_hp;
    Color color;
    bool active;
};

// Boss explosion particle
struct BossParticle {
    float x, y;
    float vx, vy; // velocity
    Color color;
    int size; // pixel size
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
    float vx, vy; // Velocity components for directional movement
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
    const int logical_width = initial_window_width;  // Logical game resolution
    const int logical_height = initial_window_height;
    int window_width = initial_window_width;   // Actual window size
    int window_height = initial_window_height;
    const int triangle_base = 60;
    const int triangle_height = 40;
    const float triangle_speed = 5.0f;
    float triangle_x = logical_width / 2.0f;

    SDL_Window* window = SDL_CreateWindow("Space Invaders", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, window_width, window_height, SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE);
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
    // Set logical size for automatic scaling
    SDL_RenderSetLogicalSize(renderer, logical_width, logical_height);
    SDL_RenderSetIntegerScale(renderer, SDL_FALSE); // Allow non-integer scaling for smooth resize
    
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
    // Load boss background JPG
    SDL_Texture* bossBackgroundTexture = nullptr;
    {
        SDL_Surface* bossBackgroundSurface = IMG_Load("boss_background.jpg");
        if (!bossBackgroundSurface) {
            std::cerr << "IMG_Load Error: " << IMG_GetError() << std::endl;
        } else {
            bossBackgroundTexture = SDL_CreateTextureFromSurface(renderer, bossBackgroundSurface);
            SDL_FreeSurface(bossBackgroundSurface);
        }
    }

    // Geschossverwaltung
    std::vector<Projectile> projectiles;
    const int max_projectiles = 3;
    const float projectile_speed = 6.0f;
    
    // Boss projectiles
    std::vector<Projectile> boss_projectiles;
    const float boss_projectile_speed = 3.0f;
    int boss_shoot_cooldown = 100 + rand() % 201; // Random initial cooldown (100-300 frames = 1-3 seconds)

    bool running = true;
    bool win = false;
    bool game_over = false;
    bool boss_fight = false;
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
    
    // Boss initialization
    Boss boss;
    boss.x = logical_width / 2.0f;
    boss.y = 100.0f;
    boss.initial_x = boss.x;
    boss.max_hp = 30;
    boss.hp = boss.max_hp;
    boss.color = ega_palette[14]; // Yellow
    boss.active = false;
    const int boss_size = 192; // 4x the regular alien size
    float boss_figure8_time = 0.0f;
    float boss_random_offset_x = 0.0f;
    float boss_random_offset_y = 0.0f;
    float boss_target_offset_x = 0.0f;
    float boss_target_offset_y = 0.0f;
    int boss_random_update_counter = 0;
    const int boss_random_update_interval = 30; // Update randomness every 30 frames
    
    // Boss explosion particles
    std::vector<BossParticle> boss_particles;
    bool boss_dying = false;
    bool boss_exploding = false;
    int death_timer = 0;
    const int death_duration = 180; // 3 seconds at 60fps for blinking
    int explosion_timer = 0;
    const int explosion_duration = 180; // 3s at 60 FPS
    const float gravity = 0.5f;
    const float damping = 0.8f; // Energy loss on collision
    
    float time_counter = 0.0f;
    const float wave_amplitude = 30.0f; // How far left/right aliens move
    const float wave_frequency = 0.02f; // Speed of the wave
    const float row_phase_delay = 0.5f; // Phase delay between rows
    SDL_Event event;
    bool is_fullscreen = false;
    while (running) {
        time_counter += 1.0f;
        // Event-Handling
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                running = false;
            }
            // Handle window resize
            else if (event.type == SDL_WINDOWEVENT) {
                if (event.window.event == SDL_WINDOWEVENT_RESIZED) {
                    window_width = event.window.data1;
                    window_height = event.window.data2;
                }
            }
            // Handle ALT-ENTER for fullscreen toggle
            else if (event.type == SDL_KEYDOWN) {
                if (event.key.keysym.sym == SDLK_RETURN && (event.key.keysym.mod & KMOD_ALT)) {
                    is_fullscreen = !is_fullscreen;
                    if (is_fullscreen) {
                        SDL_SetWindowFullscreen(window, SDL_WINDOW_FULLSCREEN_DESKTOP);
                        SDL_GetWindowSize(window, &window_width, &window_height);
                    } else {
                        SDL_SetWindowFullscreen(window, 0);
                        window_width = initial_window_width;
                        window_height = initial_window_height;
                        SDL_SetWindowSize(window, window_width, window_height);
                    }
                }
            }
        }

        // Boss fight trigger - when all regular aliens are defeated
        if (!boss_fight && !game_over && aliens.empty() && !boss.active) {
            boss_fight = true;
            boss.active = true;
            boss.x = logical_width / 2.0f;
            boss.y = 100.0f;
            boss.initial_x = boss.x;
            boss.hp = boss.max_hp;
            boss_figure8_time = 0.0f;
            boss_random_update_counter = 0;
        }

        // Win detection - defeat the boss triggers death phase
        if (boss_fight && !game_over && !boss_dying && !boss_exploding && boss.hp <= 0) {
            // Trigger death/blinking phase
            boss_dying = true;
            death_timer = 0;
        }
        
        // Death phase - boss blinks before exploding
        if (boss_dying) {
            death_timer++;
            if (death_timer >= death_duration) {
                // Trigger explosion after blinking
                boss_exploding = true;
                boss_dying = false;
                boss.active = false;
                explosion_timer = 0;
                
                // Create explosion particles from boss pixels
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
                int pixel_size = boss_size / N;
                for (int row = 0; row < N; ++row) {
                    for (int col = 0; col < N; ++col) {
                        if (alien[row] & (1 << (7 - col))) {
                            BossParticle p;
                            p.x = boss.x - boss_size/2 + col * pixel_size + pixel_size/2;
                            p.y = boss.y + row * pixel_size + pixel_size/2;
                            // Random velocity for explosion
                            float angle = ((float)rand() / RAND_MAX) * 2.0f * M_PI;
                            float speed = 2.0f + ((float)rand() / RAND_MAX) * 8.0f;
                            p.vx = cos(angle) * speed;
                            p.vy = sin(angle) * speed - 5.0f; // Bias upward
                            p.color = ega_palette[15]; // White color for particles
                            p.size = pixel_size;
                            boss_particles.push_back(p);
                        }
                    }
                }
            }
        }
        
        // Check if explosion is complete
        if (boss_exploding) {
            explosion_timer++;
            if (explosion_timer >= explosion_duration) {
                win = true;
                boss_exploding = false;
            }
        }

        // Game Over detection - check if any alien reached the bottom
        if (!game_over && !win) {
            for (const auto& alien : aliens) {
                if (alien.y + alien_size >= logical_height) {
                    game_over = true;
                    break;
                }
            }
            // Boss doesn't trigger game over by position since it flies in a pattern
        }

        // Tastenzustand abfragen
        const Uint8* state = SDL_GetKeyboardState(NULL);

        if (game_over) {
            SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0);
            SDL_RenderClear(renderer);
            SDL_Color textColor = {255, 0, 0, 255}; // Rot für Game Over
            std::string gameOverText = "Game Over";
            SDL_Surface* textSurface = TTF_RenderUTF8_Blended(font, gameOverText.c_str(), textColor);
            int textY = logical_height/2 - 60;
            if (textSurface) {
                SDL_Texture* textTexture = SDL_CreateTextureFromSurface(renderer, textSurface);
                int tw = textSurface->w;
                int th = textSurface->h;
                SDL_Rect dstRect = {logical_width/2 - tw/2, textY, tw, th};
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
                SDL_Rect bottomRect = {logical_width/2 - btw/2, logical_height - bth - 40, btw, bth};
                SDL_RenderCopy(renderer, bottomTexture, NULL, &bottomRect);
                SDL_DestroyTexture(bottomTexture);
                SDL_FreeSurface(bottomSurface);
            }
            // Only restart on ENTER
            bool enter_pressed = state[SDL_SCANCODE_RETURN];
            if (enter_pressed) {
                aliens = init_aliens();
                projectiles.clear();
                boss_projectiles.clear();
                boss_particles.clear();
                triangle_x = logical_width / 2.0f;
                time_counter = 0.0f;
                boss.active = false;
                boss.hp = boss.max_hp;
                boss_fight = false;
                boss_dying = false;
                boss_exploding = false;
                death_timer = 0;
                explosion_timer = 0;
                boss_figure8_time = 0.0f;
                boss_random_update_counter = 0;
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
            int textY = logical_height/2 - 60;
            int trophyY = logical_height/2 + 10;
            if (textSurface) {
                SDL_Texture* textTexture = SDL_CreateTextureFromSurface(renderer, textSurface);
                int tw = textSurface->w;
                int th = textSurface->h;
                SDL_Rect dstRect = {logical_width/2 - tw/2, textY, tw, th};
                SDL_RenderCopy(renderer, textTexture, NULL, &dstRect);
                SDL_DestroyTexture(textTexture);
                SDL_FreeSurface(textSurface);
            }
            // Draw trophy PNG below the text
            if (trophyTexture) {
                int trophyW = 64, trophyH = 64;
                SDL_Rect trophyRect = {logical_width/2 - trophyW/2, trophyY, trophyW, trophyH};
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
                SDL_Rect bottomRect = {logical_width/2 - btw/2, logical_height - bth - 40, btw, bth};
                SDL_RenderCopy(renderer, bottomTexture, NULL, &bottomRect);
                SDL_DestroyTexture(bottomTexture);
                SDL_FreeSurface(bottomSurface);
            }
            // Only restart on ENTER
            bool enter_pressed = state[SDL_SCANCODE_RETURN];
            if (enter_pressed) {
                aliens = init_aliens();
                projectiles.clear();
                boss_projectiles.clear();
                boss_particles.clear();
                triangle_x = logical_width / 2.0f;
                time_counter = 0.0f;
                boss.active = false;
                boss.hp = boss.max_hp;
                boss_fight = false;
                boss_dying = false;
                boss_exploding = false;
                death_timer = 0;
                explosion_timer = 0;
                boss_figure8_time = 0.0f;
                boss_random_update_counter = 0;
                win = false;
            }
            SDL_RenderPresent(renderer);
            SDL_Delay(10);
            continue;
        }

        // Hintergrund zeichnen
        if (boss_fight && bossBackgroundTexture) {
            SDL_RenderCopy(renderer, bossBackgroundTexture, NULL, NULL);
        } else if (backgroundTexture) {
            SDL_RenderCopy(renderer, backgroundTexture, NULL, NULL);
        } else {
            SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0);
            SDL_RenderClear(renderer);
        }

        // Gorilla zeichnen
        if (gorillaTexture) {
            int gorillaW = 64, gorillaH = 64;
            SDL_Rect gorillaRect = {static_cast<int>(triangle_x) - gorillaW/2, logical_height - gorillaH - 10, gorillaW, gorillaH};
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
        triangle_x = std::min(triangle_x, logical_width - 32.0f);
        // Geschoss abfeuern, wenn Leertaste gedrückt und weniger als 3 aktiv
        static bool space_was_pressed = false;
        if (state[SDL_SCANCODE_SPACE]) {
            if (!space_was_pressed && projectiles.size() < max_projectiles) {
                Projectile p;
                p.x = triangle_x;
                p.y = static_cast<float>(logical_height - 32); // Unterkante Gorilla
                p.speed = projectile_speed;
                p.vx = 0.0f; // Player projectiles go straight up
                p.vy = -projectile_speed;
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

        // Kollisionserkennung: Geschoss trifft Boss
        if (boss.active) {
            for (auto proj_it = projectiles.begin(); proj_it != projectiles.end(); ) {
                SDL_Rect boss_rect = { static_cast<int>(boss.x - boss_size/2), static_cast<int>(boss.y), boss_size, boss_size };
                int barrelW = 32, barrelH = 48;
                SDL_Rect proj_rect = { static_cast<int>(proj_it->x) - barrelW/2, static_cast<int>(proj_it->y) - barrelH, barrelW, barrelH };
                if (SDL_HasIntersection(&boss_rect, &proj_rect)) {
                    boss.hp--;
                    proj_it = projectiles.erase(proj_it);
                } else {
                    ++proj_it;
                }
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

        // Boss bewegen und zeichnen
        if (boss.active) {
            // Only move if not dying
            if (!boss_dying) {
                // Update random target offsets periodically
                boss_random_update_counter--;
                if (boss_random_update_counter <= 0) {
                    boss_target_offset_x = (rand() % 61 - 30); // -30 to +30 pixels
                    boss_target_offset_y = (rand() % 41 - 20); // -20 to +20 pixels
                    boss_random_update_counter = boss_random_update_interval;
                }
                
                // Smoothly interpolate current offsets toward target offsets
                float lerp_factor = 0.05f; // Adjust for smoothness (lower = smoother)
                boss_random_offset_x += (boss_target_offset_x - boss_random_offset_x) * lerp_factor;
                boss_random_offset_y += (boss_target_offset_y - boss_random_offset_y) * lerp_factor;
                
                // Figure-eight (lemniscate) pattern
                boss_figure8_time += 0.015f; // Speed of figure-eight
                float scale = 150.0f; // Size of the figure-eight
                float t = boss_figure8_time;
                
                // Lemniscate of Gerono equations with center at initial position
                float center_x = logical_width / 2.0f;
                float center_y = 150.0f; // Keep boss near top of screen
                
                // Figure-eight parametric equations
                float eight_x = scale * sin(t);
                float eight_y = scale * sin(t) * cos(t); // Creates the figure-eight shape
                
                boss.x = center_x + eight_x + boss_random_offset_x;
                boss.y = center_y + eight_y + boss_random_offset_y;
            }
            
            // Render boss with blinking effect during death phase
            if (boss_dying) {
                // Blink effect: visible every 8 frames, gradually faster
                int blink_speed = 8 - (death_timer * 6 / death_duration); // Speed up blinking
                if (blink_speed < 2) blink_speed = 2;
                if ((death_timer / blink_speed) % 2 == 0) {
                    DrawAlien(renderer, static_cast<int>(boss.x - boss_size/2), static_cast<int>(boss.y), boss_size, ega_palette[15]); // White
                }
            } else {
                DrawAlien(renderer, static_cast<int>(boss.x - boss_size/2), static_cast<int>(boss.y), boss_size, boss.color);
            }
            
            // Draw boss health bar
            int bar_width = 200;
            int bar_height = 20;
            int bar_x = logical_width / 2 - bar_width / 2;
            int bar_y = 20;
            // Background (red)
            SDL_SetRenderDrawColor(renderer, 100, 0, 0, 255);
            SDL_Rect bg_rect = {bar_x, bar_y, bar_width, bar_height};
            SDL_RenderFillRect(renderer, &bg_rect);
            // Health (green)
            int health_width = (bar_width * boss.hp) / boss.max_hp;
            SDL_SetRenderDrawColor(renderer, 0, 255, 0, 255);
            SDL_Rect health_rect = {bar_x, bar_y, health_width, bar_height};
            SDL_RenderFillRect(renderer, &health_rect);
            // Border (white)
            SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
            SDL_RenderDrawRect(renderer, &bg_rect);
            
            // Boss shooting logic (only when not dying)
            if (!boss_dying) {
                boss_shoot_cooldown--;
                if (boss_shoot_cooldown <= 0) {
                    Projectile laser;
                    laser.x = boss.x;
                    laser.y = boss.y + boss_size;
                    
                    // Calculate direction to player with randomness
                    float target_x = triangle_x + (rand() % 81 - 40); // +/- 40 pixels randomness
                    float target_y = logical_height - 32; // Player's approximate y position
                    float dx = target_x - laser.x;
                    float dy = target_y - laser.y;
                    float distance = sqrt(dx*dx + dy*dy);
                    
                    // Normalize and scale to boss_projectile_speed
                    laser.vx = (dx / distance) * boss_projectile_speed;
                    laser.vy = (dy / distance) * boss_projectile_speed;
                    laser.speed = boss_projectile_speed; // Keep for compatibility
                    
                    boss_projectiles.push_back(laser);
                    boss_shoot_cooldown = 100 + rand() % 201; // Random cooldown (100-300 frames = 1-3 seconds)
                }
            }
        }

        // Boss explosion particles - physics simulation
        if (boss_exploding) {
            for (auto& particle : boss_particles) {
                // Apply gravity
                particle.vy += gravity;
                
                // Update position
                particle.x += particle.vx;
                particle.y += particle.vy;
                
                // Collision with screen edges
                if (particle.x - particle.size/2 <= 0 || particle.x + particle.size/2 >= window_width) {
                    particle.vx = -particle.vx * damping;
                    particle.x = (particle.x < logical_width/2) ? particle.size/2 : window_width - particle.size/2;
                }
                if (particle.y - particle.size/2 <= 0 || particle.y + particle.size/2 >= logical_height) {
                    particle.vy = -particle.vy * damping;
                    particle.y = (particle.y < logical_height/2) ? particle.size/2 : logical_height - particle.size/2;
                }
                
                // Simple particle-to-particle collision
                for (auto& other : boss_particles) {
                    if (&particle == &other) continue;
                    
                    float dx = other.x - particle.x;
                    float dy = other.y - particle.y;
                    float dist = sqrt(dx*dx + dy*dy);
                    float min_dist = particle.size;
                    
                    if (dist < min_dist && dist > 0.1f) {
                        // Normalize collision vector
                        dx /= dist;
                        dy /= dist;
                        
                        // Separate particles
                        float overlap = min_dist - dist;
                        particle.x -= dx * overlap * 0.5f;
                        particle.y -= dy * overlap * 0.5f;
                        other.x += dx * overlap * 0.5f;
                        other.y += dy * overlap * 0.5f;
                        
                        // Exchange velocities along collision normal (simplified elastic collision)
                        float relative_vx = particle.vx - other.vx;
                        float relative_vy = particle.vy - other.vy;
                        float dot = (relative_vx * dx + relative_vy * dy) * damping;
                        
                        particle.vx -= dot * dx;
                        particle.vy -= dot * dy;
                        other.vx += dot * dx;
                        other.vy += dot * dy;
                    }
                }
            }
            
            // Draw particles
            for (const auto& particle : boss_particles) {
                SDL_SetRenderDrawColor(renderer, particle.color.r, particle.color.g, particle.color.b, 255);
                SDL_Rect rect = {
                    static_cast<int>(particle.x - particle.size/2),
                    static_cast<int>(particle.y - particle.size/2),
                    particle.size,
                    particle.size
                };
                SDL_RenderFillRect(renderer, &rect);
            }
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
        
        // Boss projectiles bewegen und zeichnen
        for (auto& laser : boss_projectiles) {
            laser.x += laser.vx; // Move using velocity components
            laser.y += laser.vy;
        }
        // Entferne Boss-Geschosse, die den unteren Rand verlassen haben
        boss_projectiles.erase(
            std::remove_if(boss_projectiles.begin(), boss_projectiles.end(), [&](const Projectile& p) {
                return p.y > window_height;
            }),
            boss_projectiles.end()
        );
        // Zeichne alle verbleibenden Boss-Geschosse als rote Kugeln
        for (const auto& laser : boss_projectiles) {
            int laser_radius = 12;
            SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255); // Red
            // Draw filled circle
            for (int w = 0; w < laser_radius * 2; w++) {
                for (int h = 0; h < laser_radius * 2; h++) {
                    int dx = laser_radius - w;
                    int dy = laser_radius - h;
                    if ((dx*dx + dy*dy) <= (laser_radius * laser_radius)) {
                        SDL_RenderDrawPoint(renderer, static_cast<int>(laser.x) + dx, static_cast<int>(laser.y) + dy);
                    }
                }
            }
        }
        
        // Kollisionserkennung: Boss laser trifft Spieler
        int gorillaW = 64, gorillaH = 64;
        SDL_Rect player_rect = {static_cast<int>(triangle_x) - gorillaW/2, logical_height - gorillaH - 10, gorillaW, gorillaH};
        for (const auto& laser : boss_projectiles) {
            int laser_radius = 12;
            SDL_Rect laser_rect = {static_cast<int>(laser.x) - laser_radius, static_cast<int>(laser.y) - laser_radius, laser_radius * 2, laser_radius * 2};
            if (SDL_HasIntersection(&player_rect, &laser_rect)) {
                game_over = true;
                break;
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
    if (bossBackgroundTexture) SDL_DestroyTexture(bossBackgroundTexture);
    TTF_CloseFont(font);
    TTF_Quit();
    IMG_Quit();
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
    return 0;
}
