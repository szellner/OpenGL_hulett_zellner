#include <iostream>
#include <glut/glut.h>
#include "SOIL.h"
#include <math.h>

enum {
    M_HELP,
    M_AMBIENT,
    M_POINTLIGHT,
    M_PERSPECTIVE,
    M_ROLLERCOASTER,
    M_SPACE,
    M_SKY,
    M_OCEAN,
    M_CRYSTAL,
    M_FOG,
    R_WRIST,
    R_ELBOW,
    R_SHOULDER
};

#define MAX_NO_TEXTURES 1
#define MAX_FILE_NAME 512
char textureFileNameWithPath[MAX_FILE_NAME];
GLuint textureIds[MAX_NO_TEXTURES];


// function prototypes
int LoadGLTextures(char* fname);
void display(void);
void ride();
void reshape(int width, int height);
void idle(void);
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
void drawRollercoaster();
void drawCurve(int startPoint);
void drawCone(GLdouble base, GLdouble height, GLint slices, GLint stacks);
void drawCube();
void drawFog();
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

// rotation parameters
float xrot=0;
float yrot=0;
float zrot=0;

// environment globals
int textures[4];
bool fog=false;

// rollercoaster globals
float curveVerts[25][3] = {
    {35, -27,-18},
    {46.1538, -27.8462,-24.8462},
    {65.3077, -26.6923,-17.6923},
    {61.4615, -27.5385,12.4615},
    {57.6154, -28.3846,42.6154},
    {21.7692, -3.23077,30.7692},
    {17.9231, 3.92308,26.9231},
    {14.0769, 11.0769,23.0769},
    {8.23077, 21.2308,19.2308},
    {4.38461, 17.3846,15.3846},
    {0.538462, 13.5385,11.5385},
    {-3.30769, 8.69231,7.69231},
    {-5.15385, 3.84615,3.84615},
    {-7, -1,0},
    {4.15385, -12.8462,-18.8462},
    {0.307693, -16.6923,-22.6923},
    {-3.53846, -20.5385,-26.5385},
    {-38.3846, -14.3846,-0.384615},
    {-43.2308, -18.2308,-16.2308},
    {-48.0769, -22.0769,-32.0769},
    {11.0769, -20.9231,-77.9231},
    {22.2308, -30.7692,-77.7692},
    {33.3846, -40.6154,-77.6154},
    {47.5385, -38.4615,-9.46154},
    {-27.3077, -42.3077,-53.3077}};
bool rollercoaster=false;
float curveDrawStep=0;
float curveCamStep=0;
float rcx=0;
float rcy=0;
float rcz=0;
bool storeNext=false;
float nextx=0;
float nexty=0;
float nextz=0;

using namespace std;

bool makeFnameWithPath(char* fname, char* pathName, char* fnameWithPath)
{
    
    int last = -1;
    for (int i = 0; last==-1 && i < MAX_FILE_NAME; ++i) {
        if (pathName[i] == 0) {
            last=i;
            if (i>0 && pathName[i-1]!='/') {
                fnameWithPath[i]='/';
                last ++;
            }
        }
        else {
            fnameWithPath[i]=pathName[i];
        }
    }
    
    // if the pathname exceeds our space bound we return false
    // we could make this more robust by dynamically allocating the right amout of space!
    if (last == -1) {
        return false;
    }
    
    bool done=false;
    
    for (int i=0; !done && i<MAX_FILE_NAME; ++i) {
        fnameWithPath[last+i] = fname[i];
        if (fname[i]==0) {
            done=true;
        }
    }
    return done;
}

