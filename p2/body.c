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

#define WIDTH  500
#define HEIGHT 700

#define BG_BRIGHTNESS 0.07

#define LIMBS_COUNT     4
#define OBJECTS_SIZE    1
#define ROTATION_SPEED  1
#define TIME_STEP      15

#define SLOW      1.5
#define FAST      3.5
#define VERY_FAST 7.0

typedef GLfloat Vec3[3];

// rotation is defined like a vector
// where the x represent the amount of rotation around the x axis and so on
// i defined rotation in that way instead of using a struct to be able to
// use pointer arithmetic to change values instead of having duplicated code
typedef Vec3 Rotation;

typedef struct
{
    Vec3 pos;
    Rotation fstRotation, sndRotation;
} LimbTransform;

typedef struct
{
    int wired;
    int autoRotate;
    float animationSpeed;
} Config;

typedef struct
{
    int x, y;
    int shift, alt;
} MouseClick;




int currentLimb = 0;

LimbTransform limbs[LIMBS_COUNT] =
{
    {
        {-2.5f, 2.5f, 0.0f},
        {0.0f, 0.0f, -15.0f},
        {0.0f, 0.0f, 15.0f}
    },
    {
        {2.5f, 2.5f, 0.0f},
        {0.0f, 0.0f, 15.0f},
        {0.0f, 0.0f, -15.0f}
    },
    {
        {-1.5f, -3.5f, 0.0f},
        {0.0f, 0.0f, 0.0f},
        {0.0f, 0.0f, 0.0f}
    },
    {
        {1.5f, -3.5f, 0.0f},
        {0.0f, 0.0f, 0.0f},
        {0.0f, 0.0f, 0.0f}
    }
};

Config config = {0, 0, SLOW};

MouseClick click = {0, 0, 0, 0};




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
    glViewport(0, 0, (GLsizei)width, (GLsizei)height);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();

    // with a FOV of 45 instead of 60 shapes appear less distorted
    gluPerspective(45.0f, (GLfloat)width / (GLfloat)height, 1.0f, 100.0f);
    glMatrixMode(GL_MODELVIEW);
}

void drawCube(float size)
{
    if (!config.wired)
    {
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
    }
    else
    {
        glColor3f(1.0f, 0.71f, 0.31f);
        glutWireCube(size);
    }
}

void drawSphere(double diameter)
{
    glColor3f(0.2f, 0.67f, 1.0f);

    if (config.wired)
    {
        glutWireSphere(diameter / 2, 10, 5);
    }
    else
    {
        glutSolidSphere(diameter / 2, 40, 30);
    }
}

void applyRotation(Rotation rot)
{
    glRotatef(rot[0], 1.0f, 0.0f, 0.0f);    
    glRotatef(rot[1], 0.0f, 1.0f, 0.0f);
    glRotatef(rot[2], 0.0f, 0.0f, 1.0f);
}

void drawBody()
{
    glPushMatrix();
    glTranslatef(0.0f, 4.5f, 0.0f);
    drawSphere(OBJECTS_SIZE * 3);
    glPopMatrix();

    glPushMatrix();
    glScalef(4.0f, 6.0f, 1.0f);
    drawCube(OBJECTS_SIZE);
    glPopMatrix();
}

void drawPart(GLfloat x, GLfloat y, GLfloat z, Rotation rot)
{
    glTranslatef(x, y, z);
    drawSphere(OBJECTS_SIZE);
    
    applyRotation(rot);
    glTranslatef(0.0f, -OBJECTS_SIZE * 1.5f, 0.0f);

    glPushMatrix();
    glScalef(1.0, 2.0, 1.0);
    drawCube(OBJECTS_SIZE);
    glPopMatrix();
}

void drawLimb(LimbTransform limb)
{
    glPushMatrix();
    
    drawPart(limb.pos[0], limb.pos[1], limb.pos[2], limb.fstRotation);
    drawPart(0.0, -OBJECTS_SIZE * 1.5f, 0.0, limb.sndRotation);
    
    glPopMatrix();
}

