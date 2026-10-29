
/*
 * 
 * - Scene 1: Airport takeoff with retracting landing gear.
 * - Scene 2: Cruising plane over a bustling city highway with obstacles & coins.
 * - Scene 3: Cross-section underground metro with a moving train.
 * - Scene 4: Serene village with river, moving boats, and drifting clouds.
 * - Scene 5: Sunset beach with ocean, sand grains, and a ship.
 * - Scene 6: Desert Night with camp, starry sky, scrolling background, and cars.
 *
 * - CONTROLS:
 *   Press '1' to play Scene 1 (Auto-transitions all the way to 6)
 *   Press '2', '3', '4', '5', '6' to manually jump to specific scenes.
 */

#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif
#include <cmath>
#include <stdlib.h>
#include <time.h>
#include <string>

const int WIDTH  = 1200;
const int HEIGHT = 700;
const float s3_PI = 3.14159265f;
const float s5_PI = 3.14159265f;
const float s6_PI = 3.14159265f;

// Scene Controller
int current_scene = 1;

// [SHARED ANIMATION VARIABLES]
float cloud_offset = 0.0f;

float scene1_bg_offset = 0.0f;
float plane_x     = 60.0f;
float plane_y     = 95.0f;
float plane_angle = 0.0f;
int   plane_phase = 0;

int   scene2_frames = 0;

float car1_x = 0.0f;
float car2_x = 450.0f;
float car3_x = 900.0f;
float car4_x = 1200.0f;

float p1_x = 100.0f;
float p2_x = 800.0f;
float p3_x = 400.0f;

float scene2_plane_base_y = 480.0f;
float scene2_plane_bob = 0.0f;
float bob_time = 0.0f;

float scene2_bg_offset = 0.0f;

float obs_x = WIDTH + 100.0f;
float obs_y = 480.0f;
float obs_speed = 7.0f;
int collision_flash = 0;

int plane_life = 3;
bool plane_crashed = false;
bool is_game_over = false;
float crash_fall_speed = 0.0f;

int player_score = 0;
float coin_x = WIDTH + 500.0f;
float coin_y = 450.0f;
int coin_type = 1;
float coin_speed = 6.5f;

int   scene3_frames = 0;
float scene3_bg_offset = 0.0f;
float train_offset_x = 0.0f;

int   scene4_frames = 0;
float scene4_bg_offset = 0.0f;
float s4_boat_offset = 0.0f;

int   scene5_frames = 0;
float scene5_bg_offset = 0.0f;
float s5_ship_x = 1.5f;

float scene6_bg_offset = 0.0f;
float s6_car1_x = 2.0f;
float s6_car2_x = 2.5f;


// ..........................................................................
// .......................... SHARED LOW-LEVEL HELPERS ......................
// ..........................................................................

void setColor3f(float r, float g, float b) { glColor3f(r, g, b); }

void fillRect(float x, float y, float w, float h, float r, float g, float b) {
    glColor3f(r, g, b);
    glBegin(GL_QUADS);
        glVertex2f(x,     y);
        glVertex2f(x + w, y);
        glVertex2f(x + w, y + h);
        glVertex2f(x,     y + h);
    glEnd();
}

void fillTri(float x1,float y1, float x2,float y2, float x3,float y3, float r, float g, float b) {
    glColor3f(r,g,b);
    glBegin(GL_TRIANGLES);
        glVertex2f(x1,y1); glVertex2f(x2,y2); glVertex2f(x3,y3);
    glEnd();
}

void fillCircle(float cx, float cy, float radius, float r, float g, float b, int segs) {
    glColor3f(r, g, b);
    glBegin(GL_TRIANGLE_FAN);
        glVertex2f(cx, cy);
        for (int i = 0; i <= segs; ++i) {
            float a = i * 2.0f * 3.14159265f / segs;
            glVertex2f(cx + cosf(a)*radius, cy + sinf(a)*radius);
        }
    glEnd();
}
void fillCircle(float cx, float cy, float radius, float r, float g, float b) {
    fillCircle(cx, cy, radius, r, g, b, 32);
}

void drawText(const char *text, float x, float y) {
    glColor3f(1.0f, 0.2f, 0.2f);
    glRasterPos2f(x, y);
    while (*text) {
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, *text);
        text++;
    }
}

void drawWhiteText(const char *text, float x, float y) {
    glColor3f(1.0f, 1.0f, 1.0f);
    glRasterPos2f(x, y);
    while (*text) {
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, *text);
        text++;
    }
}


// ..........................................................................
// .......................... SHARED ENVIRONMENT LOGIC ......................
// ..........................................................................

void draw_sky() {
    glBegin(GL_QUADS);
        glColor3f(0.20f, 0.60f, 0.95f);
        glVertex2f(0,         0);
        glVertex2f((float)WIDTH, 0);
        glColor3f(0.10f, 0.40f, 0.85f);
        glVertex2f((float)WIDTH, (float)HEIGHT);
        glVertex2f(0,         (float)HEIGHT);
    glEnd();
}

void draw_sun_scene2(float cx, float cy) {
    float scale = 6.0f;
    const float PI = 3.14159265f;

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glColor4f(1.0f, 0.85f, 0.15f, 0.25f);
    glBegin(GL_TRIANGLE_FAN);
    glVertex2f(cx, cy);
    for (int i = 0; i <= 48; ++i) {
        float a = 2.0f * PI * i / 48.0f;
        glVertex2f(cx + (12.0f * scale) * cosf(a), cy + (12.0f * scale) * sinf(a));
    }
    glEnd();

    glColor4f(1.0f, 0.80f, 0.10f, 0.45f);
    glBegin(GL_TRIANGLE_FAN);
    glVertex2f(cx, cy);
    for (int i = 0; i <= 48; ++i) {
        float a = 2.0f * PI * i / 48.0f;
        glVertex2f(cx + (8.0f * scale) * cosf(a), cy + (8.0f * scale) * sinf(a));
    }
    glEnd();

    glColor4f(1.0f, 0.90f, 0.30f, 1.0f);
    glBegin(GL_TRIANGLE_FAN);
    glVertex2f(cx, cy);
    for (int i = 0; i <= 48; ++i) {
        float a = 2.0f * PI * i / 48.0f;
        glVertex2f(cx + (4.5f * scale) * cosf(a), cy + (4.5f * scale) * sinf(a));
    }
    glEnd();

    glColor4f(1.0f, 0.95f, 0.60f, 1.0f);
    glBegin(GL_TRIANGLE_FAN);
    glVertex2f(cx, cy);
    for (int i = 0; i <= 32; ++i) {
        float a = 2.0f * PI * i / 32.0f;
        glVertex2f(cx + (3.0f * scale) * cosf(a), cy + (3.0f * scale) * sinf(a));
    }
    glEnd();

    glColor4f(1.0f, 0.85f, 0.20f, 0.8f);
    glLineWidth(2.5f);
    glBegin(GL_LINES);
    for (int i = 0; i < 12; ++i) {
        float a = 2.0f * PI * i / 12.0f;
        glVertex2f(cx + (5.0f * scale) * cosf(a), cy + (5.0f * scale) * sinf(a));
        glVertex2f(cx + (9.0f * scale) * cosf(a), cy + (9.0f * scale) * sinf(a));
    }
    glEnd();
    glLineWidth(1.0f);

    glDisable(GL_BLEND);
}

void draw_cloud_puff(float cx, float cy, float rx, float ry) {
    glBegin(GL_TRIANGLE_FAN);
        glVertex2f(cx, cy);
        for (int i = 0; i <= 32; ++i) {
            float a = i * 2.0f * 3.14159265f / 32;
            glVertex2f(cx + cosf(a)*rx, cy + sinf(a)*ry);
        }
    glEnd();
}

void draw_cloud(float x, float y, float scale) {
    glColor3f(0.92f, 0.95f, 0.98f);
    draw_cloud_puff(x,          y - 4*scale, 28*scale, 14*scale);
    draw_cloud_puff(x+35*scale, y - 4*scale, 24*scale, 12*scale);
    draw_cloud_puff(x-20*scale, y - 4*scale, 18*scale, 10*scale);

    glColor3f(1.0f, 1.0f, 1.0f);
    draw_cloud_puff(x,          y,          28*scale, 16*scale);
    draw_cloud_puff(x+35*scale, y - 2*scale,24*scale, 14*scale);
    draw_cloud_puff(x-20*scale, y - 2*scale,18*scale, 12*scale);
    draw_cloud_puff(x+15*scale, y + 8*scale,20*scale, 13*scale);
}

void draw_clouds() {
    struct Cloud { float x, y, scale; };
    Cloud clouds[] = {
        {160,  580, 1.0f},
        {420,  530, 0.85f},
        {750,  560, 0.7f},
        {980,  545, 0.9f}
    };

    for (int i = 0; i < 4; ++i) {
        float current_x = clouds[i].x + cloud_offset;
        current_x = fmod(current_x + 150.0f, WIDTH + 300.0f) - 150.0f;
        draw_cloud(current_x, clouds[i].y, clouds[i].scale);
    }
}


// ..........................................................................
// .......................... GAMEPLAY ELEMENTS (PLANE, COINS) ..............
// ..........................................................................

void draw_airplane(float ox, float oy) {
    glPushMatrix();
    glTranslatef(ox, oy, 0);

    float sc = 1.55f;
    glScalef(sc, sc, 1.0f);

    if (plane_phase == 0 && current_scene == 1) {
        glColor4f(0.0f, 0.0f, 0.0f, 0.18f);
        glBegin(GL_POLYGON);
            glVertex2f(12, -22); glVertex2f(105, -22); glVertex2f(112, -18);
            glVertex2f(95, -15); glVertex2f(25, -15); glVertex2f(8, -18);
        glEnd();
    }

    glColor3f(0.18f, 0.18f, 0.20f);
    glBegin(GL_POLYGON);
        glVertex2f(0, -2); glVertex2f(14, -3); glVertex2f(14, 3); glVertex2f(0, 2);
    glEnd();
    fillRect(0, -1, 5, 2, 0.05f, 0.05f, 0.05f);

    glBegin(GL_POLYGON);
        glColor3f(0.85f, 0.90f, 0.95f); glVertex2f(12, -3);
        glColor3f(0.90f, 0.95f, 1.00f); glVertex2f(55, -5);
        glColor3f(0.95f, 0.98f, 1.00f); glVertex2f(98, -5);
        glColor3f(1.00f, 1.00f, 1.00f); glVertex2f(125, -1);
        glColor3f(0.90f, 0.95f, 1.00f); glVertex2f(120, 5);
        glColor3f(0.85f, 0.90f, 0.95f); glVertex2f(98, 8);
        glColor3f(0.80f, 0.85f, 0.90f); glVertex2f(65, 8);
        glColor3f(0.75f, 0.80f, 0.85f); glVertex2f(35, 6);
        glColor3f(0.70f, 0.75f, 0.80f); glVertex2f(12, 3);
    glEnd();

    glBegin(GL_POLYGON);
        glColor3f(1.00f, 1.00f, 1.0f);  glVertex2f(25, 2); glVertex2f(88, 3);
        glColor3f(0.85f, 0.90f, 1.0f); glVertex2f(105, 5); glVertex2f(75, 6); glVertex2f(35, 5);
    glEnd();

    glColor3f(0.70f, 0.74f, 0.78f);
    glBegin(GL_POLYGON);
        glVertex2f(115, -3); glVertex2f(138, 1); glVertex2f(115, 5);
    glEnd();

    glColor3f(0.08f, 0.10f, 0.12f);
    glBegin(GL_POLYGON);
        glVertex2f(68, 8); glVertex2f(73, 15); glVertex2f(88, 18); glVertex2f(102, 8);
    glEnd();

    glColor3f(0.20f, 0.60f, 1.00f);
    glBegin(GL_POLYGON);
        glVertex2f(74, 14); glVertex2f(86, 15); glVertex2f(82, 11); glVertex2f(75, 10);
    glEnd();

    glColor3f(0.50f, 0.55f, 0.60f);
    glBegin(GL_POLYGON);
        glVertex2f(63, -5); glVertex2f(87, -5); glVertex2f(80, -11); glVertex2f(64, -11);
    glEnd();
    fillRect(78, -10, 5, 4, 0.08f, 0.08f, 0.08f);

    glBegin(GL_POLYGON);
        glColor3f(0.72f, 0.76f, 0.80f); glVertex2f(18, 4);
        glColor3f(0.85f, 0.90f, 0.95f); glVertex2f(42, 6);
        glColor3f(0.40f, 0.60f, 0.90f); glVertex2f(24, 38);
        glColor3f(0.30f, 0.50f, 0.80f); glVertex2f(12, 37);
    glEnd();

    fillRect(11, 35, 16, 2.5f, 0.15f, 0.35f, 0.70f);

    glBegin(GL_POLYGON);
        glColor3f(0.75f, 0.80f, 0.85f); glVertex2f(2, -2);
        glColor3f(0.65f, 0.70f, 0.75f); glVertex2f(30, -2);
        glColor3f(0.50f, 0.55f, 0.60f); glVertex2f(20, -8);
        glColor3f(0.40f, 0.45f, 0.50f); glVertex2f(0, -6);
    glEnd();

    glBegin(GL_POLYGON);
        glColor3f(0.85f, 0.90f, 0.95f); glVertex2f(40, -1);
        glColor3f(0.75f, 0.80f, 0.85f); glVertex2f(84, -2);
        glColor3f(0.55f, 0.60f, 0.65f); glVertex2f(66, -16);
        glColor3f(0.45f, 0.50f, 0.55f); glVertex2f(30, -14);
    glEnd();

    glBegin(GL_POLYGON);
        glColor3f(0.95f, 0.98f, 1.00f); glVertex2f(45, -2); glVertex2f(72, -3);
        glColor3f(0.65f, 0.75f, 0.85f); glVertex2f(58, -8); glVertex2f(40, -7);
    glEnd();

    fillRect(46, -20, 24, 5, 0.80f, 0.82f, 0.85f);
    fillCircle(46, -17.5f, 2.5f, 0.80f, 0.82f, 0.85f, 12);
    fillCircle(70, -17.5f, 2.5f, 0.80f, 0.82f, 0.85f, 12);

    fillRect(28, -15, 38, 1.5f, 0.35f, 0.45f, 0.55f);

    if (plane_phase == 0 && current_scene == 1) {
        glColor3f(0.25f, 0.25f, 0.25f);
        glLineWidth(2.5f);
        glBegin(GL_LINES);
            glVertex2f(56, -5);  glVertex2f(52, -21);
            glVertex2f(97, -4);  glVertex2f(97, -20);
        glEnd();
        glLineWidth(1.0f);

        fillCircle(52, -21, 4.5f, 0.05f, 0.05f, 0.05f, 32);
        fillCircle(97, -20, 3.5f, 0.05f, 0.05f, 0.05f, 32);
        fillCircle(52, -21, 1.3f, 0.80f, 0.80f, 0.80f, 32);
        fillCircle(97, -20, 1.0f, 0.80f, 0.80f, 0.80f, 32);
    }

    glColor3f(0.50f, 0.55f, 0.60f);
    glLineWidth(1.0f);
    glBegin(GL_LINES);
        glVertex2f(28, 1);  glVertex2f(95, 1);
        glVertex2f(40, -2); glVertex2f(65, -12);
        glVertex2f(65, 4);  glVertex2f(85, 4);
    glEnd();

    glDisable(GL_BLEND);
    glPopMatrix();
}

void draw_obstacle(float ox, float oy) {
    glPushMatrix();
    glTranslatef(ox, oy, 0);

    fillRect(-8, -35, 16, 12, 0.5f, 0.2f, 0.1f);
    glColor3f(0.05f, 0.05f, 0.05f);
    glBegin(GL_LINES);
        glVertex2f(-8, -23); glVertex2f(-15, -10);
        glVertex2f(8, -23);  glVertex2f(15, -10);
    glEnd();

    fillCircle(0, 5, 22, 1.0f, 0.15f, 0.15f, 32);
    fillTri(-20, 0, 20, 0, 0, -15, 1.0f, 0.15f, 0.15f);

    fillCircle(0, 5, 12, 1.0f, 0.9f, 0.0f, 32);
    fillTri(-10, 0, 10, 0, 0, -15, 1.0f, 0.9f, 0.0f);
    glPopMatrix();
}

void draw_coin(float ox, float oy, int type) {
    glPushMatrix();
    glTranslatef(ox, oy, 0);

    if (type == 1) {
        fillCircle(0, 0, 15, 1.0f, 0.85f, 0.0f, 32);
        fillCircle(0, 0, 10, 1.0f, 1.0f, 0.2f, 32);
        glColor3f(0.9f, 0.65f, 0.0f);
        glLineWidth(3.0f);
        glBegin(GL_LINES);
            glVertex2f(0, 5); glVertex2f(0, -5);
        glEnd();
        glLineWidth(1.0f);
    }
    else if (type == 2) {
        fillCircle(0, 0, 25, 1.0f, 0.55f, 0.0f, 32);
        fillCircle(0, 0, 18, 1.0f, 0.8f, 0.0f, 32);
        glColor3f(1.0f, 0.4f, 0.0f);
        glLineWidth(4.0f);
        glBegin(GL_LINES);
            glVertex2f(-5, 8); glVertex2f(-5, -8);
            glVertex2f(5, 8);  glVertex2f(5, -8);
        glEnd();
        glLineWidth(1.0f);
    }
    glPopMatrix();
}


// ..........................................................................
// ................................ SCENE 1 .................................
// ..........................................................................

void draw_brick_wall() {
    float ground_y = 195.0f;
    float wall_h = 28.0f;
    float ext_width = (float)WIDTH * 3.0f;

    fillRect(0, ground_y - 5.0f, ext_width, wall_h + 5.0f, 0.45f, 0.45f, 0.45f);
    float block_size = 14.0f;
    float mortar = 2.0f;
    int rows = (int)(wall_h / (block_size + mortar));

    for (int r = 0; r < rows; ++r) {
        float y = ground_y + r * (block_size + mortar);
        float offset = (r % 2 == 0) ? 0.0f : -(block_size / 2.0f);
        for (float x = offset; x < ext_width; x += (block_size + mortar)) {
            fillRect(x, y, block_size, block_size, 0.85f, 0.30f, 0.15f);
            fillRect(x, y + block_size - 2.0f, block_size, 2.0f, 0.95f, 0.40f, 0.25f);
            fillRect(x + block_size - 2.0f, y, 2.0f, block_size, 0.65f, 0.15f, 0.05f);
        }
    }
    float cap_y = ground_y + rows * (block_size + mortar);
    fillRect(0, cap_y, ext_width, 5.0f, 0.55f, 0.55f, 0.55f);
    fillRect(0, cap_y - 2.0f, ext_width, 2.0f, 0.40f, 0.40f, 0.40f);
}

void draw_big_tree(float ox, float oy, float scale) {
    glPushMatrix();
    glTranslatef(ox, oy, 0);
    glScalef(scale, scale, 1.0f);

    glColor3f(0.35f, 0.20f, 0.05f);
    glBegin(GL_POLYGON);
        glVertex2f(-12, 0); glVertex2f(12, 0); glVertex2f(7, 20);
        glVertex2f(5, 70); glVertex2f(-5, 70); glVertex2f(-7, 20);
    glEnd();

    glLineWidth(4.0f);
    glBegin(GL_LINES);
        glVertex2f(-4, 50);  glVertex2f(-30, 85);
        glVertex2f(-15, 70); glVertex2f(-40, 95);
        glVertex2f(4, 55);   glVertex2f(30, 90);
        glVertex2f(15, 75);  glVertex2f(40, 100);
        glVertex2f(0, 70);   glVertex2f(0, 110);
    glEnd();
    glLineWidth(1.0f);

    float baseR = 0.10f, baseG = 0.65f, baseB = 0.15f;
    fillCircle(-45, 95, 35, baseR, baseG, baseB);
    fillCircle(45, 95, 35, baseR, baseG, baseB);
    fillCircle(-35, 130, 40, baseR, baseG, baseB);
    fillCircle(35, 130, 40, baseR, baseG, baseB);
    fillCircle(0, 150, 45, baseR, baseG, baseB);
    fillCircle(0, 110, 50, baseR, baseG, baseB);

    float lightR = 0.20f, lightG = 0.85f, lightB = 0.25f;
    fillCircle(-35, 95, 25, lightR, lightG, lightB);
    fillCircle(35, 95, 25, lightR, lightG, lightB);
    fillCircle(-25, 125, 30, lightR, lightG, lightB);
    fillCircle(25, 125, 30, lightR, lightG, lightB);
    fillCircle(0, 140, 35, lightR, lightG, lightB);
    fillCircle(0, 105, 45, lightR, lightG, lightB);

    glPopMatrix();
}

