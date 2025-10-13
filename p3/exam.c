// This exercise provides far more options than the one from last week so i decided
// to list the controls all of them here:

// select the limb you want to use with 1 or 2 for the arms and 3 or 4 for the legs

// rotate that limb with qawsed and hold shift to rotate the second part

// left click and drag to rotate arround x and z axis
// alt + left click and drag to rotate arround x and y axis
// shift + left click and drag to rotate the second part
// note 1: of course you can do a alt + shift + left click
// note 2: the key modifiers must be pressed BEFORE clicking or it won't be detected

// open the menu with right click

#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glut.h>
#include <math.h>

#define ESCAPE_KEY 27

#define ORIGIN_X 100
#define ORIGIN_Y 100

#define WIDTH  700
#define HEIGHT 700

#define BG_BRIGHTNESS 0.07

#define OBJECTS_SIZE    1
#define ROTATION_SPEED  1
#define TIME_STEP      15

typedef GLfloat Vec3[3];

// rotation is defined like a vector
// where the x represent the amount of rotation around the x axis and so on
// i defined rotation in that way instead of using a struct to be able to
// use pointer arithmetic to change values instead of having duplicated code
typedef Vec3 Rotation;

typedef Vec3 Color;

typedef struct
{
    GLfloat globalRotSpeed;
    int perspective;
    GLfloat maxRot;
    int armAnimation;
    int w, h;
    Color palette;
} Config;

typedef struct
{
    int x, y;
    int alt;
} MouseClick;



Config config = {20.0f, 1, 45.0f, 1, 0, 0, {0.9f, 0.71f, 0.31f}};
MouseClick click = {0, 0, 0};
GLfloat oddRot = 20.0f, evenRot = 0.0f;
Rotation globalRot = {0.0f, 0.0f, 0.0f};


void initWindow()
{
    glutInitWindowPosition(ORIGIN_X, ORIGIN_Y);
    glutInitWindowSize(WIDTH, HEIGHT);
    glutCreateWindow("Body");
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
    config.w = width;
    config.h = height;

    glViewport(0, 0, (GLsizei)width, (GLsizei)height);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();

    
    if (config.perspective)
    {
        // with a FOV of 45 instead of 60 shapes appear less distorted
        gluPerspective(45.0f, (GLfloat)width / (GLfloat)height, 1.0f, 100.0f);
    }
    else       
    {
        glOrtho(-5, 5, -5, 5, 1, 100);
    }

    glMatrixMode(GL_MODELVIEW);
}

void drawCube()
{
    GLfloat r = config.palette[0], g = config.palette[1], b = config.palette[2];

    // One color is used for the faces perpendicular to each axis

    GLfloat x = (GLfloat)OBJECTS_SIZE / 2;
    glBegin(GL_QUADS);
    
    // Z faces

    glColor3f(r * 1.2f, g * 1.2f, b * 1.2f);

        glVertex3f( x, -x, -x);
        glVertex3f(-x, -x, -x);
        glVertex3f(-x,  x, -x);
        glVertex3f( x,  x, -x);

        glVertex3f(-x, -x,  x);
        glVertex3f( x, -x,  x);
        glVertex3f( x,  x,  x);
        glVertex3f(-x,  x,  x);

    // X faces

    glColor3f(r, g, b);

        glVertex3f(-x, -x, -x);
        glVertex3f(-x, -x,  x);
        glVertex3f(-x,  x,  x);
        glVertex3f(-x,  x, -x);

        glVertex3f( x, -x,  x);
        glVertex3f( x, -x, -x);
        glVertex3f( x,  x, -x);
        glVertex3f( x,  x,  x);
    
    // Y faces

    glColor3f(r * 0.75f, g * 0.75f, b * 0.75f);

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
}

void applyRotation(Rotation rot)
{
    glRotatef(rot[0], 1.0f, 0.0f, 0.0f);    
    glRotatef(rot[1], 0.0f, 1.0f, 0.0f);
    glRotatef(rot[2], 0.0f, 0.0f, 1.0f);
}

void drawBase()
{
    GLUquadricObj *quadratic = gluNewQuadric();

    glPushMatrix();
        glRotatef(90.0f, -1.0f, 0.0f, 0.0f);
        
        glPushMatrix();
            glTranslatef(0.0f, 0.0f, -1.0f);
            glColor3f(0.0f, 0.4f, 1.0f);
            glutSolidCone(2.0, 2.0, 20, 20);    
            gluCylinder(quadratic, 2.0, 2.0, 1.0, 20, 20);   
        glPopMatrix();

        glColor3f(1.0f, 0.4f, 0.0f);
        glutSolidCone(2.0, 2.0, 20, 20);
    glPopMatrix();

    gluDeleteQuadric(quadratic);
}

