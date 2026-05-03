/*
 * Interior Design Room Scene with Smooth Natural Cat Animation
 *
 * Controls:
 *   ESC / q    - Exit
 *   Arrow keys - Rotate normal view
 *   + / -      - Zoom in/out in normal view
 *   F          - Fan ON/OFF
 *   C          - Toggle CCTV camera mode
 *   Z / X      - CCTV zoom in / zoom out
 */

#ifdef __APPLE__
#define FREEGLUT_STATIC
#include <GL/freeglut.h>
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif

#include <stdlib.h>
#include <math.h>

 /* ---------------- Camera ---------------- */
static float angleX = 20.0f;
static float angleY = 30.0f;
static float zoomLevel = -18.0f;

/* ---------------- Scene animation ---------------- */
static float fanRotation = 0.0f;
static float clockAngle = 0.0f;
static int   fanOn = 1;

/* ---------------- CCTV ---------------- */
static int   cctvMode = 0;
static float cctvDistance = 10.0f;

/* ---------------- Cat state ---------------- */
static int   catRunning = 0;
static int   catSitting = 1;
static float catPosX = -5.8f;
static float catPosZ = -2.0f;
static float catDirection = 1.0f;

static float catAnimTime = 0.0f;
static float catStateTime = 0.0f;
static int   lastUpdateTimeMs = -1;
static const float catWalkDuration = 8.0f;
static const float catSitDuration = 4.0f;

static float catBaseSpeed = 0.018f;
static float catCurrentSpeed = 0.0f;

static int   catJumping = 0;
static float catJumpTime = 0.0f;

static int lightOn = 1;



/* =========================================================
   Helper
   ========================================================= */
void drawCube(float width, float height, float depth)
{
    glPushMatrix();
    glScalef(width, height, depth);
    glutSolidCube(1.0);
    glPopMatrix();
}

/* =========================================================
   Room
   ========================================================= */
void drawFloor()
{
    glColor3f(0.6f, 0.4f, 0.2f);
    glPushMatrix();
    glTranslatef(0.0f, -2.0f, 0.0f);
    drawCube(16.0f, 0.1f, 16.0f);
    glPopMatrix();
}

void drawWalls()
{
    glColor3f(0.9f, 0.9f, 0.85f);

    glPushMatrix();
    glTranslatef(0.0f, 2.0f, -8.0f);
    drawCube(16.0f, 8.0f, 0.2f);
    glPopMatrix();

    glPushMatrix();
    glTranslatef(-8.0f, 2.0f, 0.0f);
    drawCube(0.2f, 8.0f, 16.0f);
    glPopMatrix();

    glPushMatrix();
    glTranslatef(8.0f, 2.0f, 0.0f);
    drawCube(0.2f, 8.0f, 16.0f);
    glPopMatrix();
}

void drawCeiling()
{
    glColor3f(0.95f, 0.95f, 0.95f);
    glPushMatrix();
    glTranslatef(0.0f, 6.0f, 0.0f);
    drawCube(16.0f, 0.1f, 16.0f);
    glPopMatrix();
}

void drawTable()
{
    glColor3f(0.4f, 0.2f, 0.1f);

    glPushMatrix();
    glTranslatef(0.0f, -0.5f, -2.0f);
    drawCube(3.0f, 0.1f, 1.5f);
    glPopMatrix();

    float legPositions[4][2] = {
        {-1.4f, -0.7f}, {1.4f, -0.7f}, {-1.4f, 0.7f}, {1.4f, 0.7f}
    };
    for (int i = 0; i < 4; i++) {
        glPushMatrix();
        glTranslatef(legPositions[i][0], -1.2f, -2.0f + legPositions[i][1]);
        drawCube(0.1f, 1.4f, 0.1f);
        glPopMatrix();
    }
}

void drawChair(float x, float z, float rotation)
{
    glPushMatrix();
    glTranslatef(x, 0.0f, z);
    glRotatef(rotation, 0.0f, 1.0f, 0.0f);

    glColor3f(0.5f, 0.3f, 0.15f);

    glPushMatrix();
    glTranslatef(0.0f, -1.0f, 0.0f);
    drawCube(0.8f, 0.1f, 0.8f);
    glPopMatrix();

    glPushMatrix();
    glTranslatef(0.0f, -0.5f, -0.35f);
    drawCube(0.8f, 1.0f, 0.1f);
    glPopMatrix();

    float legPos[4][2] = {
        {-0.35f, -0.35f}, {0.35f, -0.35f}, {-0.35f, 0.35f}, {0.35f, 0.35f}
    };
    for (int i = 0; i < 4; i++) {
        glPushMatrix();
        glTranslatef(legPos[i][0], -1.5f, legPos[i][1]);
        drawCube(0.08f, 1.0f, 0.08f);
        glPopMatrix();
    }

    glPopMatrix();
}