int main(int argc, char **argv)
{
//    // name of texture file
//    char* fname="clouds.jpg";
//    
//    // texture file is expected to be in build directory (i.e. argv[0]) unless the project folder is given as
//    // a command line argument
//    // edit the project scheme to add command line argument $SRSROOT (which xcode knows as the project dir)
//    if (argc==2) {
//        makeFnameWithPath(fname, argv[1], textureFileNameWithPath);
//    }
//    else {
//        makeFnameWithPath(fname, argv[0], textureFileNameWithPath);
//    }

    // initialize glut
    glutInit(&argc, argv);
    
    // set window size
    glutInitWindowSize(windowWidth,windowHeight);
    
    // establish glut display parameters
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB  | GLUT_DEPTH | GLUT_STENCIL);
    
    // create window
    glutCreateWindow("Mike Wazowskibot");
    
    // register callback functions
    glutDisplayFunc(display);
    glutReshapeFunc(reshape);
    glutSpecialFunc(skeyboard);
    glutKeyboardFunc(keyboard);
    glutMouseFunc(mouse);
    glutMotionFunc(motion);
    glutIdleFunc(idle);
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
    glutAddMenuEntry("Rollercoaster Mode", M_ROLLERCOASTER);
    
    int environ = glutCreateMenu(menu);
    glutAddMenuEntry("Space", M_SPACE);
    glutAddMenuEntry("Sky", M_SKY);
    glutAddMenuEntry("Ocean", M_OCEAN);
    glutAddMenuEntry("Crystal", M_CRYSTAL);
    glutAddMenuEntry("Toggle Fog", M_FOG);
    
    glutCreateMenu(menu);
    glutAddMenuEntry("Help", M_HELP);
    
    glutAddSubMenu("Lighting", lighting);
    glutAddSubMenu("Joint Controls", armSegments);
    glutAddSubMenu("Camera Controls", camSettings);
    glutAddSubMenu("Environment", environ);
    
    glutAttachMenu(GLUT_RIGHT_BUTTON);
    
    // initialize background color to black
    glClearColor(0,0,0,0);
    glClearStencil(0.0);
    
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
    // load textures
    textures[0] = LoadGLTextures("/Users/stephaniezellner/Desktop/OpenGL_hulett_zellner/Robot/Robot/newspace.jpg");
    textures[1] = LoadGLTextures("/Users/stephaniezellner/Desktop/OpenGL_hulett_zellner/Robot/Robot/clouds.jpg");
    textures[2] = LoadGLTextures("/Users/stephaniezellner/Desktop/OpenGL_hulett_zellner/Robot/Robot/greenwater.jpg");
    textures[3] = LoadGLTextures("/Users/stephaniezellner/Desktop/OpenGL_hulett_zellner/Robot/Robot/diamond.jpg");
    glBindTexture(GL_TEXTURE_2D, textures[2]);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_DECAL);
    
    // fog parameters
    GLfloat fogColor[4] = {0.9,0.9,0.9,1.0};
    glFogi(GL_FOG_MODE, GL_EXP2);
    glFogfv(GL_FOG_COLOR, fogColor);
    glFogf(GL_FOG_DENSITY, 0.05);
    glHint(GL_FOG_HINT, GL_FASTEST);
    glFogf(GL_FOG_START, 0);
    glFogf(GL_FOG_END, 1);
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
        case M_ROLLERCOASTER:
            rollercoaster = true;
            ride();
            break;
        case M_SPACE:
            glBindTexture(GL_TEXTURE_2D, textures[0]);
            break;
        case M_SKY:
            glBindTexture(GL_TEXTURE_2D, textures[1]);
            break;
        case M_OCEAN:
            glBindTexture(GL_TEXTURE_2D, textures[2]);
            break;
        case M_CRYSTAL:
            glBindTexture(GL_TEXTURE_2D, textures[3]);
            break;
        case M_FOG:
            fog = !fog;
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

void ride() {
    curveCamStep = 0;
    rcx = 0.1*curveVerts[0][0];
    rcy = 0.1*curveVerts[0][1];
    rcz = 0.1*curveVerts[0][2];
    // depends on the number of samples being taken
    while (curveCamStep<8.0) {
        cout << "camstep " << curveCamStep << endl;
        curveCamStep += 0.01;
        display();
    }
    // end of ride signals reset to previous camera settings
    rollercoaster=false;
    glutPostRedisplay();
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
    if (button==GLUT_LEFT_BUTTON && state==GLUT_DOWN && campersp==true) {
        motion(-1,-1);
    }
}