void draw_grass() {
    float ext_width = (float)WIDTH * 3.0f;
    fillRect(0,   0, ext_width, 200, 0.25f, 0.68f, 0.15f);
    fillRect(0,   0, ext_width, 145, 0.20f, 0.60f, 0.10f);
    fillRect(0, 145, ext_width,   8, 0.30f, 0.75f, 0.20f);
}

void draw_runway() {
    float ry = 35.0f, rh = 120.0f;
    float ext_width = (float)WIDTH * 3.0f;

    glBegin(GL_QUADS);
        glColor3f(0.20f,0.21f,0.22f); glVertex2f(0, ry); glVertex2f(ext_width, ry);
        glColor3f(0.25f,0.26f,0.28f); glVertex2f(ext_width, ry+rh); glVertex2f(0, ry+rh);
    glEnd();

    fillRect(0, ry,        ext_width, 6,  0.15f,0.16f,0.17f);
    fillRect(0, ry+rh-6,   ext_width, 6,  0.15f,0.16f,0.17f);

    for (int i = 0; i < 8; ++i) {
        fillRect(20 + i*12.0f, ry+8, 7, rh-16, 1.0f,1.0f,1.0f);
        fillRect(ext_width - 120 + i*12.0f, ry+8, 7, rh-16, 1.0f,1.0f,1.0f);
    }

    float dashW = 70.0f, gapW = 50.0f;
    float cy = ry + rh*0.5f - 3.0f;
    for (float x = 0.0f; x <= ext_width; x += dashW + gapW)
        fillRect(x, cy, dashW, 6, 1.0f,1.0f,1.0f);

    for (float x = 0; x <= ext_width; x += 60) {
        fillCircle(x, ry+3,   4, 1.0f, 0.85f, 0.0f);
        fillCircle(x, ry+rh-3, 4, 1.0f, 0.85f, 0.0f);
    }
}

void draw_simple_building(float ox, float oy, float bw, float bh) {
    glPushMatrix();
    glTranslatef(ox, oy, 0);

    fillRect(-5, -5, bw+10, bh+10, 0.35f, 0.38f, 0.40f);

    glBegin(GL_QUADS);
        glColor3f(0.85f, 0.80f, 0.70f); glVertex2f(0, 0);   glVertex2f(bw, 0);
        glColor3f(0.95f, 0.90f, 0.85f); glVertex2f(bw, bh); glVertex2f(0, bh);
    glEnd();

    int cols = (int)(bw / 15.0f);
    int rows = (int)(bh / 15.0f) - 1;
    float x_margin = (bw - (cols * 10.0f + (cols - 1) * 5.0f)) / 2.0f;

    for (int row = 0; row < rows; ++row) {
        for (int col = 0; col < cols; ++col) {
            float wx = x_margin + col * 15.0f;
            float wy = 10.0f + row * 15.0f;
            float pulse = (sinf(bob_time * 4.0f + row + col) + 1.0f) * 0.5f;
            float bright = ((row+col)%4==0) ? (0.4f + 0.6f * pulse) : 0.3f;
            fillRect(wx, wy, 10, 10, bright*0.20f, bright*0.80f, bright*1.0f);
        }
    }

    fillRect(-2, bh, bw+4, 10, 0.45f, 0.48f, 0.52f);
    fillCircle(bw*0.5f, bh+14, 2.5f, 1.0f, 0.1f, 0.1f);
    glPopMatrix();
}

void draw_twin_towers(float ox, float oy) {
    glPushMatrix();
    glTranslatef(ox, oy, 0);

    float tw = 85.0f, th = 360.0f;
    float gap = 50.0f;

    fillRect(-5,   -5, tw+10, th+15, 0.35f,0.38f,0.42f);
    fillRect(tw+gap-5, -5, tw+10, th+15, 0.35f,0.38f,0.42f);

    glBegin(GL_QUADS);
        glColor3f(0.50f,0.60f,0.70f); glVertex2f(0,  0);   glVertex2f(tw,  0);
        glColor3f(0.70f,0.80f,0.95f); glVertex2f(tw,  th);  glVertex2f(0,   th);
    glEnd();
    glBegin(GL_QUADS);
        glColor3f(0.45f,0.55f,0.65f); glVertex2f(tw+gap, 0);   glVertex2f(tw*2+gap, 0);
        glColor3f(0.65f,0.75f,0.90f); glVertex2f(tw*2+gap, th);glVertex2f(tw+gap,   th);
    glEnd();

    for (int row = 0; row < 28; ++row) {
        for (int col = 0; col < 5; ++col) {
            float wx = 8.0f + col * 15.0f;
            float wy = 10.0f + row * 12.5f;
            float pulse = (sinf(bob_time * 5.0f + row + col) + 1.0f) * 0.5f;
            float bright = ((row+col)%3==0) ? (0.4f + 0.6f * pulse) : 0.2f;
            fillRect(wx, wy, 10, 9, bright*0.1f, bright*0.8f, bright*1.0f);
        }
    }
    for (int row = 0; row < 28; ++row) {
        for (int col = 0; col < 5; ++col) {
            float wx = tw+gap + 8.0f + col*15.0f;
            float wy = 10.0f + row*12.5f;
            float pulse = (sinf(bob_time * 5.0f + row + col) + 1.0f) * 0.5f;
            float bright = ((row+col+1)%3==0) ? (0.4f + 0.6f * pulse) : 0.2f;
            fillRect(wx, wy, 10, 9, bright*0.1f, bright*0.8f, bright*1.0f);
        }
    }

    fillRect(-3,   th,   tw+6,  14, 0.45f,0.48f,0.52f);
    fillRect(tw+gap-3, th, tw+6,14,  0.45f,0.48f,0.52f);
    fillRect(0,   th+14, tw,   6,  0.25f,0.27f,0.30f);
    fillRect(tw+gap, th+14, tw, 6,  0.25f,0.27f,0.30f);

    glLineWidth(2.0f);
    glColor3f(0.20f,0.22f,0.25f);
    glBegin(GL_LINES);
        glVertex2f(tw*0.5f, th+20); glVertex2f(tw*0.5f, th+55);
        glVertex2f(tw+gap+tw*0.5f, th+20); glVertex2f(tw+gap+tw*0.5f, th+48);
    glEnd();
    glLineWidth(1.0f);

    fillCircle(tw*0.5f, th+55, 2.5f, 1.0f, 0.1f, 0.1f);
    fillCircle(tw+gap+tw*0.5f, th+48, 2.5f, 1.0f, 0.1f, 0.1f);

    float bry = th * 0.52f;
    fillRect(tw, bry,       gap, 28, 0.40f,0.45f,0.50f);
    fillRect(tw, bry+4,     gap, 18, 0.45f,0.65f,0.95f);
    glColor3f(0.20f,0.22f,0.25f);
    glBegin(GL_LINES);
    for (int i=0;i<=4;++i){
        glVertex2f(tw + i*(gap/4.0f), bry);
        glVertex2f(tw + i*(gap/4.0f), bry+28);
    }
    glEnd();
    glPopMatrix();
}

void draw_control_tower(float ox, float oy) {
    glPushMatrix();
    glTranslatef(ox, oy, 0);

    fillRect(-18, 0, 36, 10, 0.20f,0.20f,0.20f);

    glColor3f(0.85f,0.75f,0.55f);
    glBegin(GL_QUADS);
        glVertex2f(-14, 10);  glVertex2f(14, 10);
        glVertex2f(10,  160); glVertex2f(-10, 160);
    glEnd();
    glColor3f(0.95f,0.85f,0.65f);
    glBegin(GL_QUADS);
        glVertex2f(-3, 10); glVertex2f(3, 10);
        glVertex2f(3, 160); glVertex2f(-3, 160);
    glEnd();
    glColor3f(0.70f,0.60f,0.40f);
    glBegin(GL_QUADS);
        glVertex2f(10, 10); glVertex2f(14, 10);
        glVertex2f(10,160); glVertex2f(10, 10);
    glEnd();

    fillRect(-18, 158, 36,  8, 0.50f,0.45f,0.35f);
    fillRect(-22, 164, 44,  6, 0.40f,0.35f,0.25f);
    fillRect(-24, 170, 48, 14, 0.75f,0.65f,0.45f);

    fillRect(-28, 184, 56, 42, 0.30f,0.30f,0.30f);
    fillRect(-25, 186, 20, 36, 0.40f,0.80f,1.0f);
    fillRect(-5,  186, 20, 36, 0.50f,0.85f,1.0f);
    fillRect( 5,  186, 20, 36, 0.35f,0.75f,1.0f);
    glColor3f(0.15f,0.15f,0.15f);
    glBegin(GL_LINES);
        glVertex2f(-25,186); glVertex2f(-25,222);
        glVertex2f(-5, 186); glVertex2f(-5, 222);
        glVertex2f( 5, 186); glVertex2f( 5, 222);
        glVertex2f(25, 186); glVertex2f(25, 222);
        glVertex2f(-28,200); glVertex2f(28, 200);
        glVertex2f(-28,212); glVertex2f(28, 212);
    glEnd();

    fillRect(-30, 226, 60, 8, 0.30f,0.28f,0.25f);
    fillRect(-32, 234, 64, 6, 0.40f,0.38f,0.35f);
    fillRect(-2, 240, 4, 55, 0.25f,0.25f,0.25f);

    glLineWidth(2.0f);
    glColor3f(0.25f,0.25f,0.25f);
    glBegin(GL_LINES);
        glVertex2f(-10,270); glVertex2f(10,270);
        glVertex2f(-7, 280); glVertex2f(7, 280);
        glVertex2f(-5, 288); glVertex2f(5, 288);
    glEnd();

    fillCircle(0, 295, 7, 0.40f,0.35f,0.30f);

    float red_pulse = (sinf(bob_time * 10.0f) + 1.0f) * 0.5f;
    fillCircle(0, 303, 2.5f, 0.5f + 0.5f * red_pulse, 0.1f, 0.1f);

    glLineWidth(1.0f);
    glPopMatrix();
}

void draw_scene1_environment() {
    draw_simple_building(730, 220, 70, 240);
    draw_simple_building(1060, 220, 60, 270);
    draw_twin_towers(820, 220);

    draw_big_tree(330, 205, 0.6f);
    draw_big_tree(530, 205, 0.7f);
    draw_control_tower(430, 220);

    draw_brick_wall();
    draw_grass();
    draw_runway();
}


// ..........................................................................
// ................................ SCENE 2 .................................
// ..........................................................................

void draw_modern_building(float ox, float oy, float bw, float bh, float r, float g, float b) {
    glPushMatrix();
    glTranslatef(ox, oy, 0);

    fillRect(0, 0, bw, bh, r, g, b);
    fillRect(-5, bh, bw+10, 15, r*0.7f, g*0.7f, b*0.7f);

    int cols = (int)((bw - 10.0f) / 18.0f);
    int rows = (int)(bh / 20.0f) - 1;
    float block_width = (cols > 0) ? ((cols - 1) * 18.0f + 14.0f) : 0;
    float x_margin = (bw - block_width) / 2.0f;

    for (int row = 0; row < rows; ++row) {
        for (int col = 0; col < cols; ++col) {
            float wx = x_margin + 1.0f + col * 18.0f;
            float wy = 15.0f + row * 20.0f;

            float frameR = 0.4f, frameG = 0.6f, frameB = 0.8f;
            fillRect(wx - 1, wy - 1, 14, 16, frameR, frameG, frameB);

            float glass_wr, glass_wg, glass_wb;
            if ((row + col) % 2 == 0) { glass_wr = 0.1f; glass_wg = 0.8f; glass_wb = 0.95f; }
            else { glass_wr = 0.05f; glass_wg = 0.4f; glass_wb = 0.85f; }

            if ((row + col * 3) % 7 == 0) {
                fillRect(wx + 1, wy + 1, 10, 12, 1.0f, 0.95f, 0.2f);
            } else {
                fillRect(wx + 1, wy + 1, 10, 12, glass_wr, glass_wg, glass_wb);
            }
        }
    }
    glPopMatrix();
}

void draw_cutout_building(float ox, float oy, float bw, float bh, float r, float g, float b) {
    glPushMatrix();
    glTranslatef(ox, oy, 0);

    fillRect(0, 0, bw, bh, r, g, b);
    fillRect(-5, bh, bw+10, 15, r*0.7f, g*0.7f, b*0.7f);

    int cols = (int)((bw - 10.0f) / 18.0f);
    int rows = (int)(bh / 20.0f) - 1;
    float block_width = (cols > 0) ? ((cols - 1) * 18.0f + 14.0f) : 0;
    float x_margin = (bw - block_width) / 2.0f;

    for (int row = 0; row < rows; ++row) {
        if (row % 4 == 3) {
            fillRect(-2, 15.0f + row * 20.0f - 2, bw + 4, 18, 0.10f, 0.12f, 0.18f);
        } else {
            for (int col = 0; col < cols; ++col) {
                float wx = x_margin + 1.0f + col * 18.0f;
                float wy = 15.0f + row * 20.0f;

                float frameR = 0.4f, frameG = 0.6f, frameB = 0.8f;
                fillRect(wx - 1, wy - 1, 14, 16, frameR, frameG, frameB);

                float glass_wr, glass_wg, glass_wb;
                if ((row + col) % 2 == 0) { glass_wr = 0.1f; glass_wg = 0.8f; glass_wb = 0.95f; }
                else { glass_wr = 0.05f; glass_wg = 0.4f; glass_wb = 0.85f; }

                if ((row + col * 3) % 7 == 0) {
                    fillRect(wx + 1, wy + 1, 10, 12, 1.0f, 0.95f, 0.2f);
                } else {
                    fillRect(wx + 1, wy + 1, 10, 12, glass_wr, glass_wg, glass_wb);
                }
            }
        }
    }
    glPopMatrix();
}

void draw_spire_tower(float ox, float oy) {
    glPushMatrix();
    glTranslatef(ox, oy, 0);

    fillRect(-40, 0, 80, 150, 0.20f, 0.25f, 0.35f);
    fillRect(-50, 0, 15, 100, 0.15f, 0.20f, 0.28f);
    fillRect(35, 0, 15, 100, 0.15f, 0.20f, 0.28f);

    glColor3f(0.10f, 0.12f, 0.15f);
    glLineWidth(2.0f);
    glBegin(GL_LINES);
    for (int y = 15; y < 100; y += 15) {
        glVertex2f(-50, y); glVertex2f(-35, y);
        glVertex2f(35, y);  glVertex2f(50, y);
    }
    for (int y = 20; y < 130; y += 35) {
        glVertex2f(-35, y);       glVertex2f(35, y + 35);
        glVertex2f(-35, y + 35);  glVertex2f(35, y);
    }
    glEnd();
    glLineWidth(1.0f);

    fillRect(-55, 0, 110, 12, 0.15f, 0.18f, 0.22f);
    fillRect(-60, 0, 120, 6,  0.10f, 0.12f, 0.15f);
    fillRect(-45, 140, 90, 10, 0.12f, 0.15f, 0.20f);

    fillRect(-30, 150, 60, 180, 0.25f, 0.30f, 0.40f);
    fillRect(-35, 150, 5, 160, 0.18f, 0.22f, 0.28f);
    fillRect(30, 150, 5, 160, 0.18f, 0.22f, 0.28f);

    for(int i = 0; i < 3; ++i) {
        fillRect(-20 + i*16, 155, 8, 170, 0.35f, 0.75f, 1.0f);
    }

    fillRect(-40, 270, 80, 8, 0.10f, 0.12f, 0.15f);
    fillTri(-30, 330, 30, 330, 0, 310, 0.15f, 0.18f, 0.22f);

    fillCircle(0, 335, 45, 0.40f, 0.45f, 0.50f, 24);
    fillCircle(0, 335, 35, 0.50f, 0.85f, 1.0f, 24);
    fillRect(-45, 330, 90, 8, 0.10f, 0.12f, 0.15f);
    fillRect(-10, 335, 20, 40, 0.30f, 0.35f, 0.40f);
    fillTri(-6, 375, 6, 375, 0, 480, 0.55f, 0.60f, 0.65f);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    float pulse = (sinf(bob_time * 8.0f) + 1.0f) * 0.5f;
    glColor4f(1.0f, 0.0f, 0.3f, 0.3f + 0.6f * pulse);

    glBegin(GL_TRIANGLE_FAN);
    glVertex2f(0, 480);
    for (int i = 0; i <= 16; ++i) {
        float a = i * 2.0f * 3.14159265f / 16.0f;
        glVertex2f(0 + cosf(a)*15.0f, 480 + sinf(a)*15.0f);
    }
    glEnd();
    glDisable(GL_BLEND);

    fillCircle(0, 480, 3.5f, 1.0f, 0.1f, 0.2f, 12);
    glPopMatrix();
}

void draw_burj_khalifa(float ox, float oy) {
    glPushMatrix();
    glTranslatef(ox, oy, 0);

    float r = 0.75f, g = 0.76f, b = 0.78f; //base color
    float tiers[4][3] = {
        {0.0f, 100.0f, 150.0f},
        {150.0f, 70.0f, 120.0f},
        {270.0f, 40.0f, 100.0f},
        {370.0f, 20.0f, 80.0f}
    };

    for (int t = 0; t < 4; ++t) {
        float t_y = tiers[t][0];
        float t_w = tiers[t][1];
        float t_h = tiers[t][2];
        float x_offset = -t_w / 2.0f;

        // Draw tier base
        fillRect(x_offset, t_y, t_w, t_h, r, g, b);
        // Draw tier roof lip
        fillRect(x_offset - 2.0f, t_y + t_h, t_w + 4.0f, 6.0f, r * 0.7f, g * 0.7f, b * 0.7f);

        // Draw windows for this tier
        int cols = (int)((t_w - 4.0f) / 14.0f);
        int rows = (int)(t_h / 18.0f) - 1;
        float block_width = (cols > 0) ? ((cols - 1) * 14.0f + 10.0f) : 0;
        float x_margin = x_offset + (t_w - block_width) / 2.0f;

        for (int row = 0; row < rows; ++row) {
            for (int col = 0; col < cols; ++col) {
                float wx = x_margin + col * 14.0f;
                float wy = t_y + 10.0f + row * 18.0f;

                fillRect(wx - 1, wy - 1, 10, 14, 0.4f, 0.6f, 0.8f);

                float glass_wr, glass_wg, glass_wb;
                if ((row + col + t) % 2 == 0) { glass_wr = 0.1f; glass_wg = 0.8f; glass_wb = 0.95f; }
                else { glass_wr = 0.05f; glass_wg = 0.4f; glass_wb = 0.85f; }

                if ((row + col * 3 + t) % 7 == 0) {
                    fillRect(wx + 1, wy + 1, 6, 10, 1.0f, 0.95f, 0.2f);
                } else {
                    fillRect(wx + 1, wy + 1, 6, 10, glass_wr, glass_wg, glass_wb);
                }
            }
        }
    }

    // Top Spire
    fillRect(-2.0f, 450.0f, 4.0f, 45.0f, 0.6f, 0.6f, 0.65f);
    fillCircle(0.0f, 495.0f, 3.0f, 1.0f, 0.1f, 0.1f);

    glPopMatrix();
}

void draw_billboard(float ox, float oy) {
    glPushMatrix();
    glTranslatef(ox, oy, 0);

    // White Pillar
    fillRect(-4, 0, 8, 90, 1.0f, 1.0f, 1.0f);

    // Dark Frame
    fillRect(-40, 90, 80, 50, 0.15f, 0.15f, 0.15f);

    // Screen/ Grey-White
    fillRect(-36, 94, 72, 42, 0.85f, 0.88f, 0.90f);

    // Inner design
    fillCircle(0, 115, 12, 0.4f, 0.4f, 0.45f, 20);

    glPopMatrix();
}

void draw_scene2_cityscape() {
    draw_modern_building(50, 160, 120, 300, 0.15f, 0.22f, 0.35f);
    draw_billboard(185, 160);
    draw_modern_building(200, 160, 100, 400, 0.20f, 0.28f, 0.40f);
    draw_spire_tower(400, 160);
    draw_cutout_building(550, 160, 180, 250, 0.18f, 0.24f, 0.32f);
    draw_billboard(750, 160);
    draw_modern_building(780, 160, 140, 350, 0.22f, 0.30f, 0.42f);
    draw_burj_khalifa(1050, 160);
    draw_big_tree(350, 150, 0.5f);
    draw_big_tree(950, 150, 0.6f);
}