void drawCeilingFan()
{
    glPushMatrix();
    glTranslatef(0.0f, 5.5f, 0.0f);

    glColor3f(0.3f, 0.3f, 0.3f);
    glPushMatrix();
    drawCube(0.2f, 0.3f, 0.2f);
    glPopMatrix();

    glRotatef(fanRotation, 0.0f, 1.0f, 0.0f);
    glColor3f(0.7f, 0.7f, 0.7f);

    for (int i = 0; i < 4; i++) {
        glPushMatrix();
        glRotatef(i * 90.0f, 0.0f, 1.0f, 0.0f);
        glTranslatef(0.6f, 0.0f, 0.0f);
        drawCube(1.0f, 0.05f, 0.2f);
        glPopMatrix();
    }

    glPopMatrix();
}

void drawWallClock()
{
    glPushMatrix();
    glTranslatef(-5.0f, 2.5f, -7.9f);

    glColor3f(1.0f, 1.0f, 1.0f);
    glPushMatrix();
    glScalef(1.0f, 1.0f, 0.15f);
    glutSolidSphere(0.4, 20, 20);
    glPopMatrix();

    glColor3f(0.2f, 0.2f, 0.2f);
    glPushMatrix();
    glScalef(1.0f, 1.0f, 0.15f);
    glutWireSphere(0.42, 20, 20);
    glPopMatrix();

    glColor3f(0.0f, 0.0f, 0.0f);
    glPushMatrix();
    glTranslatef(0.0f, 0.0f, 0.06f);
    glRotatef(clockAngle, 0.0f, 0.0f, 1.0f);
    drawCube(0.02f, 0.3f, 0.02f);
    glPopMatrix();

    glPopMatrix();
}

void drawPictureFrame(float x, float y)
{
    glPushMatrix();
    glTranslatef(x, y, -7.9f);

    glColor3f(0.6f, 0.4f, 0.2f);
    drawCube(1.0f, 1.2f, 0.05f);

    glColor3f(0.3f, 0.5f, 0.8f);
    glPushMatrix();
    glTranslatef(0.0f, 0.0f, 0.03f);
    drawCube(0.85f, 1.0f, 0.02f);
    glPopMatrix();

    glPopMatrix();
}


void drawLamp()
{
    glPushMatrix();
    glTranslatef(1.0f, -0.4f, -2.0f);

    glColor3f(0.3f, 0.3f, 0.3f);
    glPushMatrix();
    glutSolidSphere(0.15, 20, 20);
    glPopMatrix();

    glPushMatrix();
    glTranslatef(0.0f, 0.3f, 0.0f);
    drawCube(0.05f, 0.6f, 0.05f);
    glPopMatrix();

    glColor3f(0.9f, 0.8f, 0.6f);
    glPushMatrix();
    glTranslatef(0.0f, 0.6f, 0.0f);
    glRotatef(-90, 1.0f, 0.0f, 0.0f);
    glutSolidCone(0.25, 0.3, 20, 20);
    glPopMatrix();

    /* 🔥 ADD HERE */
    glColor3f(1.0f, 0.9f, 0.6f);
    glPushMatrix();
    glTranslatef(0.0f, 0.6f, 0.0f);
    glutSolidSphere(0.08, 20, 20);
    glPopMatrix();

    glPopMatrix();
}

void drawOpenBook()
{
    glPushMatrix();
    glTranslatef(-0.5f, -0.4f, -1.8f);
    glRotatef(25, 0.0f, 1.0f, 0.0f);

    glColor3f(0.95f, 0.92f, 0.85f);
    glPushMatrix();
    glTranslatef(-0.15f, 0.01f, 0.0f);
    glRotatef(-15, 0.0f, 0.0f, 1.0f);
    drawCube(0.3f, 0.02f, 0.4f);
    glPopMatrix();

    glPushMatrix();
    glTranslatef(0.15f, 0.01f, 0.0f);
    glRotatef(15, 0.0f, 0.0f, 1.0f);
    drawCube(0.3f, 0.02f, 0.4f);
    glPopMatrix();

    glColor3f(0.6f, 0.3f, 0.2f);
    glPushMatrix();
    glTranslatef(0.0f, 0.01f, 0.0f);
    drawCube(0.04f, 0.03f, 0.42f);
    glPopMatrix();

    glPopMatrix();
}

void drawBookStack()
{
    glPushMatrix();
    glTranslatef(-1.2f, -0.45f, -2.3f);

    glColor3f(0.7f, 0.2f, 0.2f);
    glPushMatrix();
    drawCube(0.35f, 0.08f, 0.25f);
    glPopMatrix();

    glColor3f(0.2f, 0.4f, 0.7f);
    glPushMatrix();
    glTranslatef(0.02f, 0.08f, 0.0f);
    drawCube(0.32f, 0.08f, 0.23f);
    glPopMatrix();

    glColor3f(0.3f, 0.6f, 0.3f);
    glPushMatrix();
    glTranslatef(-0.01f, 0.16f, 0.0f);
    glRotatef(8, 0.0f, 1.0f, 0.0f);
    drawCube(0.3f, 0.08f, 0.22f);
    glPopMatrix();

    glPopMatrix();
}