void idle(void)
{
    // update rotation
    xrot+=0.006f;
    yrot+=0.004f;
    zrot+=0.008f;
    glutPostRedisplay();
    
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

int LoadGLTextures(char* fname)
{
    int textureId = SOIL_load_OGL_texture(fname, SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_INVERT_Y);
    if(textureId == 0) {
        cerr << "Texture failed to load." << endl;
        return 0;
    }
    
    return textureId;
}


void display()
{
    // clear buffers
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
    // initialize modelview matrix
    glMatrixMode(GL_MODELVIEW_MATRIX);
    
    if (campersp==true && !rollercoaster) {
        glLoadIdentity();
        double x = 15.0*sin(camphi);
        double y = 15*sin(camtheta);
        double z = 15.0*cos(camphi);
        gluLookAt(x,y,z,0,0,0,0,1,0);
    }
    if (campersp == false && !rollercoaster) {
        glLoadIdentity();
        double v = headVertTheta * M_PI / 180;
        double h = headHorizTheta * M_PI / 180;
        gluLookAt(robotX, 3, robotZ+1, robotX+sin(h)*cos(v), 3-sin(v), robotZ+2+cos(h)*cos(v), 0, 1, 0);
    }
    if (rollercoaster) {
        glLoadIdentity();
        cout << "rcx/y/z " << rcx << "/" << rcy << "/" << rcz << endl;
        gluLookAt(rcx+6,rcy+5,rcz,nextx+6,nexty+5,nextz,0,1,0);
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
    if (fog==true)
        glEnable(GL_FOG);
    else if (fog==false)
        glDisable(GL_FOG);
    
    glLineWidth(4);
    
    glNormal3f(0,1,0);
    drawFloor();
    drawRobot(1);
    drawTeapot();
    drawCube();
    drawIcosahedron();
    drawCone(1,4,20,20);
    drawRollercoaster();
    glutSwapBuffers();
}

void drawCurve(int startPoint) {
    
    if (startPoint<0 || startPoint+2>=25)
        return;
    
    float coeff[3][4];
    float basisMatrix[4][4] = {
        {-1,  3, -3, 1},
        { 3, -6,  3, 0},
        {-3,  3,  0, 0},
        { 1,  0,  0, 0}};
    /*  compute coefficients for the x,y, and z cubic polynomials */
    for (int d=0; d<3; d++) { // compute for dimension x, y, and z
        for (int i=0; i< 4; i++) { // compute coeff[d][i]
            coeff[d][i]=0;
            for (int j=0;j<4;j++) {
                coeff[d][i] += basisMatrix[i][j] * curveVerts[j+startPoint][d];
            }
        }
    }
    
    /*  approximate the curve by a line strip through sample points	*/
    glBegin(GL_LINE_STRIP);
    float numSamples=20;
    float t=0;
    while(t<1) {
        curveDrawStep += 0.05;
        float polyVal[3];
        for (int i=0;i<3;i++) {
            polyVal[i] = coeff[i][0]*t*t*t + coeff[i][1]*t*t + coeff[i][2]*t + coeff[i][3];
        }
        glVertex3f(0.1*polyVal[0], 0.1*polyVal[1], 0.1*polyVal[2]);
        if (rollercoaster)
            cout << curveDrawStep << "/" << curveCamStep << endl;
        if (fabs(curveDrawStep-curveCamStep)<0.0001){
            storeNext=true;
            cout << "equal " << curveDrawStep << endl;
            rcx=0.1*polyVal[0];
            rcy=0.1*polyVal[1];
            rcz=0.1*polyVal[2];
        }
        else if (storeNext) {
            nextx=0.1*polyVal[0];
            nexty=0.1*polyVal[1];
            nextz=0.1*polyVal[2];
            storeNext=false;
        }
        
        t += 1.0/numSamples;
    }
    /* the curve ends at a control point when t=1  				*/
    /* because the increment 1.0/numSamples  has finite precision	*/
    /* t probably won't hit 1.0 exactly, so we force it			*/
        
    glVertex3f(0.1*curveVerts[startPoint+3][0],0.1*curveVerts[startPoint+3][1],0.1*curveVerts[startPoint+3][2]);
    glEnd();
    
}

void drawRollercoaster() {
    glPushMatrix();
    glTranslatef(6, 5, 0);
    GLfloat yellow[] = {1,1,0};
    glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, yellow);
	glMaterialfv(GL_FRONT, GL_SPECULAR, yellow);
	glMateriali(GL_FRONT,GL_SHININESS,0);
    int numCurves = 8;
    curveDrawStep = 0.0;
    for (int i=0; i<numCurves; i++)
    {
        drawCurve(3*i);
    }
    
    // restore to camera view
    glPopMatrix();
}


void drawCube () {
    float cubesize = 32;
    glEnable(GL_TEXTURE_2D);
    glPushMatrix();
    glTranslatef(0,2,0);
    // rotate cube
    glRotatef ( xrot, 1.0, 0.0, 0.0 );
    glRotatef ( yrot, 0.0, 1.0, 0.0 );
    glRotatef ( zrot, 0.0, 0.0, 1.0 );
    glBegin(GL_QUADS);
    // front face
    glTexCoord2f(0.0f, 0.0f); glVertex3f(-cubesize, -cubesize,  cubesize);
    glTexCoord2f(1.0f, 0.0f); glVertex3f( cubesize, -cubesize,  cubesize);
    glTexCoord2f(1.0f, 1.0f); glVertex3f( cubesize,  cubesize,  cubesize);
    glTexCoord2f(0.0f, 1.0f); glVertex3f(-cubesize,  cubesize,  cubesize);
    // Back Face
    glTexCoord2f(1.0f, 0.0f); glVertex3f(-cubesize, -cubesize, -cubesize);
    glTexCoord2f(1.0f, 1.0f); glVertex3f(-cubesize,  cubesize, -cubesize);
    glTexCoord2f(0.0f, 1.0f); glVertex3f( cubesize,  cubesize, -cubesize);
    glTexCoord2f(0.0f, 0.0f); glVertex3f( cubesize, -cubesize, -cubesize);
    // Top Face
    glTexCoord2f(0.0f, 1.0f); glVertex3f(-cubesize,  cubesize, -cubesize);
    glTexCoord2f(0.0f, 0.0f); glVertex3f(-cubesize,  cubesize,  cubesize);
    glTexCoord2f(1.0f, 0.0f); glVertex3f( cubesize,  cubesize,  cubesize);
    glTexCoord2f(1.0f, 1.0f); glVertex3f( cubesize,  cubesize, -cubesize);
    // Bottom Face
    glTexCoord2f(1.0f, 1.0f); glVertex3f(-cubesize, -cubesize, -cubesize);
    glTexCoord2f(0.0f, 1.0f); glVertex3f( cubesize, -cubesize, -cubesize);
    glTexCoord2f(0.0f, 0.0f); glVertex3f( cubesize, -cubesize,  cubesize);
    glTexCoord2f(1.0f, 0.0f); glVertex3f(-cubesize, -cubesize,  cubesize);
    // Right face
    glTexCoord2f(1.0f, 0.0f); glVertex3f( cubesize, -cubesize, -cubesize);
    glTexCoord2f(1.0f, 1.0f); glVertex3f( cubesize,  cubesize, -cubesize);
    glTexCoord2f(0.0f, 1.0f); glVertex3f( cubesize,  cubesize,  cubesize);
    glTexCoord2f(0.0f, 0.0f); glVertex3f( cubesize, -cubesize,  cubesize);
    // Left Face
    glTexCoord2f(0.0f, 0.0f); glVertex3f(-cubesize, -cubesize, -cubesize);
    glTexCoord2f(1.0f, 0.0f); glVertex3f(-cubesize, -cubesize,  cubesize);
    glTexCoord2f(1.0f, 1.0f); glVertex3f(-cubesize,  cubesize,  cubesize);
    glTexCoord2f(0.0f, 1.0f); glVertex3f(-cubesize,  cubesize, -cubesize);
    glEnd();
    glPopMatrix();
    glDisable(GL_TEXTURE_2D);
    
}

void drawFloor() {
    glPushMatrix();
    int floorSize = 8;
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
    glTranslatef(-4, size/1.325, -4);
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