void draw_footpath(float offset) {
    fillRect(0, 0, (float)WIDTH, 25, 0.45f, 0.45f, 0.45f);
    float shift = fmod(offset, 35.0f);
    glColor3f(0.35f, 0.35f, 0.35f);
    glLineWidth(2.0f);
    glBegin(GL_LINES);
    for(float x = -shift; x < WIDTH + 35.0f; x += 35.0f) {
        glVertex2f(x, 0.0f);
        glVertex2f(x, 25.0f);
    }
    glVertex2f(0.0f, 25.0f);
    glVertex2f((float)WIDTH, 25.0f);
    glEnd();
    glLineWidth(1.0f);
}

void draw_person(float ox, float oy, float r, float g, float b, bool facing_right) {
    glPushMatrix();
    glTranslatef(ox, oy, 0);
    if (!facing_right) glScalef(-1.0f, 1.0f, 1.0f);

    float leg_swing = sinf(ox * 0.1f) * 5.0f;

    fillRect(-4, 8, 8, 12, r, g, b);
    fillCircle(0, 24, 4.5f, 0.9f, 0.7f, 0.6f, 12);

    glColor3f(0.10f, 0.10f, 0.20f);
    glLineWidth(3.0f);
    glBegin(GL_LINES);
        glVertex2f(-2.0f, 8.0f); glVertex2f(-2.0f + leg_swing, 0.0f);
        glVertex2f(2.0f, 8.0f);  glVertex2f(2.0f - leg_swing, 0.0f);
    glEnd();
    glLineWidth(1.0f);
    glPopMatrix();
}

void draw_highway(float offset) {
    float hy = 25.0f, hh = 135.0f;

    glBegin(GL_QUADS);
        glColor3f(0.12f,0.12f,0.12f); glVertex2f(0, hy); glVertex2f((float)WIDTH, hy);
        glColor3f(0.18f,0.18f,0.18f); glVertex2f((float)WIDTH, hy+hh); glVertex2f(0, hy+hh);
    glEnd();

    fillRect(0, hy, (float)WIDTH, 8, 0.5f, 0.5f, 0.5f);
    fillRect(0, hy+8, (float)WIDTH, 3, 0.7f, 0.7f, 0.7f);

    float dashW = 60.0f, gapW = 40.0f;
    float cy = hy + hh * 0.5f - 4.0f;
    float dash_shift = fmod(offset, dashW + gapW);

    for (float x = -dash_shift; x < WIDTH + (dashW + gapW); x += dashW + gapW)
        fillRect(x, cy, dashW, 8, 1.0f, 0.85f, 0.0f);

    fillRect(0, hy + hh - 20, (float)WIDTH, 20, 0.35f, 0.35f, 0.35f);
    fillRect(0, hy + hh - 25, (float)WIDTH, 5, 0.65f, 0.65f, 0.65f);
}

void draw_car(float ox, float oy, float r, float g, float b, bool facing_right) {
    glPushMatrix();
    glTranslatef(ox, oy, 0);
    if (!facing_right) glScalef(-1.0f, 1.0f, 1.0f);

    fillRect(0, 10, 70, 20, r, g, b);

    glBegin(GL_QUADS);
        glColor3f(0.85f * r, 0.85f * g, 0.85f * b);
        glVertex2f(15, 30); glVertex2f(50, 30); glVertex2f(45, 45); glVertex2f(25, 45);
    glEnd();

    glBegin(GL_QUADS);
        glColor3f(0.2f, 0.7f, 1.0f);
        glVertex2f(26, 31); glVertex2f(34, 31); glVertex2f(34, 43); glVertex2f(28, 43);
        glVertex2f(36, 31); glVertex2f(48, 31); glVertex2f(43, 43); glVertex2f(36, 43);
    glEnd();

    fillCircle(18, 10, 10, 0.05f, 0.05f, 0.05f, 32);
    fillCircle(52, 10, 10, 0.05f, 0.05f, 0.05f, 32);
    fillCircle(18, 10, 4, 0.7f, 0.7f, 0.7f, 32);
    fillCircle(52, 10, 4, 0.7f, 0.7f, 0.7f, 32);

    fillRect(65, 18, 5, 6, 1.0f, 1.0f, 0.2f);
    fillRect(0, 18, 5, 6, 1.0f, 0.1f, 0.1f);
    glPopMatrix();
}

void draw_street_lamp(float ox, float oy) {
    glPushMatrix();
    glTranslatef(ox, oy, 0);

    // Pole - White
    fillRect(-3, 0, 6, 110, 1.0f, 1.0f, 1.0f);
    // Arm - White
    fillRect(-25, 105, 25, 5, 1.0f, 1.0f, 1.0f);
    // Bulb housing
    fillRect(-30, 100, 15, 8, 0.15f, 0.15f, 0.15f);

    // Light bulb - Bright white
    fillCircle(-22.5f, 100, 5, 1.0f, 1.0f, 1.0f, 16);


    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glColor4f(1.0f, 1.0f, 1.0f, 0.35f);
    glBegin(GL_POLYGON);
        glVertex2f(-22.5f, 100.0f);
        glVertex2f(-60.0f, 0.0f);
        glVertex2f(15.0f, 0.0f);
    glEnd();
    glDisable(GL_BLEND);

    glPopMatrix();
}

// ..........................................................................
// ................................ SCENE 3 .................................
// ..........................................................................

void s3_drawSky() {
    glBegin(GL_QUADS);
    glColor3f(0.38f, 0.68f, 0.88f);
    glVertex2f(-1.0f, -0.18f); glVertex2f(1.0f, -0.18f);
    glColor3f(0.28f, 0.58f, 0.82f);
    glVertex2f(1.0f,  1.0f);  glVertex2f(-1.0f,  1.0f);
    glEnd();
}

void s3_drawCircle(float cx, float cy, float r) {
    glBegin(GL_POLYGON);
    for (int i = 0; i < 60; i++) {
        float a = i * 2.0f * s3_PI / 60.0f;
        glVertex2f(cx + cosf(a) * r, cy + sinf(a) * r);
    }
    glEnd();
}

void s3_drawSun(float cx, float cy, float r) {
    glColor3f(1.0f, 0.88f, 0.10f);
    s3_drawCircle(cx, cy, r);

    glColor3f(1.0f, 0.85f, 0.10f);
    glLineWidth(2.5f);
    glBegin(GL_LINES);
    for (int i = 0; i < 12; i++) {
        float a = i * s3_PI / 6.0f;
        float inner = r * 1.08f, outer = r * 1.50f;
        glVertex2f(cx + cosf(a) * inner, cy + sinf(a) * inner);
        glVertex2f(cx + cosf(a) * outer, cy + sinf(a) * outer);
    }
    glEnd();
}

void s3_drawCloud(float cx, float cy, float s) {
    glColor3f(1.0f, 1.0f, 1.0f);
    s3_drawCircle(cx, cy, s * 1.1f);
    s3_drawCircle(cx - s * 0.8f, cy - s * 0.1f, s * 0.85f);
    s3_drawCircle(cx - s * 1.4f, cy - s * 0.2f, s * 0.60f);
    s3_drawCircle(cx + s * 0.8f, cy - s * 0.1f, s * 0.85f);
    s3_drawCircle(cx + s * 1.4f, cy - s * 0.2f, s * 0.60f);
    s3_drawCircle(cx - s * 0.4f, cy + s * 0.5f, s * 0.75f);
    s3_drawCircle(cx + s * 0.4f, cy + s * 0.5f, s * 0.75f);
    s3_drawCircle(cx - s * 0.9f, cy - s * 0.3f, s * 0.55f);
    s3_drawCircle(cx + s * 0.9f, cy - s * 0.3f, s * 0.55f);
    s3_drawCircle(cx,             cy - s * 0.3f, s * 0.70f);
}

void s3_drawClouds() {
    struct S3Cloud { float x, y, scale; };
    S3Cloud clouds[] = {
        {0.38f, 0.76f, 0.045f},
        {-0.72f, 0.78f, 0.048f},
        {-0.14f, 0.70f, 0.038f},
        { 0.28f, 0.75f, 0.035f}
    };

    float s3_offset = cloud_offset * (2.0f / WIDTH);

    for (int i = 0; i < 4; ++i) {
        float current_x = clouds[i].x + s3_offset;
        current_x = fmod(current_x + 1.5f, 3.0f) - 1.5f;
        s3_drawCloud(current_x, clouds[i].y, clouds[i].scale);
    }
}

void s3_drawBird(float x, float y, float s) {
    glColor3f(0.05f, 0.05f, 0.05f);
    glLineWidth(1.8f);
    glBegin(GL_LINE_STRIP);
    glVertex2f(x - s,       y - s*0.45f);
    glVertex2f(x - s*0.28f, y);
    glVertex2f(x,           y - s*0.65f);
    glVertex2f(x + s*0.28f, y);
    glVertex2f(x + s,       y - s*0.45f);
    glEnd();
}

void s3_drawQuad(float x0, float y0, float x1, float y1) {
    glBegin(GL_QUADS);
    glVertex2f(x0, y0); glVertex2f(x1, y0);
    glVertex2f(x1, y1); glVertex2f(x0, y1);
    glEnd();
}

void s3_drawMountain(float bx, float by, float w, float h) {
    float px0 = bx - w * 0.05f; float py0 = by + h * 0.96f;
    float px1 = bx;             float py1 = by + h;
    float px2 = bx + w * 0.06f; float py2 = by + h * 0.94f;
    float lx = bx - w * 0.5f;
    float rx = bx + w * 0.5f;

    glColor3f(0.42f, 0.45f, 0.50f);
    glBegin(GL_POLYGON);
    glVertex2f(lx, by); glVertex2f(px0, py0); glVertex2f(px1, py1); glVertex2f(bx, by);
    glEnd();

    glColor3f(0.30f, 0.32f, 0.36f);
    glBegin(GL_POLYGON);
    glVertex2f(bx, by); glVertex2f(px1, py1); glVertex2f(px2, py2); glVertex2f(rx, by);
    glEnd();

    glColor3f(0.94f, 0.94f, 0.96f);
    glBegin(GL_POLYGON);
    glVertex2f(px0, py0);
    glVertex2f(px1, py1);
    glVertex2f(px2, py2);
    glVertex2f(px2 + (rx - px2) * 0.25f, py2 - h * 0.22f);
    glVertex2f(bx + w * 0.02f, by + h * 0.72f);
    glVertex2f(bx - w * 0.03f, by + h * 0.68f);
    glVertex2f(px0 - (px0 - lx) * 0.22f, py0 - h * 0.20f);
    glEnd();
}

void s3_drawPineTree(float x, float y, float height) {
    float s = height;

    glColor3f(0.42f, 0.26f, 0.09f);
    glBegin(GL_QUADS);
    glVertex2f(x - s*0.06f, y);             glVertex2f(x + s*0.06f, y);
    glVertex2f(x + s*0.045f, y + s*0.28f);  glVertex2f(x - s*0.045f, y + s*0.28f);
    glEnd();

    glColor3f(0.07f, 0.38f, 0.13f);
    glBegin(GL_TRIANGLES);
    glVertex2f(x - s*0.52f, y + s*0.18f); glVertex2f(x + s*0.52f, y + s*0.18f); glVertex2f(x, y + s*0.64f);
    glEnd();

    glColor3f(0.09f, 0.42f, 0.16f);
    glBegin(GL_TRIANGLES);
    glVertex2f(x - s*0.38f, y + s*0.50f); glVertex2f(x + s*0.38f, y + s*0.50f); glVertex2f(x, y + s*0.88f);
    glEnd();

    glColor3f(0.12f, 0.46f, 0.19f);
    glBegin(GL_TRIANGLES);
    glVertex2f(x - s*0.25f, y + s*0.76f); glVertex2f(x + s*0.25f, y + s*0.76f); glVertex2f(x, y + s*1.08f);
    glEnd();

    glColor3f(0.18f, 0.55f, 0.24f);
    glBegin(GL_TRIANGLES);
    glVertex2f(x - s*0.20f, y + s*0.80f); glVertex2f(x, y + s*1.08f); glVertex2f(x, y + s*0.84f);
    glEnd();
}

void s3_drawGroundLayers() {
    glColor3f(0.22f, 0.50f, 0.18f); s3_drawQuad(-1.0f, -0.24f, 1.0f, -0.18f);
    glColor3f(0.60f, 0.36f, 0.16f); s3_drawQuad(-1.0f, -0.42f, 1.0f, -0.24f);
    glColor3f(0.44f, 0.24f, 0.10f); s3_drawQuad(-1.0f, -0.46f, 1.0f, -0.42f);

    glColor3f(0.38f, 0.20f, 0.08f);
    glPointSize(3.5f);
    glBegin(GL_POINTS);
    for (float sx = -0.96f; sx < 0.96f; sx += 0.068f) {
        glVertex2f(sx,         -0.28f);
        glVertex2f(sx + 0.03f, -0.34f);
        glVertex2f(sx - 0.02f, -0.40f);
    }
    glEnd();

    glColor3f(0.32f, 0.16f, 0.06f);
    glPointSize(4.0f);
    glBegin(GL_POINTS);
    for (float sx = -0.94f; sx < 0.94f; sx += 0.10f) {
        glVertex2f(sx, -0.44f);
    }
    glEnd();

    glColor3f(0.0f, 0.0f, 0.0f);     s3_drawQuad(-1.0f, -0.84f, 1.0f, -0.46f);
    glColor3f(0.14f, 0.16f, 0.22f);  s3_drawQuad(-1.0f, -0.80f, 1.0f, -0.78f);

    glColor3f(0.55f, 0.55f, 0.58f);
    glLineWidth(4.5f);
    glBegin(GL_LINES);
    glVertex2f(-1.0f, -0.792f); glVertex2f(1.0f, -0.792f);
    glVertex2f(-1.0f, -0.796f); glVertex2f(1.0f, -0.796f);
    glEnd();

    glColor3f(0.60f, 0.36f, 0.16f); s3_drawQuad(-1.0f, -0.96f, 1.0f, -0.84f);
    glColor3f(0.44f, 0.24f, 0.10f); s3_drawQuad(-1.0f, -1.0f, 1.0f, -0.96f);

    glColor3f(0.38f, 0.20f, 0.08f);
    glPointSize(3.5f);
    glBegin(GL_POINTS);
    for (float sx = -0.96f; sx < 0.96f; sx += 0.068f) {
        glVertex2f(sx,         -0.88f);
        glVertex2f(sx + 0.03f, -0.91f);
        glVertex2f(sx - 0.02f, -0.94f);
    }
    glEnd();

    glColor3f(0.32f, 0.16f, 0.06f);
    glPointSize(4.0f);
    glBegin(GL_POINTS);
    for (float sx = -0.94f; sx < 0.94f; sx += 0.10f) {
        glVertex2f(sx, -0.98f);
    }
    glEnd();
}

void draw_scene3_scrolling_elements() {
    float birds[][3] = {
        {-0.62f, 0.60f, 0.022f}, {-0.52f, 0.66f, 0.020f}, {-0.56f, 0.55f, 0.018f},
        {-0.38f, 0.62f, 0.021f}, {-0.28f, 0.56f, 0.019f},
        {-0.02f, 0.68f, 0.022f}, { 0.06f, 0.60f, 0.018f},
        { 0.82f, 0.62f, 0.022f}, { 0.90f, 0.68f, 0.020f}, { 0.87f, 0.55f, 0.018f},
        { 0.96f, 0.62f, 0.019f}
    };
    for (auto& b : birds) s3_drawBird(b[0], b[1], b[2]);

    s3_drawMountain(-0.68f, -0.18f, 0.72f, 0.58f);
    s3_drawMountain( 0.02f, -0.18f, 0.80f, 0.66f);
    s3_drawMountain( 0.62f, -0.18f, 0.70f, 0.58f);
    s3_drawMountain(-0.22f, -0.18f, 0.48f, 0.40f);

    s3_drawGroundLayers();

    s3_drawPineTree(-0.90f, -0.18f, 0.32f);
    s3_drawPineTree(-0.70f, -0.18f, 0.28f);
    s3_drawPineTree(-0.48f, -0.18f, 0.30f);
    s3_drawPineTree(-0.02f, -0.18f, 0.40f);
    s3_drawPineTree( 0.22f, -0.18f, 0.30f);
    s3_drawPineTree( 0.50f, -0.18f, 0.28f);
    s3_drawPineTree( 0.72f, -0.18f, 0.32f);
    s3_drawPineTree( 0.92f, -0.18f, 0.30f);
}

void s3_drawCar(float x, float y, float w, float h) {
    glColor3f(0.90f, 0.90f, 0.92f);  s3_drawQuad(x, y, x + w, y + h);
    glColor3f(0.74f, 0.74f, 0.77f);  s3_drawQuad(x, y, x + w, y + h * 0.20f);
    glColor3f(0.82f, 0.04f, 0.04f);  s3_drawQuad(x, y + h - 0.012f, x + w, y + h);

    float dW = w * 0.215f, dH = h * 0.83f, dY = y + h * 0.09f;
    float dX[2] = { x + w * 0.17f, x + w * 0.60f };

    for (int d = 0; d < 2; d++) {
        float dx = dX[d];
        glColor3f(0.70f, 0.70f, 0.72f);
        s3_drawQuad(dx, dY, dx + dW, dY + dH);

        glBegin(GL_QUADS);
        glColor3f(0.92f, 0.60f, 0.18f);
        glVertex2f(dx + 0.007f, dY + 0.008f); glVertex2f(dx + dW - 0.007f, dY + 0.008f);
        glColor3f(1.00f, 0.88f, 0.52f);
        glVertex2f(dx + dW - 0.007f, dY + dH - 0.008f); glVertex2f(dx + 0.007f, dY + dH - 0.008f);
        glEnd();

        glColor3f(0.86f, 0.04f, 0.04f);
        s3_drawQuad(dx + dW*0.30f, dY + dH + 0.003f, dx + dW*0.70f, dY + dH + 0.013f);
    }

    float wW = w * 0.14f, wH = h * 0.33f, wY = y + h * 0.46f;
    float wX[3] = { x + w * 0.03f, x + w * 0.43f, x + w * 0.83f };

    for (int i = 0; i < 3; i++) {
        float wx = wX[i];
        if (wx + wW > x + w - 0.005f) continue;

        glBegin(GL_QUADS);
        glColor3f(0.92f, 0.60f, 0.18f);
        glVertex2f(wx, wY); glVertex2f(wx + wW, wY);
        glColor3f(1.00f, 0.88f, 0.52f);
        glVertex2f(wx + wW, wY + wH); glVertex2f(wx, wY + wH);
        glEnd();

        glColor3f(0.58f, 0.58f, 0.60f);
        glLineWidth(1.4f);
        glBegin(GL_LINE_LOOP);
        glVertex2f(wx, wY); glVertex2f(wx + wW, wY);
        glVertex2f(wx + wW, wY + wH); glVertex2f(wx, wY + wH);
        glEnd();
    }

    float wr = 0.036f, wyc = -0.792f + wr;
    float wxs[4] = { x + w*0.16f, x + w*0.28f, x + w*0.68f, x + w*0.80f };

    glColor3f(0.30f, 0.30f, 0.32f);
    s3_drawQuad(x + w*0.10f, wyc - 0.010f, x + w*0.34f, wyc + 0.012f);
    s3_drawQuad(x + w*0.62f, wyc - 0.010f, x + w*0.86f, wyc + 0.012f);

    glColor3f(0.18f, 0.18f, 0.20f);
    for (int i = 0; i < 4; i++) s3_drawCircle(wxs[i], wyc, wr);

    glColor3f(0.50f, 0.50f, 0.54f);
    for (int i = 0; i < 4; i++) s3_drawCircle(wxs[i], wyc, wr * 0.45f);

    glColor3f(0.62f, 0.62f, 0.64f);
    glLineWidth(1.8f);
    glBegin(GL_LINES);
    glVertex2f(x + w, y); glVertex2f(x + w, y + h);
    glEnd();
}