void drawBed()
{
    glPushMatrix();
    glTranslatef(5.5f, -1.0f, -5.0f);
    glRotatef(-90, 0.0f, 1.0f, 0.0f);

    glColor3f(0.9f, 0.9f, 0.95f);
    glPushMatrix();
    drawCube(3.0f, 0.3f, 4.5f);
    glPopMatrix();

    glColor3f(0.4f, 0.25f, 0.15f);
    glPushMatrix();
    glTranslatef(0.0f, -0.25f, 0.0f);
    drawCube(3.2f, 0.2f, 4.7f);
    glPopMatrix();

    float legPos[4][2] = {
        {-1.5f, -2.2f}, {1.5f, -2.2f}, {-1.5f, 2.2f}, {1.5f, 2.2f}
    };
    glColor3f(0.35f, 0.2f, 0.1f);
    for (int i = 0; i < 4; i++) {
        glPushMatrix();
        glTranslatef(legPos[i][0], -0.5f, legPos[i][1]);
        drawCube(0.15f, 0.5f, 0.15f);
        glPopMatrix();
    }

    glColor3f(0.45f, 0.3f, 0.2f);
    glPushMatrix();
    glTranslatef(0.0f, 0.7f, -2.3f);
    drawCube(3.2f, 1.7f, 0.15f);
    glPopMatrix();

    glColor3f(0.95f, 0.95f, 1.0f);
    glPushMatrix();
    glTranslatef(-0.7f, 0.25f, -1.7f);
    drawCube(0.8f, 0.2f, 0.5f);
    glPopMatrix();

    glPushMatrix();
    glTranslatef(0.7f, 0.25f, -1.7f);
    drawCube(0.8f, 0.2f, 0.5f);
    glPopMatrix();

    glColor3f(0.3f, 0.4f, 0.6f);
    glPushMatrix();
    glTranslatef(0.0f, 0.18f, 0.5f);
    drawCube(2.8f, 0.15f, 3.0f);
    glPopMatrix();

    glPopMatrix();
}

void drawBookshelf()
{
    glPushMatrix();
    glTranslatef(-7.8f, 0.0f, -3.0f);
    glRotatef(90, 0.0f, 1.0f, 0.0f);

    glColor3f(0.35f, 0.2f, 0.1f);
    drawCube(2.5f, 4.0f, 0.6f);

    glColor3f(0.3f, 0.18f, 0.08f);
    for (int i = 0; i < 5; i++) {
        glPushMatrix();
        glTranslatef(0.0f, -1.7f + i * 0.85f, 0.0f);
        drawCube(2.4f, 0.06f, 0.58f);
        glPopMatrix();
    }

    for (int shelf = 0; shelf < 4; shelf++) {
        for (int book = 0; book < 5; book++) {
            float colors[5][3] = {
                {0.8f, 0.2f, 0.2f},
                {0.2f, 0.5f, 0.8f},
                {0.3f, 0.7f, 0.3f},
                {0.7f, 0.5f, 0.2f},
                {0.6f, 0.2f, 0.6f}
            };
            glColor3f(colors[book][0], colors[book][1], colors[book][2]);
            glPushMatrix();
            glTranslatef(-1.0f + book * 0.45f, -1.3f + shelf * 0.85f, 0.28f);
            drawCube(0.3f, 0.7f, 0.1f);
            glPopMatrix();
        }
    }

    glPopMatrix();
}

void drawPlant(float x, float z)
{
    glPushMatrix();
    glTranslatef(x, -1.5f, z);

    glColor3f(0.7f, 0.3f, 0.2f);
    glPushMatrix();
    glScalef(1.0f, 0.8f, 1.0f);
    glutSolidCone(0.25, 0.4, 20, 20);
    glPopMatrix();

    glColor3f(0.2f, 0.5f, 0.2f);
    glPushMatrix();
    glTranslatef(0.0f, 0.3f, 0.0f);
    drawCube(0.05f, 0.6f, 0.05f);
    glPopMatrix();

    glColor3f(0.1f, 0.6f, 0.1f);
    for (int i = 0; i < 3; i++) {
        glPushMatrix();
        glTranslatef(0.15f * cos(i * 2.0f), 0.5f + i * 0.15f, 0.15f * sin(i * 2.0f));
        glutSolidSphere(0.15, 10, 10);
        glPopMatrix();
    }

    glPopMatrix();
}

void drawRug()
{
    glPushMatrix();
    glTranslatef(0.0f, -1.95f, -2.0f);

    glColor3f(0.6f, 0.2f, 0.2f);
    drawCube(4.5f, 0.02f, 3.0f);

    glColor3f(0.8f, 0.6f, 0.2f);
    drawCube(4.4f, 0.03f, 2.9f);

    glPopMatrix();
}

