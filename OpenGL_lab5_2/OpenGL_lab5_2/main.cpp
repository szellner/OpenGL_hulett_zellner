//
// This tutorial was adapted from http://nehe.gamedev.net/ opengl tutorials
//

#include <iostream>
#include <OpenGL/gl.h>
#include <OpenGL/glu.h>
#include <glut/glut.h>
#include "SOIL.h"
#include <math.h>


using namespace std;

// window width/height
int windowWidth=500;
int windowHeight=500;

// camera rotation parameters
float phi=0;
float theta=.5;

// rotation parameters
float xrot=0;
float yrot=0;
float zrot=0;


#define MAX_NO_TEXTURES 1
#define MAX_FILE_NAME 512
//
char textureFileNameWithPath[MAX_FILE_NAME];
GLuint textureIds[MAX_NO_TEXTURES];

bool LoadGLTextures(char* fname);


void init(void)
{
    // set up the view volume
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(60,1,1,200);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    
    // clear color, depth test.
    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
    glClearStencil(0.0);

    glEnable(GL_DEPTH_TEST);
    LoadGLTextures("/Users/stephaniezellner/Desktop/OpenGL_hulett_zellner/OpenGL_lab5/OpenGL_lab5/cat.png");
    
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
 
        if (lastY==-1 || y==-1) {
            lastY = y;
        }
        else {
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



void idle(void)
{
    // update rotation
    xrot+=0.3f;
    yrot+=0.2f;
    zrot+=0.4f;
    glutPostRedisplay();
    
}


void reshape(int width, int height)
{
    if (width<height)
        glViewport(0,0,width,width);
    else
        glViewport(0,0,height,height);
}

void drawCube () {
    glEnable(GL_TEXTURE_2D);
    glPushMatrix();
    glTranslatef(0,2,0);
    // rotate cube
    glRotatef ( xrot, 1.0, 0.0, 0.0 );
    glRotatef ( yrot, 0.0, 1.0, 0.0 );
    glRotatef ( zrot, 0.0, 0.0, 1.0 );
    glBegin(GL_QUADS);
    // front face
    glTexCoord2f(0.0f, 0.0f); glVertex3f(-1.0f, -1.0f,  1.0f);
    glTexCoord2f(1.0f, 0.0f); glVertex3f( 1.0f, -1.0f,  1.0f);
    glTexCoord2f(1.0f, 1.0f); glVertex3f( 1.0f,  1.0f,  1.0f);
    glTexCoord2f(0.0f, 1.0f); glVertex3f(-1.0f,  1.0f,  1.0f);
    // Back Face
    glTexCoord2f(1.0f, 0.0f); glVertex3f(-1.0f, -1.0f, -1.0f);
    glTexCoord2f(1.0f, 1.0f); glVertex3f(-1.0f,  1.0f, -1.0f);
    glTexCoord2f(0.0f, 1.0f); glVertex3f( 1.0f,  1.0f, -1.0f);
    glTexCoord2f(0.0f, 0.0f); glVertex3f( 1.0f, -1.0f, -1.0f);
    // Top Face
    glTexCoord2f(0.0f, 1.0f); glVertex3f(-1.0f,  1.0f, -1.0f);
    glTexCoord2f(0.0f, 0.0f); glVertex3f(-1.0f,  1.0f,  1.0f);
    glTexCoord2f(1.0f, 0.0f); glVertex3f( 1.0f,  1.0f,  1.0f);
    glTexCoord2f(1.0f, 1.0f); glVertex3f( 1.0f,  1.0f, -1.0f);
    // Bottom Face
    glTexCoord2f(1.0f, 1.0f); glVertex3f(-1.0f, -1.0f, -1.0f);
    glTexCoord2f(0.0f, 1.0f); glVertex3f( 1.0f, -1.0f, -1.0f);
    glTexCoord2f(0.0f, 0.0f); glVertex3f( 1.0f, -1.0f,  1.0f);
    glTexCoord2f(1.0f, 0.0f); glVertex3f(-1.0f, -1.0f,  1.0f);
    // Right face
    glTexCoord2f(1.0f, 0.0f); glVertex3f( 1.0f, -1.0f, -1.0f);
    glTexCoord2f(1.0f, 1.0f); glVertex3f( 1.0f,  1.0f, -1.0f);
    glTexCoord2f(0.0f, 1.0f); glVertex3f( 1.0f,  1.0f,  1.0f);
    glTexCoord2f(0.0f, 0.0f); glVertex3f( 1.0f, -1.0f,  1.0f);
    // Left Face
    glTexCoord2f(0.0f, 0.0f); glVertex3f(-1.0f, -1.0f, -1.0f);
    glTexCoord2f(1.0f, 0.0f); glVertex3f(-1.0f, -1.0f,  1.0f);
    glTexCoord2f(1.0f, 1.0f); glVertex3f(-1.0f,  1.0f,  1.0f);
    glTexCoord2f(0.0f, 1.0f); glVertex3f(-1.0f,  1.0f, -1.0f);
    glEnd();
    glPopMatrix();
    glDisable(GL_TEXTURE_2D);
    
}

bool LoadGLTextures(char* fname)
{
    int textureId = SOIL_load_OGL_texture(fname, SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_INVERT_Y);
    if(textureId == 0)
        return false;
    
    glBindTexture(GL_TEXTURE_2D, textureId);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
    glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_DECAL);
    
    return true;
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
    double x = 10.0*sin(phi);
    double y = 10*sin(theta);
    double z = 10.0*cos(phi);
    gluLookAt(x,y,z,0,0,0,0,1,0);
    glPushMatrix();
    
    
    // draw cube
    glColor3f(0,0,1);
    drawCube();
 
    
    glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);
    glDisable(GL_DEPTH_TEST);
    
    glEnable(GL_STENCIL_TEST);
    glStencilFunc(GL_EQUAL,0,1);
    glStencilOp(GL_KEEP, GL_KEEP, GL_INCR);

    glEnable(GL_CULL_FACE);
    // draw floor
    float s=5;
    glColor4f(1,0,0,0.8);
    glBegin(GL_QUADS);
    glVertex3f(-s,0,-s);
    glVertex3f(-s, 0, s);
    glVertex3f(s,0,s);
    glVertex3f(s,0,-s);
    glEnd();
    glDisable(GL_CULL_FACE);
    
    glEnable(GL_DEPTH_TEST);
    
    glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
    glStencilFunc(GL_LEQUAL,1,3);
    glStencilOp(GL_KEEP, GL_KEEP, GL_INCR);
    
    // draw "reflected cube"
    glColor3f(0,0,1);
    glPushMatrix();
    glScalef(1,-1,1);
    drawCube();
    glPopMatrix();

    glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
    glStencilFunc(GL_GEQUAL,1,3);
    glStencilOp(GL_KEEP, GL_KEEP, GL_KEEP);
    
    // draw floor
    glColor4f(1,0,0,0.8);
    glBegin(GL_QUADS);
    glVertex3f(-s,0,-s);
    glVertex3f(-s, 0, s);
    glVertex3f(s,0,s);
    glVertex3f(s,0,-s);
    glEnd();
    
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    
    glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
    glStencilFunc(GL_LESS,1,3);
    glStencilOp(GL_KEEP, GL_KEEP, GL_KEEP);
    
    // draw floor
    glColor4f(1,0,0,0.8);
    glBegin(GL_QUADS);
    glVertex3f(-s,0,-s);
    glVertex3f(-s, 0, s);
    glVertex3f(s,0,s);
    glVertex3f(s,0,-s);
    glEnd();

    
    glDisable(GL_BLEND);
    glDisable(GL_STENCIL_TEST);
    
    
    glPopMatrix();
    glutSwapBuffers();
}


