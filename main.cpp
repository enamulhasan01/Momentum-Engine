
/*
 * Static GLUT Scene 1: Airport Takeoff
 */

#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif
#include <cmath>
#include <stdlib.h>

const int WIDTH  = 1200;
const int HEIGHT = 700;

// Shared variables (kept static for the still image)
float cloud_offset = 0.0f;
float bob_time = 0.0f;
float plane_x = 200.0f;
float plane_y = 95.0f;

// ..........................................................................
// .......................... LOW-LEVEL HELPERS .............................
// ..........................................................................

void fillRect(float x, float y, float w, float h, float r, float g, float b) {
    glColor3f(r, g, b);
    glBegin(GL_QUADS);
        glVertex2f(x,     y);
        glVertex2f(x + w, y);
        glVertex2f(x + w, y + h);
        glVertex2f(x,     y + h);
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

// ..........................................................................
// .......................... ENVIRONMENT LOGIC .............................
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

void draw_sun(float cx, float cy) {
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

void draw_airplane(float ox, float oy) {
    glPushMatrix();
    glTranslatef(ox, oy, 0);

    float sc = 1.55f;
    glScalef(sc, sc, 1.0f);

    // Landing gear shadow
    glColor4f(0.0f, 0.0f, 0.0f, 0.18f);
    glBegin(GL_POLYGON);
        glVertex2f(12, -22); glVertex2f(105, -22); glVertex2f(112, -18);
        glVertex2f(95, -15); glVertex2f(25, -15); glVertex2f(8, -18);
    glEnd();

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

    // Landing gear struts and wheels
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

// ..........................................................................
// .......................... SCENE 1 ENVIRONMENT ...........................
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
            float bright = ((row+col)%4==0) ? 0.7f : 0.3f; // Simplified static brightness
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
        glColor3f(0.70f,0.80f,0.95f); glVertex2f(tw,  th); glVertex2f(0,   th);
    glEnd();
    glBegin(GL_QUADS);
        glColor3f(0.45f,0.55f,0.65f); glVertex2f(tw+gap, 0);   glVertex2f(tw*2+gap, 0);
        glColor3f(0.65f,0.75f,0.90f); glVertex2f(tw*2+gap, th);glVertex2f(tw+gap,   th);
    glEnd();

    for (int row = 0; row < 28; ++row) {
        for (int col = 0; col < 5; ++col) {
            float wx = 8.0f + col * 15.0f;
            float wy = 10.0f + row * 12.5f;
            float bright = ((row+col)%3==0) ? 0.7f : 0.2f;
            fillRect(wx, wy, 10, 9, bright*0.1f, bright*0.8f, bright*1.0f);
        }
    }
    for (int row = 0; row < 28; ++row) {
        for (int col = 0; col < 5; ++col) {
            float wx = tw+gap + 8.0f + col*15.0f;
            float wy = 10.0f + row*12.5f;
            float bright = ((row+col+1)%3==0) ? 0.7f : 0.2f;
            fillRect(wx, wy, 10, 9, bright*0.1f, bright*0.8f, bright*1.0f);
        }
    }

    fillRect(-3,   th,   tw+6,  14, 0.45f,0.48f,0.52f);
    fillRect(tw+gap-3, th, tw+6,14,  0.45f,0.48f,0.52f);
    fillRect(0,   th+14, tw,  6,  0.25f,0.27f,0.30f);
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

    // Static red light
    fillCircle(0, 303, 2.5f, 1.0f, 0.1f, 0.1f);

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
// .......................... MASTER RENDER  ................................
// ..........................................................................

void display(void) {
    glClear(GL_COLOR_BUFFER_BIT);

    // Scene 1: Airport Background and Environment
    draw_sky();
    draw_sun(250, 580);
    draw_clouds();
    draw_scene1_environment();

    // Scene 1: The Airplane
    glPushMatrix();
        float pivot_x = plane_x + 105.0f;
        float pivot_y = plane_y;
        glTranslatef(pivot_x, pivot_y, 0.0f);
        glRotatef(0.0f, 0.0f, 0.0f, 1.0f); // Static Angle
        glTranslatef(-pivot_x, -pivot_y, 0.0f);
        draw_airplane(plane_x, plane_y);
    glPopMatrix();

    glutSwapBuffers();
}

void resize(int width, int height) {
    glViewport(0, 0, width, height);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluOrtho2D(0.0, (float)WIDTH, 0.0, (float)HEIGHT);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
}

int main(int argc, char *argv[]) {
    glutInit(&argc, argv);
    glutInitWindowSize(WIDTH, HEIGHT);
    glutInitWindowPosition(100, 50);
    glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE);

    glutCreateWindow("Scene 1: Airport Takeoff (Still Image)");

    glutReshapeFunc(resize);
    glutDisplayFunc(display);

    glClearColor(0.48f, 0.74f, 0.93f, 1.0f);
    glEnable(GL_LINE_SMOOTH);
    glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);
    glEnable(GL_POINT_SMOOTH);

    glutMainLoop();
    return EXIT_SUCCESS;
}