void s3_drawMetroTrain_Moving() {
    const int   NUM_CARS = 5;
    const float CAR_W   = 0.345f;
    const float CAR_H   = 0.220f;
    const float GAP     = 0.003f;
    float wr = 0.036f;
    float wyc = -0.792f + wr;
    float BASE_Y  = wyc + 0.018f;
    const float START_X = -0.88f;
    float cabX = START_X - 0.085f;
    float y0   = BASE_Y;

    glColor3f(0.92f, 0.92f, 0.94f);
    glBegin(GL_POLYGON);
    glVertex2f(START_X, y0);
    glVertex2f(START_X, y0 + CAR_H);
    glVertex2f(cabX + 0.04f, y0 + CAR_H);
    glVertex2f(cabX, y0 + CAR_H * 0.40f);
    glVertex2f(cabX + 0.01f, y0);
    glEnd();

    glColor3f(0.82f, 0.04f, 0.04f);
    glBegin(GL_POLYGON);
    glVertex2f(START_X, y0 + CAR_H - 0.012f);
    glVertex2f(START_X, y0 + CAR_H);
    glVertex2f(cabX + 0.04f, y0 + CAR_H);
    glVertex2f(cabX + 0.035f, y0 + CAR_H - 0.012f);
    glEnd();

    glColor3f(0.12f, 0.12f, 0.16f);
    glBegin(GL_POLYGON);
    glVertex2f(START_X - 0.005f, y0 + CAR_H * 0.42f);
    glVertex2f(START_X - 0.005f, y0 + CAR_H - 0.045f);
    glVertex2f(cabX + 0.045f,    y0 + CAR_H - 0.045f);
    glVertex2f(cabX + 0.012f,    y0 + CAR_H * 0.42f);
    glEnd();

    for (int i = 0; i < NUM_CARS; i++) {
        float cx = START_X + i * (CAR_W + GAP);
        s3_drawCar(cx, BASE_Y, CAR_W, CAR_H);
    }
}


// ..........................................................................
// ................................ SCENE 4 .................................
// ..........................................................................

void s4_sky() {
    glBegin(GL_QUADS);
    glColor3f(0.0, 0.35, 0.75);
    glVertex2f(0,600);
    glVertex2f(1000,600);
    glColor3f(0.80, 0.95, 1.0);
    glVertex2f(1000,300);
    glVertex2f(0,300);
    glEnd();
}

void s4_Sun() {
    glBegin(GL_TRIANGLE_FAN);
    glColor3f(1.0f, 0.85f, 0.1f);
    glVertex2f(490, 350);
    for(int i = 0; i <= 360; i++) {
        float angle = i * 3.14159f / 180.0f;
        float x = 490 + 40 * cos(angle);
        float y = 350 + 40 * sin(angle);
        glVertex2f(x, y);
    }
    glEnd();
}

void s4_drawCircle(float cx, float cy, float r) {
    glBegin(GL_TRIANGLE_FAN);
    glVertex2f(cx, cy);
    for(int i = 0; i <= 360; i++) {
        float angle = i * 3.1416 / 180.0;
        float x = cx + r * cos(angle);
        float y = cy + r * sin(angle);
        glVertex2f(x, y);
    }
    glEnd();
}

void s4_Cloud(float x, float y) {
    glColor3f(1.0, 1.0, 1.0);
    s4_drawCircle(x, y, 20);
    s4_drawCircle(x + 18, y + 8, 18);
    s4_drawCircle(x + 35, y, 20);
    s4_drawCircle(x + 15, y - 8, 18);
    s4_drawCircle(x + 28, y - 8, 16);
}

void s4_draw_clouds() {
    float s4_offset = fmod(cloud_offset, 1000.0f);
    glPushMatrix();
    glTranslatef(s4_offset, 0.0f, 0.0f);
    s4_Cloud(120, 420);
    s4_Cloud(300, 450);
    s4_Cloud(650, 430);

    glTranslatef(-1000.0f, 0.0f, 0.0f);
    s4_Cloud(120, 420);
    s4_Cloud(300, 450);
    s4_Cloud(650, 430);
    glPopMatrix();
}

void s4_ground() {
    glBegin(GL_QUADS);
    glColor3f(0.20, 0.75, 0.25);
    glVertex2f(0, 210);
    glVertex2f(1000, 210);
    glVertex2f(1000, 200);
    glVertex2f(0, 200);
    glEnd();

    glBegin(GL_QUADS);
    glColor3f(0.45, 0.30, 0.15);
    glVertex2f(0, 200);
    glVertex2f(1000, 200);
    glVertex2f(1000, 188);
    glVertex2f(0, 188);
    glEnd();
}

void s4_Field() {
    glBegin(GL_QUADS);
    glColor3f(0.80f, 0.60f, 0.30f);
    glVertex2f(0, 250);
    glVertex2f(0, 300);
    glVertex2f(1000, 300);
    glColor3f(0.45f, 0.32f, 0.23f);
    glVertex2f(1000, 250);
    glEnd();
}

void s4_Field2() {
    glBegin(GL_QUADS);
    glColor3f(0.4f, 0.8f, 0.2f);
    glVertex2f(0, 200);
    glVertex2f(0, 300);
    glVertex2f(1000, 300);
    glVertex2f(1000, 200);
    glEnd();

    glLineWidth(2);
    glBegin(GL_LINE_LOOP);
    glColor3f(0.10f, 0.30f, 0.05f);
    glVertex2f(0, 200);
    glVertex2f(0, 300);
    glVertex2f(1000, 300);
    glVertex2f(1000, 200);
    glEnd();
}

void s4_GrassBlade(float x, float y, float s) {
    glBegin(GL_TRIANGLES);
    glColor3f(0.0f, 0.5f, 0.0f);
    glVertex2f(x, y);
    glVertex2f(x - 3*s, y + 10*s);
    glVertex2f(x - 1*s, y + 10*s);

    glVertex2f(x + 2*s, y);
    glVertex2f(x + 1*s, y + 12*s);
    glVertex2f(x + 3*s, y + 12*s);

    glVertex2f(x + 4*s, y);
    glVertex2f(x + 6*s, y + 10*s);
    glVertex2f(x + 5*s, y + 10*s);
    glEnd();
}

void s4_DrawBezierRoad1() {
    float x0 = 200, y0 = 265;
    float x1 = 760, y1 = 280;
    float x2 = 400, y2 = 210;
    float x3 = 0, y3 = 209;

    glBegin(GL_LINE_STRIP);
    for(float t = 0; t <= 1.0; t += 0.01) {
        float u = 1 - t;
        float x = u*u*u*x0 + 3*u*u*t*x1 + 3*u*t*t*x2 + t*t*t*x3;
        float y = u*u*u*y0 + 3*u*u*t*y1 + 3*u*t*t*y2 + t*t*t*y3;
        glVertex2f(x, y);
    }
    glEnd();
}

void s4_DrawBezierRoad2() {
    float x0 = 1000, y0 = 209;
    float x1 = 760, y1 = 240;
    float x2 = 600, y2 = 210;
    float x3 = 237, y3 = 215;

    glBegin(GL_LINE_STRIP);
    for(float t = 0; t <= 1.0; t += 0.01) {
        float u = 1 - t;
        float x = u*u*u*x0 + 3*u*u*t*x1 + 3*u*t*t*x2 + t*t*t*x3;
        float y = u*u*u*y0 + 3*u*u*t*y1 + 3*u*t*t*y2 + t*t*t*y3;
        glVertex2f(x, y);
    }
    glEnd();
}

void s4_Road() {
    glColor3f(0.22, 0.15, 0.10);
    glLineWidth(10);
    s4_DrawBezierRoad1();
    s4_DrawBezierRoad2();

    glColor3f(0.35, 0.25, 0.15);
    glLineWidth(7);
    s4_DrawBezierRoad1();
    s4_DrawBezierRoad2();
}

void s4_River() {
    glBegin(GL_QUADS);
    glColor3f(0.20, 0.60, 0.95);
    glVertex2f(0, 188);
    glVertex2f(1000, 188);
    glColor3f(0.05, 0.20, 0.55);
    glVertex2f(1000, 0);
    glVertex2f(0, 0);
    glEnd();

    for(int i = 0; i < 3; i++) {
        float baseY = 160 - i * 40;
        glColor4f(0.8, 0.95, 1.0, 0.25);
        glBegin(GL_TRIANGLE_STRIP);
        for(int x = 0; x <= 1000; x += 20) {
            float wave = 5 * sin((x * 0.0188495f) + i);
            glVertex2f(x, baseY + wave);
            glVertex2f(x, baseY + 10 + wave);
        }
        glEnd();
    }

    glBegin(GL_TRIANGLE_STRIP);
    glColor3f(0.70, 0.90, 1.0);
    glVertex2f(0, 188);
    glVertex2f(1000, 188);
    glColor3f(0.20, 0.60, 0.95);
    glVertex2f(0, 175);
    glVertex2f(1000, 175);
    glEnd();
}

void s4_Mountain1(float i, float j) {
    glBegin(GL_TRIANGLE_FAN);
    glColor3f(0.32, 0.55, 0.50);
    glVertex2f(i + 180, j + 340);
    glVertex2f(i + 0,   j + 150);
    glVertex2f(i + 30,  j + 175);
    glVertex2f(i + 60,  j + 205);
    glVertex2f(i + 90,  j + 240);
    glVertex2f(i + 120, j + 280);
    glVertex2f(i + 150, j + 315);
    glVertex2f(i + 210, j + 320);
    glVertex2f(i + 240, j + 290);
    glVertex2f(i + 270, j + 255);
    glVertex2f(i + 300, j + 225);
    glVertex2f(i + 330, j + 195);
    glVertex2f(i + 360, j + 170);
    glVertex2f(i + 390, j + 150);
    glVertex2f(i + 0, j + 150);
    glEnd();

    glEnable(GL_LINE_SMOOTH);
    glLineWidth(2.0);
    glBegin(GL_LINE_LOOP);
    glColor3f(0.20, 0.38, 0.35);
    glVertex2f(i + 0,   j + 150);
    glVertex2f(i + 30,  j + 175);
    glVertex2f(i + 60,  j + 205);
    glVertex2f(i + 90,  j + 240);
    glVertex2f(i + 120, j + 280);
    glVertex2f(i + 150, j + 315);
    glVertex2f(i + 180, j + 340);
    glVertex2f(i + 210, j + 320);
    glVertex2f(i + 240, j + 290);
    glVertex2f(i + 270, j + 255);
    glVertex2f(i + 300, j + 225);
    glVertex2f(i + 330, j + 195);
    glVertex2f(i + 360, j + 170);
    glVertex2f(i + 390, j + 150);
    glEnd();
}

void s4_Mountain2(float i, float j) {
    glBegin(GL_TRIANGLE_FAN);
    glColor3f(0.45, 0.65, 0.62);
    glVertex2f(i + 120, j + 300);
    glVertex2f(i + 0,   j + 150);
    glVertex2f(i + 20,  j + 180);
    glVertex2f(i + 40,  j + 210);
    glVertex2f(i + 60,  j + 240);
    glVertex2f(i + 80,  j + 265);
    glVertex2f(i + 100, j + 285);
    glVertex2f(i + 140, j + 280);
    glVertex2f(i + 160, j + 255);
    glVertex2f(i + 180, j + 230);
    glVertex2f(i + 200, j + 200);
    glVertex2f(i + 220, j + 175);
    glVertex2f(i + 240, j + 150);
    glVertex2f(i + 0, j + 150);
    glEnd();

    glEnable(GL_LINE_SMOOTH);
    glLineWidth(1.5);
    glBegin(GL_LINE_LOOP);
    glColor3f(0.30, 0.45, 0.42);
    glVertex2f(i + 0,   j + 150);
    glVertex2f(i + 20,  j + 180);
    glVertex2f(i + 40,  j + 210);
    glVertex2f(i + 60,  j + 240);
    glVertex2f(i + 80,  j + 265);
    glVertex2f(i + 100, j + 285);
    glVertex2f(i + 120, j + 300);
    glVertex2f(i + 140, j + 280);
    glVertex2f(i + 160, j + 255);
    glVertex2f(i + 180, j + 230);
    glVertex2f(i + 200, j + 200);
    glVertex2f(i + 220, j + 175);
    glVertex2f(i + 240, j + 150);
    glEnd();
}

void s4_Mountain3(float i, float j) {
    glBegin(GL_TRIANGLE_FAN);
    glColor3f(0.45, 0.65, 0.62);
    glVertex2f(i + 84, j + 210);
    glVertex2f(i + 0,  j + 105);
    glVertex2f(i + 14, j + 126);
    glVertex2f(i + 28, j + 147);
    glVertex2f(i + 42, j + 168);
    glVertex2f(i + 56, j + 186);
    glVertex2f(i + 70, j + 200);
    glVertex2f(i + 98, j + 196);
    glVertex2f(i + 112, j + 179);
    glVertex2f(i + 126, j + 162);
    glVertex2f(i + 140, j + 140);
    glVertex2f(i + 154, j + 123);
    glVertex2f(i + 168, j + 105);
    glVertex2f(i + 0, j + 105);
    glEnd();

    glEnable(GL_LINE_SMOOTH);
    glLineWidth(1.5);
    glBegin(GL_LINE_LOOP);
    glColor3f(0.30, 0.45, 0.42);
    glVertex2f(i + 0,  j + 105);
    glVertex2f(i + 14, j + 126);
    glVertex2f(i + 28, j + 147);
    glVertex2f(i + 42, j + 168);
    glVertex2f(i + 56, j + 186);
    glVertex2f(i + 70, j + 200);
    glVertex2f(i + 84, j + 210);
    glVertex2f(i + 98, j + 196);
    glVertex2f(i + 112, j + 179);
    glVertex2f(i + 126, j + 162);
    glVertex2f(i + 140, j + 140);
    glVertex2f(i + 154, j + 123);
    glVertex2f(i + 168, j + 105);
    glEnd();
}

void s4_Tree(float i, float j) {
    float x = i;
    float y = j;
    glColor3f(0.40, 0.25, 0.10);
    glBegin(GL_QUADS);
    glVertex2f(x + 9,  y + 0);
    glVertex2f(x + 16, y + 0);
    glVertex2f(x + 16, y + 65);
    glVertex2f(x + 9,  y + 65);
    glEnd();

    float topX = x + 12.5;
    float topY = y + 35;
    glColor3f(0.20, 0.60, 0.25);

    glBegin(GL_TRIANGLE_FAN);
    glVertex2f(topX, topY + 32);
    for(int angle = 0; angle <= 360; angle += 20) {
        float rad = angle * 3.1416 / 180.0;
        glVertex2f(topX + 20 * cos(rad), topY + 32 + 17 * sin(rad));
    }
    glEnd();

    glBegin(GL_TRIANGLE_FAN);
    glVertex2f(topX - 12, topY + 22);
    for(int angle = 0; angle <= 360; angle += 20) {
        float rad = angle * 3.1416 / 180.0;
        glVertex2f(topX - 12 + 14 * cos(rad), topY + 22 + 12 * sin(rad));
    }
    glEnd();

    glBegin(GL_TRIANGLE_FAN);
    glVertex2f(topX + 12, topY + 22);
    for(int angle = 0; angle <= 360; angle += 20) {
        float rad = angle * 3.1416 / 180.0;
        glVertex2f(topX + 12 + 14 * cos(rad), topY + 22 + 12 * sin(rad));
    }
    glEnd();
}

void s4_Tree2(float i, float j) {
    float x = i;
    float y = j;
    glColor3f(0.40, 0.25, 0.10);
    glBegin(GL_QUADS);
    glVertex2f(x + 9,  y + 0);
    glVertex2f(x + 16, y + 0);
    glVertex2f(x + 16, y + 65);
    glVertex2f(x + 9,  y + 65);
    glEnd();

    float topX = x + 12.5;
    float topY = y + 38;

    glColor3f(0.35, 0.22, 0.10);
    glLineWidth(3);
    glBegin(GL_LINES);
    glVertex2f(topX, topY);
    glVertex2f(topX - 18, topY + 20);
    glVertex2f(topX, topY + 5);
    glVertex2f(topX + 22, topY + 25);
    glVertex2f(topX, topY + 10);
    glVertex2f(topX - 5, topY + 35);
    glEnd();

    glColor3f(0.20, 0.60, 0.25);
    glBegin(GL_TRIANGLE_FAN);
    glVertex2f(topX, topY + 35);
    for(int angle = 0; angle <= 360; angle += 20) {
        float rad = angle * 3.1416 / 180.0;
        glVertex2f(topX + 22 * cos(rad), topY + 35 + 18 * sin(rad));
    }
    glEnd();

    glBegin(GL_TRIANGLE_FAN);
    glVertex2f(topX - 12, topY + 22);
    for(int angle = 0; angle <= 360; angle += 20) {
        float rad = angle * 3.1416 / 180.0;
        glVertex2f(topX - 12 + 14 * cos(rad), topY + 22 + 12 * sin(rad));
    }
    glEnd();

    glBegin(GL_TRIANGLE_FAN);
    glVertex2f(topX + 12, topY + 22);
    for(int angle = 0; angle <= 360; angle += 20) {
        float rad = angle * 3.1416 / 180.0;
        glVertex2f(topX + 12 + 14 * cos(rad), topY + 22 + 12 * sin(rad));
    }
    glEnd();
}

void s4_Tree3(float i, float j) {
    float x = i;
    float y = j;
    glColor3f(0.35, 0.22, 0.10);
    glBegin(GL_QUADS);
    glVertex2f(x + 10, y + 0);
    glVertex2f(x + 14, y + 0);
    glVertex2f(x + 14, y + 65);
    glVertex2f(x + 10, y + 65);
    glEnd();

    glColor3f(0.15, 0.55, 0.20);
    glBegin(GL_TRIANGLES);
    glVertex2f(x + 0,  y + 35);
    glVertex2f(x + 24, y + 35);
    glVertex2f(x + 12, y + 70);
    glEnd();

    glBegin(GL_TRIANGLES);
    glVertex2f(x + 3,  y + 50);
    glVertex2f(x + 21, y + 50);
    glVertex2f(x + 12, y + 85);
    glEnd();

    glBegin(GL_TRIANGLES);
    glVertex2f(x + 6,  y + 65);
    glVertex2f(x + 18, y + 65);
    glVertex2f(x + 12, y + 100);
    glEnd();
}

void s4_Tree4(float i, float j) {
    float x = i;
    float y = j;
    glColor3f(0.40, 0.25, 0.10);
    glBegin(GL_QUADS);
    glVertex2f(x + 12, y + 0);
    glVertex2f(x + 18, y + 0);
    glVertex2f(x + 18, y + 65);
    glVertex2f(x + 12, y + 65);
    glEnd();

    glColor3f(0.18, 0.55, 0.22);
    float cx = x + 15;
    float cy = y + 45;

    glBegin(GL_TRIANGLE_FAN);
    glVertex2f(cx, cy + 10);
    for(int a = 0; a <= 360; a += 20) {
        float r = a * 3.1416 / 180.0;
        glVertex2f(cx + 18*cos(r), cy + 10 + 15*sin(r));
    }
    glEnd();

    glBegin(GL_TRIANGLE_FAN);
    glVertex2f(cx - 15, cy);
    for(int a = 0; a <= 360; a += 20) {
        float r = a * 3.1416 / 180.0;
        glVertex2f(cx - 15 + 14*cos(r), cy + 12*sin(r));
    }
    glEnd();

    glBegin(GL_TRIANGLE_FAN);
    glVertex2f(cx + 15, cy);
    for(int a = 0; a <= 360; a += 20) {
        float r = a * 3.1416 / 180.0;
        glVertex2f(cx + 15 + 14*cos(r), cy + 12*sin(r));
    }
    glEnd();
}

