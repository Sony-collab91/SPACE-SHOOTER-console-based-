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
const char PLAYER_SHAPE = '^';
const char ENEMY_SHAPE = (char)219;
const char BULLET_SHAPE = '|';
const char LIFE_CHAR = 3;

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

// High score read/write
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

void writeHighScore(int h)
{
    ofstream fout("highscore.txt");
    if (fout.is_open())
    {
        fout << h;
        fout.close();
    }
}

// Spawn enemies
void spawnEnemies(Enemy enemies[], int enemyCount, int level)
{
    int cols = enemyCount;
    if (cols > WIDTH - 6)
        cols = WIDTH - 6;
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
        if (enemies[i].x >= WIDTH - 2)
            enemies[i].x = (WIDTH - 3) - i;
        enemies[i].y = 1 + (i / cols);
        if (enemies[i].y >= 4)
            enemies[i].y = 1 + (i % 2);
    }
}

void drawFrame(int playerX, int playerY,
    int bulletX[], int bulletY[], int bulletCount,
    Enemy enemies[], int enemyCount,
    int score, int lives, int level, int highScore)
{
    moveCursorToTopLeft();

    setColor(11);
    cout << "SPACE SHOOTER  | Level: " << level << "  Score: " << score << "  Lives: ";
    setColor(12);
    for (int i = 0; i < lives; i++)
        cout << LIFE_CHAR << ' ';
    setColor(11);
    cout << "  High: ";
    setColor(14);
    cout << highScore << "\n";
    setColor(7);
    for (int i = 0; i < WIDTH; i++)
        cout << (char)196;
    cout << "\n";

    for (int y = 0; y < HEIGHT; y++)
    {
        for (int x = 0; x < WIDTH; x++)
        {
            bool printed = false;

            // Player
            if (x == playerX && y == playerY)
            {
                setColor(10);
                cout << PLAYER_SHAPE;
                setColor(7);
                printed = true;
            }

            // Bullets
            for (int b = 0; b < bulletCount; b++)
            {
                if (x == bulletX[b] && y == bulletY[b])
                {
                    setColor(14);
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
                    setColor(12);
                    cout << ENEMY_SHAPE;
                    setColor(7);
                    printed = true;
                }
            }

            // Background
            if (!printed)
            {
                setColor(8);
                cout << '.';
                setColor(7);
            }
        }
        cout << "\n";
    }

    for (int i = 0; i < WIDTH; i++)
        cout << (char)196;
    cout << "\n";
    setColor(15);
    cout << "Controls: a = left    d = right    w = shoot    q = quit\n";
    cout << "Press keys directly; game runs in real time.\n";
    setColor(7);
}

int main()
{
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

    while (true)
    {
        if (_kbhit())
        {
            char c = _getch();
            if (c == ' ')
                break;
            if (c == 'q' || c == 'Q')
                return 0;
        }
    }

    int highScore = readHighScore();

    int playerX = WIDTH / 2;
    int playerY = HEIGHT - 2;
    int bulletX[MAX_BULLETS], bulletY[MAX_BULLETS], bulletCount = 0;
    Enemy enemies[MAX_ENEMIES];
    int level = 1, lives = 3, score = 0;
    bool gameRunning = true;
    int enemyCount = 5;
    int frameDelay = 250;

    spawnEnemies(enemies, enemyCount, level);

    while (gameRunning)
    {
        drawFrame(playerX, playerY, bulletX, bulletY, bulletCount, enemies, enemyCount, score, lives, level, highScore);

        // Input
        if (_kbhit())
        {
            char key = _getch();
            if (key == 'a' || key == 'A')
                if (playerX > 1)
                    playerX--;
                else if (key == 'd' || key == 'D')
                    if (playerX < WIDTH - 2)
                        playerX++;
                    else if (key == 'w' || key == 'W')
                    {
                        if (bulletCount < MAX_BULLETS)
                        {
                            bulletX[bulletCount] = playerX;
                            bulletY[bulletCount] = playerY - 1;
                            bulletCount++;
                        }
                    }
                    else if (key == 'q' || key == 'Q')
                    {
                        gameRunning = false;
                        break;
                    }
        }

        // Move bullets
        for (int b = 0; b < bulletCount; b++)
            bulletY[b]--;

        // Remove off-screen bullets
        for (int b = 0; b < bulletCount; b++)
        {
            if (bulletY[b] < 0)
            {
                for (int j = b; j < bulletCount - 1; j++)
                {
                    bulletX[j] = bulletX[j + 1];
                    bulletY[j] = bulletY[j + 1];
                }
                bulletCount--;
                b--;
            }
        }

        // Move enemies
        for (int e = 0; e < enemyCount; e++)
        {
            if (!enemies[e].alive)
                continue;
            static int tick = 0;
            tick++;
            int moveRate = 6 - (level);
            if (moveRate < 1)
                moveRate = 1;
            if ((tick + e) % moveRate == 0)
                enemies[e].y++;

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

        // Bullet-enemy collision
        for (int b = 0; b < bulletCount; b++)
        {
            for (int e = 0; e < enemyCount; e++)
            {
                if (!enemies[e].alive)
                    continue;
                if (bulletX[b] == enemies[e].x && bulletY[b] == enemies[e].y)
                {
                    enemies[e].alive = false;
                    score++;
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

        if (!gameRunning)
            break;

        // Level up
        if (score >= level * 10)
        {
            level++;
            if (level > 5)
            {
                drawFrame(playerX, playerY, bulletX, bulletY, bulletCount, enemies, enemyCount, score, lives, level, highScore);
                setColor(13);
                cout << "\nCONGRATULATIONS! You finished all levels!\n";
                setColor(7);
                if (score > highScore)
                    writeHighScore(score);
                cout << "Press any key to exit...\n";
                _getch();
                return 0;
            }
            enemyCount += 3;
            if (enemyCount > MAX_ENEMIES)
                enemyCount = MAX_ENEMIES;
            frameDelay -= 40;
            if (frameDelay < 80)
                frameDelay = 80;
            spawnEnemies(enemies, enemyCount, level);
            setColor(13);
            cout << "\n*** LEVEL UP! Now Level " << level << " ***\n";
            setColor(7);
            Sleep(900);
        }

        // Respawn if all enemies dead
        bool anyAlive = false;
        for (int e = 0; e < enemyCount; e++)
            if (enemies[e].alive)
            {
                anyAlive = true;
                break;
            }
        if (!anyAlive)
            spawnEnemies(enemies, enemyCount, level);

        if (score > highScore)
            highScore = score;

        Sleep(frameDelay);
    }

    // Game over
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
