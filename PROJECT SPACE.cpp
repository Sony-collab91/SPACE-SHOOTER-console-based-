
#include <iostream>
#include <conio.h>
#include <windows.h>
#include <fstream>
using namespace std;

// Screen size 
const int WIDTH = 40;
const int HEIGHT = 16;

const int MAX_BULLETS = 12;
const int MAX_ENEMIES = 18;

// ASCII shapes 
const char PLAYER_SHAPE = '^';      // spaceship top
const char ENEMY_SHAPE = (char)219; // solid block
const char BULLET_SHAPE = '|';
const char EMPTY_CHAR = ' ';
const char LIFE_CHAR = 3;        // heart symbol 

// Enemy struct
struct Enemy 
{
    int x;
    int y;
    bool alive;
};

// Console helpers
HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);

void setColor(int color) 
{
    SetConsoleTextAttribute(hConsole, color);
}

void moveCursorToTopLeft()
{
    COORD coord = { 0, 0 };
    SetConsoleCursorPosition(hConsole, coord);
}

// Read / write high score 
int readHighScore() 
{
    ifstream fin("highscore.txt");
    int h = 0;
    if (fin.is_open()) 
    {
        fin >> h;
        fin.close();
    }
    return h;
}
void writeHighScore(int v) 
{
    ofstream fout("highscore.txt");
    if (fout.is_open()) 
    {
        fout << v;
        fout.close();
    }
}

//  Enemy spawn 
void spawnEnemies(Enemy enemies[], int enemyCount, int level)
{
    int cols = enemyCount;
    if (cols > WIDTH - 6) cols = WIDTH - 6;
    int gap = (WIDTH - 6) / (cols > 0 ? cols : 1);
    int startX = 3;
    for (int i = 0; i < MAX_ENEMIES; i++) 
    {
        enemies[i].alive = false;
        enemies[i].x = 1;
        enemies[i].y = 1;
    }
    for (int i = 0; i < enemyCount && i < MAX_ENEMIES; i++) 
    {
        enemies[i].alive = true;
        enemies[i].x = startX + i * gap;
        if (enemies[i].x >= WIDTH - 2) enemies[i].x = (WIDTH - 3) - i;
        enemies[i].y = 1 + (i / cols); 
        if (enemies[i].y >= 4) enemies[i].y = 1 + (i % 2);
    }
}

void drawFrame(int playerX, int playerY,
    int bulletX[], int bulletY[], int bulletCount,
    Enemy enemies[], int enemyCount,
    int score, int lives, int level, int highscore)
{
    // Move cursor to top instead of clearing screen
    moveCursorToTopLeft();

    // Header (colored)
    setColor(11); // cyan
    cout << "SPACE SHOOTER  | Level: " << level << "  Score: " << score << "  Lives: ";
    setColor(12); // red for hearts
    for (int i = 0; i < lives; i++) 
    {
        cout << LIFE_CHAR << ' ';
    }
    setColor(11);
    cout << "  High: ";
    setColor(14);
    cout << highscore;
    setColor(7);
    cout << "\n";
    for (int i = 0; i < WIDTH; i++) 
        cout << (char)196; // header separator
    cout << "\n";

    // Draw playfield
    for (int y = 0; y < HEIGHT; y++)
    {
        for (int x = 0; x < WIDTH; x++) 
        {
            bool printed = false;

            // Player
            if (x == playerX && y == playerY)
            {
                setColor(10); // green
                cout << PLAYER_SHAPE;
                setColor(7);
                printed = true;
            }

            // Bullets
            for (int b = 0; b < bulletCount; b++) 
            {
                if (x == bulletX[b] && y == bulletY[b])
                {
                    setColor(14); // yellow
                    cout << BULLET_SHAPE;
                    setColor(7);
                    printed = true;
                }
            }

            // Enemies
            for (int e = 0; e < enemyCount; e++) 
            {
                if (enemies[e].alive && x == enemies[e].x && y == enemies[e].y) 
                {
                    setColor(12); // red
                    cout << ENEMY_SHAPE;
                    setColor(7);
                    printed = true;
                }
            }

            // background pattern to resemble document style (dots)
            if (!printed)
            {
                setColor(8); // dark grey
                cout << '.';
                setColor(7);
            }
        }
        cout << "\n";
    }

    for (int i = 0; i < WIDTH; i++) cout << (char)196;
    cout << "\n";
    setColor(15);
    cout << "Controls: a = left    d = right    w = shoot    q = quit\n";
    cout << "Press keys directly; game runs in real time.\n";
    // keep color normal
    setColor(7);
}

