
#include <iostream>
#include <glut/glut.h>
#include <math.h>



// function prototypes
void display(void);
void reshape(int width, int height);
void init();
void mouse(int button, int state, int x, int y);
void motion(int x, int y);

using namespace std;

double phi = 0.0;

int main(int argc, char **argv)
{
    
    // initialize glut
    glutInit(&argc, argv);
    
    // set window size
    glutInitWindowSize(600,600);
    
    // establish glut display parameters
    glutInitDisplayMode(GLUT_DOUBLE   | GLUT_RGB  |GLUT_DEPTH);
    
    // create window
    glutCreateWindow("My Second OpenGL program");
    
    // register callback functions
    glutDisplayFunc(display);
    glutReshapeFunc(reshape);
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
    gluPerspective(20.0, 1.0, 1.0, 100.0);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    
    // initialize background color to black
    glClearColor(0,0,0,0);
    
    // enable light0 and lighting
	glEnable(GL_LIGHT0);
	glEnable(GL_LIGHTING);
    

    // set color of light0
    GLfloat white[] = {1,1,1,0};		      // light color
    glLightfv(GL_LIGHT0, GL_DIFFUSE, white);   // set diffuse light color
    glLightfv(GL_LIGHT0, GL_SPECULAR, white);  // set specular light color
    
    // enable depth buffering
    glEnable(GL_DEPTH_TEST);
}




void reshape(int width, int height)
{
    if (width<height)
        glViewport(0,0,width,width);
    else
        glViewport(0,0,height,height);
    
}

void mouse(int button, int state, int x, int y)
{
	if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN)
		cout << "Left click with cursor at" << x << " " << y << endl;
}

void motion(int x, int y)
{
    cout << "Mouse at" << x << " " << y << endl;
    double w = glutGet(GLUT_WINDOW_WIDTH);
    phi = 360.0*x/w - 180.0;
    
    glutPostRedisplay();
}

void display()
{
    GLfloat white[] = {1,1,1,0};	    // white
    GLfloat red[] = {1,0,0,0};              // red
    GLfloat green[] = {0,1,0,0};             // green
    GLfloat purple[] = {1,0,1,0};	    // purple
    // clear buffers
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
    // initialize modelview matrix
    glMatrixMode(GL_MODELVIEW_MATRIX);
    glLoadIdentity();
    // set viewpoint position/orientation
    gluLookAt(20*sin(phi*3.14/180.0),0,20*cos(phi*3.14/180.0),0,0,0,0,1,0);
    // position of light0
	GLfloat lightPosition[]={1,1,1,1};
	glLightfv(GL_LIGHT0, GL_POSITION, lightPosition);


    glMaterialfv(GL_FRONT, GL_DIFFUSE, red);
	glMaterialfv(GL_FRONT, GL_SPECULAR, red);
	glMateriali(GL_FRONT,GL_SHININESS,0);
    
    glNormal3f(1,0,0);
    // draw a red triangle
    //glColor3f(1,0,0);
    glBegin(GL_TRIANGLES);
    glVertex3f(-3,-1,-8);
    glVertex3f(3,-1,-10);
    glVertex3f(0,3,-9);
    glEnd();
    
    glMaterialfv(GL_FRONT, GL_DIFFUSE, green);
	glMaterialfv(GL_FRONT, GL_SPECULAR, green);
	glMateriali(GL_FRONT,GL_SHININESS,0);
    
    glNormal3f(0,1,0);
    
    // draw Triangle strip
    //GLfloat green[] = {0,1,0,0};			// gree
    glColor4fv(green);
    glBegin(GL_TRIANGLE_STRIP);
    glVertex3f(-10.0,-1.0,-10.0);
    glVertex3f(-10.0,-1.0,10.0);
    glVertex3f(10.0,-1.0,-10.0);
    glVertex3f(10.0,-1.0,10.0);
    glEnd();
    
    glMaterialfv(GL_FRONT, GL_DIFFUSE, purple);
	glMaterialfv(GL_FRONT, GL_SPECULAR, purple);
	glMateriali(GL_FRONT,GL_SHININESS,50);
    
    // draw Sphere
    //GLfloat purple[] = {1,0,1,0};			// purple
    glColor4fv(purple);
    glutSolidSphere(.75,100,100);
    
    // swap buffers
    glutSwapBuffers();
    
    
    
}