void s4_Tree5(float i, float j) {
    glPushMatrix();
    glTranslatef(i, j, 0);
    glScalef(2.0, 2.0, 1.0);
    glTranslatef(-i, -j, 0);

    float x = i;
    float y = j;
    glColor3f(0.40, 0.25, 0.10);
    glBegin(GL_QUADS);
    glVertex2f(x + 12, y + 0);
    glVertex2f(x + 18, y + 0);
    glVertex2f(x + 18, y + 65);
    glVertex2f(x + 12, y + 65);
    glEnd();

    glColor3f(0.18, 0.55, 0.22);
    float cx = x + 15;
    float cy = y + 45;

    glBegin(GL_TRIANGLE_FAN);
    glVertex2f(cx, cy + 10);
    for(int a = 0; a <= 360; a += 20) {
        float r = a * 3.1416 / 180.0;
        glVertex2f(cx + 18*cos(r), cy + 10 + 15*sin(r));
    }
    glEnd();

    glBegin(GL_TRIANGLE_FAN);
    glVertex2f(cx - 15, cy);
    for(int a = 0; a <= 360; a += 20) {
        float r = a * 3.1416 / 180.0;
        glVertex2f(cx - 15 + 14*cos(r), cy + 12*sin(r));
    }
    glEnd();

    glBegin(GL_TRIANGLE_FAN);
    glVertex2f(cx + 15, cy);
    for(int a = 0; a <= 360; a += 20) {
        float r = a * 3.1416 / 180.0;
        glVertex2f(cx + 15 + 14*cos(r), cy + 12*sin(r));
    }
    glEnd();
    glPopMatrix();
}

void s4_Forest() {
    int startX = 0;
    int gap = 35;
    int y = 250;

    for(int i = 0; i < 29; i++) {
        int x = startX + (i * gap);
        if(x >= 400 && x <= 500) continue;

        if(i % 4 == 0) s4_Tree(x, y);
        else if(i % 4 == 1) s4_Tree2(x, y);
        else if(i % 4 == 2) s4_Tree3(x, y);
        else s4_Tree4(x, y);
    }
}

void s4_House() {
    glLineWidth(2);
    glBegin(GL_QUADS);
    glColor3f(0.65, 0.20, 0.20);
    glVertex2f(110,250); glVertex2f(100,275); glVertex2f(160,275); glVertex2f(170,252);
    glEnd();

    glBegin(GL_LINE_LOOP);
    glColor3f(0.45, 0.08, 0.08);
    glVertex2f(110,250); glVertex2f(100,275); glVertex2f(160,275); glVertex2f(170,252);
    glEnd();

    glBegin(GL_QUADS);
    glColor3f(0.93, 0.85, 0.75);
    glVertex2f(110,250); glVertex2f(110,220); glVertex2f(165,222); glVertex2f(165,252);
    glEnd();

    glBegin(GL_LINE_LOOP);
    glColor3f(0.72, 0.62, 0.52);
    glVertex2f(110,250); glVertex2f(110,220); glVertex2f(165,222); glVertex2f(165,252);
    glEnd();

    glBegin(GL_TRIANGLES);
    glColor3f(0.55, 0.18, 0.18);
    glVertex2f(100,275); glVertex2f(95,255); glVertex2f(110,250);
    glEnd();

    glBegin(GL_LINE_LOOP);
    glColor3f(0.38, 0.08, 0.08);
    glVertex2f(100,275); glVertex2f(95,255); glVertex2f(110,250);
    glEnd();

    glBegin(GL_QUADS);
    glColor3f(0.80, 0.72, 0.62);
    glVertex2f(100,255); glVertex2f(100,230); glVertex2f(110,220); glVertex2f(110,250);
    glEnd();

    glBegin(GL_LINE_LOOP);
    glColor3f(0.60, 0.50, 0.40);
    glVertex2f(100,255); glVertex2f(100,230); glVertex2f(110,220); glVertex2f(110,250);
    glEnd();

    glBegin(GL_QUADS);
    glColor3f(0.40, 0.20, 0.05);
    glVertex2f(130,224); glVertex2f(130,240); glVertex2f(142,242); glVertex2f(142,225);
    glEnd();

    glBegin(GL_LINE_LOOP);
    glColor3f(0.22, 0.10, 0.02);
    glVertex2f(130,224); glVertex2f(130,240); glVertex2f(142,242); glVertex2f(142,225);
    glEnd();
}

void s4_House2() {
    glLineWidth(2);
    glBegin(GL_QUADS);
    glColor3f(0.65, 0.20, 0.20);
    glVertex2f(360,300); glVertex2f(350,325); glVertex2f(410,325); glVertex2f(420,302);
    glEnd();

    glBegin(GL_LINE_LOOP);
    glColor3f(0.45, 0.08, 0.08);
    glVertex2f(360,300); glVertex2f(350,325); glVertex2f(410,325); glVertex2f(420,302);
    glEnd();

    glBegin(GL_QUADS);
    glColor3f(0.93, 0.85, 0.75);
    glVertex2f(360,300); glVertex2f(360,270); glVertex2f(415,272); glVertex2f(415,302);
    glEnd();

    glBegin(GL_LINE_LOOP);
    glColor3f(0.72, 0.62, 0.52);
    glVertex2f(360,300); glVertex2f(360,270); glVertex2f(415,272); glVertex2f(415,302);
    glEnd();

    glBegin(GL_TRIANGLES);
    glColor3f(0.55, 0.18, 0.18);
    glVertex2f(350,325); glVertex2f(345,305); glVertex2f(360,300);
    glEnd();

    glBegin(GL_LINE_LOOP);
    glColor3f(0.38, 0.08, 0.08);
    glVertex2f(350,325); glVertex2f(345,305); glVertex2f(360,300);
    glEnd();

    glBegin(GL_QUADS);
    glColor3f(0.80, 0.72, 0.62);
    glVertex2f(350,305); glVertex2f(350,280); glVertex2f(360,270); glVertex2f(360,300);
    glEnd();

    glBegin(GL_LINE_LOOP);
    glColor3f(0.60, 0.50, 0.40);
    glVertex2f(350,305); glVertex2f(350,280); glVertex2f(360,270); glVertex2f(360,300);
    glEnd();

    glBegin(GL_QUADS);
    glColor3f(0.40, 0.20, 0.05);
    glVertex2f(370,274); glVertex2f(370,290); glVertex2f(382,292); glVertex2f(382,275);
    glEnd();

    glBegin(GL_LINE_LOOP);
    glColor3f(0.22, 0.10, 0.02);
    glVertex2f(370,274); glVertex2f(370,290); glVertex2f(382,292); glVertex2f(382,275);
    glEnd();
}

void s4_House3() {
    glLineWidth(2);
    glBegin(GL_QUADS);
    glColor3f(0.65, 0.20, 0.20);
    glVertex2f(560,300); glVertex2f(550,325); glVertex2f(610,325); glVertex2f(620,302);
    glEnd();

    glBegin(GL_LINE_LOOP);
    glColor3f(0.45, 0.08, 0.08);
    glVertex2f(560,300); glVertex2f(550,325); glVertex2f(610,325); glVertex2f(620,302);
    glEnd();

    glBegin(GL_QUADS);
    glColor3f(0.93, 0.85, 0.75);
    glVertex2f(560,300); glVertex2f(560,270); glVertex2f(615,272); glVertex2f(615,302);
    glEnd();

    glBegin(GL_LINE_LOOP);
    glColor3f(0.72, 0.62, 0.52);
    glVertex2f(560,300); glVertex2f(560,270); glVertex2f(615,272); glVertex2f(615,302);
    glEnd();

    glBegin(GL_TRIANGLES);
    glColor3f(0.55, 0.18, 0.18);
    glVertex2f(550,325); glVertex2f(545,305); glVertex2f(560,300);
    glEnd();

    glBegin(GL_LINE_LOOP);
    glColor3f(0.38, 0.08, 0.08);
    glVertex2f(550,325); glVertex2f(545,305); glVertex2f(560,300);
    glEnd();

    glBegin(GL_QUADS);
    glColor3f(0.80, 0.72, 0.62);
    glVertex2f(550,305); glVertex2f(550,280); glVertex2f(560,270); glVertex2f(560,300);
    glEnd();

    glBegin(GL_LINE_LOOP);
    glColor3f(0.60, 0.50, 0.40);
    glVertex2f(550,305); glVertex2f(550,280); glVertex2f(560,270); glVertex2f(560,300);
    glEnd();

    glBegin(GL_QUADS);
    glColor3f(0.40, 0.20, 0.05);
    glVertex2f(570,274); glVertex2f(570,290); glVertex2f(582,292); glVertex2f(582,275);
    glEnd();

    glBegin(GL_LINE_LOOP);
    glColor3f(0.22, 0.10, 0.02);
    glVertex2f(570,274); glVertex2f(570,290); glVertex2f(582,292); glVertex2f(582,275);
    glEnd();
}

void s4_House4() {
    glLineWidth(2);
    glBegin(GL_QUADS);
    glColor3f(0.65, 0.20, 0.20);
    glVertex2f(660,280); glVertex2f(650,305); glVertex2f(710,305); glVertex2f(720,282);
    glEnd();

    glBegin(GL_LINE_LOOP);
    glColor3f(0.45, 0.08, 0.08);
    glVertex2f(660,280); glVertex2f(650,305); glVertex2f(710,305); glVertex2f(720,282);
    glEnd();

    glBegin(GL_QUADS);
    glColor3f(0.93, 0.85, 0.75);
    glVertex2f(660,280); glVertex2f(660,250); glVertex2f(715,252); glVertex2f(715,282);
    glEnd();

    glBegin(GL_LINE_LOOP);
    glColor3f(0.72, 0.62, 0.52);
    glVertex2f(660,280); glVertex2f(660,250); glVertex2f(715,252); glVertex2f(715,282);
    glEnd();

    glBegin(GL_TRIANGLES);
    glColor3f(0.55, 0.18, 0.18);
    glVertex2f(650,305); glVertex2f(645,285); glVertex2f(660,280);
    glEnd();

    glBegin(GL_LINE_LOOP);
    glColor3f(0.38, 0.08, 0.08);
    glVertex2f(650,305); glVertex2f(645,285); glVertex2f(660,280);
    glEnd();

    glBegin(GL_QUADS);
    glColor3f(0.80, 0.72, 0.62);
    glVertex2f(650,285); glVertex2f(650,260); glVertex2f(660,250); glVertex2f(660,280);
    glEnd();

    glBegin(GL_LINE_LOOP);
    glColor3f(0.60, 0.50, 0.40);
    glVertex2f(650,285); glVertex2f(650,260); glVertex2f(660,250); glVertex2f(660,280);
    glEnd();

    glBegin(GL_QUADS);
    glColor3f(0.40, 0.20, 0.05);
    glVertex2f(670,254); glVertex2f(670,270); glVertex2f(682,272); glVertex2f(682,255);
    glEnd();

    glBegin(GL_LINE_LOOP);
    glColor3f(0.22, 0.10, 0.02);
    glVertex2f(670,254); glVertex2f(670,270); glVertex2f(682,272); glVertex2f(682,255);
    glEnd();
}

void s4_House5() {
    glLineWidth(2);
    glBegin(GL_QUADS);
    glColor3f(0.65, 0.20, 0.20);
    glVertex2f(890,260); glVertex2f(880,285); glVertex2f(940,285); glVertex2f(950,262);
    glEnd();

    glBegin(GL_LINE_LOOP);
    glColor3f(0.45, 0.08, 0.08);
    glVertex2f(890,260); glVertex2f(880,285); glVertex2f(940,285); glVertex2f(950,262);
    glEnd();

    glBegin(GL_QUADS);
    glColor3f(0.93, 0.85, 0.75);
    glVertex2f(890,260); glVertex2f(890,230); glVertex2f(945,232); glVertex2f(945,262);
    glEnd();

    glBegin(GL_LINE_LOOP);
    glColor3f(0.72, 0.62, 0.52);
    glVertex2f(890,260); glVertex2f(890,230); glVertex2f(945,232); glVertex2f(945,262);
    glEnd();

    glBegin(GL_TRIANGLES);
    glColor3f(0.55, 0.18, 0.18);
    glVertex2f(880,285); glVertex2f(875,265); glVertex2f(890,260);
    glEnd();

    glBegin(GL_LINE_LOOP);
    glColor3f(0.38, 0.08, 0.08);
    glVertex2f(880,285); glVertex2f(875,265); glVertex2f(890,260);
    glEnd();

    glBegin(GL_QUADS);
    glColor3f(0.80, 0.72, 0.62);
    glVertex2f(880,265); glVertex2f(880,240); glVertex2f(890,230); glVertex2f(890,260);
    glEnd();

    glBegin(GL_LINE_LOOP);
    glColor3f(0.60, 0.50, 0.40);
    glVertex2f(880,265); glVertex2f(880,240); glVertex2f(890,230); glVertex2f(890,260);
    glEnd();

    glBegin(GL_QUADS);
    glColor3f(0.40, 0.20, 0.05);
    glVertex2f(910,234); glVertex2f(910,250); glVertex2f(922,252); glVertex2f(922,235);
    glEnd();

    glBegin(GL_LINE_LOOP);
    glColor3f(0.22, 0.10, 0.02);
    glVertex2f(910,234); glVertex2f(910,250); glVertex2f(922,252); glVertex2f(922,235);
    glEnd();
}

void s4_UpperHalfCircle(float cx, float cy, float radius) {
    glBegin(GL_TRIANGLE_FAN);
    glColor3f(0.85, 0.70, 0.10);
    glVertex2f(cx, cy);
    for(int i = 0; i <= 180; i++) {
        float angle = i * 3.14159f / 180.0f;
        float x = cx + radius * cos(angle);
        float y = cy + radius * sin(angle);
        glVertex2f(x, y);
    }
    glEnd();
}

void s4_Fence(float x, float y, float size) {
    float w = 30 * size;
    float h = 50 * size;

    glBegin(GL_QUADS);
    glColor3f(0.45, 0.25, 0.10);
    glVertex2f(x, y); glVertex2f(x, y - h); glVertex2f(x + 10 * size, y - h); glVertex2f(x + 10 * size, y);
    glEnd();

    glBegin(GL_QUADS);
    glColor3f(0.45, 0.25, 0.10);
    glVertex2f(x + w, y); glVertex2f(x + w, y - h); glVertex2f(x + w + 10 * size, y - h); glVertex2f(x + w + 10 * size, y);
    glEnd();

    glBegin(GL_QUADS);
    glColor3f(0.45, 0.25, 0.10);
    glVertex2f(x + 2 * w, y); glVertex2f(x + 2 * w, y - h); glVertex2f(x + 2 * w + 10 * size, y - h); glVertex2f(x + 2 * w + 10 * size, y);
    glEnd();

    glBegin(GL_QUADS);
    glColor3f(0.30, 0.15, 0.05);
    glVertex2f(x - 5, y); glVertex2f(x - 5, y + 5 * size); glVertex2f(x + 2 * w + 15 * size, y + 5 * size); glVertex2f(x + 2 * w + 15 * size, y);
    glEnd();

    glBegin(GL_QUADS);
    glColor3f(0.30, 0.15, 0.05);
    glVertex2f(x - 5, y - h); glVertex2f(x - 5, y - h - 5 * size); glVertex2f(x + 2 * w + 15 * size, y - h - 5 * size); glVertex2f(x + 2 * w + 15 * size, y - h);
    glEnd();
}

void s4_Human(float x, float y, float s) {
    glColor3f(0.95, 0.82, 0.65);
    glBegin(GL_QUADS);
    glVertex2f(x + 3*s, y); glVertex2f(x + 3*s, y + 12*s); glVertex2f(x + 14*s, y + 12*s); glVertex2f(x + 14*s, y);
    glEnd();

    glColor3f(0.90, 0.75, 0.60);
    glBegin(GL_QUADS);
    glVertex2f(x + 7*s, y - 2*s); glVertex2f(x + 7*s, y); glVertex2f(x + 11*s, y); glVertex2f(x + 11*s, y - 2*s);
    glEnd();

    glColor3f(0.20, 0.40, 0.80);
    glBegin(GL_QUADS);
    glVertex2f(x + 2*s, y - 2*s); glVertex2f(x + 2*s, y - 32*s); glVertex2f(x + 16*s, y - 32*s); glVertex2f(x + 16*s, y - 2*s);
    glEnd();

    glColor3f(0.90, 0.75, 0.60);
    glBegin(GL_QUADS);
    glVertex2f(x - 2*s, y - 6*s); glVertex2f(x - 2*s, y - 26*s); glVertex2f(x + 2*s, y - 26*s); glVertex2f(x + 2*s, y - 6*s);
    glEnd();

    glBegin(GL_QUADS);
    glVertex2f(x + 16*s, y - 6*s); glVertex2f(x + 16*s, y - 26*s); glVertex2f(x + 20*s, y - 26*s); glVertex2f(x + 20*s, y - 6*s);
    glEnd();

    glColor3f(0.10, 0.10, 0.10);
    glBegin(GL_QUADS);
    glVertex2f(x + 5*s, y - 32*s); glVertex2f(x + 5*s, y - 58*s); glVertex2f(x + 9*s, y - 58*s); glVertex2f(x + 9*s, y - 32*s);
    glEnd();

    glBegin(GL_QUADS);
    glVertex2f(x + 11*s, y - 32*s); glVertex2f(x + 11*s, y - 58*s); glVertex2f(x + 15*s, y - 58*s); glVertex2f(x + 15*s, y - 32*s);
    glEnd();
}

void s4_Bird(float x, float y, float s) {
    glColor3f(0.05, 0.05, 0.05);
    glBegin(GL_QUADS);
    glVertex2f(x - 2*s, y); glVertex2f(x - 2*s, y + 2*s); glVertex2f(x + 2*s, y + 2*s); glVertex2f(x + 2*s, y);
    glEnd();

    glBegin(GL_QUADS);
    glVertex2f(x, y + 1*s); glVertex2f(x - 12*s, y + 6*s); glVertex2f(x - 10*s, y + 8*s); glVertex2f(x + 1*s, y + 2*s);
    glEnd();

    glBegin(GL_QUADS);
    glVertex2f(x, y + 1*s); glVertex2f(x + 12*s, y + 6*s); glVertex2f(x + 10*s, y + 8*s); glVertex2f(x - 1*s, y + 2*s);
    glEnd();

    glBegin(GL_QUADS);
    glVertex2f(x - 1*s, y); glVertex2f(x + 1*s, y); glVertex2f(x - 6*s, y - 4*s); glVertex2f(x + 6*s, y - 4*s);
    glEnd();
}

void s4_Boat1() {
    glBegin(GL_POLYGON);
    glColor3f(0.36f, 0.25f, 0.20f);
    glVertex2f(615, 150); glVertex2f(715, 150); glVertex2f(695, 130); glVertex2f(635, 130);
    glEnd();

    glLineWidth(3);
    glBegin(GL_LINES);
    glColor3f(0.0f, 0.0f, 0.0f);
    glVertex2f(665, 150); glVertex2f(665, 200);
    glEnd();

    glBegin(GL_TRIANGLES);
    glColor3f(0.9f, 0.9f, 0.9f);
    glVertex2f(665, 200); glVertex2f(665, 160); glVertex2f(705, 160);
    glEnd();

    glBegin(GL_QUADS);
    glColor3f(0.2f, 0.1f, 0.0f);
    glVertex2f(645, 150); glVertex2f(685, 150); glVertex2f(680, 170); glVertex2f(650, 170);
    glEnd();
}

void s4_Boat2() {
    glBegin(GL_POLYGON);
    glColor3f(0.36f, 0.25f, 0.20f);
    glVertex2f(450, 65); glVertex2f(610, 65); glVertex2f(585, 40); glVertex2f(475, 40);
    glEnd();

    glLineWidth(3);
    glBegin(GL_LINES);
    glColor3f(0.0f, 0.0f, 0.0f);
    glVertex2f(530, 65); glVertex2f(530, 135);
    glEnd();

    glBegin(GL_TRIANGLES);
    glColor3f(0.9f, 0.9f, 0.9f);
    glVertex2f(530, 135); glVertex2f(530, 85); glVertex2f(600, 85);
    glEnd();

    glBegin(GL_QUADS);
    glColor3f(0.2f, 0.1f, 0.0f);
    glVertex2f(500, 65); glVertex2f(560, 65); glVertex2f(555, 85); glVertex2f(505, 85);
    glEnd();
}

void s4_Boat3() {
    glBegin(GL_POLYGON);
    glColor3f(0.36f, 0.25f, 0.20f);
    glVertex2f(0, 120); glVertex2f(100, 120); glVertex2f(80, 100); glVertex2f(20, 100);
    glEnd();

    glLineWidth(3);
    glBegin(GL_LINES);
    glColor3f(0.0f, 0.0f, 0.0f);
    glVertex2f(50, 120); glVertex2f(50, 170);
    glEnd();

    glBegin(GL_TRIANGLES);
    glColor3f(0.9f, 0.9f, 0.9f);
    glVertex2f(50, 170); glVertex2f(50, 130); glVertex2f(90, 130);
    glEnd();

    glBegin(GL_QUADS);
    glColor3f(0.2f, 0.1f, 0.0f);
    glVertex2f(30, 120); glVertex2f(70, 120); glVertex2f(65, 140); glVertex2f(35, 140);
    glEnd();
}