void drawWindow(float x, float y, float z)
{
    glPushMatrix();
    glTranslatef(x, y, z);

    glColor3f(0.3f, 0.25f, 0.2f);
    drawCube(1.5f, 1.8f, 0.1f);

    glColor3f(0.7f, 0.85f, 0.95f);

    glPushMatrix();
    glTranslatef(-0.38f, 0.45f, 0.06f);
    drawCube(0.65f, 0.8f, 0.02f);
    glPopMatrix();

    glPushMatrix();
    glTranslatef(0.38f, 0.45f, 0.06f);
    drawCube(0.65f, 0.8f, 0.02f);
    glPopMatrix();

    glPushMatrix();
    glTranslatef(-0.38f, -0.45f, 0.06f);
    drawCube(0.65f, 0.8f, 0.02f);
    glPopMatrix();

    glPushMatrix();
    glTranslatef(0.38f, -0.45f, 0.06f);
    drawCube(0.65f, 0.8f, 0.02f);
    glPopMatrix();

    glPopMatrix();
}

void drawSideTable(float x, float z)
{
    glPushMatrix();
    glTranslatef(x, -1.2f, z);

    glColor3f(0.5f, 0.35f, 0.2f);

    glPushMatrix();
    drawCube(0.8f, 0.08f, 0.8f);
    glPopMatrix();

    float legPos[4][2] = {
        {-0.35f, -0.35f}, {0.35f, -0.35f}, {-0.35f, 0.35f}, {0.35f, 0.35f}
    };
    for (int i = 0; i < 4; i++) {
        glPushMatrix();
        glTranslatef(legPos[i][0], -0.4f, legPos[i][1]);
        drawCube(0.08f, 0.8f, 0.08f);
        glPopMatrix();
    }

    glColor3f(0.2f, 0.3f, 0.6f);
    glPushMatrix();
    glTranslatef(0.0f, 0.15f, 0.0f);
    glutSolidSphere(0.12, 15, 15);
    glPopMatrix();

    glPopMatrix();
}

void drawLightFixture(float x, float z)
{
    glPushMatrix();
    glTranslatef(x, 5.8f, z);

    glColor3f(0.8f, 0.8f, 0.8f);
    glPushMatrix();
    glutSolidSphere(0.15, 15, 15);
    glPopMatrix();

    glColor3f(0.95f, 0.95f, 0.7f);
    glPushMatrix();
    glTranslatef(0.0f, -0.2f, 0.0f);
    glRotatef(-90, 1.0f, 0.0f, 0.0f);
    glutSolidCone(0.3, 0.4, 20, 20);
    glPopMatrix();

    glPopMatrix();
}

/* =========================================================
   Cat helper
   ========================================================= */
void drawCatLeg(float x, float y, float z, float angleDeg, float height)
{
    glPushMatrix();
    glTranslatef(x, y, z);
    glRotatef(angleDeg, 0.0f, 0.0f, 1.0f);
    glTranslatef(0.0f, -height * 0.5f, 0.0f);
    drawCube(0.10f, height, 0.10f);
    glPopMatrix();
}

/* =========================================================
   Smooth natural cat
   ========================================================= */