void display()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glLoadIdentity();

    // draw all objects with their respective transformations away from the camera
    glTranslatef(0.0, 1.5, -22.0);

    drawBody();
    for (int i = 0; i < LIMBS_COUNT; i++)
    {
        drawLimb(limbs[i]);
    }

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
    switch (keyPressed)
    {
    case ESCAPE_KEY:
        exit(0);
        break;

    default:
        if (keyPressed >= '1' && keyPressed <= '4')
        {
            currentLimb = keyPressed - '1';
        }
        else
        {
            manageRotation(keyPressed, "qawsed", limbs[currentLimb].fstRotation);
            manageRotation(keyPressed, "QAWSED", limbs[currentLimb].sndRotation); // hold shift or toggle with bloqmayus
        }
        break;
    }
}

void modelMenu(int option)
{
    config.wired = option;
}

void animationMenu(int option)
{
    config.autoRotate = option;
}

void speedMenu(int option)
{
    switch (option)
    {
    case 0:
        config.animationSpeed = SLOW;
        break;

    case 1:
        config.animationSpeed = FAST;
        break;

    case 2:
        config.animationSpeed = VERY_FAST;
        break;
    }
}

// this function overwrites allmos all values so if you want to change them
// manually disable autorotation first
void timerRotation(int step)
{
    // time since the program started
    float time = config.animationSpeed * (float) step / (float)TIME_STEP;

    if (config.autoRotate)
    { 
        // arm

        limbs[0].fstRotation[0] = 50.0f * sin(time * 0.5f) + 10.0f;
        limbs[1].fstRotation[0] = 50.0f * sin(time * 0.5f + 3.1415f) + 10.0f;

        limbs[0].fstRotation[1] = 0.0f;
        limbs[1].fstRotation[1] = 0.0f;

        limbs[0].fstRotation[2] = -15.0f;
        limbs[1].fstRotation[2] = 15.0f;

        // forearm

        for (int i = 0; i < 2; i++)
        {
            limbs[i].sndRotation[0] = -80.0f;
            limbs[i].sndRotation[1] = 0.0f;
        }
        limbs[0].sndRotation[2] = 15.0f;
        limbs[1].sndRotation[2] = -15.0f;

        // legs

        limbs[2].fstRotation[0] = 40.0f * sin(time * 0.5f + 3.1415f) - 10.0f;
        limbs[3].fstRotation[0] = 40.0f * sin(time * 0.5f) - 10.0f;

        limbs[2].fstRotation[1] = 0.0f;
        limbs[3].fstRotation[1] = 0.0f;
        limbs[2].fstRotation[2] = 0.0f;
        limbs[3].fstRotation[2] = 0.0f;

        // knees

        limbs[2].sndRotation[0] = 55.0f * fabs(sin(time * 0.5f));
        limbs[3].sndRotation[0] = 55.0f * fabs(sin(time * 0.5f + 3.1f));
        limbs[2].sndRotation[1] = 0.0f;
        limbs[3].sndRotation[1] = 0.0f;
        limbs[2].sndRotation[2] = 0.0f;
        limbs[3].sndRotation[2] = 0.0f;
    }

    glutTimerFunc(TIME_STEP, timerRotation, step + 1);
}

void createMenus()
{
    int _modelMenu = glutCreateMenu(modelMenu);
    glutAddMenuEntry("Solid", 0);
    glutAddMenuEntry("Wired", 1);

    int _animationMenu = glutCreateMenu(animationMenu);
    glutAddMenuEntry("Disabled", 0);
    glutAddMenuEntry("Enabled", 1);

    int _speedMenu = glutCreateMenu(speedMenu);
    glutAddMenuEntry("Slow", 0);
    glutAddMenuEntry("Fast", 1);
    glutAddMenuEntry("Very fast", 2);

    glutCreateMenu(NULL);
    glutAddSubMenu("Model", _modelMenu);
    glutAddSubMenu("Automatic movement", _animationMenu);
    glutAddSubMenu("Movement speed", _speedMenu);
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
        click.shift = modifiers & GLUT_ACTIVE_SHIFT;
    }
}

void motion(int x, int y)
{
    float incX = (float)(x - click.x) * 0.2f;
    float incY = (float)(y - click.y) * 0.2f;

    LimbTransform *limb = &limbs[currentLimb];
    Rotation *rot = click.shift ? &limb->sndRotation : &limb->fstRotation;

    (*rot)[0] += incY;
    (*rot)[click.alt ? 1 : 2] += incX;

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

    glutMainLoop();
    return 0;
}