void s4_cropField() {
    glBegin(GL_QUADS);
    glColor3f(0.22, 0.15, 0.10);
    glVertex2f(160,230); glVertex2f(180,245); glVertex2f(250,245); glVertex2f(270,230);
    glEnd();

    s4_GrassBlade(162,231,2); s4_GrassBlade(172,231,2); s4_GrassBlade(182,231,2);
    s4_GrassBlade(192,231,2); s4_GrassBlade(202,231,2); s4_GrassBlade(212,231,2);
    s4_GrassBlade(222,231,2); s4_GrassBlade(232,231,2); s4_GrassBlade(242,231,2);
    s4_GrassBlade(252,231,2);

    s4_Fence(157,250,0.4); s4_Fence(167,250,0.4); s4_Fence(192,250,0.4);
    s4_Fence(227,250,0.4); s4_Fence(237,250,0.4);
}

void s4_draw_landscape_static() {
    s4_Field2();
    s4_Field();

    s4_Mountain2(-210, 110);

    s4_House2();
    s4_Road();
    s4_UpperHalfCircle(440, 272, 20);
    s4_Fence(414,285,0.25);
    s4_Fence(434,285,0.25);
    s4_Fence(444,285,0.25);
    s4_House3();
    s4_Mountain1(0,100);
    s4_Mountain2(790,110);
    s4_Mountain3(520,200);
    s4_Bird(865,350,0.75);
    s4_Mountain1(550,100);
    s4_House4();
    s4_GrassBlade(642,247,1.5);
    s4_GrassBlade(644,247,1.5);
    s4_GrassBlade(646,247,1.5);
    s4_GrassBlade(650,247,1.5);
    s4_GrassBlade(700,250,1);
    s4_GrassBlade(705,250,1);
    s4_GrassBlade(112,250,1);
    s4_Human(740,260,0.25);
    s4_Human(735,260,0.25);
    s4_Forest();
    s4_House5();
    s4_River();
    s4_ground();
    s4_cropField();
    s4_Tree5(75,220);
    s4_House();
    s4_Bird(155,280,1);
    s4_Human(100,235,0.35);
    s4_Bird(150,550,0.5);
    s4_Bird(165,550,0.5);
    s4_GrassBlade(160,220,1);
    s4_GrassBlade(157,220,1);
    s4_GrassBlade(155,220,1);
}

void s4_draw_boats_animated() {
    glPushMatrix();
    float wrap_boats = fmod(s4_boat_offset, 1000.0f);
    glTranslatef(-wrap_boats, 0.0f, 0.0f);
    s4_Boat1();
    s4_Boat2();
    s4_Boat3();

    glTranslatef(1000.0f, 0.0f, 0.0f);
    s4_Boat1();
    s4_Boat2();
    s4_Boat3();
    glPopMatrix();
}


// ..........................................................................
// ................................ SCENE 5 .................................
// ..........................................................................

void s5_drawCircle(float cx, float cy, float r, int seg) {
    glBegin(GL_TRIANGLE_FAN);
    glVertex2f(cx, cy);
    for (int i = 0; i <= seg; i++) {
        float a = 2.0f * s5_PI * i / seg;
        glVertex2f(cx + r * cosf(a), cy + r * sinf(a));
    }
    glEnd();
}

void s5_drawHalfCircle(float cx, float cy, float r, int seg) {
    glBegin(GL_TRIANGLE_FAN);
    glVertex2f(cx, cy);
    for (int i = 0; i <= seg; i++) {
        float a = s5_PI * i / seg;
        glVertex2f(cx + r * cosf(a), cy + r * sinf(a));
    }
    glEnd();
}

void s5_gradQuad(float x0, float x1, float yBot, float yTop,
              float r0, float g0, float b0,
              float r1, float g1, float b1) {
    glBegin(GL_QUADS);
    glColor3f(r1, g1, b1); glVertex2f(x0, yTop); glVertex2f(x1, yTop);
    glColor3f(r0, g0, b0); glVertex2f(x1, yBot); glVertex2f(x0, yBot);
    glEnd();
}

void s5_drawSky() {
    s5_gradQuad(-1, 1, 0.0f, 1.0f,  1.0f, 0.55f, 0.20f,  0.08f, 0.20f, 0.45f);
    s5_gradQuad(-1, 1, -0.05f, 0.0f,  1.0f, 0.75f, 0.40f,  1.0f, 0.55f, 0.20f);
}

void s5_drawSun() {
    float cx = 0.30f, cy = 0.0f;
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    for (int i = 5; i >= 1; i--) {
        glColor4f(1.0f, 0.70f, 0.20f, 0.055f * i);
        s5_drawCircle(cx, cy, 0.06f * i + 0.12f, 60);
    }
    glDisable(GL_BLEND);
    glColor3f(1.00f, 0.92f, 0.50f); s5_drawHalfCircle(cx, cy, 0.12f, 60);
    glColor3f(1.00f, 1.00f, 0.88f); s5_drawHalfCircle(cx, cy, 0.07f, 60);
}

void s5_drawHorizonHaze() {
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glColor4f(1.0f, 0.88f, 0.60f, 0.30f);
    glBegin(GL_QUADS);
    glVertex2f(-1.0f, -0.01f); glVertex2f(1.0f, -0.01f);
    glVertex2f(1.0f, 0.03f); glVertex2f(-1.0f, 0.03f);
    glEnd();
    glDisable(GL_BLEND);
}

void s5_drawOcean() {
    s5_gradQuad(-1, 1, -0.35f, 0.0f, 0.12f, 0.70f, 0.80f, 0.05f, 0.28f, 0.52f);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glBegin(GL_QUADS);
    glColor4f(1.0f, 0.75f, 0.35f, 0.45f); glVertex2f(0.20f, 0.0f);  glVertex2f(0.42f, 0.0f);
    glColor4f(1.0f, 0.55f, 0.10f, 0.05f); glVertex2f(0.42f, -0.35f); glVertex2f(0.20f, -0.35f);
    glEnd();

    float wY[3] = {-0.08f, -0.18f, -0.28f}, wA[3] = {0.22f, 0.15f, 0.10f};
    glLineWidth(1.5f);
    for (int w = 0; w < 3; w++) {
        glColor4f(0.80f, 0.96f, 1.0f, wA[w]);
        glBegin(GL_LINE_STRIP);
        for (int x = -100; x <= 100; x++) {
            float fx = x / 100.0f;
            glVertex2f(fx, wY[w] + 0.013f * sinf(fx * s5_PI * 6.0f + w * 1.2f));
        }
        glEnd();
    }
    glLineWidth(1.0f);
    glDisable(GL_BLEND);
}

void s5_drawFoam() {
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glColor4f(0.92f, 0.97f, 1.0f, 0.55f);
    glBegin(GL_QUADS);
    glVertex2f(-1.0f, -0.340f); glVertex2f(1.0f, -0.340f);
    glVertex2f(1.0f, -0.370f); glVertex2f(-1.0f, -0.370f);
    glEnd();
    glDisable(GL_BLEND);
}

void s5_drawSand() {
    s5_gradQuad(-1, 1, -1.0f, -0.35f, 0.58f, 0.38f, 0.12f, 0.86f, 0.68f, 0.38f);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glColor4f(0.36f, 0.74f, 0.85f, 0.30f);
    glBegin(GL_QUADS);
    glVertex2f(-1.0f, -0.35f); glVertex2f(1.0f, -0.35f);
    glVertex2f(1.0f, -0.44f); glVertex2f(-1.0f, -0.44f);
    glEnd();

    srand(12345);
    glPointSize(1.5f);
    glBegin(GL_POINTS);
    for (int i = 0; i < 4000; i++) {
        float x = ((rand() % 2000) / 1000.0f) - 1.0f;
        float y = -1.0f + ((rand() % 650) / 1000.0f);

        if (rand() % 2 == 0) glColor4f(0.9f, 0.75f, 0.50f, 0.6f);
        else                 glColor4f(0.45f, 0.28f, 0.10f, 0.4f);
        glVertex2f(x, y);
    }
    glEnd();
    glPointSize(1.0f);

    glDisable(GL_BLEND);
}

void s5_drawShip(float ox, float oy, float s) {
    glColor3f(0.72f, 0.18f, 0.18f);
    glBegin(GL_QUADS);
    glVertex2f(ox - 0.14f * s, oy); glVertex2f(ox + 0.14f * s, oy);
    glVertex2f(ox + 0.11f * s, oy - 0.04f * s); glVertex2f(ox - 0.11f * s, oy - 0.04f * s);
    glEnd();

    glColor3f(0.90f, 0.90f, 0.90f);
    glBegin(GL_QUADS);
    glVertex2f(ox - 0.14f * s, oy + 0.005f * s); glVertex2f(ox + 0.14f * s, oy + 0.005f * s);
    glVertex2f(ox + 0.14f * s, oy + 0.016f * s); glVertex2f(ox - 0.14f * s, oy + 0.016f * s);
    glEnd();

    glColor3f(0.88f, 0.88f, 0.88f);
    glBegin(GL_QUADS);
    glVertex2f(ox - 0.10f * s, oy + 0.016f * s); glVertex2f(ox + 0.10f * s, oy + 0.016f * s);
    glVertex2f(ox + 0.10f * s, oy + 0.062f * s); glVertex2f(ox - 0.10f * s, oy + 0.062f * s);
    glEnd();

    glColor3f(0.82f, 0.82f, 0.82f);
    glBegin(GL_QUADS);
    glVertex2f(ox - 0.05f * s, oy + 0.062f * s); glVertex2f(ox + 0.05f * s, oy + 0.062f * s);
    glVertex2f(ox + 0.05f * s, oy + 0.092f * s); glVertex2f(ox - 0.05f * s, oy + 0.092f * s);
    glEnd();

    glColor3f(0.45f, 0.75f, 0.90f);
    float wx[6] = {-0.085f, -0.055f, -0.025f, 0.005f, 0.035f, 0.065f};
    for (int i = 0; i < 6; i++) {
        glBegin(GL_QUADS);
        glVertex2f(ox + wx[i] * s,         oy + 0.023f * s);
        glVertex2f(ox + (wx[i] + 0.018f) * s, oy + 0.023f * s);
        glVertex2f(ox + (wx[i] + 0.018f) * s, oy + 0.045f * s);
        glVertex2f(ox + wx[i] * s,         oy + 0.045f * s);
        glEnd();
    }

    glColor3f(0.28f, 0.28f, 0.28f);
    glBegin(GL_QUADS);
    glVertex2f(ox + 0.018f * s, oy + 0.092f * s); glVertex2f(ox + 0.042f * s, oy + 0.092f * s);
    glVertex2f(ox + 0.042f * s, oy + 0.118f * s); glVertex2f(ox + 0.018f * s, oy + 0.118f * s);
    glEnd();

    glColor3f(0.40f, 0.40f, 0.40f);
    glLineWidth(2.0f);
    glBegin(GL_LINES);
    glVertex2f(ox, oy + 0.092f * s); glVertex2f(ox, oy + 0.190f * s);
    glEnd();
    glBegin(GL_LINES);
    glVertex2f(ox - 0.04f * s, oy + 0.145f * s); glVertex2f(ox + 0.04f * s, oy + 0.145f * s);
    glEnd();

    glColor3f(0.90f, 0.45f, 0.10f);
    glBegin(GL_TRIANGLES);
    glVertex2f(ox, oy + 0.190f * s); glVertex2f(ox + 0.030f * s, oy + 0.173f * s); glVertex2f(ox, oy + 0.156f * s);
    glEnd();

    glColor3f(1.0f, 0.55f, 0.0f);
    s5_drawCircle(ox - 0.07f * s, oy + 0.062f * s, 0.012f * s, 12);
    s5_drawCircle(ox + 0.07f * s, oy + 0.062f * s, 0.012f * s, 12);
    glLineWidth(1.0f);
}

void s5_drawSailboat(float ox, float oy, float s) {
    glColor3f(0.55f, 0.18f, 0.18f);
    glBegin(GL_QUADS);
    glVertex2f(ox - 0.06f * s, oy); glVertex2f(ox + 0.06f * s, oy);
    glVertex2f(ox + 0.045f * s, oy - 0.026f * s); glVertex2f(ox - 0.045f * s, oy - 0.026f * s);
    glEnd();

    glColor3f(0.50f, 0.50f, 0.50f);
    glLineWidth(1.5f);
    glBegin(GL_LINES);
    glVertex2f(ox, oy); glVertex2f(ox, oy + 0.15f * s);
    glEnd();

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glColor4f(0.95f, 0.95f, 0.95f, 0.90f);
    glBegin(GL_TRIANGLES);
    glVertex2f(ox, oy + 0.15f * s); glVertex2f(ox + 0.09f * s, oy + 0.01f * s); glVertex2f(ox, oy + 0.006f * s);
    glEnd();
    glBegin(GL_TRIANGLES);
    glVertex2f(ox, oy + 0.11f * s); glVertex2f(ox - 0.065f * s, oy + 0.01f * s); glVertex2f(ox, oy + 0.006f * s);
    glEnd();
    glDisable(GL_BLEND);
    glLineWidth(1.0f);
}

void s5_drawPalmTree(float ox, float oy) {
    int segments = 12;
    float curve = 0.0f;
    float trunkWidth = 0.025f;

    glBegin(GL_QUAD_STRIP);
    for(int i = 0; i <= segments; i++) {
        float t = (float)i / segments;
        float y = oy + t * 0.45f;
        float x = ox + curve;

        glColor3f(0.35f, 0.22f, 0.10f);
        glVertex2f(x - trunkWidth * (1.0f - t * 0.4f), y);
        glColor3f(0.60f, 0.40f, 0.20f);
        glVertex2f(x + trunkWidth * (1.0f - t * 0.4f), y);

        curve += 0.015f * t;
    }
    glEnd();

    float lx = ox + curve;
    float ly = oy + 0.44f;

    for(int i = 0; i < 7; i++) {
        float angle = i * (s5_PI / 6.0f);
        float length = 0.22f - fabs(i - 3.0f) * 0.02f;

        glColor3f(0.12f, 0.40f + i * 0.02f, 0.15f);
        glBegin(GL_POLYGON);
        glVertex2f(lx, ly);
        glVertex2f(lx + cosf(angle - 0.12f) * length * 0.5f, ly + sinf(angle - 0.12f) * length * 0.5f);
        glVertex2f(lx + cosf(angle) * length, ly + sinf(angle) * length - 0.08f);
        glVertex2f(lx + cosf(angle + 0.12f) * length * 0.5f, ly + sinf(angle + 0.12f) * length * 0.5f);
        glEnd();
    }

    for(int i = 0; i < 7; i++) {
        float angle = i * (s5_PI / 6.0f) + 0.1f;
        float length = 0.16f;

        glColor3f(0.18f, 0.55f + i * 0.02f, 0.18f);
        glBegin(GL_POLYGON);
        glVertex2f(lx, ly);
        glVertex2f(lx + cosf(angle - 0.08f) * length * 0.5f, ly + sinf(angle - 0.08f) * length * 0.5f);
        glVertex2f(lx + cosf(angle) * length, ly + sinf(angle) * length - 0.05f);
        glVertex2f(lx + cosf(angle + 0.08f) * length * 0.5f, ly + sinf(angle + 0.08f) * length * 0.5f);
        glEnd();
    }

    glColor3f(0.35f, 0.20f, 0.05f);
    s5_drawCircle(lx - 0.015f, ly + 0.005f, 0.020f, 16);
    s5_drawCircle(lx + 0.020f, ly - 0.005f, 0.018f, 16);
    s5_drawCircle(lx + 0.002f, ly - 0.015f, 0.016f, 16);
}

void s5_drawBeachUmbrella(float ox, float oy, float r, float g, float b) {
    glColor3f(0.70f, 0.70f, 0.70f);
    glLineWidth(3.0f);
    glBegin(GL_LINES);
    glVertex2f(ox, oy); glVertex2f(ox + 0.010f, oy + 0.28f);
    glEnd();
    glLineWidth(1.0f);

    float tipX = ox + 0.010f, tipY = oy + 0.28f;
    float edgeX[9], edgeY[9];
    for (int i = 0; i <= 8; i++) {
        float a = s5_PI * i / 8.0f;
        edgeX[i] = tipX + 0.165f * cosf(a + s5_PI);
        edgeY[i] = oy + 0.18f + 0.022f * sinf(a * 2.0f);
    }

    for (int i = 0; i < 8; i++) {
        if (i % 2 == 0) {
            glColor3f(r, g, b);
        } else {
            if (g > 0.5f) {
                glColor3f(r * 0.85f, g * 0.85f, b * 0.85f);
            } else {
                glColor3f(0.96f, 0.96f, 0.96f);
            }
        }
        glBegin(GL_TRIANGLES);
        glVertex2f(tipX, tipY); glVertex2f(edgeX[i], edgeY[i]); glVertex2f(edgeX[i+1], edgeY[i+1]);
        glEnd();
    }

    glColor3f(r * 0.7f, g * 0.7f, b * 0.7f);
    glLineWidth(1.5f);
    glBegin(GL_LINE_LOOP);
    for (int i = 0; i <= 8; i++) glVertex2f(edgeX[i], edgeY[i]);
    glEnd();
    glLineWidth(1.0f);

    glColor3f(r, g, b);
    s5_drawCircle(tipX, tipY, 0.013f, 12);
}

void s5_drawLoungeChair(float ox, float oy, float r, float g, float b) {
    glColor3f(0.65f, 0.52f, 0.25f);
    float legX[4] = {ox + 0.010f, ox + 0.072f, ox + 0.134f, ox + 0.174f};
    for (int i = 0; i < 4; i++) {
        glBegin(GL_QUADS);
        glVertex2f(legX[i],       oy); glVertex2f(legX[i] + 0.008f, oy);
        glVertex2f(legX[i] + 0.008f, oy - 0.026f); glVertex2f(legX[i], oy - 0.026f);
        glEnd();
    }

    glColor3f(0.88f, 0.82f, 0.62f);
    glBegin(GL_QUADS);
    glVertex2f(ox, oy); glVertex2f(ox + 0.200f, oy);
    glVertex2f(ox + 0.200f, oy + 0.018f); glVertex2f(ox, oy + 0.018f);
    glEnd();

    glColor3f(r, g, b);
    glBegin(GL_QUADS);
    glVertex2f(ox + 0.003f, oy + 0.018f); glVertex2f(ox + 0.197f, oy + 0.018f);
    glVertex2f(ox + 0.197f, oy + 0.030f); glVertex2f(ox + 0.003f, oy + 0.030f);
    glEnd();

    glColor3f(0.88f, 0.82f, 0.62f);
    glBegin(GL_QUADS);
    glVertex2f(ox + 0.168f, oy + 0.018f); glVertex2f(ox + 0.200f, oy + 0.018f);
    glVertex2f(ox + 0.200f, oy + 0.095f); glVertex2f(ox + 0.168f, oy + 0.095f);
    glEnd();
}

void s5_drawSeagulls() {
    glColor3f(0.10f, 0.22f, 0.38f);
    glLineWidth(1.8f);
    float birds[6][2] = {{-0.38f, 0.54f}, {-0.28f, 0.56f}, {0.12f, 0.66f},
                       {0.20f, 0.62f}, {0.24f, 0.69f}, {-0.60f, 0.44f}};
    for (int i = 0; i < 6; i++) {
        glBegin(GL_LINE_STRIP);
        glVertex2f(birds[i][0] - 0.030f, birds[i][1]);
        glVertex2f(birds[i][0],       birds[i][1] + 0.013f);
        glVertex2f(birds[i][0] + 0.030f, birds[i][1]);
        glEnd();
    }
    glLineWidth(1.0f);
}

void s5_draw_landscape_bg() {
    s5_drawOcean();
    s5_drawFoam();
    s5_drawSand();
}

void s5_draw_landscape_fg() {
    s5_drawSailboat(-0.50f, -0.115f, 0.55f);
    s5_drawPalmTree(-0.82f, -0.36f);

    s5_drawBeachUmbrella(-0.42f, -0.50f, 0.85f, 0.18f, 0.18f);
    s5_drawLoungeChair(  -0.56f, -0.570f, 0.82f, 0.18f, 0.18f);
    s5_drawLoungeChair(  -0.34f, -0.570f, 0.25f, 0.60f, 0.85f);

    s5_drawBeachUmbrella( 0.36f, -0.50f, 0.15f, 0.65f, 0.25f);
    s5_drawLoungeChair(   0.20f, -0.570f, 0.15f, 0.65f, 0.25f);
    s5_drawLoungeChair(   0.42f, -0.570f, 0.88f, 0.50f, 0.10f);
}

