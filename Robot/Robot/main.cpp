#include <iostream>
#include <glut/glut.h>
#include <math.h>

enum {
    M_HELP,
    M_AMBIENT,
    M_POINTLIGHT,
    M_PERSPECTIVE,
    R_WRIST,
    R_ELBOW,
    R_SHOULDER
};


// function prototypes
void display(void);
void reshape(int width, int height);
void skeyboard(int key, int x, int y);
void keyboard(unsigned char key, int x, int y);
void init();
void motion(int x, int y);
void mouse(int button, int state, int x, int y);
void drawRobot(GLfloat radius);
void drawBottom();
void drawLegs();
void drawArms();
void headAssembly();
void drawEyes();
void drawFloor();
void drawTeapot();
void drawIcosahedron();
void drawCone(GLdouble base, GLdouble height, GLint slices, GLint stacks);
void shootLaser();

void menu(int button);


int windowWidth=600;
int windowHeight=600;
double robotX=0;
double robotZ=0;
double headVertTheta=0;
double headHorizTheta=0;
bool headRotationMode=true;
bool rightSide=true;
int armSegment = R_ELBOW;

float rightShoulderVertTheta=0;
float rightShoulderHorizTheta=0;
float rightElbowVertTheta=0;
float rightElbowHorizTheta=0;
float rightWristVertTheta=0;
float rightWristHorizTheta=0;

float leftShoulderVertTheta=0;
float leftShoulderHorizTheta=0;
float leftElbowVertTheta=0;
float leftElbowHorizTheta=0;
float leftWristVertTheta=0;
float leftWristHorizTheta=0;

// camera controls
float camtheta=M_PI/6;
float camphi=0;
float camzoom=0;
bool campersp=true;

// light controls
bool amblight = true;
bool ptlight = true;
bool laser = false;



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
    glutCreateWindow("Mike Wazowskibot");
    
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
    gluPerspective(40.0+camzoom, 1.0, 1, 100.0);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    
    // initialize menu system
    int lighting = glutCreateMenu(menu);
    
    glutAddMenuEntry("Toggle Ambient", M_AMBIENT);
    glutAddMenuEntry("Toggle Point Light", M_POINTLIGHT);
    
    int armSegments = glutCreateMenu(menu);
    
    glutAddMenuEntry("Control Shoulder Joint", R_SHOULDER);
    glutAddMenuEntry("Control Elbow Joint", R_ELBOW);
    glutAddMenuEntry("Control Wrist Joint", R_WRIST);
    
    int camSettings = glutCreateMenu(menu);
    
    glutAddMenuEntry("Toggle Global Perspective", M_PERSPECTIVE);

    glutCreateMenu(menu);
    glutAddMenuEntry("Help", M_HELP);
    
    glutAddSubMenu("Lighting", lighting);
    glutAddSubMenu("Joint Controls", armSegments);
    glutAddSubMenu("Camera Controls", camSettings);
    
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
    GLfloat white[] = {0.5,0.5,0.5,0};		      // light color
    glLightfv(GL_LIGHT0, GL_DIFFUSE, white);   // set diffuse light color
    glLightfv(GL_LIGHT0, GL_SPECULAR, white);  // set specular light color
    
    // set color of light1 (laser)
    GLfloat blue[] = {0,0,1};
    glLightfv(GL_LIGHT1, GL_DIFFUSE, blue);
    glLightfv(GL_LIGHT1, GL_SPECULAR, blue);
    glLightf(GL_LIGHT1, GL_SPOT_CUTOFF, 30.0);
//    glLightf(GL_LIGHT1, GL_SPOT_EXPONENT, 2.0);
    
    // enable depth buffering
    glEnable(GL_DEPTH_TEST);
}