void drawArm(int i)
{
    glPushMatrix();
        glRotatef(i % 2 == 0 ? evenRot : oddRot, 0.0f, 0.0f, 1.0f);
        glTranslatef(OBJECTS_SIZE * 2.0f, 0.0f, 0.0f);

        glPushMatrix();
            glScalef(4.0f, 0.3f, 0.3f);
            drawCube();
        glPopMatrix();

        glTranslatef(OBJECTS_SIZE * 2.0f, 0.0f, 0.0f);
        glRotatef(i % 2 == 0 ? evenRot : oddRot, 0.0f, 0.0f, -1.0f);
        drawCube();
    glPopMatrix();
}

void display()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glLoadIdentity();

    // draw all objects with their respective transformations away from the camera
    glTranslatef(0.0, -1.5, -18.0);

    applyRotation(globalRot);
    drawBase();
    glTranslatef(0.0f, 2.0f, 0.0f);
    
    for (int i = 0; i < 6; i++)
    {
        drawArm(i);
        glRotatef(60.0f, 0.0f, 1.0f, 0.0f);
    }

    glFlush();
    glutSwapBuffers();
}

void manageKeyboard(unsigned char keyPressed, int _x, int _y)
{
    switch (keyPressed)
    {
    case ESCAPE_KEY:
        exit(0);
        break;
    
    case 'p':
        config.perspective = 1;
        reshape(config.w, config.h);
        break;

    case 'o':
        config.perspective = 0;
        reshape(config.w, config.h);
        break;

    case 'r':
        config.globalRotSpeed *= -1;
        break;

    case 's':
        config.armAnimation = 1 - config.armAnimation;
        break;
    }
}

void heightMenu(int option)
{
    switch (option)
    {
    case 0:
        config.maxRot = 30.0f;
        break;
    
    case 1:
        config.maxRot = 45.0f;
        break;

    case 2:
        config.maxRot = 70.0f;
        break;
    }
}

void colorMenu(int option)
{
    switch (option)
    {
    case 0:
        
        config.palette[0] = 0.9f; 
        config.palette[1] = 0.71f;
        config.palette[2] = 0.31f;
        break;
    
    case 1:
        config.palette[0] = 0.31f;
        config.palette[1] = 0.9f; 
        config.palette[2] = 0.71f;
        break;
    
    case 2:
        config.palette[0] = 0.9f; 
        config.palette[1] = 0.31f;
        config.palette[2] = 0.71f;
        break;
    }
}

void exitMenu(int option)
{
    exit(0);
}

float lerpSine(float in, float min, float max)
{
    in += 1.0f;
    in /= 2.0f;
    in *= max - min;
    in += min;
    return in;
}

void timerRotation(int step)
{    
    globalRot[1] += 1.0f / (float)TIME_STEP * config.globalRotSpeed;
    glutTimerFunc(TIME_STEP, timerRotation, step + 1);
}

void timerPosition(int step)
{
    float time = (float) step / (float)TIME_STEP;
    oddRot = lerpSine(sin(time * 0.2f), -20.0f, config.maxRot);
    evenRot = lerpSine(sin(time * 0.2f + 3.1415), -20.0f, config.maxRot);

    glutTimerFunc(TIME_STEP, timerPosition, step + config.armAnimation);
}

void createMenus()
{
    int _heightMenu = glutCreateMenu(heightMenu);
    glutAddMenuEntry("Low", 0);
    glutAddMenuEntry("Medium", 1);
    glutAddMenuEntry("High", 2);

    int _colorMenu = glutCreateMenu(colorMenu);
    glutAddMenuEntry("A", 0);
    glutAddMenuEntry("B", 1);
    glutAddMenuEntry("C", 2);

    int _exitMenu = glutCreateMenu(exitMenu);
    glutAddMenuEntry("Confirm exit", 0);

    glutCreateMenu(NULL);
    glutAddSubMenu("Height", _heightMenu);
    glutAddSubMenu("Color", _colorMenu);
    glutAddSubMenu("Exit", _exitMenu);
    glutAttachMenu(GLUT_RIGHT_BUTTON);
}

void mouse(int button, int state, int x, int y)
{
    if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN)
    {
        click.x = x;
        click.y = y;
        int modifiers = glutGetModifiers();
        click.alt = modifiers & GLUT_ACTIVE_ALT;
    }
}

void motion(int x, int y)
{
    float incX = (float)(x - click.x);
    float incY = (float)(y - click.y) * 0.2f;

    if (click.alt)
    {
        globalRot[1] += incX;
    }
    else
    {
        globalRot[0] += incY;
    }

    click.x = x;
    click.y = y;
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
    glutMouseFunc(mouse);
    glutMotionFunc(motion);

    createMenus();
    glutTimerFunc(TIME_STEP, timerRotation, 0);
    glutTimerFunc(TIME_STEP, timerPosition, 0);
    glutMainLoop();
    return 0;
}