void drawCat()
{
    float gait = sinf(catAnimTime * 1.7f);
    float gait2 = sinf(catAnimTime * 1.7f + 3.1415926f);
    float smallWave = sinf(catAnimTime * 0.9f);

    float speedRatio = 0.0f;
    if (catBaseSpeed > 0.0f) {
        speedRatio = catCurrentSpeed / catBaseSpeed;
    }
    if (speedRatio < 0.0f) speedRatio = 0.0f;
    if (speedRatio > 1.2f) speedRatio = 1.2f;

    float bodyBounce = fabs(sinf(catAnimTime * 3.4f)) * 0.05f * speedRatio;
    float headBob = sinf(catAnimTime * 3.4f + 0.5f) * 0.03f * speedRatio;
    float tailSwing = sinf(catAnimTime * 2.0f) * 20.0f * (0.3f + 0.7f * speedRatio);
    float backArch = sinf(catAnimTime * 1.2f) * 4.0f * speedRatio;

    float jumpHeight = 0.0f;
    if (catJumping) {
        jumpHeight = sinf(catJumpTime * 3.1415926f) * 0.65f;
        if (jumpHeight < 0.0f) jumpHeight = 0.0f;
    }

    glPushMatrix();
    glTranslatef(catPosX, -1.33f + bodyBounce + jumpHeight, catPosZ);
    glScalef(0.95f, 0.95f, 0.95f);

    if (catDirection < 0.0f)
        glRotatef(180.0f, 0.0f, 1.0f, 0.0f);

    if (catSitting && !catRunning) {
        float tailIdle = sinf(catAnimTime * 1.4f) * 8.0f;

        glColor3f(0.86f, 0.56f, 0.22f);
        glPushMatrix();
        glTranslatef(-0.10f, -0.05f, 0.0f);
        glScalef(1.15f, 0.90f, 0.78f);
        glutSolidSphere(0.40, 28, 28);
        glPopMatrix();

        glColor3f(0.95f, 0.90f, 0.82f);
        glPushMatrix();
        glTranslatef(0.08f, -0.15f, 0.0f);
        glScalef(0.62f, 0.42f, 0.45f);
        glutSolidSphere(0.30, 18, 18);
        glPopMatrix();

        glColor3f(0.86f, 0.56f, 0.22f);
        glPushMatrix();
        glTranslatef(0.48f, 0.20f + sinf(catAnimTime * 0.5f) * 0.02f, 0.0f);
        glutSolidSphere(0.28, 24, 24);
        glPopMatrix();

        glColor3f(0.96f, 0.92f, 0.85f);
        glPushMatrix();
        glTranslatef(0.61f, 0.12f, 0.0f);
        glScalef(0.52f, 0.34f, 0.42f);
        glutSolidSphere(0.18, 18, 18);
        glPopMatrix();

        glColor3f(0.86f, 0.56f, 0.22f);
        glPushMatrix();
        glTranslatef(0.50f, 0.43f, 0.13f);
        glRotatef(-90, 1.0f, 0.0f, 0.0f);
        glutSolidCone(0.09, 0.16, 12, 12);
        glPopMatrix();
        glPushMatrix();
        glTranslatef(0.50f, 0.43f, -0.13f);
        glRotatef(-90, 1.0f, 0.0f, 0.0f);
        glutSolidCone(0.09, 0.16, 12, 12);
        glPopMatrix();

        glColor3f(0.10f, 0.55f, 0.18f);
        glPushMatrix();
        glTranslatef(0.67f, 0.19f, 0.09f);
        glScalef(0.07f, 0.10f, 0.04f);
        glutSolidSphere(1.0, 12, 12);
        glPopMatrix();
        glPushMatrix();
        glTranslatef(0.67f, 0.19f, -0.09f);
        glScalef(0.07f, 0.10f, 0.04f);
        glutSolidSphere(1.0, 12, 12);
        glPopMatrix();

        glColor3f(0.95f, 0.60f, 0.65f);
        glPushMatrix();
        glTranslatef(0.73f, 0.08f, 0.0f);
        glScalef(0.05f, 0.035f, 0.05f);
        glutSolidSphere(1.0, 10, 10);
        glPopMatrix();

        glColor3f(0.82f, 0.52f, 0.20f);
        drawCatLeg(0.18f, -0.12f, 0.18f, -70.0f, 0.36f);
        drawCatLeg(0.18f, -0.12f, -0.18f, -70.0f, 0.36f);
        drawCatLeg(-0.20f, -0.08f, 0.18f, -110.0f, 0.30f);
        drawCatLeg(-0.20f, -0.08f, -0.18f, -110.0f, 0.30f);

        glColor3f(0.86f, 0.56f, 0.22f);
        glPushMatrix();
        glTranslatef(-0.45f, -0.18f, 0.0f);
        glRotatef(-55.0f + tailIdle, 0.0f, 0.0f, 1.0f);
        glTranslatef(0.0f, 0.20f, 0.0f);
        drawCube(0.09f, 0.40f, 0.09f);
        glPopMatrix();

        glPopMatrix();
        return;
    }

    /* Body */
    glPushMatrix();
    glRotatef(backArch, 0.0f, 0.0f, 1.0f);
    glColor3f(0.86f, 0.56f, 0.22f);
    glPushMatrix();
    glScalef(1.65f, 0.72f, 0.72f);
    glutSolidSphere(0.40, 28, 28);
    glPopMatrix();

    /* Lower body white fur */
    glColor3f(0.95f, 0.90f, 0.82f);
    glPushMatrix();
    glTranslatef(0.25f, -0.10f, 0.0f);
    glScalef(0.85f, 0.30f, 0.42f);
    glutSolidSphere(0.32, 18, 18);
    glPopMatrix();
    glPopMatrix();

    /* Head */
    glColor3f(0.86f, 0.56f, 0.22f);
    glPushMatrix();
    glTranslatef(0.82f, 0.17f + headBob, 0.0f);
    glScalef(1.0f, 0.95f, 0.95f);
    glutSolidSphere(0.28, 24, 24);
    glPopMatrix();

    /* Muzzle */
    glColor3f(0.96f, 0.92f, 0.85f);
    glPushMatrix();
    glTranslatef(0.97f, 0.09f + headBob, 0.0f);
    glScalef(0.52f, 0.34f, 0.42f);
    glutSolidSphere(0.18, 18, 18);
    glPopMatrix();

    /* Ears */
    glColor3f(0.86f, 0.56f, 0.22f);
    glPushMatrix();
    glTranslatef(0.84f, 0.42f + headBob, 0.13f);
    glRotatef(-90, 1.0f, 0.0f, 0.0f);
    glutSolidCone(0.09, 0.16, 12, 12);
    glPopMatrix();

    glPushMatrix();
    glTranslatef(0.84f, 0.42f + headBob, -0.13f);
    glRotatef(-90, 1.0f, 0.0f, 0.0f);
    glutSolidCone(0.09, 0.16, 12, 12);
    glPopMatrix();

    /* Inner ears */
    glColor3f(0.95f, 0.72f, 0.72f);
    glPushMatrix();
    glTranslatef(0.84f, 0.39f + headBob, 0.13f);
    glRotatef(-90, 1.0f, 0.0f, 0.0f);
    glutSolidCone(0.045, 0.10, 10, 10);
    glPopMatrix();

    glPushMatrix();
    glTranslatef(0.84f, 0.39f + headBob, -0.13f);
    glRotatef(-90, 1.0f, 0.0f, 0.0f);
    glutSolidCone(0.045, 0.10, 10, 10);
    glPopMatrix();

    /* Eyes */
    glColor3f(0.10f, 0.55f, 0.18f);
    glPushMatrix();
    glTranslatef(1.02f, 0.18f + headBob, 0.09f);
    glScalef(0.07f, 0.10f, 0.04f);
    glutSolidSphere(1.0, 12, 12);
    glPopMatrix();

    glPushMatrix();
    glTranslatef(1.02f, 0.18f + headBob, -0.09f);
    glScalef(0.07f, 0.10f, 0.04f);
    glutSolidSphere(1.0, 12, 12);
    glPopMatrix();

    /* Pupils */
    glColor3f(0.0f, 0.0f, 0.0f);
    glPushMatrix();
    glTranslatef(1.05f, 0.18f + headBob, 0.09f);
    glScalef(0.02f, 0.07f, 0.02f);
    glutSolidSphere(1.0, 10, 10);
    glPopMatrix();

    glPushMatrix();
    glTranslatef(1.05f, 0.18f + headBob, -0.09f);
    glScalef(0.02f, 0.07f, 0.02f);
    glutSolidSphere(1.0, 10, 10);
    glPopMatrix();

    /* Nose */
    glColor3f(0.95f, 0.60f, 0.65f);
    glPushMatrix();
    glTranslatef(1.08f, 0.08f + headBob, 0.0f);
    glScalef(0.05f, 0.035f, 0.05f);
    glutSolidSphere(1.0, 10, 10);
    glPopMatrix();

    /* Tail - smoother and segmented feel */
    glColor3f(0.86f, 0.56f, 0.22f);
    glPushMatrix();
    glTranslatef(-0.68f, 0.14f, 0.0f);
    glRotatef(-28.0f + tailSwing, 0.0f, 0.0f, 1.0f);

    glPushMatrix();
    glTranslatef(0.0f, 0.22f, 0.0f);
    drawCube(0.10f, 0.45f, 0.10f);
    glPopMatrix();

    glTranslatef(0.0f, 0.42f, 0.0f);
    glRotatef(sinf(catAnimTime * 2.3f + 0.5f) * 14.0f, 0.0f, 0.0f, 1.0f);

    glPushMatrix();
    glTranslatef(0.0f, 0.18f, 0.0f);
    drawCube(0.085f, 0.36f, 0.085f);
    glPopMatrix();
    glPopMatrix();

    /* Legs */
    glColor3f(0.82f, 0.52f, 0.20f);

    float frontLeft = 26.0f * gait * speedRatio;
    float frontRight = 26.0f * gait2 * speedRatio;
    float backLeft = 24.0f * gait2 * speedRatio;
    float backRight = 24.0f * gait * speedRatio;

    if (catJumping) {
        frontLeft = frontRight = -18.0f;
        backLeft = backRight = 14.0f;
    }

    drawCatLeg(0.46f, -0.34f, 0.20f, frontLeft, 0.50f);
    drawCatLeg(0.46f, -0.34f, -0.20f, frontRight, 0.50f);
    drawCatLeg(-0.34f, -0.37f, 0.20f, backLeft, 0.52f);
    drawCatLeg(-0.34f, -0.37f, -0.20f, backRight, 0.52f);

    /* Paws */
    glColor3f(0.97f, 0.94f, 0.88f);

    glPushMatrix();
    glTranslatef(0.46f, -0.63f + 0.01f * smallWave, 0.20f);
    glScalef(0.11f, 0.05f, 0.13f);
    glutSolidSphere(1.0, 10, 10);
    glPopMatrix();

    glPushMatrix();
    glTranslatef(0.46f, -0.63f - 0.01f * smallWave, -0.20f);
    glScalef(0.11f, 0.05f, 0.13f);
    glutSolidSphere(1.0, 10, 10);
    glPopMatrix();

    glPushMatrix();
    glTranslatef(-0.34f, -0.64f - 0.01f * smallWave, 0.20f);
    glScalef(0.11f, 0.05f, 0.13f);
    glutSolidSphere(1.0, 10, 10);
    glPopMatrix();

    glPushMatrix();
    glTranslatef(-0.34f, -0.64f + 0.01f * smallWave, -0.20f);
    glScalef(0.11f, 0.05f, 0.13f);
    glutSolidSphere(1.0, 10, 10);
    glPopMatrix();

    glPopMatrix();
}

