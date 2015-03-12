#include <iostream>
#include <glut/glut.h>
#include <math.h>




// function prototypes
void display(void);
void reshape(int width, int height);
void skeyboard(int key, int x, int y);
void keyboard(unsigned char key, int x, int y);
void init();
void drawSnowman(GLfloat radius);
void drawBottom();
void drawMiddle();
void headAssembly();
void drawEyes();
void drawFloor();


int windowWidth=600;
int windowHeight=600;
double snowmanX=0;
double snowmanZ=0;
double theta=0;

using namespace std;

int main(int argc, char **argv)
{
    
    // initialize glut
    glutInit(&argc, argv);
    
    // set window size
    glutInitWindowSize(windowWidth,windowHeight);
    
    // establish glut display parameters
    glutInitDisplayMode(GLUT_DOUBLE   | GLUT_RGB  |GLUT_DEPTH);
    
    // create window
    glutCreateWindow("My Third OpenGL program");
    
    // register callback functions
    glutDisplayFunc(display);
    glutReshapeFunc(reshape);
    glutSpecialFunc(skeyboard);
    glutKeyboardFunc(keyboard);

    // initalize opengl parameters
    init();
    
    // loop until something happens
    glutMainLoop();
    return 0;
}

void init()
{
    // initialize viewing system
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(20.0, 1.0, 1, 100.0);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    
    // initialize background color to black
    glClearColor(0,0,0,0);
    
    // enable depth buffering
    glEnable(GL_DEPTH_TEST);
    
}

void skeyboard(int key, int x, int y)
{
    if (key == GLUT_KEY_LEFT)
        snowmanX--;
    if (key == GLUT_KEY_RIGHT)
        snowmanX++;
    if (key == GLUT_KEY_UP)
        snowmanZ--;
    if (key == GLUT_KEY_DOWN)
        snowmanZ++;
    glutPostRedisplay();
}

void keyboard(unsigned char key, int x, int y)
{
    if (key == 'w')
        theta -= 2.0;
    if (key == 's')
        theta += 2.0;
    glutPostRedisplay();
}

void reshape(int width, int height)
{
    if (width<height)
        glViewport(0,0,width,width);
    else
        glViewport(0,0,height,height);
    
}

void display()
{

    // clear buffers
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    
    // initialize modelview matrix
    glMatrixMode(GL_MODELVIEW_MATRIX);
    glLoadIdentity();
    gluLookAt(0, 15, 30, 0, 0, 0, 0, 1, 0);
    drawFloor();
    drawSnowman(1);
    
    glutSwapBuffers();
}

void drawSnowman(GLfloat radius) {
    glPushMatrix();
    glTranslatef(snowmanX,radius,snowmanZ);
    drawBottom();
    drawMiddle();
    headAssembly();
    glPopMatrix();
    
}

void drawBottom() {
    GLfloat radius=1;
    GLfloat white[] = {1,1,1};
    glColor3fv(white);
    glutSolidSphere(radius, 20, 20);
}

void drawMiddle() {
    glPushMatrix();
    glTranslatef(0, 1.8, 0);
    GLfloat radius=0.8;
    GLfloat white[] = {1,1,1};
    glColor3fv(white);
    glutSolidSphere(radius, 20, 20);
    glPopMatrix();
}

void headAssembly() {
    glPushMatrix();
    glTranslatef(0, 3.1, 0);
    glRotatef(theta, 1, 0, 0);
    GLfloat radius=0.6;
    GLfloat white[] = {1,1,1};
    glColor3fv(white);
    glutSolidSphere(radius, 20, 20);
    drawEyes();
    glPopMatrix();
}

void drawEyes() {
    glPushMatrix();
    glTranslatef(-0.3, 0.2, 0.51);
    GLfloat radius=0.05;
    GLfloat black[] = {0,0,0};
    glColor3fv(black);
    glutSolidSphere(radius, 20, 20);
    glTranslatef(0.6, 0, 0);
    glutSolidSphere(radius, 20, 20);
    glPopMatrix();
}



void drawFloor() {
    glColor3f(0, 0, 1);
    glBegin(GL_QUADS);
    glVertex3f(5, 0, 5);
    glVertex3f(5, 0, -5);
    glVertex3f(-5, 0, -5);
    glVertex3f(-5, 0, 5);
    glEnd();
    
}



