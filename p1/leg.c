#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glut.h>

// This is used for drawing alternative cubes and spheres of the ones mentioned on
// the assignement because i think its very difficult to check if the 3d 
// transformations are ok with their solid version.
// So comment/uncomment the line below to appreciate it better.
#define SHADED

#define ESCAPE_KEY 27

#define ORIGIN_X 100
#define ORIGIN_Y 100

#define WIDTH  500
#define HEIGHT 500

#define BG_BRIGHTNESS 0.07

#define OBJECTS_SIZE   1
#define ROTATION_SPEED 1

typedef GLfloat Vec3[3];

// rotation is defined like a vector
// where the x represent the amount of rotation around the x axis and so on
// i defined rotation in that way instead of using a struct to be able to
// use pointer arithmetic to change values instead of having duplicated code
typedef Vec3 Rotation;

Rotation fstRotation = { 0.0f, 0.0f, 0.0f }, // first part of the object
         sndRotation = { 0.0f, 0.0f, 0.0f }; // second part (relative to the first)

void initWindow()
{
    glutInitWindowPosition(ORIGIN_X, ORIGIN_Y);
    glutInitWindowSize(WIDTH, HEIGHT);
    glutCreateWindow("Leg");
}

void initGL()
{
    glClearColor(BG_BRIGHTNESS, BG_BRIGHTNESS, BG_BRIGHTNESS, 0);
    glEnable(GL_DEPTH_TEST);

    // optimization mentioned at class to not render the backface of the polygons
    glEnable(GL_CULL_FACE);
}

void reshape(int width, int height)
{
    glViewport(0, 0, (GLsizei)width, (GLsizei)height);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();

    // with a FOV of 45 instead of 60 shapes appear less distorted
    gluPerspective(45.0f, (GLfloat)width / (GLfloat)height, 1.0f, 20.0f);
    glMatrixMode(GL_MODELVIEW);
}

void drawCube(float size)
{
    #ifdef SHADED
        // One color is used for the faces perpendicular to each axis

        GLfloat x = (GLfloat)size / 2;
        glBegin(GL_QUADS);
        
        // Z faces

        glColor3f(1.0f, 0.53f, 0.2f);

            glVertex3f( x, -x, -x);
            glVertex3f(-x, -x, -x);
            glVertex3f(-x,  x, -x);
            glVertex3f( x,  x, -x);

            glVertex3f(-x, -x,  x);
            glVertex3f( x, -x,  x);
            glVertex3f( x,  x,  x);
            glVertex3f(-x,  x,  x);

        // X faces

        glColor3f(1.0f, 0.71f, 0.31f);

            glVertex3f(-x, -x, -x);
            glVertex3f(-x, -x,  x);
            glVertex3f(-x,  x,  x);
            glVertex3f(-x,  x, -x);

            glVertex3f( x, -x,  x);
            glVertex3f( x, -x, -x);
            glVertex3f( x,  x, -x);
            glVertex3f( x,  x,  x);
        
        // Y faces

        glColor3f(1.0f, 0.93f, 0.6f);

            glVertex3f(-x,  x,  x);
            glVertex3f( x,  x,  x);
            glVertex3f( x,  x, -x);
            glVertex3f(-x,  x, -x);

            glVertex3f( x, -x, -x);
            glVertex3f( x, -x,  x);
            glVertex3f(-x, -x,  x);
            glVertex3f(-x, -x, -x);
        
        // end

        glEnd();
    #else
        glColor3f(1.0f, 0.71f, 0.31f);
        glutSolidCube(size);
    #endif
}

void drawSphere(double diameter)
{
    glColor3f(0.2f, 0.67f, 1.0f);

    #ifdef SHADED
        glutWireSphere(diameter / 2, 40, 30);
    #else
        glutSolidSphere(diameter / 2, 40, 30);
    #endif
}

void applyRotation(Rotation rot)
{
    glRotatef(rot[0], 1.0f, 0.0f, 0.0f);    
    glRotatef(rot[1], 0.0f, 1.0f, 0.0f);
    glRotatef(rot[2], 0.0f, 0.0f, 1.0f);
}

void drawPart(Rotation rot)
{
    glTranslatef(0.0f, -OBJECTS_SIZE, 0.0f);
        drawSphere(OBJECTS_SIZE);
    
    applyRotation(rot);
    glTranslatef(0.0f, -OBJECTS_SIZE, 0.0f);
        drawCube(OBJECTS_SIZE);
}

void display()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glLoadIdentity();

    // draw the four objects with their respective transformations away from the camera
    glTranslatef(0.0f, 2.5f, -7.0f);

    drawPart(fstRotation);
    drawPart(sndRotation);

    glFlush();
    glutSwapBuffers();
}

// controls[0], controls[1] : increase/decrease rotation arround x
//      ...[2],      ...[3] : ...                                y
//      ...[4],      ...[5] : ...                                z
// the parameter rotation is changed as it is passed as a reference
void manageRotation(unsigned char keyPressed, char *controls, Rotation rot)
{
    // pointer arithmetic trick for avoiding code duplication, to allow to pass controlls as parameters
    // the first cordinate is the one always used and the vectors are shifted instead

    for (; *controls; controls += 2, rot++) // for each coordinate
    {
        if (keyPressed == controls[0])
        {
            *rot += ROTATION_SPEED;
        }
        else if (keyPressed == controls[1])
        {
            *rot -= ROTATION_SPEED;
        }
    }
}

void manageKeyboard(unsigned char keyPressed, int _x, int _y)
{
    if (keyPressed == ESCAPE_KEY)
    { 
        exit(0);
    }
    else
    {
        manageRotation(keyPressed, "qawsed", fstRotation);
        manageRotation(keyPressed, "QAWSED", sndRotation); // hold shift or toggle with bloqmayus
    }
}

int main(int argc, char **argv)
{
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
    initWindow();
    initGL();

    glutDisplayFunc(display);
    glutIdleFunc(display);
    glutReshapeFunc(reshape);
    glutKeyboardFunc(manageKeyboard);

    glutMainLoop();
    return 0;
}