/* =========================================================
   Resize
   ========================================================= */
static void resize(int width, int height)
{
    if (height == 0) height = 1;
    const float ar = (float)width / (float)height;

    glViewport(0, 0, width, height);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(60.0, ar, 0.1, 100.0);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
}

/* =========================================================
   Display
   ========================================================= */
static void display(void)
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glLoadIdentity();

    if (cctvMode) {
        float camX = catPosX + 3.0f;
        float camY = 4.2f;
        float camZ = catPosZ + cctvDistance;

        gluLookAt(
            camX, camY, camZ,
            catPosX, -1.15f, catPosZ,
            0.0f, 1.0f, 0.0f
        );
    }
    else {
        glTranslatef(0.0f, 0.0f, zoomLevel);
        glRotatef(angleX, 1.0f, 0.0f, 0.0f);
        glRotatef(angleY, 0.0f, 1.0f, 0.0f);
    }

    drawFloor();
    drawWalls();
    drawCeiling();
    drawRug();
    drawTable();
    drawChair(-2.2f, -2.0f, 90.0f);
    drawChair(2.2f, -2.0f, -90.0f);
    drawChair(0.0f, -3.5f, 0.0f);
    drawCeilingFan();
    drawWallClock();
    drawPictureFrame(2.0f, 2.0f);
    drawPictureFrame(-1.5f, 0.5f);
    drawPictureFrame(4.5f, 3.2f);
    /* Lamp light position */
    {
        GLfloat lampLightPos[] = { 1.0f, 0.5f, -2.0f, 1.0f };
        glLightfv(GL_LIGHT1, GL_POSITION, lampLightPos);
    }

    drawLamp();
    drawOpenBook();
    drawBookStack();
    drawBed();
    drawBookshelf();
    drawPlant(-5.5f, 2.0f);
    drawPlant(5.0f, 1.5f);
    drawSideTable(-4.5f, -5.5f);
    drawSideTable(4.5f, -5.5f);
    drawWindow(3.0f, 2.5f, -7.95f);
    drawLightFixture(-3.0f, -3.0f);
    drawLightFixture(3.0f, -3.0f);
    drawCat();

    glutSwapBuffers();
}