void menu(int button) {
    switch (button)
    {
        case M_HELP:
            cerr << "Robot Controls\n\
            \n\
            action                result\n\
            ------                ------\n\
            left mouse            rotate view\n\
            right mouse           access pop-up menu\n\
            up arrow              move robot up\n\
            down arrow            move robot down\n\
            left arrow            move robot left\n\
            right arrow           move robot right\n\
            f                     enter full-screen mode\n\
            esc                   exit full-screen mode\n\
            i                     zoom in\n\
            o                     zoom out\n\
            l                     select left arm for movement\n\
            r                     select right arm for movement\n\
            h                     select head for movement\n\
            w                     rotate selected object up\n\
            a                     rotate selected object left\n\
            s                     rotate selected object down\n\
            d                     rotate selected object right\n\
            spacebar              toggle the spotlight\n\
            \n\
            more fine-grained control of the arm may be found\n\
            under 'Joint Controls' in the popup menu\n\
            \n\
            switch to the robot's perspective by going to\n\
            'Camera Controls' and selecting 'Toggle Global\n\
            Perspective'\n\
            \n\
            the ambient and point lights may be toggled\n\
            by selecting either option under 'Lighting'\n\
            \n"
            << endl;
            break;
        case M_AMBIENT:
            amblight = !amblight;
            glutPostRedisplay();
            break;
        case M_POINTLIGHT:
            ptlight = !ptlight;
            glutPostRedisplay();
            break;
        case M_PERSPECTIVE:
            campersp = !campersp;
            glutPostRedisplay();
            break;
        case R_SHOULDER:
            armSegment=R_SHOULDER;
            break;
        case R_ELBOW:
            armSegment=R_ELBOW;
            break;
        case R_WRIST:
            armSegment=R_WRIST;
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
    cout << "CP " << campersp << endl;
    if (button==GLUT_LEFT_BUTTON && state==GLUT_DOWN && campersp==true) {
        cout << "\nWHYY\n" << endl;
        motion(-1,-1);
    }
}

void skeyboard(int key, int x, int y)
{
    if (key == GLUT_KEY_LEFT)
        robotX--;
    if (key == GLUT_KEY_RIGHT)
        robotX++;
    if (key == GLUT_KEY_UP)
        robotZ--;
    if (key == GLUT_KEY_DOWN)
        robotZ++;
    glutPostRedisplay();
}

void keyboard(unsigned char key, int x, int y)
{
    if (key == ' ')
        laser = !laser;
    if (key == 'h')
        headRotationMode=true;
    if (key == 'l') {
        headRotationMode=false;
        rightSide=false;
    }
    if (key == 'r') {
        headRotationMode=false;
        rightSide=true;
    }
    
    int angleSwitch=1;
    if (headRotationMode) {
        if (key == 'w' && headVertTheta>-20)
            headVertTheta -= 2.0;
        if (key == 's' && headVertTheta<40)
            headVertTheta += 2.0;
        if (key == 'a' && headHorizTheta>-30)
            headHorizTheta -= 2.0;
        if (key == 'd' && headHorizTheta<30)
            headHorizTheta += 2.0;
    }
    else {
        float* vertTheta;
        float* horizTheta;
        switch (armSegment)
        {
            case R_SHOULDER:
                if (rightSide) {
                    angleSwitch=1;
                    vertTheta = &rightShoulderVertTheta;
                    horizTheta = &rightShoulderHorizTheta;
                    
                }
                else {
                    angleSwitch=-1;
                    vertTheta = &leftShoulderVertTheta;
                    horizTheta = &leftShoulderHorizTheta;
                }
                break;
            case R_ELBOW:
                if (rightSide) {
                    angleSwitch=1;
                    vertTheta = &rightElbowVertTheta;
                    horizTheta = &rightElbowHorizTheta;
                }
                else {
                    angleSwitch=-1;
                    vertTheta = &leftElbowVertTheta;
                    horizTheta = &leftElbowHorizTheta;
                }
                break;
            case R_WRIST:
                if (rightSide) {
                    angleSwitch=1;
                    vertTheta = &rightWristVertTheta;
                    horizTheta = &rightWristHorizTheta;
                }
                else {
                    angleSwitch=-1;
                    vertTheta = &leftWristVertTheta;
                    horizTheta = &leftWristHorizTheta;
                }
                break;
                
        }

        if (key == 'w' && *vertTheta >-30)
            *vertTheta -= 2.0;
        if (key == 's' && *vertTheta<30)
            *vertTheta += 2.0;
        if (rightSide) {
            if (key == 'a' && *horizTheta<30)
                *horizTheta += 2.0*angleSwitch;
            if (key == 'd' && *horizTheta>-30)
                *horizTheta -= 2.0*angleSwitch;
        }
        else {
            if (key == 'a' && *horizTheta>-30)
                *horizTheta += 2.0*angleSwitch;
            if (key == 'd' && *horizTheta<30)
                *horizTheta -= 2.0*angleSwitch;
        }

    }
   
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
    gluPerspective(40.0+camzoom, 1.0, 1, 100.0);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
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
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
    // initialize modelview matrix
    glMatrixMode(GL_MODELVIEW_MATRIX);
    
    if (campersp==true) {
        glLoadIdentity();
        double x = 15.0*sin(camphi);
        double y = 15*sin(camtheta);
        double z = 15.0*cos(camphi);
        gluLookAt(x,y,z,0,0,0,0,1,0);
    }
    if (campersp == false) {
        glLoadIdentity();
        double v = headVertTheta * M_PI / 180;
        double h = headHorizTheta * M_PI / 180;
        gluLookAt(robotX, 3, robotZ+1, robotX+sin(h)*cos(v), 3-sin(v), robotZ+2+cos(h)*cos(v), 0, 1, 0);
    }


    // position of light0
    GLfloat lightPosition[]={1,1,5,1};
    glLightfv(GL_LIGHT0, GL_POSITION, lightPosition);
    glLightf(GL_LIGHT0, GL_QUADRATIC_ATTENUATION, 0.04);
    
    if (amblight==false) {
        GLfloat ambient[4] = {0,0,0,0};
        glLightModelfv(GL_LIGHT_MODEL_AMBIENT,ambient);
    }
    else if (amblight==true) {
        GLfloat ambient[4] = {.4,.4,.4,1};
        glLightModelfv(GL_LIGHT_MODEL_AMBIENT,ambient);
    }
    if (ptlight==false)
        glDisable(GL_LIGHT0);
    else if (ptlight==true)
        glEnable(GL_LIGHT0);
    if (laser==false)
        glDisable(GL_LIGHT1);
    else if (laser==true)
        glEnable(GL_LIGHT1);

    
    glNormal3f(0,1,0);
    drawFloor();
    drawRobot(1);
    drawTeapot();
    drawIcosahedron();
    drawCone(1,4,20,20);
    glutSwapBuffers();
}

void drawFloor() {
    glPushMatrix();
    int floorSize = 6;
    for (int i=-floorSize; i<floorSize; i++) {
        for (int j=-floorSize; j<floorSize; j++) {
            if ((i+j)%2 == 0){
                GLfloat red[] = {0.9,0,0};
                glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, red);
                glMaterialfv(GL_FRONT, GL_SPECULAR, red);
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
    glTranslatef(0, 4, 10);
    glPopMatrix();
}

void drawRobot(GLfloat radius) {
    glPushMatrix();
    glTranslatef(robotX,radius,robotZ);
//    drawBottom();
//    drawMiddle();
    drawLegs();
    drawArms();
    headAssembly();
    glPopMatrix();
    
}

void drawBottom() {
    glPushMatrix();
    GLfloat radius=1;
    GLfloat white[] = {1,1,1};
    glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, white);
    glMaterialfv(GL_FRONT, GL_SPECULAR, white);
    glMateriali(GL_FRONT,GL_SHININESS,0);
    glutSolidSphere(radius, 20, 20);
    glPopMatrix();
}

void drawLegs() {
    glPushMatrix();
    GLfloat radius=0.15;
    GLfloat green[] = {0,0.8,0};
    glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, green);
    glMaterialfv(GL_FRONT, GL_SPECULAR, green);
    glMateriali(GL_FRONT,GL_SHININESS,0);
    GLUquadricObj* quadric = gluNewQuadric();
    glTranslatef(-0.4, 0.75, 0);
    glRotatef(90, 1, 0, 0);
    gluCylinder(quadric, radius, radius, 1.6, 20, 20);
    glTranslatef(0.8, 0, 0);
    gluCylinder(quadric, radius, radius, 1.6, 20, 20);
    glPopMatrix();
    
    // Feet
    glPushMatrix();
    glEnable(GL_POLYGON_OFFSET_FILL);
    glPolygonOffset(1, -1);
    glTranslatef(-0.4,-0.85,0);
    glutSolidSphere(0.15,20,20);
    glTranslatef(0.8,0,0);
    glutSolidSphere(0.15,20,20);
    glDisable(GL_POLYGON_OFFSET_FILL);
    glPopMatrix();
    gluDeleteQuadric(quadric);
}

void headAssembly() {
    glPushMatrix();
    glTranslatef(0, 2, 0);
    glRotatef(headVertTheta, 1, 0, 0);
    glRotatef(headHorizTheta, 0, 1, 0);
    GLfloat radius=1.5;
    GLfloat green[] = {0,0.8,0};
    glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, green);
    glMaterialfv(GL_FRONT, GL_SPECULAR, green);
    glMateriali(GL_FRONT,GL_SHININESS,0);
    glutSolidSphere(radius, 20, 20);
    drawEyes();
    glPopMatrix();
}