// ..........................................................................
// ................................ SCENE 6 .................................
// ..........................................................................

void s6_drawCircle(float cx, float cy, float r, int seg) {
    glBegin(GL_TRIANGLE_FAN);
    glVertex2f(cx, cy);
    for (int i = 0; i <= seg; i++) {
        float a = 2.0f * s6_PI * i / seg;
        glVertex2f(cx + r * cosf(a), cy + r * sinf(a));
    }
    glEnd();
}

void s6_drawEllipse(float cx, float cy, float rx, float ry, int seg) {
    glBegin(GL_TRIANGLE_FAN);
    glVertex2f(cx, cy);
    for (int i = 0; i <= seg; i++) {
        float a = 2.0f * s6_PI * i / seg;
        glVertex2f(cx + rx * cosf(a), cy + ry * sinf(a));
    }
    glEnd();
}

void s6_drawRect(float x, float y, float w, float h, float r, float g, float b) {
    glColor3f(r, g, b);
    glBegin(GL_QUADS);
    glVertex2f(x, y);         glVertex2f(x + w, y);
    glVertex2f(x + w, y + h); glVertex2f(x, y + h);
    glEnd();
}

void s6_drawSky() {
    glBegin(GL_QUADS);
    glColor3f(0.15f, 0.26f, 0.60f); glVertex2f(-2.0f, -0.35f);
    glColor3f(0.15f, 0.26f, 0.60f); glVertex2f( 2.0f, -0.35f);
    glColor3f(0.09f, 0.17f, 0.47f); glVertex2f( 2.0f,  0.30f);
    glColor3f(0.09f, 0.17f, 0.47f); glVertex2f(-2.0f,  0.30f);
    glEnd();
    glBegin(GL_QUADS);
    glColor3f(0.09f, 0.17f, 0.47f); glVertex2f(-2.0f, 0.30f);
    glColor3f(0.09f, 0.17f, 0.47f); glVertex2f( 2.0f, 0.30f);
    glColor3f(0.056f, 0.09f, 0.28f); glVertex2f( 2.0f, 1.20f);
    glColor3f(0.056f, 0.09f, 0.28f); glVertex2f(-2.0f, 1.20f);
    glEnd();
}

void s6_drawStars() {
    srand(12345);
    glPointSize(1.2f);
    glColor3f(0.85f, 0.90f, 1.0f);
    glBegin(GL_POINTS);
    for (int i = 0; i < 350; i++) {
        float x = ((float)rand() / RAND_MAX) * 4.0f - 2.0f;
        float y = ((float)rand() / RAND_MAX) * 1.55f - 0.35f;
        glVertex2f(x, y);
    }
    glEnd();

    glPointSize(2.5f);
    glColor3f(1.0f, 1.0f, 1.0f);
    glBegin(GL_POINTS);
    for (int i = 0; i < 80; i++) {
        float x = ((float)rand() / RAND_MAX) * 4.0f - 2.0f;
        float y = ((float)rand() / RAND_MAX) * 1.55f - 0.35f;
        glVertex2f(x, y);
    }
    glEnd();
    glPointSize(1.0f);
}

void s6_drawMoon() {
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    float mx = 0.55f, my = 0.62f;

    glColor4f(0.85f, 0.90f, 1.00f, 0.10f); s6_drawCircle(mx, my, 0.22f, 80);
    glColor4f(0.85f, 0.90f, 1.00f, 0.16f); s6_drawCircle(mx, my, 0.16f, 80);
    glColor4f(0.96f, 0.98f, 0.85f, 1.00f); s6_drawCircle(mx, my, 0.110f, 80);

    glDisable(GL_BLEND);
    glColor3f(0.09f, 0.17f, 0.47f);
    s6_drawCircle(mx + 0.068f, my + 0.018f, 0.095f, 80);

    glEnable(GL_BLEND);
    glColor4f(0.70f, 0.74f, 0.60f, 0.25f);
    s6_drawCircle(mx - 0.022f, my + 0.015f, 0.040f, 40);
    glColor4f(0.70f, 0.74f, 0.60f, 0.15f);
    s6_drawCircle(mx - 0.010f, my - 0.030f, 0.025f, 30);
    glDisable(GL_BLEND);
}

void s6_drawMountains() {
    glColor3f(0.38f, 0.24f, 0.14f);
    glBegin(GL_TRIANGLES);
    glVertex2f(-1.10f, -0.38f); glVertex2f(-0.55f,  0.28f); glVertex2f( 0.00f, -0.38f);
    glVertex2f( 0.10f, -0.38f); glVertex2f( 0.65f,  0.22f); glVertex2f( 1.15f, -0.38f);
    glVertex2f(-0.20f, -0.38f); glVertex2f( 0.10f,  0.08f); glVertex2f( 0.40f, -0.38f);
    glVertex2f(-1.60f, -0.38f); glVertex2f(-1.20f,  0.15f); glVertex2f(-0.80f, -0.38f);
    glVertex2f(-0.60f, -0.38f); glVertex2f(-0.25f,  0.18f); glVertex2f( 0.10f, -0.38f);
    glVertex2f( 0.60f, -0.38f); glVertex2f( 0.95f,  0.10f); glVertex2f( 1.40f, -0.38f);
    glVertex2f( 1.30f, -0.38f); glVertex2f( 1.85f,  0.28f); glVertex2f( 2.40f, -0.38f);
    glEnd();

    glColor3f(0.32f, 0.18f, 0.10f);
    glBegin(GL_TRIANGLES);
    glVertex2f(-0.90f, -0.40f); glVertex2f(-0.48f,  0.12f); glVertex2f(-0.05f, -0.40f);
    glVertex2f( 0.30f, -0.40f); glVertex2f( 0.72f,  0.16f); glVertex2f( 1.10f, -0.40f);
    glVertex2f( 1.50f, -0.40f); glVertex2f( 1.92f,  0.12f); glVertex2f( 2.35f, -0.40f);
    glEnd();

    glColor3f(0.24f, 0.12f, 0.06f);
    glBegin(GL_TRIANGLES);
    glVertex2f(-1.20f, -0.42f); glVertex2f(-0.72f,  0.05f); glVertex2f(-0.28f, -0.42f);
    glVertex2f( 0.25f, -0.42f); glVertex2f( 0.68f,  0.08f); glVertex2f( 1.20f, -0.42f);
    glVertex2f(-0.35f, -0.42f); glVertex2f(-0.08f, -0.12f); glVertex2f( 0.22f, -0.42f);
    glVertex2f( 1.20f, -0.42f); glVertex2f( 1.68f,  0.05f); glVertex2f( 2.12f, -0.42f);
    glEnd();
}

void s6_drawGround() {
    glBegin(GL_QUADS);
    glColor3f(0.45f, 0.34f, 0.22f); glVertex2f(-2.0f, -0.42f);
    glColor3f(0.45f, 0.34f, 0.22f); glVertex2f( 2.0f, -0.42f);
    glColor3f(0.34f, 0.24f, 0.15f); glVertex2f( 2.0f, -1.20f);
    glColor3f(0.34f, 0.24f, 0.15f); glVertex2f(-2.0f, -1.20f);
    glEnd();

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glBegin(GL_QUADS);
    glColor4f(0.70f, 0.75f, 0.95f, 0.00f); glVertex2f(-2.0f, -0.55f);
    glColor4f(0.70f, 0.75f, 0.95f, 0.00f); glVertex2f( 2.0f, -0.55f);
    glColor4f(0.70f, 0.75f, 0.95f, 0.15f); glVertex2f( 2.0f, -0.42f);
    glColor4f(0.70f, 0.75f, 0.95f, 0.15f); glVertex2f(-2.0f, -0.42f);
    glEnd();
    glDisable(GL_BLEND);
}

void s6_drawSandDots() {
    glPointSize(2.0f);
    glBegin(GL_POINTS);
    for (float x = -2.0f; x <= 2.0f; x += 0.06f) {
        for (float y = -1.2f; y <= -0.42f; y += 0.06f) {
            float offsetX = sinf(y * 123.456f) * 0.03f;
            float offsetY = cosf(x * 654.321f) * 0.03f;
            float px = x + offsetX;
            float py = y + offsetY;

            if (py < -0.65f && py > -0.98f) continue;
            float depth = (py + 1.2f) / 0.78f;
            glColor3f(0.35f * depth, 0.22f * depth, 0.12f * depth);
            glVertex2f(px, py);
        }
    }
    glEnd();
    glPointSize(1.0f);
}

void s6_drawRoad() {
    glBegin(GL_QUADS);
    glColor3f(0.08f, 0.08f, 0.08f); glVertex2f(-2.0f, -0.68f);
    glColor3f(0.08f, 0.08f, 0.08f); glVertex2f( 2.0f, -0.68f);
    glColor3f(0.05f, 0.05f, 0.05f); glVertex2f( 2.0f, -0.95f);
    glColor3f(0.05f, 0.05f, 0.05f); glVertex2f(-2.0f, -0.95f);
    glEnd();

    glColor3f(0.7f, 0.6f, 0.2f);
    glLineWidth(3.0f);
    glBegin(GL_LINES);
    for(float x = -2.0f; x < 2.0f; x += 0.25f) {
        glVertex2f(x, -0.81f);
        glVertex2f(x + 0.12f, -0.81f);
    }
    glEnd();
    glLineWidth(1.0f);
}

void s6_drawCar(float x, float y, float r, float g, float b) {
    s6_drawRect(x - 0.15f, y, 0.30f, 0.06f, r, g, b);
    s6_drawRect(x - 0.08f, y + 0.06f, 0.16f, 0.05f, r * 0.8f, g * 0.8f, b * 0.8f);
    s6_drawRect(x - 0.07f, y + 0.065f, 0.06f, 0.035f, 0.7f, 0.8f, 0.9f);
    s6_drawRect(x + 0.01f, y + 0.065f, 0.06f, 0.035f, 0.7f, 0.8f, 0.9f);
    s6_drawRect(x - 0.16f, y + 0.02f, 0.03f, 0.03f, 1.0f, 0.9f, 0.5f);
    s6_drawRect(x + 0.14f, y + 0.02f, 0.02f, 0.03f, 0.9f, 0.1f, 0.1f);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glBegin(GL_TRIANGLES);
    glColor4f(1.0f, 0.9f, 0.5f, 0.35f); glVertex2f(x - 0.16f, y + 0.035f);
    glColor4f(1.0f, 0.9f, 0.5f, 0.00f); glVertex2f(x - 0.65f, y - 0.060f);
    glColor4f(1.0f, 0.9f, 0.5f, 0.00f); glVertex2f(x - 0.65f, y + 0.120f);
    glEnd();
    glDisable(GL_BLEND);

    glColor3f(0.02f, 0.02f, 0.02f);
    s6_drawCircle(x - 0.09f, y, 0.035f, 20);
    s6_drawCircle(x + 0.09f, y, 0.035f, 20);
    glColor3f(0.6f, 0.6f, 0.6f);
    s6_drawCircle(x - 0.09f, y, 0.015f, 15);
    s6_drawCircle(x + 0.09f, y, 0.015f, 15);
}

void s6_drawTent(float x, float y, float s, float tc_r, float tc_g, float tc_b, bool hasFireLight) {
    glColor3f(tc_r, tc_g, tc_b);
    glBegin(GL_POLYGON);
    glVertex2f(x - 0.28f*s, y);
    glVertex2f(x,             y + 0.22f*s);
    glVertex2f(x + 0.28f*s, y);
    glEnd();

    if (hasFireLight) {
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        glColor4f(0.90f, 0.40f, 0.05f, 0.25f);
        glBegin(GL_TRIANGLES);
        glVertex2f(x,             y + 0.22f*s);
        glVertex2f(x + 0.28f*s, y);
        glVertex2f(x + 0.10f*s, y);
        glEnd();
        glDisable(GL_BLEND);
    }

    glColor3f(tc_r * 0.6f, tc_g * 0.6f, tc_b * 0.6f);
    glLineWidth(2.0f);
    glBegin(GL_LINES);
    glVertex2f(x, y + 0.22f*s);
    glVertex2f(x, y + 0.26f*s);
    glEnd();
    glLineWidth(1.0f);

    glColor3f(tc_r * 0.8f, tc_g * 0.8f, tc_b * 0.8f);
    glBegin(GL_POLYGON);
    glVertex2f(x - 0.28f*s, y);
    glVertex2f(x - 0.10f*s, y + 0.08f*s);
    glVertex2f(x - 0.38f*s, y + 0.04f*s);
    glVertex2f(x - 0.44f*s, y - 0.03f*s);
    glEnd();

    glColor3f(tc_r * 0.3f, tc_g * 0.3f, tc_b * 0.3f);
    glBegin(GL_POLYGON);
    glVertex2f(x + 0.04f*s, y);
    glVertex2f(x + 0.18f*s, y);
    glVertex2f(x + 0.16f*s, y + 0.08f*s);
    glVertex2f(x + 0.06f*s, y + 0.08f*s);
    glEnd();

    glColor3f(0.35f, 0.26f, 0.17f);
    glLineWidth(1.0f);
    glBegin(GL_LINES);
    glVertex2f(x,             y + 0.22f*s); glVertex2f(x - 0.50f*s, y - 0.02f*s);
    glVertex2f(x,             y + 0.22f*s); glVertex2f(x + 0.50f*s, y - 0.02f*s);
    glVertex2f(x - 0.10f*s,  y + 0.14f*s); glVertex2f(x - 0.55f*s, y - 0.02f*s);
    glEnd();

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glColor4f(0.12f, 0.075f, 0.03f, 0.45f);
    glLineWidth(0.8f);
    for (int i = 1; i <= 3; i++) {
        float t = (float)i / 4.0f;
        float lx = x - 0.28f*s + t * 0.56f*s;
        float ly = y + (0.22f*s) * (1.0f - fabsf(t - 0.5f) * 2.0f) * 0.5f;
        glBegin(GL_LINES);
        glVertex2f(lx, y);
        glVertex2f(x + (lx - x) * 0.4f, y + 0.22f*s * 0.7f);
        glEnd();
    }
    glDisable(GL_BLEND);
}

void s6_drawCampfire(float fx, float fy, float s) {
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glColor4f(1.0f, 0.45f, 0.05f, 0.25f);
    s6_drawEllipse(fx, fy - 0.02f*s, 0.35f*s, 0.10f*s, 50);
    glColor4f(1.0f, 0.55f, 0.10f, 0.40f);
    s6_drawEllipse(fx, fy - 0.02f*s, 0.20f*s, 0.06f*s, 50);
    glDisable(GL_BLEND);

    glColor3f(0.30f, 0.18f, 0.08f);
    glLineWidth(4.0f);
    glBegin(GL_LINES);
    glVertex2f(fx - 0.06f*s, fy); glVertex2f(fx + 0.05f*s, fy + 0.04f*s);
    glVertex2f(fx + 0.06f*s, fy); glVertex2f(fx - 0.04f*s, fy + 0.035f*s);
    glEnd();
    glLineWidth(1.0f);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glColor4f(0.95f, 0.35f, 0.02f, 0.75f);
    glBegin(GL_TRIANGLES);
    glVertex2f(fx - 0.05f*s, fy + 0.005f*s);
    glVertex2f(fx + 0.05f*s, fy + 0.005f*s);
    glVertex2f(fx,             fy + 0.15f*s);
    glEnd();

    glColor4f(1.00f, 0.60f, 0.05f, 0.85f);
    glBegin(GL_TRIANGLES);
    glVertex2f(fx - 0.03f*s, fy + 0.008f*s);
    glVertex2f(fx + 0.03f*s, fy + 0.008f*s);
    glVertex2f(fx,             fy + 0.10f*s);
    glEnd();

    glColor4f(1.00f, 0.92f, 0.40f, 0.95f);
    glBegin(GL_TRIANGLES);
    glVertex2f(fx - 0.015f*s, fy + 0.010f*s);
    glVertex2f(fx + 0.015f*s, fy + 0.010f*s);
    glVertex2f(fx,             fy + 0.06f*s);
    glEnd();

    glDisable(GL_BLEND);
}

void s6_drawCactus(float bx, float by, float s) {
    float cr = 0.15f, cg = 0.23f, cb = 0.11f;

    s6_drawRect(bx - 0.028f*s, by, 0.056f*s, 0.32f*s, cr, cg, cb);
    glColor3f(cr, cg, cb);
    s6_drawEllipse(bx, by + 0.32f*s, 0.028f*s, 0.020f*s, 30);

    s6_drawRect(bx - 0.082f*s, by + 0.14f*s, 0.056f*s, 0.026f*s, cr, cg, cb);
    s6_drawRect(bx - 0.082f*s, by + 0.14f*s, 0.026f*s, 0.11f*s,  cr, cg, cb);
    glColor3f(cr, cg, cb);
    s6_drawEllipse(bx - 0.069f*s, by + 0.25f*s, 0.016f*s, 0.013f*s, 20);

    s6_drawRect(bx + 0.025f*s, by + 0.18f*s, 0.060f*s, 0.026f*s, cr, cg, cb);
    s6_drawRect(bx + 0.060f*s, by + 0.18f*s, 0.026f*s, 0.09f*s,  cr, cg, cb);
    glColor3f(cr, cg, cb);
    s6_drawEllipse(bx + 0.073f*s, by + 0.27f*s, 0.016f*s, 0.013f*s, 20);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glColor4f(0.60f, 0.70f, 0.85f, 0.40f);
    glLineWidth(1.0f);
    glBegin(GL_LINES);
    glVertex2f(bx - 0.027f*s, by + 0.03f*s);
    glVertex2f(bx - 0.027f*s, by + 0.30f*s);
    glEnd();
    glDisable(GL_BLEND);
}

void s6_drawCacti() {
    s6_drawCactus(-0.82f, -0.42f, 1.10f);
    s6_drawCactus( 0.75f, -0.44f, 1.00f);
    s6_drawCactus(-0.50f, -0.41f, 0.68f);
    s6_drawCactus( 0.46f, -0.42f, 0.65f);
    s6_drawCactus(-1.20f, -0.40f, 0.85f);
    s6_drawCactus( 1.15f, -0.43f, 0.90f);
    s6_drawCactus(-0.10f, -0.37f, 0.50f);
}

void s6_drawBird(float x, float y, float size) {
    glBegin(GL_LINE_STRIP);
    for (int i = 0; i <= 10; i++) {
        float t = (float)i / 10.0f;
        glVertex2f(x - size + t * size, y + sinf(t * s6_PI) * size * 0.45f);
    }
    glEnd();
    glBegin(GL_LINE_STRIP);
    for (int i = 0; i <= 10; i++) {
        float t = (float)i / 10.0f;
        glVertex2f(x + t * size, y + sinf(t * s6_PI) * size * 0.45f);
    }
    glEnd();
}

void s6_drawBirds() {
    glColor3f(0.06f, 0.06f, 0.12f);
    glLineWidth(1.6f);
    s6_drawBird(-0.50f, 0.45f, 0.038f);
    s6_drawBird(-0.38f, 0.48f, 0.032f);
    s6_drawBird(-0.28f, 0.44f, 0.028f);
    s6_drawBird( 0.35f, 0.38f, 0.035f);
    s6_drawBird( 0.46f, 0.40f, 0.030f);
    glLineWidth(1.0f);
}

void s6_draw_loopable_landscape() {
    s6_drawMountains();
    s6_drawGround();
    s6_drawSandDots();
    s6_drawRoad();
    s6_drawCacti();
    s6_drawTent(-0.35f, -0.42f, 1.0f, 0.50f, 0.25f, 0.05f, true);
    s6_drawTent( 0.15f, -0.44f, 0.70f, 0.15f, 0.30f, 0.55f, true);
    s6_drawTent( 0.65f, -0.43f, 0.85f, 0.15f, 0.30f, 0.55f, true);
    s6_drawCampfire(-0.20f, -0.52f, 1.1f);
    s6_drawCampfire( 0.28f, -0.53f, 0.8f);
    s6_drawCampfire( 0.80f, -0.52f, 0.9f);
}


