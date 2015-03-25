//
// opengl odds & ends
//

#include <iostream>
#include <OpenGL/gl.h>
#include <OpenGL/glu.h>
#include <glut/glut.h>
#include <math.h>


using namespace std;

// window width/height
int windowWidth=500;
int windowHeight=500;

// camera rotation parameters
float phi=0;
float theta=.5;



void init(void)
{
    // set up the view volume
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(80,1, 1, 200);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    
    // clear colors
    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);

    // enabling/disabling opengl features
    glEnable(GL_DEPTH_TEST);
    
    // initialize stencil clear value
    glClearStencil(0.0);
}

void motion(int x, int y) {
    static int lastX=-1;
    static int lastY=-1;
    
    if (lastX==-1 || x==-1) {
        lastX = x;
    }
    else {
        phi -=(float)(x-lastX)/ (float) windowWidth;
        lastX=x;
    }
    
    if (lastY==-1 || y==-1 ) {
        lastY = y;
    }
    else {
        if ((y>lastY && theta<3.14159/2.2) || (y<lastY && theta>-3.14159/2.2))
            theta +=(float)(y-lastY)/ (float) windowHeight;
        lastY=y;
    }
    
    glutPostRedisplay();
}

void mouse(int button, int state, int x, int y) {
    if (button==GLUT_LEFT_BUTTON && state==GLUT_DOWN) {
        motion(-1,-1);
    }
}

void arrow_keys ( int a_keys, int x, int y )
{
    switch ( a_keys ) {
        case GLUT_KEY_UP:
            glutFullScreen ( );
            break;
        case GLUT_KEY_DOWN: {
            windowWidth=500;
            windowHeight=500;
            glutReshapeWindow ( windowWidth, windowHeight );
            break;
        }
        default:
            break;
    }
}

void keyboard(unsigned char key, int x, int y)
{
    if (key == 27) {
        windowWidth=500;
        windowHeight=500;
        glutReshapeWindow ( windowWidth, windowHeight );
    }
}

void reshape(int width, int height)
{
    int size = min(width,height);
    windowWidth=width;
    windowHeight=width;
    glViewport(0,0,size,size);
}

void display( void )
{
    GLenum glErr = glGetError();
    if (glErr != GL_NO_ERROR){
         cout << "glError: " << gluErrorString(glErr) << endl;
    }
    
    // clear buffers
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

    // reset modelview matrix for viewpoint (0,0,5) and save
    glLoadIdentity();
    double x = 15.0*sin(phi);
    double y = 15*sin(theta);
    double z = 15.0*cos(phi);
    gluLookAt(x,y,z,0,0,0,0,1,0);
    glPushMatrix();
    
    float size=20;
    
    glEnable(GL_POLYGON_OFFSET_FILL);
    glPolygonOffset(-1,-1);
    
    glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);
    glDisable(GL_DEPTH_TEST);
    
    glEnable(GL_STENCIL_TEST);
    glStencilFunc(GL_EQUAL,0,1);
    glStencilOp(GL_KEEP, GL_KEEP, GL_INCR);
    
    // draw small red floor
    glColor3f(1,0,0);
    glBegin(GL_QUADS);
    glVertex3f (-size/2, 0, -size/2);
    glVertex3f (-size/2, 0, size/2);
    glVertex3f (size/2, 0, size/2);
    glVertex3f (size/2, 0, -size/2);
    glEnd();
    
    glEnable(GL_DEPTH_TEST);
    glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
    
    glDisable(GL_POLYGON_OFFSET_FILL);
    
    glStencilFunc(GL_EQUAL,0,1);
    glStencilOp(GL_KEEP, GL_KEEP, GL_KEEP);
    
    // draw big blue floor
    glColor3f(0,0,1);
    glBegin(GL_QUADS);
    glVertex3f (-size, 0, -size);
    glVertex3f (size, 0, -size);
    glVertex3f (size, 0, size);
    glVertex3f (-size, 0, size);
    glEnd();
    
    glDisable(GL_STENCIL_TEST);
    
    //* set up a 2d world
    glPushMatrix();
    glLoadIdentity();
    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();
    gluOrtho2D(-1,1,-1,1);
    
    glDisable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    
    // draw a heads up display
    glColor4f(0,1,0,.5);
    glBegin(GL_QUADS);
    glVertex2d(-1, 1);
    glVertex2d(-1, .5);
    glVertex2d(-.5, .5);
    glVertex2d(-.5,1);
    glEnd();
    
    glDisable(GL_BLEND);
    glEnable(GL_DEPTH_TEST);
    
    // restore 3d viewing
    glPopMatrix();
    glMatrixMode(GL_MODELVIEW);
    glPopMatrix();
    
    glPopMatrix();
    glutSwapBuffers();
}


int main ( int argc, char** argv )
{

    glutInit(&argc, argv );
    glutInitDisplayMode( GLUT_DEPTH | GLUT_DOUBLE | GLUT_RGBA | GLUT_STENCIL); // Display Mode
    glutInitWindowPosition(0,0);
    glutInitWindowSize(windowWidth, windowHeight);
    glutCreateWindow("Odds & ends tutorial");
    init ();
    glutDisplayFunc(display);
    glutReshapeFunc(reshape);
    glutMouseFunc(mouse);
    glutMotionFunc(motion);
    glutSpecialFunc(arrow_keys);
    glutKeyboardFunc(keyboard);
    glutMainLoop( );
    
    return 1;
}