void keyboard ( unsigned char key, int x, int y )  // Create Keyboard Function
{

}

void arrow_keys ( int a_keys, int x, int y )  // Create Special Function (required for arrow keys)
{
    switch ( a_keys ) {
        case GLUT_KEY_UP:     // When Up Arrow Is Pressed...
            glutFullScreen ( ); // Go Into Full Screen Mode
            break;
        case GLUT_KEY_DOWN:               // When Down Arrow Is Pressed...
            glutReshapeWindow ( 500, 500 ); // Go Into A 500 By 500 Window
            break;
        default:
            break;
    }
}

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

int main ( int argc, char** argv )
{
    // name of texture file
    char* fname="cat.png";
    
    // texture file is expected to be in build directory (i.e. argv[0]) unless the project folder is given as
    // a command line argument
    // edit the project scheme to add command line argument $SRSROOT (which xcode knows as the project dir)
    if (argc==2) {
        makeFnameWithPath(fname, argv[1], textureFileNameWithPath);
    }
    else {
        makeFnameWithPath(fname, argv[0], textureFileNameWithPath);
    }
    glutInit(&argc, argv );
    glutInitDisplayMode ( GLUT_DEPTH | GLUT_DOUBLE | GLUT_RGBA | GLUT_STENCIL ); // Display Mode
    glutInitWindowPosition (0,0);
    glutInitWindowSize  ( 500, 500 );
    glutCreateWindow    ( "Texture tutorial" );
    init ();
    glutDisplayFunc     ( display );
    glutReshapeFunc     ( reshape );
    glutKeyboardFunc    ( keyboard );
    glutMouseFunc(mouse);
    glutMotionFunc(motion);
    glutSpecialFunc     ( arrow_keys );
    glutIdleFunc		( idle );
    glutMainLoop        ( );
    
    return 1;
}




