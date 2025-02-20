#include <GL/glut.h>
#include <cmath>
#include <vector>
#include <cstdlib>
#include <ctime>
#include <windows.h>
#include <mmsystem.h>
#include <string>

#pragma comment(lib, "winmm.lib")

float pacmanX = 0.0f, pacmanY = 0.0f;
float pacmanRadius = 0.1f;
bool mouthOpen = true;
int score = 0;
bool gameOver = false;

struct Food {
    float x, y;
    bool isCircle;
};

struct Ghost {
    float x, y;
};

struct Star {
    float x, y;
};

std::vector<Food> foods;
std::vector<Star> stars;
Ghost ghost = {0.5f, 0.5f};
float ghostSpeed = 0.01f;

void addFood() {
    Food food;
    food.x = (rand() % 200 - 100) / 100.0f;
    food.y = (rand() % 200 - 100) / 100.0f;
    food.isCircle = rand() % 2;
    foods.push_back(food);
}

void generateFood(int num) {
    foods.clear();
    srand(time(0));
    for (int i = 0; i < num; i++) {
        addFood();
    }
}

void addStar() {
    Star star;
    star.x = (rand() % 200 - 100) / 100.0f;
    star.y = (rand() % 200 - 100) / 100.0f;
    stars.push_back(star);
}

void generateStars(int num) {
    stars.clear();
    srand(time(0));
    for (int i = 0; i < num; i++) {
        addStar();
    }
}

void drawPacman() {
    glColor3f(1.0f, 1.0f, 0.0f);
    glBegin(GL_TRIANGLE_FAN);
    glVertex2f(pacmanX, pacmanY);

    float startAngle = mouthOpen ? 30 : 0;
    float endAngle = mouthOpen ? 330 : 360;

    for (int i = startAngle; i <= endAngle; i += 10) {
        float angle = i * 3.14159 / 180;
        glVertex2f(pacmanX + cos(angle) * pacmanRadius, pacmanY + sin(angle) * pacmanRadius);
    }
    glEnd();
}

void drawFood() {
    for (const auto& food : foods) {
        if (food.isCircle) {
            glColor3f(1.0f, 0.0f, 0.0f);
            glBegin(GL_TRIANGLE_FAN);
            glVertex2f(food.x, food.y);
            for (int i = 0; i <= 360; i += 10) {
                float angle = i * 3.14159 / 180;
                glVertex2f(food.x + cos(angle) * 0.05, food.y + sin(angle) * 0.05);
            }
            glEnd();
        } else {
            glColor3f(0.0f, 1.0f, 0.0f);
            glBegin(GL_QUADS);
            glVertex2f(food.x - 0.05, food.y - 0.05);
            glVertex2f(food.x + 0.05, food.y - 0.05);
            glVertex2f(food.x + 0.05, food.y + 0.05);
            glVertex2f(food.x - 0.05, food.y + 0.05);
            glEnd();
        }
    }
}

void drawStars() {
    glColor3f(1.0f, 1.0f, 1.0f);
    for (const auto& star : stars) {
        glBegin(GL_TRIANGLE_FAN);
        glVertex2f(star.x, star.y);
        for (int i = 0; i <= 360; i += 72) {
            float angle = i * 3.14159 / 180;
            glVertex2f(star.x + cos(angle) * 0.07, star.y + sin(angle) * 0.07);
        }
        glEnd();
    }
}

void drawGhost() {
    glColor3f(1.0f, 0.0f, 1.0f);
    glBegin(GL_TRIANGLE_FAN);
    glVertex2f(ghost.x, ghost.y);
    for (int i = 0; i <= 360; i += 10) {
        float angle = i * 3.14159 / 180;
        glVertex2f(ghost.x + cos(angle) * pacmanRadius, ghost.y + sin(angle) * pacmanRadius);
    }
    glEnd();
}

void moveGhost() {
    float dirX = pacmanX - ghost.x;
    float dirY = pacmanY - ghost.y;
    float length = sqrt(dirX * dirX + dirY * dirY);

    if (length > 0.01f) {
        ghost.x += (dirX / length) * ghostSpeed;
        ghost.y += (dirY / length) * ghostSpeed;
    }

    if (length < pacmanRadius) {
        gameOver = true;
    }
    pacmanX = std::max(-1.0f + pacmanRadius, std::min(1.0f - pacmanRadius, pacmanX));
    pacmanY = std::max(-1.0f + pacmanRadius, std::min(1.0f - pacmanRadius, pacmanY));
}

void drawScore() {
    std::string scoreText = "Score: " + std::to_string(score);
    glColor3f(1.0f, 1.0f, 1.0f);
    glRasterPos2f(-0.9f, 0.9f);
    for (char c : scoreText) {
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, c);
    }
    if (gameOver) {
        std::string gameOverText = "GAME OVER";
        glRasterPos2f(-0.2f, 0.0f);
        for (char c : gameOverText) {
            glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, c);
        }
    }
}


bool checkStarCollision() {
    for (const auto& star : stars) {
        float distX = pacmanX - star.x;
        float distY = pacmanY - star.y;
        float distance = sqrt(distX * distX + distY * distY);
        if (distance < pacmanRadius) {
            return true;
        }
    }
    return false;
}

void checkCollision() {
    // ⭐ Check if Pac-Man touched any Star
    if (checkStarCollision()) {
        gameOver = true;
        return;
    }

    // 🍎 Check if Pac-Man ate any Food
    for (auto it = foods.begin(); it != foods.end();) {
        float distX = pacmanX - it->x;
        float distY = pacmanY - it->y;
        float distance = sqrt(distX * distX + distY * distY);

        if (distance < pacmanRadius) {
            PlaySound(TEXT("eat.wav"), NULL, SND_FILENAME | SND_ASYNC);
            score += (it->isCircle ? 10 : 5);
            it = foods.erase(it);  // Remove eaten food
            addFood();  // Add new food
        } else {
            ++it;
        }
    }
}

void display() {
    glClear(GL_COLOR_BUFFER_BIT);
    drawPacman();
    drawFood();
    drawGhost();
    drawStars();
    drawScore();
    glutSwapBuffers();
}

void update(int value) {
    if (!gameOver) {
        // Increase ghost speed if score > 50
        if (score > 100) {
            ghostSpeed = 0.03f; // Faster speed
        }else if(score > 50) {
            ghostSpeed = 0.02f;
        }
        else {
            ghostSpeed = 0.01f; // Normal speed
        }

        moveGhost();
        glutPostRedisplay();
        glutTimerFunc(100, update, 0);
    }
}

void keyboard(int key, int x, int y) {
    if (!gameOver) {
        float speed = 0.05f;
        if (key == GLUT_KEY_LEFT) pacmanX -= speed;
        if (key == GLUT_KEY_RIGHT) pacmanX += speed;
        if (key == GLUT_KEY_UP) pacmanY += speed;
        if (key == GLUT_KEY_DOWN) pacmanY -= speed;

        mouthOpen = !mouthOpen;

        checkCollision();
        glutPostRedisplay();
    }
}

void init() {
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glMatrixMode(GL_PROJECTION);
    gluOrtho2D(-1, 1, -1, 1);
    generateFood(5);
    generateStars(5);
}

int main(int argc, char** argv) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
    glutInitWindowSize(500, 500);
    glutCreateWindow("Pac-Man OpenGL");
    init();
    glutDisplayFunc(display);
    glutSpecialFunc(keyboard);
    glutTimerFunc(100, update, 0);
    glutMainLoop();
    return 0;
}
