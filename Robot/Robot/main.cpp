#include <iostream>
#include <glut/glut.h>
#include <math.h>

enum {
    M_HELP,
    M_AMBIENT,
    M_POINTLIGHT
};


// function prototypes
void display(void);
void reshape(int width, int height);
void skeyboard(int key, int x, int y);
void keyboard(unsigned char key, int x, int y);
void init();
void motion(int x, int y);
void mouse(int button, int state, int x, int y);
void drawSnowman(GLfloat radius);
void drawBottom();
void drawMiddle();
void headAssembly();
void drawEyes();
void drawFloor();
void drawTeapot();
void drawIcosahedron();
void drawCone(GLdouble base, GLdouble height, GLint slices, GLint stacks);
void menu(int button);


int windowWidth=600;
int windowHeight=600;
double snowmanX=0;
double snowmanZ=0;
double theta=0;

// camera angles
float camtheta=0;
float camphi=0;
float camzoom = 1;

// light controls
bool amblight = true;
bool ptlight = true;

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
    glutMouseFunc(mouse);
    glutMotionFunc(motion);

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
    
    // initialize menu system
    int lighting = glutCreateMenu(menu);
    
    glutAddMenuEntry("Toggle Ambient", M_AMBIENT);
    glutAddMenuEntry("Toggle Point Light", M_POINTLIGHT);
    
    glutCreateMenu(menu);
    glutAddMenuEntry("Help", M_HELP);
    
    glutAddSubMenu("Lighting", lighting);
    
    glutAttachMenu(GLUT_RIGHT_BUTTON);
    
    // initialize background color to black
    glClearColor(0,0,0,0);
    
    // enable lighting
    glEnable(GL_LIGHT0);
    glEnable(GL_AMBIENT);
	glEnable(GL_LIGHTING);
    GLfloat ambient[4] = {.2,.2,.2,1};
    glLightModelfv(GL_LIGHT_MODEL_AMBIENT,ambient);
    
    // set color of light0
    GLfloat white[] = {1,1,1,0};		      // light color
    glLightfv(GL_LIGHT0, GL_DIFFUSE, white);   // set diffuse light color
    glLightfv(GL_LIGHT0, GL_SPECULAR, white);  // set specular light color
//    glLightfv(GL_LIGHT0, GL_AMBIENT, ambient);
    
    // enable depth buffering
    glEnable(GL_DEPTH_TEST);
}

void menu(int button) {
    switch (button)
    {
        case M_HELP:
            cerr << "Robot CO\n\
            \n\
            action                result\n\
            ------                ------\n\
            left mouse            rotate view\n\
            right mouse           access pop-up menu\n\
            up arrow              move robot up\n\
            down arrow            move robot down\n\
            left arrow            move robot left\n\
            right arrow           move robot right\n\
            f                     full-screen mode\n\
            i                     zoom in\n\
            o                     zoom out\n\
            \n"
            << endl;
        case M_AMBIENT:
            amblight = !amblight;
            glutPostRedisplay();
            break;
        case M_POINTLIGHT:
            ptlight = !ptlight;
            glutPostRedisplay();
            break;
            
    }
}

void motion(int x, int y) {
    static int lastX=-1;
    static int lastY=-1;
    
    if (lastX==-1 || x==-1) {
        lastX = x;
    }
    else {
        camphi -=(float)(x-lastX)/ (float) windowWidth;
        lastX=x;
    }
    
    if (lastY==-1 || y==-1 ) {
        lastY = y;
    }
    else {
        if ((y>lastY && camtheta<3.14159/2.2) || (y<lastY && camtheta>-3.14159/2.2))
            camtheta +=(float)(y-lastY)/ (float) windowHeight;
        lastY=y;
    }
    
    glutPostRedisplay();
}

void mouse(int button, int state, int x, int y) {
    if (button==GLUT_LEFT_BUTTON && state==GLUT_DOWN) {
        motion(-1,-1);
    }
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
    if (key == 'f')
        glutFullScreen ( );
    if (key == 27) {
        windowWidth=500;
        windowHeight=500;
        glutReshapeWindow ( windowWidth, windowHeight );
    }
    if (key == 'i')
        camzoom--;
    if (key == 'o')
        camzoom++;
    
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(20.0+camzoom, 1.0, 1, 100.0);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
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
    double x = 15.0*sin(camphi);
    double y = 15*sin(camtheta);
    double z = 15.0*cos(camphi);
    gluLookAt(x,y,z,0,0,0,0,1,0);
    
    // position of light0
	GLfloat lightPosition[]={1,1,5,1};
	glLightfv(GL_LIGHT0, GL_POSITION, lightPosition);
    glLightf(GL_LIGHT0, GL_QUADRATIC_ATTENUATION, 0.04);
    
    if (amblight==false) {
        GLfloat ambient[4] = {0,0,0,0};
        glLightModelfv(GL_LIGHT_MODEL_AMBIENT,ambient);
    }
    else if (amblight==true) {
        GLfloat ambient[4] = {.2,.2,.2,1};
        glLightModelfv(GL_LIGHT_MODEL_AMBIENT,ambient);
    }
    if (ptlight==false)
        glDisable(GL_LIGHT0);
    else if (ptlight==true)
        glEnable(GL_LIGHT0);
    
    drawFloor();
//    drawSnowman(1);
    drawTeapot();
    drawIcosahedron();
    drawCone(1,4,20,20);
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

// teapot
void drawTeapot() {
    glPushMatrix();
    GLdouble size = 2;
    glTranslatef(-2, size/1.325, -2);
    GLfloat blue[] = {0,0,1};
    glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, blue);
	glMaterialfv(GL_FRONT, GL_SPECULAR, blue);
	glMateriali(GL_FRONT,GL_SHININESS,0);
    
    glutSolidTeapot(size);
    glPopMatrix();
}

// wire
void drawIcosahedron() {
    glPushMatrix();
    glTranslatef(3, 1, 1);
    GLfloat green[] = {0,1,0};
    glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, green);
	glMaterialfv(GL_FRONT, GL_SPECULAR, green);
	glMateriali(GL_FRONT,GL_SHININESS,0);
    glutWireIcosahedron();
    glPopMatrix();
}

// cone
void drawCone(GLdouble base, GLdouble height, GLint slices, GLint stacks) {
    glPushMatrix();
    glTranslatef(2.5, 0, -4);
    glRotatef(-90, 0, 0, 0);
    GLfloat white[] = {1,1,1};
    glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, white);
	glMaterialfv(GL_FRONT, GL_SPECULAR, white);
	glMateriali(GL_FRONT,GL_SHININESS,0);
    glutSolidCone(base, height, slices, stacks);
    glPopMatrix();

}

void drawFloor() {
    int floorSize = 6;
    for (int i=-floorSize; i<floorSize; i++) {
        for (int j=-floorSize; j<floorSize; j++) {
            if ((i+j)%2 == 0){
                GLfloat green[] = {0.9,0,0};
                glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, green);
                glMaterialfv(GL_FRONT, GL_SPECULAR, green);
                glMateriali(GL_FRONT,GL_SHININESS,0);
            }
            else {
                GLfloat white[] = {1,1,1};
                glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, white);
                glMaterialfv(GL_FRONT, GL_SPECULAR, white);
                glMateriali(GL_FRONT,GL_SHININESS,0);
            }
            glBegin(GL_QUADS);
            glVertex3f(i+1, 0, j);
            glVertex3f(i+1, 0, j-1);
            glVertex3f(i, 0, j-1);
            glVertex3f(i, 0, j);
            glEnd();
            
        }
    }
    
}



