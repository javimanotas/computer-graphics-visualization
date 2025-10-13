// notas para la correccion:
// hay que pulsar el alt antes del click si se desea rotar todo
// se ha limitado la inclinación de la plataforma verde para prohibir rotaciones que "ropan" la forma

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
    GLfloat inclination;
    Rotation globalRot;
    Rotation tilt;
    int perspective;
    int w, h;
    Color palette;
    GLfloat baseSpeed;
    int animation;
} Config;

typedef struct
{
    int x, y;
    int alt;
} MouseClick;



Config config = {0.0f, {0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 0.0f}, 1, 0, 0, {1.0f, 1.0f, 0.5f}, 30.0f, 1};

MouseClick click = {0, 0, 0};

void initWindow()
{
    glutInitWindowPosition(ORIGIN_X, ORIGIN_Y);
    glutInitWindowSize(WIDTH, HEIGHT);
    glutCreateWindow("Exam");
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
        glOrtho(-8, 8, -8, 8, 1, 100);
    }

    glMatrixMode(GL_MODELVIEW);
}

void drawCube(GLfloat r, GLfloat g, GLfloat b)
{
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
            glColor3f(1.0f, 0.7f, 0.5f);
            glutSolidCone(3.0, 2.0, 30, 30);    
            gluCylinder(quadratic, 3.0, 3.0, 1.0, 30, 30);   
        glPopMatrix();

        glColor3f(0.5f, 0.7f, 1.0f);
        glutSolidCone(3.0, 2.0, 20, 20);
    glPopMatrix();

    gluDeleteQuadric(quadratic);
}

void drawArm()
{
    glPushMatrix();
        glRotatef(config.inclination, 0.0f, 0.0f, 1.0f);
        glTranslatef(OBJECTS_SIZE * 2.0f, 0.0f, 0.0f);

        glPushMatrix();
            glScalef(4.0f, 0.3f, 0.3f);
            drawCube(0.5f, 0.5f, 0.5f);
        glPopMatrix();

        glTranslatef(OBJECTS_SIZE * 2.0f, 0.0f, 0.0f);
        glRotatef(config.inclination, 0.0f, 0.0f, -1.0f);
        glRotatef(90.0f, 1.0f, 0.0f, 0.0f);
        glColor3f(0.4f, 1.0f, 0.4f);
        glTranslatef(0.0f, 0.0f, -0.8f);
        applyRotation(config.tilt);
        glutSolidCone(2.0, 1.0, 20, 20);

        // squares
        glScalef(0.5f, 0.5f, 0.5f);

        glTranslatef(1.0f, 1.0f, -0.5f);
        drawCube(config.palette[0], config.palette[1], config.palette[2]);
        
        glTranslatef(-2.0f, 0.0f, 0.0f);
        drawCube(config.palette[0], config.palette[1], config.palette[2]);
        
        glTranslatef(0.0f, -2.0f, 0.0f);
        drawCube(config.palette[0], config.palette[1], config.palette[2]);
        
        glTranslatef(2.0f, 0.0f, 0.0f);
        drawCube(config.palette[0], config.palette[1], config.palette[2]);
        
    glPopMatrix();
}

void display()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glLoadIdentity();

    // draw all objects with their respective transformations away from the camera
    glTranslatef(0.0, -2.25, -18.0);

    applyRotation(config.globalRot);
    drawBase();
    glTranslatef(0.0f, 2.0f, 0.0f);
    drawArm();

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
        config.baseSpeed *= -1;
        break;

    case 's':
        config.animation = 1 - config.animation;
        break;
    }
}

int sign(int x)
{
    return x > 0 ? 1 : -1;
}

void speedMenu(int option)
{
    switch (option)
    {
    case 0:
        config.baseSpeed = 30.0f * sign(config.baseSpeed);
        break;
    
    case 1:
        config.baseSpeed = 60.0f * sign(config.baseSpeed);
        break;

    case 2:
        config.baseSpeed = 90.0f * sign(config.baseSpeed);
        break;
    }
}

void colorMenu(int option)
{
    switch (option)
    {
    case 0:
        
        config.palette[0] = 1.0f; 
        config.palette[1] = 1.0f;
        config.palette[2] = 0.5f;
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

    case 3:
        config.palette[0] = 0.9f;
        config.palette[1] = 0.5f; 
        config.palette[2] = 0.31f;
        break;
    }
}

void exitMenu(int option)
{
    exit(0);
}

// aux function i made for remapping a [-1, 1] sine wave into [min, max]
// very usefull for animations depending on time
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
    config.globalRot[1] += 1.0f / (float)TIME_STEP * config.baseSpeed * config.animation;
    config.tilt[2] += 1.0f / (float)TIME_STEP * abs(config.baseSpeed) * 3.0f * config.animation;
    glutTimerFunc(TIME_STEP, timerRotation, step);
}

void timerPosition(int step)
{
    float time = (float) step / (float)TIME_STEP;
    config.inclination = lerpSine(-cos(time * 0.8f), 0.0f, 45.0f);

    glutTimerFunc(TIME_STEP, timerPosition, step + config.animation);
}

void createMenus()
{
    int _speedMenu = glutCreateMenu(speedMenu);
    glutAddMenuEntry("Low", 0);
    glutAddMenuEntry("Medium", 1);
    glutAddMenuEntry("High", 2);

    int _colorMenu = glutCreateMenu(colorMenu);
    glutAddMenuEntry("Yellow", 0);
    glutAddMenuEntry("Cyan", 1);
    glutAddMenuEntry("Magenta", 2);
    glutAddMenuEntry("Orange", 3);

    int _exitMenu = glutCreateMenu(exitMenu);
    glutAddMenuEntry("Confirm exit", 0);

    glutCreateMenu(NULL);
    glutAddSubMenu("Speed", _speedMenu);
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
    float incX = (float)(x - click.x) * 0.15f;
    float incY = (float)(y - click.y) * 0.15f;

    if (click.alt)
    {
        config.globalRot[0] += incY;
    }
    else
    {
        config.tilt[1] += incX;
        config.tilt[0] += incY;

        if (config.tilt[1] < -30.0f) config.tilt[1] = -30.0f;
        if (config.tilt[1] > 30.0f) config.tilt[1] = 30.0f;
        if (config.tilt[0] < -15.0f) config.tilt[0] = -15.0f;
        if (config.tilt[0] > 15.0f) config.tilt[0] = 15.0f;
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