/* =========================================================
   Keyboard
   ========================================================= */
static void key(unsigned char key, int x, int y)
{
    (void)x;
    (void)y;

    switch (key)
    {
    case 27:
    case 'q':
    case 'Q':
        exit(0);
        break;


    case 'f':
    case 'F':
        fanOn = !fanOn;
        break;

    case 'c':
    case 'C':
        cctvMode = !cctvMode;
        break;

    case '+':
    case '=':
        if (!cctvMode) {
            zoomLevel += 0.5f;
            if (zoomLevel > -2.0f) zoomLevel = -2.0f;
        }
        break;

    case '-':
    case '_':
        if (!cctvMode) {
            zoomLevel -= 0.5f;
            if (zoomLevel < -30.0f) zoomLevel = -30.0f;
        }
        break;

    case 'z':
    case 'Z':
        if (cctvMode) {
            cctvDistance -= 0.5f;
            if (cctvDistance < 4.0f) cctvDistance = 4.0f;
        }
        break;

    case 'x':
    case 'X':
        if (cctvMode) {
            cctvDistance += 0.5f;
            if (cctvDistance > 18.0f) cctvDistance = 18.0f;
        }
        break;

    case 'l':
    case 'L':
        lightOn = !lightOn;

        if (lightOn) {
            /* Full room light */
            glEnable(GL_LIGHTING);
            glEnable(GL_LIGHT0);
            glEnable(GL_LIGHT1);
        }
        else {
            /* Night mode (only lamp ON) */
            glEnable(GL_LIGHTING);
            glDisable(GL_LIGHT0);
            glEnable(GL_LIGHT1);
        }
        break;

    default:
        break;
    }

    glutPostRedisplay();
}

/* =========================================================
   Special keys
   ========================================================= */
static void specialKey(int key, int x, int y)
{
    if (cctvMode) {
        glutPostRedisplay();
        return;
    }

    switch (key)
    {
    case GLUT_KEY_UP:
        angleX -= 5.0f;
        break;
    case GLUT_KEY_DOWN:
        angleX += 5.0f;
        break;
    case GLUT_KEY_LEFT:
        angleY -= 5.0f;
        break;
    case GLUT_KEY_RIGHT:
        angleY += 5.0f;
        break;
    }

    glutPostRedisplay();
}

