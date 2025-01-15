// Bibliotecas utilizadas pelo OpenGL
#ifdef __APPLE__
    #define GL_SILENCE_DEPRECATION
    #include <GLUT/glut.h>
    #include <OpenGL/gl.h>
    #include <OpenGL/glu.h>
#else
    #include <GL/glut.h>
    #include <GL/gl.h>
    #include <GL/glu.h>
#endif

#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <vector>
#include <algorithm>
#include <limits>
#include <iostream>

using namespace std;

// Variáveis globais para a posição e orientação da câmera
float cameraX = 0.0f, cameraY = 1.5f, cameraZ = 5.0f; // Posição da câmera
float pitch = 0.0f, yaw = -90.0f;                     // Ângulos de rotação (em graus)
float sensitivity = 0.1f;                             // Sensibilidade do mouse
float lastX = 400, lastY = 300;                       // Última posição do mouse
bool firstMouse = true;                               // Para ajustar a posição inicial do mouse
bool ignoreMouseEvent = false;                        // Controle de eventos do mouse

void init(void);
void display(void);
void keyboard(unsigned char key, int x, int y);
void mouse(int button, int state, int x, int y);
void mouseMotion(int xpos, int ypos);
void drawCrosshair();
void updateCameraDirection();
void drawFloor();
void renderCrosshair();
void drawBullets();
void updateBullets();


// Estrutura para representar uma bala
struct Bullet {
    float x, y, z;
    float dirX, dirY, dirZ;
};

// Vetor para armazenar as balas
vector<Bullet> bullets;

// Função para atualizar a posição das balas
void updateBullets() {
    for (size_t i = 0; i < bullets.size(); ++i) {
        bullets[i].x += bullets[i].dirX * 0.1f;
        bullets[i].y += bullets[i].dirY * 0.1f;
        bullets[i].z += bullets[i].dirZ * 0.1f;
    }
}

// Função para desenhar as balas
void drawBullets() {
    glColor3f(1.0f, 1.0f, 0.0f); // Amarelo para as balas
    for (size_t i = 0; i < bullets.size(); ++i) {
        glPushMatrix();
        glTranslatef(bullets[i].x, bullets[i].y, bullets[i].z);
        glutSolidSphere(0.05f, 10, 10); // Desenhar a bala como uma pequena esfera
        glPopMatrix();
    }
}

// Função chamada quando o botão do mouse é pressionado
void mouse(int button, int state, int x, int y) {
    if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN) {
        float radYaw = yaw * M_PI / 180.0f;
        float radPitch = pitch * M_PI / 180.0f;

        Bullet newBullet;
        newBullet.x = cameraX;
        newBullet.y = cameraY;
        newBullet.z = cameraZ;
        newBullet.dirX = cos(radYaw) * cos(radPitch);
        newBullet.dirY = sin(radPitch);
        newBullet.dirZ = sin(radYaw) * cos(radPitch);

        bullets.push_back(newBullet);
    }
}

// Função chamada quando o mouse se move
void mouseMotion(int xpos, int ypos) {
    if (firstMouse) {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }

    float xoffset = xpos - lastX;
    float yoffset = lastY - ypos; // Invertido porque Y cresce para baixo
    lastX = xpos;
    lastY = ypos;

    xoffset *= sensitivity;
    yoffset *= sensitivity;

    yaw += xoffset;
    pitch += yoffset;

    // Restringir o ângulo pitch para evitar "virar de cabeça para baixo"
    if (pitch > 89.0f) pitch = 89.0f;
    if (pitch < -89.0f) pitch = -89.0f;

    glutPostRedisplay(); // Solicitar atualização
}

void keyboard(unsigned char key, int x, int y) {
    switch (key) {
        case 27: exit(0); break; // ESC para sair
    }
}

// Função para desenhar a mira
void drawCrosshair() {
    glBegin(GL_LINES);
    glColor3f(1.0f, 0.0f, 0.0f); // Vermelho para a mira

    // Linha horizontal
    glVertex2f(-0.02f, 0.0f);
    glVertex2f(0.02f, 0.0f);

    // Linha vertical
    glVertex2f(0.0f, -0.02f);
    glVertex2f(0.0f, 0.02f);

    glEnd();
}

// Função para converter ângulos em direção
void updateCameraDirection() {
    float radYaw = yaw * M_PI / 180.0f;
    float radPitch = pitch * M_PI / 180.0f;

    float lookX = cos(radYaw) * cos(radPitch);
    float lookY = sin(radPitch);
    float lookZ = sin(radYaw) * cos(radPitch);

    // Atualizar o ponto de visão da câmera
    gluLookAt(cameraX, cameraY, cameraZ,
              cameraX + lookX, cameraY + lookY, cameraZ + lookZ,
              0.0f, 1.0f, 0.0f);
}

// Função para desenhar o chão
void drawFloor() {
    glBegin(GL_QUADS);
    glColor3f(0.35f, 0.16f, 0.04f); // Cor marrom mais escura para a madeira

    // Desenhar um chão de madeira com várias tábuas
    float plankWidth = 1.0f;
    float plankLength = 10.0f;
    for (float x = -10.0f; x < 10.0f; x += plankWidth) {
        glVertex3f(x, 0.0f, -plankLength);
        glVertex3f(x + plankWidth, 0.0f, -plankLength);
        glVertex3f(x + plankWidth, 0.0f, plankLength);
        glVertex3f(x, 0.0f, plankLength);
    }

    glEnd();
}

void renderCrosshair() {
    // Desenhar a mira
    glPushMatrix();
    glLoadIdentity();
    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();
    gluOrtho2D(-1, 1, -1, 1);
    drawCrosshair();
    glPopMatrix();
    glMatrixMode(GL_MODELVIEW);
    glPopMatrix();
}

// Função de renderização
void display() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glLoadIdentity();

    updateCameraDirection(); // Atualizar a direção da câmera
    drawFloor();             // Desenhar o chão

    updateBullets();         // Atualizar as balas
    drawBullets();           // Desenhar as balas

    renderCrosshair();       // Desenhar a mira

    glutSwapBuffers();
}

// void display() {
//     glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
//     glLoadIdentity();

//     updateCameraDirection(); // Atualizar a direção da câmera
//     drawFloor();             // Desenhar o chão

//     glutSwapBuffers();
// }

// Função para atualizar o jogo continuamente
void idle() {
    updateBullets();       // Atualizar a posição das balas
    glutPostRedisplay();   // Redesenhar a cena
}

// Configurações iniciais
void init() {
    glEnable(GL_DEPTH_TEST);
    glClearColor(0.5f, 0.8f, 1.0f, 1.0f); // Cor do céu
    glMatrixMode(GL_PROJECTION);
    gluPerspective(45.0, 1.33, 0.1, 100.0);
    glMatrixMode(GL_MODELVIEW);

    // Ocultar o cursor
    glutSetCursor(GLUT_CURSOR_NONE);

    // Centralizar o cursor no início
    // glutWarpPointer(400, 300);
}

// Função principal
int main(int argc, char** argv) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
    glutInitWindowSize(800, 600);
    glutCreateWindow("Controle da Câmera com Mouse");

    init();
    glutFullScreen();
    glutKeyboardFunc(keyboard);
    glutDisplayFunc(display);
    glutPassiveMotionFunc(mouseMotion); // Registrar movimento do mouse
    glutMouseFunc(mouse);               // Registrar cliques do mouse
    glutIdleFunc(idle);                 // Atualizar continuamente

    glutMainLoop();
    return 0;
}