void drawEyes() {
    glPushMatrix();
    glTranslatef(0, 0.325, 1);
    GLfloat radius=0.7;
    GLfloat white[] = {1,1,1};
    GLfloat teal[] = {0,0.5,0.45};
    GLfloat black[] = {0,0,0};
    glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, white);
    glMaterialfv(GL_FRONT, GL_SPECULAR, white);
    glMateriali(GL_FRONT,GL_SHININESS,0);
    glutSolidSphere(radius, 20, 20);
    glTranslatef(0, 0.1, 0.4);
    glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, teal);
    glMaterialfv(GL_FRONT, GL_SPECULAR, teal);
    glMateriali(GL_FRONT,GL_SHININESS,0);
    glutSolidSphere(radius/2, 20, 20);
    glTranslatef(0, 0.02, 0.125);
    GLfloat laserPos[] = {0,0,radius/2,1};
    glLightfv(GL_LIGHT1, GL_POSITION, laserPos);
    
    // Set the direction of the laser
    GLfloat laserDir[] = {0,0,1};
    glLightfv(GL_LIGHT1, GL_SPOT_DIRECTION, laserDir);
    glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, black);
    glMaterialfv(GL_FRONT, GL_SPECULAR, black);
    glutSolidSphere(radius/3, 20, 20);
    glPopMatrix();
}