/* =========================================================
   Idle
   ========================================================= */
static void idle(void)
{
    int nowMs = glutGet(GLUT_ELAPSED_TIME);
    float dt;

    if (lastUpdateTimeMs < 0) {
        lastUpdateTimeMs = nowMs;
    }

    dt = (nowMs - lastUpdateTimeMs) / 1000.0f;
    lastUpdateTimeMs = nowMs;

    if (dt < 0.0f) dt = 0.0f;
    if (dt > 0.05f) dt = 0.05f;

    if (fanOn) {
        fanRotation += 120.0f * dt;
        if (fanRotation > 360.0f) fanRotation -= 360.0f;
    }

    clockAngle += 6.0f * dt;
    if (clockAngle > 360.0f) clockAngle -= 360.0f;

    catStateTime += dt;

    if (catRunning) {
        catSitting = 0;
        catCurrentSpeed = catBaseSpeed;

        catPosX += catCurrentSpeed * catDirection * (dt * 60.0f);
        catAnimTime += 4.5f * dt;

        if (catPosX > 5.8f) {
            catPosX = 5.8f;
            catDirection = -1.0f;
        }
        if (catPosX < -5.8f) {
            catPosX = -5.8f;
            catDirection = 1.0f;
        }

        if (catStateTime >= catWalkDuration) {
            catRunning = 0;
            catSitting = 1;
            catCurrentSpeed = 0.0f;
            catJumping = 0;
            catJumpTime = 0.0f;
            catStateTime = 0.0f;
        }
    }
    else {
        catSitting = 1;
        catCurrentSpeed = 0.0f;
        catAnimTime += 1.0f * dt;

        if (catStateTime >= catSitDuration) {
            catRunning = 1;
            catSitting = 0;
            catJumping = 0;
            catJumpTime = 0.0f;
            catStateTime = 0.0f;
        }
    }

    glutPostRedisplay();
}

/* =========================================================
   Lighting
   ========================================================= */
const GLfloat light_ambient[] = { 0.3f, 0.3f, 0.3f, 1.0f };
const GLfloat light_diffuse[] = { 1.0f, 1.0f, 1.0f, 1.0f };
const GLfloat light_specular[] = { 1.0f, 1.0f, 1.0f, 1.0f };
const GLfloat light_position[] = { 2.0f, 5.0f, 5.0f, 0.0f };

const GLfloat mat_ambient[] = { 0.8f, 0.8f, 0.8f, 1.0f };
const GLfloat mat_diffuse[] = { 0.9f, 0.9f, 0.9f, 1.0f };
const GLfloat mat_specular[] = { 1.0f, 1.0f, 1.0f, 1.0f };
const GLfloat high_shininess[] = { 100.0f };

/* =========================================================
   Main
   ========================================================= */
int main(int argc, char* argv[])
{
    glutInit(&argc, argv);
    glutInitWindowSize(800, 600);
    glutInitWindowPosition(10, 10);
    glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH);

    glutCreateWindow("Interior Design Room Scene - Smooth Natural Cat");

    glShadeModel(GL_SMOOTH);

    glutReshapeFunc(resize);
    glutDisplayFunc(display);
    glutKeyboardFunc(key);
    glutSpecialFunc(specialKey);
    glutIdleFunc(idle);

    glClearColor(0.95f, 0.95f, 0.98f, 1.0f);

    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);

    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);

    glEnable(GL_LIGHT0);
    glEnable(GL_LIGHT1);
    glEnable(GL_NORMALIZE);
    glEnable(GL_COLOR_MATERIAL);
    glEnable(GL_LIGHTING);

    glLightfv(GL_LIGHT0, GL_AMBIENT, light_ambient);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, light_diffuse);
    glLightfv(GL_LIGHT0, GL_SPECULAR, light_specular);
    glLightfv(GL_LIGHT0, GL_POSITION, light_position);

    GLfloat light1_position[] = { -2.0f, 5.0f, -5.0f, 0.0f };
    GLfloat light1_diffuse[] = { 1.0f, 0.85f, 0.6f, 1.0f };
    glLightfv(GL_LIGHT1, GL_DIFFUSE, light1_diffuse);
    glLightfv(GL_LIGHT1, GL_POSITION, light1_position);

    glMaterialfv(GL_FRONT, GL_AMBIENT, mat_ambient);
    glMaterialfv(GL_FRONT, GL_DIFFUSE, mat_diffuse);
    glMaterialfv(GL_FRONT, GL_SPECULAR, mat_specular);
    glMaterialfv(GL_FRONT, GL_SHININESS, high_shininess);

    glutMainLoop();
    return EXIT_SUCCESS;
}