// ..........................................................................
// .......................... GAMEPLAY LOGIC......................
// ..........................................................................

void update(int value) {
    if (!is_game_over) {
        cloud_offset += 0.7f;
    }

    if (current_scene == 1) {
        if (plane_phase == 0) {
            plane_x += 2.2f;
            if (plane_x > 420.0f) plane_phase = 1;
        }
        else if (plane_phase == 1) {
            plane_x += 3.5f;
            plane_y += 2.8f;
            if (plane_angle < 18.0f) plane_angle += 0.45f;
            if (plane_y > 320.0f) plane_phase = 2;
        }
        else {
            plane_x += 4.0f;
            plane_y += 0.3f;
            if (plane_angle > 6.0f) plane_angle -= 0.08f;
        }

        scene1_bg_offset = (plane_x - 60.0f) * (1200.0f / 1390.0f);

        // Auto transition to Scene 2
        if (plane_x > (float)WIDTH + 250.0f) {
            current_scene = 2;
            scene2_frames = 0; // Reset timer
            plane_x = -200.0f; plane_y = 95.0f;
            plane_angle = 0.0f; plane_phase = 0;
            scene1_bg_offset = 0.0f;
        }
    }
    else if (current_scene >= 2) {

        // Background-specific updates based on which scene is visible
        if (current_scene == 2) {
            if (!is_game_over) {
                scene2_bg_offset += 3.5f;

                car1_x += 5.0f; car2_x += 4.2f; car3_x += 5.5f;
                if (car1_x > WIDTH + 100) car1_x = -100.0f;
                if (car2_x > WIDTH + 100) car2_x = -150.0f;
                if (car3_x > WIDTH + 100) car3_x = -200.0f;

                car4_x -= 6.0f;
                if (car4_x < -100) car4_x = WIDTH + 100.0f;

                p1_x += 1.2f; p2_x -= 1.0f; p3_x += 1.5f;
                if (p1_x > WIDTH + 50) p1_x = -50.0f;
                if (p2_x < -50) p2_x = WIDTH + 50.0f;
                if (p3_x > WIDTH + 50) p3_x = -50.0f;

                // Scene 2 to Scene 3 10-second background transition logic
                scene2_frames++;
                if (scene2_frames > 600) { // ~10 seconds at 60 fps
                    current_scene = 3;
                    scene3_frames = 0; // Prepare scene 3 timer
                }
            }
        }
        else if (current_scene == 3) {
            if (!is_game_over) {
                scene3_bg_offset += 0.006f;
                train_offset_x -= 0.008f;
                if (train_offset_x < -2.5f) train_offset_x = 2.5f;

                // Scene 3 to Scene 4 10-second background transition logic
                scene3_frames++;
                if (scene3_frames > 600) {
                    current_scene = 4;
                    scene4_frames = 0;
                }
            }
        }
        else if (current_scene == 4) {
            if (!is_game_over) {
                scene4_bg_offset += 2.5f; // Scroll landscape continuously
                s4_boat_offset += 1.5f;

                // Scene 4 to Scene 5 10-second background transition logic
                scene4_frames++;
                if (scene4_frames > 600) {
                    current_scene = 5;
                    scene5_frames = 0;
                }
            }
        }
        else if (current_scene == 5) {
            if (!is_game_over) {
                scene5_bg_offset += 0.006f; // Adjust parallax background speed
                s5_ship_x -= 0.003f;        // Ship moves opposite of plane
                if (s5_ship_x < -1.5f) {
                    s5_ship_x = 1.5f;
                }

                // Scene 5 to Scene 6 10-second background transition logic
                scene5_frames++;
                if (scene5_frames > 600) {
                    current_scene = 6;
                }
            }
        }
        else if (current_scene == 6) {
            if (!is_game_over) {
                scene6_bg_offset += 0.005f; // Scroll Scene 6 landscape
                s6_car1_x -= 0.008f;        // Scene 6 cars moving opposite to plane
                s6_car2_x -= 0.013f;
                if (s6_car1_x < -2.5f) s6_car1_x = 2.5f;
                if (s6_car2_x < -2.5f) s6_car2_x = 2.5f;
            }
        }

        // SHARED GAMEPLAY LOGIC (Obstacles, Plane, Coins, Score)
        if (!is_game_over) {
            if (collision_flash > 0) collision_flash--;
        }

        if (!plane_crashed) {
            bob_time += 0.05f;
            scene2_plane_bob = sinf(bob_time) * 15.0f;

            obs_x -= obs_speed;
            if (obs_x < -100.0f) {
                obs_x = WIDTH + 100.0f;
                obs_y = 300.0f + (rand() % 300);
            }

            coin_x -= coin_speed;
            if (coin_x < -50.0f) {
                coin_x = WIDTH + (rand() % 400) + 100.0f;
                coin_y = 250.0f + (rand() % 350);
                coin_type = (rand() % 3 == 0) ? 2 : 1;
            }

            float px = (WIDTH / 4.0f) + 65.0f;
            float py = scene2_plane_base_y + scene2_plane_bob;

            float dx_obs = obs_x - px;
            float dy_obs = obs_y - py;
            if (sqrt(dx_obs * dx_obs + dy_obs * dy_obs) < 60.0f) {
                collision_flash = 15;
                plane_life--;

                if (plane_life <= 0) plane_crashed = true;

                obs_x = WIDTH + 100.0f;
                obs_y = 300.0f + (rand() % 300);
            }

            float dcx = coin_x - px;
            float dcy = coin_y - py;
            float dist_coin = sqrt(dcx * dcx + dcy * dcy);
            float hit_radius = (coin_type == 1) ? 40.0f : 50.0f;

            if (dist_coin < hit_radius) {
                if (coin_type == 1) player_score += 10;
                else player_score += 20;

                coin_x = WIDTH + (rand() % 400) + 100.0f;
                coin_y = 250.0f + (rand() % 350);
                coin_type = (rand() % 3 == 0) ? 2 : 1;
            }
        } else {
            if (!is_game_over) {
                crash_fall_speed += 0.5f;
                scene2_plane_base_y -= crash_fall_speed;

                if (scene2_plane_base_y <= 160.0f) {
                    scene2_plane_base_y = 160.0f;
                    is_game_over = true;
                }
            }
        }
    }

    glutPostRedisplay();
    glutTimerFunc(16, update, 0);
}


// ..........................................................................
// ............................. KEYBOARD & INPUT ...........................
// ..........................................................................

static void key(unsigned char k, int x, int y) {
    if (k == 27 || k == 'q') exit(0);

    if (k == '1') {
        current_scene = 1;
        scene2_frames = 0; scene3_frames = 0; scene4_frames = 0; scene5_frames = 0;
        scene3_bg_offset = 0.0f; scene4_bg_offset = 0.0f;
        scene5_bg_offset = 0.0f; s5_ship_x = 1.5f;
        scene6_bg_offset = 0.0f; s6_car1_x = 2.0f; s6_car2_x = 2.5f;

        plane_x = 60.0f; plane_y = 95.0f; plane_angle = 0.0f;
        plane_phase = 0; scene1_bg_offset = 0.0f;

        plane_life = 3; plane_crashed = false;
        is_game_over = false; crash_fall_speed = 0.0f;
        scene2_plane_base_y = 480.0f;
        player_score = 0; coin_x = WIDTH + 500.0f;
    }
    if (k == '2') {
        current_scene = 2;
        scene2_frames = 0;
    }
    if (k == '3') {
        current_scene = 3;
        scene3_frames = 0;
    }
    if (k == '4') {
        current_scene = 4;
        scene4_frames = 0;
    }
    if (k == '5') {
        current_scene = 5;
        scene5_frames = 0;
    }
    if (k == '6') {
        current_scene = 6;
    }

    glutPostRedisplay();
}

static void specialKey(int key, int x, int y) {
    if (current_scene >= 2 && !plane_crashed) {
        if (key == GLUT_KEY_UP) {
            scene2_plane_base_y += 20.0f;
        } else if (key == GLUT_KEY_DOWN) {
            scene2_plane_base_y -= 20.0f;
        }
        if (scene2_plane_base_y > HEIGHT - 50.0f) scene2_plane_base_y = HEIGHT - 50.0f;
        if (scene2_plane_base_y < 250.0f) scene2_plane_base_y = 250.0f;
    }
    glutPostRedisplay();
}


// ..........................................................................
// .......................... MASTER RENDER PIPELINE ........................
// ..........................................................................

void draw_shared_sky_sun_clouds(int scene_number) {
    draw_sky();
    if (scene_number == 1) {
        draw_sun_scene2(250, 580); // Using scene 2 sun logic for matching size
    } else {
        draw_sun_scene2((float)WIDTH / 2.0f, 580.0f);
    }
    draw_clouds();
}

// Extracted the shared gameplay elements so they can be drawn over Scene 2, 3, 4, 5, OR 6
void draw_scene2_foreground() {
    if (!plane_crashed) {
        draw_obstacle(obs_x, obs_y);
        draw_coin(coin_x, coin_y, coin_type);
    }

    glPushMatrix();
        float cruise_x = WIDTH / 4.0f;
        float cruise_y;
        float cruise_angle;
        if (!plane_crashed) {
            cruise_y = scene2_plane_base_y + scene2_plane_bob;
            cruise_angle = 4.0f;
        } else {
            cruise_y = scene2_plane_base_y;
            cruise_angle = -35.0f;
            if (scene2_plane_base_y <= 160.0f) {
                cruise_angle = -10.0f;
            }
        }
        float pivot_x = cruise_x + 105.0f;
        float pivot_y = cruise_y;
        glTranslatef(pivot_x, pivot_y, 0.0f);
        glRotatef(cruise_angle, 0.0f, 0.0f, 1.0f);
        glTranslatef(-pivot_x, -pivot_y, 0.0f);
        draw_airplane(cruise_x, cruise_y);
    glPopMatrix();

    if (collision_flash > 0 && !plane_crashed) {
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        glColor4f(1.0f, 0.0f, 0.0f, 0.3f);
        glBegin(GL_QUADS);
            glVertex2f(0, 0); glVertex2f(WIDTH, 0);
            glVertex2f(WIDTH, HEIGHT); glVertex2f(0, HEIGHT);
        glEnd();
        glDisable(GL_BLEND);
    }

    std::string life_str;
    std::string score_str = "Score: " + std::to_string(player_score);
    if (plane_life > 0) {
        life_str = "Remaining Life: " + std::to_string(plane_life);
    } else {
        life_str = "PLANE CRASHED!";
    }
    fillRect(15.0f, HEIGHT - 65.0f, 180.0f, 50.0f, 0.0f, 0.0f, 0.0f);
    drawText(life_str.c_str(), 25.0f, HEIGHT - 35.0f);
    drawWhiteText(score_str.c_str(), 25.0f, HEIGHT - 55.0f);

    if (is_game_over) {
        float popup_w = 400.0f; float popup_h = 120.0f;
        float popup_x = (WIDTH - popup_w) / 2.0f;
        float popup_y = (HEIGHT - popup_h) / 2.0f;

        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        glColor4f(0.0f, 0.0f, 0.0f, 0.5f);
        glBegin(GL_QUADS);
            glVertex2f(0, 0); glVertex2f(WIDTH, 0);
            glVertex2f(WIDTH, HEIGHT); glVertex2f(0, HEIGHT);
        glEnd();
        glDisable(GL_BLEND);

        fillRect(popup_x, popup_y, popup_w, popup_h, 0.0f, 0.0f, 0.0f);
        std::string game_over_str = "GAME OVER!";
        std::string final_score_str = "Your Score: " + std::to_string(player_score);
        std::string restart_str = "Press 1 to Restart";
        drawText(game_over_str.c_str(), popup_x + 130.0f, popup_y + 80.0f);
        drawWhiteText(final_score_str.c_str(), popup_x + 135.0f, popup_y + 50.0f);
        drawWhiteText(restart_str.c_str(), popup_x + 110.0f, popup_y + 20.0f);
    }
}

static void resize(int width, int height) {
    glViewport(0, 0, width, height);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluOrtho2D(0.0, (float)WIDTH, 0.0, (float)HEIGHT);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
}

static void display(void) {
    glClear(GL_COLOR_BUFFER_BIT);

    // ..........................................................................
    // ................................ SCENE 1 .................................
    // ..........................................................................
    if (current_scene == 1) {
        draw_shared_sky_sun_clouds(1);

        float bg_wrap1 = fmod(scene1_bg_offset, 1200.0f);
        glPushMatrix();
            glTranslatef(-bg_wrap1, 0.0f, 0.0f);
            draw_scene1_environment();
            glTranslatef(1200.0f, 0.0f, 0.0f);
            draw_scene1_environment();
        glPopMatrix();

        glPushMatrix();
            float pivot_x = plane_x + 105.0f;
            float pivot_y = plane_y;
            glTranslatef(pivot_x, pivot_y, 0.0f);
            glRotatef(plane_angle, 0.0f, 0.0f, 1.0f);
            glTranslatef(-pivot_x, -pivot_y, 0.0f);
            draw_airplane(plane_x, plane_y);
        glPopMatrix();
    }
    // ..........................................................................
    // ................................ SCENE 2 .................................
    // ..........................................................................
    else if (current_scene == 2) {
        draw_shared_sky_sun_clouds(2);

        float bg_wrap = fmod(scene2_bg_offset, 1200.0f);
        glPushMatrix();
            glTranslatef(-bg_wrap, 0.0f, 0.0f);
            draw_scene2_cityscape();
            glTranslatef(1200.0f, 0.0f, 0.0f);
            draw_scene2_cityscape();
        glPopMatrix();

        float road_parallax = scene2_bg_offset * 1.5f;
        draw_highway(road_parallax);
        draw_footpath(road_parallax);

        float road_wrap = fmod(road_parallax, 1200.0f);
        glPushMatrix();
            glTranslatef(-road_wrap, 0.0f, 0.0f);
            draw_street_lamp(250.0f, 25.0f);
            draw_street_lamp(850.0f, 25.0f);
            glTranslatef(1200.0f, 0.0f, 0.0f);
            draw_street_lamp(250.0f, 25.0f);
            draw_street_lamp(850.0f, 25.0f);
        glPopMatrix();

        draw_person(p1_x, 2, 0.8f, 0.2f, 0.2f, true);
        draw_person(p2_x, 5, 0.2f, 0.6f, 0.3f, false);
        draw_person(p3_x, 2, 0.2f, 0.4f, 0.8f, true);

        draw_car(car4_x, 100, 1.0f, 0.1f, 0.1f, false);
        draw_car(car1_x, 35,  0.1f, 0.4f, 1.0f, true);
        draw_car(car2_x, 35,  1.0f, 0.9f, 0.0f, true);
        draw_car(car3_x, 35,  0.1f, 0.9f, 0.1f, true);

        draw_scene2_foreground();
    }
    // ..........................................................................
    // ................................ SCENE 3 .................................
    // ..........................................................................
    else if (current_scene == 3) {

        glPushMatrix();
        glTranslatef((float)WIDTH / 2.0f, (float)HEIGHT / 2.0f, 0.0f);
        glScalef((float)WIDTH / 2.0f, (float)HEIGHT / 2.0f, 1.0f);

        s3_drawSky();
        s3_drawSun(0.65f, 0.72f, 0.095f);
        s3_drawClouds();

        float bg_wrap3 = fmod(scene3_bg_offset, 2.0f);

        glPushMatrix();
            glTranslatef(-bg_wrap3, 0.0f, 0.0f);
            draw_scene3_scrolling_elements();
            glTranslatef(2.0f, 0.0f, 0.0f);
            draw_scene3_scrolling_elements();
        glPopMatrix();

        glColor3f(0.0f, 0.0f, 0.0f);
        float wr = 0.036f;
        float wyc = -0.792f + wr;
        float BASE_Y  = wyc + 0.018f;
        s3_drawQuad(-2.0f, BASE_Y - 0.02f, 2.0f, BASE_Y + 0.220f + 0.03f);

        glPushMatrix();
        glTranslatef(train_offset_x, 0.0f, 0.0f);
        s3_drawMetroTrain_Moving();
        glPopMatrix();

        glPopMatrix();

        draw_scene2_foreground();
    }
    // ..........................................................................
    // ................................ SCENE 4 .................................
    // ..........................................................................
    else if (current_scene == 4) {

        glPushMatrix();
        glScalef(1200.0f / 1000.0f, 700.0f / 600.0f, 1.0f);

        s4_sky();
        s4_Sun();
        s4_draw_clouds();

        float bg_wrap4 = fmod(scene4_bg_offset, 1000.0f);

        glPushMatrix();
            glTranslatef(-bg_wrap4, 0.0f, 0.0f);
            s4_draw_landscape_static();
            s4_draw_boats_animated();

            glTranslatef(1000.0f, 0.0f, 0.0f);
            s4_draw_landscape_static();
            s4_draw_boats_animated();
        glPopMatrix();

        glPopMatrix();

        draw_scene2_foreground();
    }
    // ..........................................................................
    // ................................ SCENE 5 .................................
    // ..........................................................................
    else if (current_scene == 5) {

        glPushMatrix();
        glTranslatef((float)WIDTH / 2.0f, (float)HEIGHT / 2.0f, 0.0f);
        glScalef((float)WIDTH / 2.0f, (float)HEIGHT / 2.0f, 1.0f);
        s5_drawSky();
        s5_drawSun();
        s5_drawHorizonHaze();
        glPopMatrix();

        draw_clouds();

        glPushMatrix();
        glTranslatef((float)WIDTH / 2.0f, (float)HEIGHT / 2.0f, 0.0f);
        glScalef((float)WIDTH / 2.0f, (float)HEIGHT / 2.0f, 1.0f);

        float bg_wrap5 = fmod(scene5_bg_offset, 2.0f);

        glPushMatrix();
            glTranslatef(-bg_wrap5, 0.0f, 0.0f);
            s5_draw_landscape_bg();
            glTranslatef(2.0f, 0.0f, 0.0f);
            s5_draw_landscape_bg();
        glPopMatrix();

        s5_drawShip(s5_ship_x, -0.080f, 1.0f);

        glPushMatrix();
            glTranslatef(-bg_wrap5, 0.0f, 0.0f);
            s5_draw_landscape_fg();
            glTranslatef(2.0f, 0.0f, 0.0f);
            s5_draw_landscape_fg();
        glPopMatrix();

        s5_drawSeagulls();

        glPopMatrix();

        draw_scene2_foreground();
    }
    // ..........................................................................
    // ................................ SCENE 6 .................................
    // ..........................................................................
    else if (current_scene == 6) {

        glPushMatrix();
        glTranslatef((float)WIDTH / 2.0f, (float)HEIGHT / 2.0f, 0.0f);
        glScalef((float)WIDTH / 2.0f, (float)HEIGHT / 2.0f, 1.0f);

        // 1. Static Sky elements
        s6_drawSky();
        s6_drawStars();
        s6_drawMoon();

        float bg_wrap6 = fmod(scene6_bg_offset, 4.0f);

        glPushMatrix();
            glTranslatef(-bg_wrap6, 0.0f, 0.0f);
            s6_draw_loopable_landscape();


            glTranslatef(4.0f, 0.0f, 0.0f);
            s6_draw_loopable_landscape();
        glPopMatrix();


        s6_drawCar(s6_car1_x, -0.76f, 0.6f, 0.15f, 0.15f);
        s6_drawCar(s6_car2_x, -0.88f, 0.15f, 0.3f, 0.6f);


        s6_drawBirds();

        glPopMatrix();


        draw_scene2_foreground();
    }

    glutSwapBuffers();
}


// ─── ENTRY ───────────────────────────────────────────────────────────────────

int main(int argc, char *argv[]) {
    srand((unsigned int)time(NULL));

    glutInit(&argc, argv);
    glutInitWindowSize(WIDTH, HEIGHT);
    glutInitWindowPosition(100, 50);
    glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE);

    glutCreateWindow("Enhanced Scene Toggle (1: Airport, 2: City, 3: Metro, 4: Village, 5: Beach, 6: Desert)");

    glutReshapeFunc(resize);
    glutDisplayFunc(display);

    glutKeyboardFunc(key);
    glutSpecialFunc(specialKey);

    glClearColor(0.48f, 0.74f, 0.93f, 1.0f);
    glEnable(GL_LINE_SMOOTH);
    glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);
    glEnable(GL_POINT_SMOOTH);

    glutTimerFunc(16, update, 0);
    glutMainLoop();
    return EXIT_SUCCESS;
}