void drawArms(){
    glPushMatrix();
    GLfloat radius=0.15;
    GLfloat green[] = {0,0.8,0};
    glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, green);
    glMaterialfv(GL_FRONT, GL_SPECULAR, green);
    glMateriali(GL_FRONT,GL_SHININESS,0);
    GLUquadricObj* quadric = gluNewQuadric();
    glTranslatef(1.5, 1.75, 0);
    glRotatef(90, 0, 1, 0);
    glutSolidSphere(radius, 20, 20);
    // right upper
    glRotatef(rightShoulderVertTheta, 1, 0, 0);
    glRotatef(rightShoulderHorizTheta, 0, 1, 0);
    gluCylinder(quadric, radius, radius, 0.6, 20, 20);
    // right elbow
    glTranslatef(0, 0, 0.6);
    glutSolidSphere(radius, 20, 20);
    // right lower
    glRotatef(rightElbowVertTheta, 1, 0, 0);
    glRotatef(rightElbowHorizTheta, 0, 1, 0);
    gluCylinder(quadric, radius, radius, 0.75, 20, 20);
    // right wrist
    glTranslatef(0, 0, 0.75);
    glutSolidSphere(radius, 20, 20);
    // right hand
    glRotatef(rightWristVertTheta, 1, 0, 0);
    glRotatef(rightWristHorizTheta, 0, 1, 0);
    gluCylinder(quadric, radius, radius, 0.2, 20, 20);
    glTranslatef(0, 0, 0.2);
    glutSolidSphere(radius, 20, 20);
    glPopMatrix();
    
    glPushMatrix();
    glTranslatef(-1.5, 1.75, 0);
    glRotatef(-90, 0, 1, 0);
    glutSolidSphere(radius, 20, 20);
    // left upper
    glRotatef(leftShoulderVertTheta, 1, 0, 0);
    glRotatef(leftShoulderHorizTheta, 0, 1, 0);
    gluCylinder(quadric, radius, radius, 0.6, 20, 20);
    // left elbow
    glTranslatef(0, 0, 0.6);
    // left lower
    glRotatef(leftElbowVertTheta, 1, 0, 0);
    glRotatef(leftElbowHorizTheta, 0, 1, 0);
    glutSolidSphere(radius, 20, 20);
    gluCylinder(quadric, radius, radius, 0.75, 20, 20);
    // left wrist
    glTranslatef(0, 0, 0.75);
    glutSolidSphere(radius, 20, 20);
    // left hand
    glRotatef(leftWristVertTheta, 1, 0, 0);
    glRotatef(leftWristHorizTheta, 0, 1, 0);
    gluCylinder(quadric, radius, radius, 0.2, 20, 20);
    glTranslatef(0, 0, 0.2);
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
	glMateriali(GL_FRONT,GL_SHININESS,50);
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