int main() 
{
    // Initial instruction screen
    setColor(11);
    cout << "=== SPACE SHOOTER (Console) ===\n\n";
    setColor(7);
    cout << "Instructions:\n";
    cout << " - Move left:  'a'\n";
    cout << " - Move right: 'd'\n";
    cout << " - Shoot:      'w'\n";
    cout << " - Quit:       'q'\n\n";
    cout << "You have 3 lives. Score >= level*10 -> advance level (5 levels total).\n";
    cout << "Press SPACE to start (or 'q' to exit)...\n";

    // wait for space or q
    while (true) 
    {
        if (_kbhit())
        {
            char c = _getch();
            if (c == ' ') break;
            if (c == 'q' || c == 'Q') return 0;
        }
    }

    int highscore = readHighScore();

    // Game state
    int playerX = WIDTH / 2;
    int playerY = HEIGHT - 2;

    int bulletX[MAX_BULLETS], bulletY[MAX_BULLETS];
    int bulletCount = 0;

    Enemy enemies[MAX_ENEMIES];
    int level = 1;
    int lives = 3;
    int score = 0;
    bool gameRunning = true;

    int enemyCount = 5; // start
    int frameDelay = 250; // ms, will decrease by level

    spawnEnemies(enemies, enemyCount, level);

    // Main loop
    while (gameRunning) 
    {
        // Draw current frame (overwrite previous, no full clear)
        drawFrame(playerX, playerY, bulletX, bulletY, bulletCount, enemies, enemyCount, score, lives, level, highscore);

        // Input (non-blocking)
        if (_kbhit())
        {
            char k = _getch();
            if (k == 'a' || k == 'A') 
            {
                if (playerX > 1) playerX--;
            }
            else if (k == 'd' || k == 'D') 
            {
                if (playerX < WIDTH - 2) playerX++;
            }
            else if (k == 'w' || k == 'W') 
            {
                if (bulletCount < MAX_BULLETS)
                {
                    bulletX[bulletCount] = playerX;
                    bulletY[bulletCount] = playerY - 1;
                    bulletCount++;
                }
            }
            else if (k == 'q' || k == 'Q') 
            {
                gameRunning = false;
                break;
            }
        }

        // Move bullets up
        for (int b = 0; b < bulletCount; b++)
        {
            bulletY[b]--;
        }
        // Remove off-screen bullets
        for (int b = 0; b < bulletCount; b++) 
        {
            if (bulletY[b] < 0) 
            {
                // shift left to remove index b
                for (int j = b; j < bulletCount - 1; j++) 
                {
                    bulletX[j] = bulletX[j + 1];
                    bulletY[j] = bulletY[j + 1];
                }
                bulletCount--;
                b--;
            }
        }

        // Move enemies down 
        for (int e = 0; e < enemyCount; e++) 
        {
            if (!enemies[e].alive) 
                continue;
            static int tick = 0;
            tick++;
            int moveRate = 6 - (level);       // higher level => smaller moveRate => move more often
            if (moveRate < 1) moveRate = 1;
            if ((tick + e) % moveRate == 0) {
                enemies[e].y += 1;
            }
            // if enemy reaches bottom -> player loses a life and enemy resets top
            if (enemies[e].y >= HEIGHT)
            {
                enemies[e].y = 1;
                enemies[e].x = 2 + (e * 3) % (WIDTH - 6);
                lives--;
                enemies[e].alive = true;
                if (lives <= 0) 
                {
                    gameRunning = false;
                    break;
                }
            }
        }

        // Bullet-enemy collisions
        for (int b = 0; b < bulletCount; b++)
        {
            for (int e = 0; e < enemyCount; e++) 
            {
                if (!enemies[e].alive) 
                    continue;
                if (bulletX[b] == enemies[e].x && bulletY[b] == enemies[e].y) {
                    // hit
                    enemies[e].alive = false;
                    score++;
                    // remove bullet b
                    for (int j = b; j < bulletCount - 1; j++) 
                    {
                        bulletX[j] = bulletX[j + 1];
                        bulletY[j] = bulletY[j + 1];
                    }
                    bulletCount--;
                    b--;
                    break;
                }
            }
        }

        // Enemy-player collision
        for (int e = 0; e < enemyCount; e++) 
        {
            if (!enemies[e].alive) 
                continue;
            if (enemies[e].x == playerX && enemies[e].y == playerY)
            {
                enemies[e].alive = false;
                lives--;
                if (lives <= 0) 
                {
                    gameRunning = false;
                    break;
                }
            }
        }

        if (!gameRunning) break;

        // If enough score -> level up
        if (score >= level * 10) 
        {
            level++;
            if (level > 5) 
            {
                // victory
                drawFrame(playerX, playerY, bulletX, bulletY, bulletCount, enemies, enemyCount, score, lives, level, highscore);
                setColor(13); // magenta
                cout << "\nCONGRATULATIONS! You finished all levels!\n";
                setColor(7);
                if (score > highscore) 
                   writeHighScore(score);
                cout << "Press any key to exit...\n";
                _getch();
                return 0;
            }
            // increase difficulty
            enemyCount += 3;
            if (enemyCount > MAX_ENEMIES) enemyCount = MAX_ENEMIES;
            frameDelay -= 40;
            if (frameDelay < 80) frameDelay = 80;
            // respawn enemies for new level
            spawnEnemies(enemies, enemyCount, level);

            // level up message (brief)
            setColor(13);
            cout << "\n*** LEVEL UP! Now Level " << level << " ***\n";
            setColor(7);
            Sleep(900);
        }

        // If all enemies dead, respawn same count
        bool anyAlive = false;
        for (int e = 0; e < enemyCount; e++) 
        {
            if (enemies[e].alive) 
            {
                anyAlive = true;
                break; 
            }
        }
        if (!anyAlive) spawnEnemies(enemies, enemyCount, level);

        // update highscore live
        if (score > highscore) highscore = score;

        Sleep(frameDelay);
    }

    // Game Over: show final screen without clearing
    moveCursorToTopLeft();
    setColor(12);
    cout << "=== GAME OVER ===\n";
    setColor(7);
    cout << "Final Score: " << score << "\n";
    if (score > readHighScore())
    {
        writeHighScore(score);
        cout << "New high score saved!\n";
    }
    else 
    {
        cout << "High Score: " << readHighScore() << "\n";
    }
    cout << "Press any key to exit...\n";
    _getch();
    return 0;
}
