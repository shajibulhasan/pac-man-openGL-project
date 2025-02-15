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
};

struct Ghost {
    float x, y;
};

std::vector<Food> foods;
Ghost ghost = {0.5f, 0.5f};
float ghostSpeed = 0.01f;

void generateFood(int num) {
    srand(time(0));
    for (int i = 0; i < num; i++) {
        Food food;
        food.x = (rand() % 200 - 100) / 100.0f;
        food.y = (rand() % 200 - 100) / 100.0f;
        foods.push_back(food);
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
    glColor3f(1.0f, 0.0f, 0.0f);
    glPointSize(5.0f);
    glBegin(GL_POINTS);
    for (const auto& food : foods) {
        glVertex2f(food.x, food.y);
    }
    glEnd();
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

void checkCollision() {
    for (auto it = foods.begin(); it != foods.end();) {
        float distX = pacmanX - it->x;
        float distY = pacmanY - it->y;
        float distance = sqrt(distX * distX + distY * distY);
        if (distance < pacmanRadius) {
            PlaySound(TEXT("eat.wav"), NULL, SND_FILENAME | SND_ASYNC);
            it = foods.erase(it);
            score += 10;
            foods.push_back({(rand() % 200 - 100) / 100.0f, (rand() % 200 - 100) / 100.0f});
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
    drawScore();
    glutSwapBuffers();
}

void update(int value) {
    if (!gameOver) {
        moveGhost();
        glutPostRedisplay();
        glutTimerFunc(100, update, 0);
    }
}

void keyboard(int key, int x, int y) {
    if (!gameOver) {
        float speed = 0.05f;
        if (key == GLUT_KEY_LEFT && pacmanX - speed > -1) pacmanX -= speed;
        if (key == GLUT_KEY_RIGHT && pacmanX + speed < 1) pacmanX += speed;
        if (key == GLUT_KEY_UP && pacmanY + speed < 1) pacmanY += speed;
        if (key == GLUT_KEY_DOWN && pacmanY - speed > -1) pacmanY -= speed;

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
