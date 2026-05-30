#include "game_internal.hpp"

namespace samsat {

void setColor(float r, float g, float b, float a) {
    glColor4f(r, g, b, a);
}

void begin2DOverlay() {
    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();
    gluOrtho2D(0, windowWidth, 0, windowHeight);

    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glLoadIdentity();

    glDisable(GL_DEPTH_TEST);
    glDisable(GL_LIGHTING);
}

void end2DOverlay() {
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_LIGHTING);

    glMatrixMode(GL_MODELVIEW);
    glPopMatrix();

    glMatrixMode(GL_PROJECTION);
    glPopMatrix();

    glMatrixMode(GL_MODELVIEW);
}

void drawRect(float x, float y, float w, float h) {
    glBegin(GL_QUADS);
    glVertex2f(x, y);
    glVertex2f(x + w, y);
    glVertex2f(x + w, y + h);
    glVertex2f(x, y + h);
    glEnd();
}

void drawLine(float x1, float y1, float x2, float y2) {
    glBegin(GL_LINES);
    glVertex2f(x1, y1);
    glVertex2f(x2, y2);
    glEnd();
}

float getTextWidth(const std::string& text, void* font) {
    float width = 0.0f;
    for (char c : text) {
        width += static_cast<float>(glutBitmapWidth(font, c));
    }
    return width;
}

void drawText(float x, float y, const std::string& text, void* font = GLUT_BITMAP_HELVETICA_18) {
    glRasterPos2f(x, y);
    for (char c : text) {
        glutBitmapCharacter(font, c);
    }
}

void drawCenteredText(float y, const std::string& text, void* font = GLUT_BITMAP_HELVETICA_18) {
    drawText((static_cast<float>(windowWidth) - getTextWidth(text, font)) * 0.5f, y, text, font);
}

int drawWrappedText(float x,
                    float y,
                    const std::string& text,
                    float maxWidth,
                    float lineHeight,
                    void* font = GLUT_BITMAP_HELVETICA_18,
                    float r = 1.0f,
                    float g = 1.0f,
                    float b = 1.0f) {
    std::istringstream words(text);
    std::string word;
    std::string line;
    int lineCount = 0;

    setColor(r, g, b);
    while (words >> word) {
        const std::string candidate = line.empty() ? word : line + " " + word;
        if (!line.empty() && getTextWidth(candidate, font) > maxWidth) {
            drawText(x, y - (static_cast<float>(lineCount) * lineHeight), line, font);
            ++lineCount;
            line = word;
        } else {
            line = candidate;
        }
    }

    if (!line.empty()) {
        drawText(x, y - (static_cast<float>(lineCount) * lineHeight), line, font);
        ++lineCount;
    }

    return lineCount;
}

void drawPanel(float x, float y, float w, float h) {
    setColor(0.0f, 0.0f, 0.0f);
    drawRect(x + 5.0f, y - 5.0f, w, h);

    setColor(0.06f, 0.07f, 0.11f);
    drawRect(x, y, w, h);

    setColor(0.17f, 0.20f, 0.30f);
    drawRect(x, y + h - 5.0f, w, 5.0f);

    setColor(1.0f, 0.78f, 0.25f);
    drawLine(x, y + h, x + w, y + h);
    setColor(0.34f, 0.38f, 0.52f);
    drawLine(x, y, x + w, y);
    drawLine(x + w, y, x + w, y + h);
    drawLine(x, y, x, y + h);
}

void drawUiBox(float x, float y, float w, float h) {
    setColor(0.00f, 0.00f, 0.00f);
    drawRect(x + 3.0f, y - 3.0f, w, h);

    setColor(0.05f, 0.06f, 0.10f);
    drawRect(x, y, w, h);

    setColor(0.12f, 0.15f, 0.23f);
    drawRect(x, y + h - 4.0f, w, 4.0f);

    setColor(0.36f, 0.42f, 0.56f);
    drawLine(x, y, x + w, y);
    drawLine(x + w, y, x + w, y + h);
    drawLine(x, y, x, y + h);
    setColor(1.0f, 0.78f, 0.25f);
    drawLine(x, y + h, x + w, y + h);
}

void drawCube(float x, float y, float z, float sx, float sy, float sz, float r, float g, float b) {
    glPushMatrix();
    setColor(r, g, b);
    glTranslatef(x, y, z);
    glScalef(sx, sy, sz);
    glutSolidCube(1.0f);
    glPopMatrix();
}

void drawSphere(float x, float y, float z, float radius, float r, float g, float b) {
    glPushMatrix();
    setColor(r, g, b);
    glTranslatef(x, y, z);
    glutSolidSphere(radius, 18, 12);
    glPopMatrix();
}

void drawCone(float x,
              float y,
              float z,
              float radius,
              float height,
              float r,
              float g,
              float b,
              float rotateX = 0.0f,
              float rotateY = 0.0f,
              float rotateZ = 0.0f) {
    glPushMatrix();
    setColor(r, g, b);
    glTranslatef(x, y, z);
    glRotatef(rotateX, 1.0f, 0.0f, 0.0f);
    glRotatef(rotateY, 0.0f, 1.0f, 0.0f);
    glRotatef(rotateZ, 0.0f, 0.0f, 1.0f);
    glutSolidCone(radius, height, 18, 8);
    glPopMatrix();
}

void drawCylinder(float x,
                  float y,
                  float z,
                  float radius,
                  float depth,
                  float r,
                  float g,
                  float b,
                  float rotateX = 0.0f,
                  float rotateY = 0.0f,
                  float rotateZ = 0.0f) {
    glPushMatrix();
    setColor(r, g, b);
    glTranslatef(x, y, z);
    glRotatef(rotateX, 1.0f, 0.0f, 0.0f);
    glRotatef(rotateY, 0.0f, 1.0f, 0.0f);
    glRotatef(rotateZ, 0.0f, 0.0f, 1.0f);
    glTranslatef(0.0f, 0.0f, -depth * 0.5f);

    GLUquadric* quadric = gluNewQuadric();
    gluQuadricNormals(quadric, GLU_SMOOTH);
    gluCylinder(quadric, radius, radius, depth, 20, 4);
    gluDisk(quadric, 0.0f, radius, 20, 1);
    glTranslatef(0.0f, 0.0f, depth);
    gluDisk(quadric, 0.0f, radius, 20, 1);
    gluDeleteQuadric(quadric);
    glPopMatrix();
}

void drawHouseRoof(float x, float baseY, float z, float width, float height, float depth, float r, float g, float b) {
    const float halfW = width * 0.5f;
    const float halfD = depth * 0.5f;

    glPushMatrix();
    glTranslatef(x, baseY, z);
    setColor(r, g, b);
    glBegin(GL_TRIANGLES);
    glNormal3f(0.0f, 0.0f, 1.0f);
    glVertex3f(-halfW, 0.0f, halfD);
    glVertex3f(halfW, 0.0f, halfD);
    glVertex3f(0.0f, height, halfD);

    glNormal3f(0.0f, 0.0f, -1.0f);
    glVertex3f(halfW, 0.0f, -halfD);
    glVertex3f(-halfW, 0.0f, -halfD);
    glVertex3f(0.0f, height, -halfD);
    glEnd();

    glBegin(GL_QUADS);
    glNormal3f(-0.55f, 0.75f, 0.0f);
    glVertex3f(-halfW, 0.0f, -halfD);
    glVertex3f(-halfW, 0.0f, halfD);
    glVertex3f(0.0f, height, halfD);
    glVertex3f(0.0f, height, -halfD);

    glNormal3f(0.55f, 0.75f, 0.0f);
    glVertex3f(0.0f, height, -halfD);
    glVertex3f(0.0f, height, halfD);
    glVertex3f(halfW, 0.0f, halfD);
    glVertex3f(halfW, 0.0f, -halfD);

    glNormal3f(0.0f, -1.0f, 0.0f);
    glVertex3f(-halfW, 0.0f, -halfD);
    glVertex3f(halfW, 0.0f, -halfD);
    glVertex3f(halfW, 0.0f, halfD);
    glVertex3f(-halfW, 0.0f, halfD);
    glEnd();
    glPopMatrix();
}

void drawTexturedGround(float size, GLuint textureId, float repeatFactor = 8.0f) {
    if (textureId != 0) {
        glEnable(GL_TEXTURE_2D);
        glBindTexture(GL_TEXTURE_2D, textureId);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glColor3f(1.0f, 1.0f, 1.0f);
    } else {
        glDisable(GL_TEXTURE_2D);
        setColor(0.25f, 0.45f, 0.25f);
    }

    glBegin(GL_QUADS);
    glNormal3f(0.0f, 1.0f, 0.0f);
    if (textureId) glTexCoord2f(0.0f, 0.0f);
    glVertex3f(-size, 0.0f, size);
    if (textureId) glTexCoord2f(repeatFactor, 0.0f);
    glVertex3f(size, 0.0f, size);
    if (textureId) glTexCoord2f(repeatFactor, repeatFactor);
    glVertex3f(size, 0.0f, -size);
    if (textureId) glTexCoord2f(0.0f, repeatFactor);
    glVertex3f(-size, 0.0f, -size);
    glEnd();

    if (textureId) {
        glDisable(GL_TEXTURE_2D);
    }
}

void drawTexturedBox(float x, float y, float z, float sx, float sy, float sz, GLuint textureId, float r, float g, float b, float uRepeat = 1.0f, float vRepeat = 1.0f) {
    glPushMatrix();
    glTranslatef(x, y, z);
    glScalef(sx, sy, sz);

    if (textureId != 0) {
        glEnable(GL_TEXTURE_2D);
        glBindTexture(GL_TEXTURE_2D, textureId);
        glColor3f(1.0f, 1.0f, 1.0f);
    } else {
        glDisable(GL_TEXTURE_2D);
        setColor(r, g, b);
    }

    glBegin(GL_QUADS);
    // Front face
    glNormal3f(0.0f, 0.0f, 1.0f);
    if (textureId) glTexCoord2f(0.0f, 0.0f);
    glVertex3f(-0.5f, -0.5f, 0.5f);
    if (textureId) glTexCoord2f(uRepeat, 0.0f);
    glVertex3f(0.5f, -0.5f, 0.5f);
    if (textureId) glTexCoord2f(uRepeat, vRepeat);
    glVertex3f(0.5f, 0.5f, 0.5f);
    if (textureId) glTexCoord2f(0.0f, vRepeat);
    glVertex3f(-0.5f, 0.5f, 0.5f);

    // Back face
    glNormal3f(0.0f, 0.0f, -1.0f);
    if (textureId) glTexCoord2f(0.0f, 0.0f);
    glVertex3f(0.5f, -0.5f, -0.5f);
    if (textureId) glTexCoord2f(uRepeat, 0.0f);
    glVertex3f(-0.5f, -0.5f, -0.5f);
    if (textureId) glTexCoord2f(uRepeat, vRepeat);
    glVertex3f(-0.5f, 0.5f, -0.5f);
    if (textureId) glTexCoord2f(0.0f, vRepeat);
    glVertex3f(0.5f, 0.5f, -0.5f);

    // Left face
    glNormal3f(-1.0f, 0.0f, 0.0f);
    if (textureId) glTexCoord2f(0.0f, 0.0f);
    glVertex3f(-0.5f, -0.5f, -0.5f);
    if (textureId) glTexCoord2f(uRepeat, 0.0f);
    glVertex3f(-0.5f, -0.5f, 0.5f);
    if (textureId) glTexCoord2f(uRepeat, vRepeat);
    glVertex3f(-0.5f, 0.5f, 0.5f);
    if (textureId) glTexCoord2f(0.0f, vRepeat);
    glVertex3f(-0.5f, 0.5f, -0.5f);

    // Right face
    glNormal3f(1.0f, 0.0f, 0.0f);
    if (textureId) glTexCoord2f(0.0f, 0.0f);
    glVertex3f(0.5f, -0.5f, 0.5f);
    if (textureId) glTexCoord2f(uRepeat, 0.0f);
    glVertex3f(0.5f, -0.5f, -0.5f);
    if (textureId) glTexCoord2f(uRepeat, vRepeat);
    glVertex3f(0.5f, 0.5f, -0.5f);
    if (textureId) glTexCoord2f(0.0f, vRepeat);
    glVertex3f(0.5f, 0.5f, 0.5f);

    // Top face
    glNormal3f(0.0f, 1.0f, 0.0f);
    if (textureId) glTexCoord2f(0.0f, 0.0f);
    glVertex3f(-0.5f, 0.5f, 0.5f);
    if (textureId) glTexCoord2f(uRepeat, 0.0f);
    glVertex3f(0.5f, 0.5f, 0.5f);
    if (textureId) glTexCoord2f(uRepeat, vRepeat);
    glVertex3f(0.5f, 0.5f, -0.5f);
    if (textureId) glTexCoord2f(0.0f, vRepeat);
    glVertex3f(-0.5f, 0.5f, -0.5f);

    // Bottom face
    glNormal3f(0.0f, -1.0f, 0.0f);
    if (textureId) glTexCoord2f(0.0f, 0.0f);
    glVertex3f(-0.5f, -0.5f, -0.5f);
    if (textureId) glTexCoord2f(uRepeat, 0.0f);
    glVertex3f(0.5f, -0.5f, -0.5f);
    if (textureId) glTexCoord2f(uRepeat, vRepeat);
    glVertex3f(0.5f, -0.5f, 0.5f);
    if (textureId) glTexCoord2f(0.0f, vRepeat);
    glVertex3f(-0.5f, -0.5f, 0.5f);
    glEnd();

    if (textureId != 0) {
        glDisable(GL_TEXTURE_2D);
    }
    glPopMatrix();
}

namespace {

void drawCharacterModel(float bodyR,
                        float bodyG,
                        float bodyB,
                        float pantsR,
                        float pantsG,
                        float pantsB,
                        float accentR,
                        float accentG,
                        float accentB,
                        bool hasCap,
                        bool hasSuspiciousCoat,
                        bool roundHead,
                        bool animateLegs,
                        float walkPhase) {
    const float skinR = 1.0f;
    const float skinG = 0.80f;
    const float skinB = 0.60f;
    const float legSwing = animateLegs ? std::sin(walkPhase) * 25.0f : 0.0f;
    const float armSwing = animateLegs ? -std::sin(walkPhase) * 16.0f : 0.0f;

    drawCube(0.0f, 1.15f, 0.0f, 0.56f, 0.88f, 0.32f, bodyR, bodyG, bodyB);
    drawSphere(-0.36f, 1.48f, 0.0f, 0.13f, bodyR * 0.92f, bodyG * 0.92f, bodyB * 0.92f);
    drawSphere(0.36f, 1.48f, 0.0f, 0.13f, bodyR * 0.92f, bodyG * 0.92f, bodyB * 0.92f);
    drawCube(0.0f, 1.62f, 0.0f, 0.28f, 0.18f, 0.24f, skinR * 0.92f, skinG * 0.92f, skinB * 0.92f);
    if (roundHead) {
        drawSphere(0.0f, 1.84f, 0.0f, 0.28f, skinR, skinG, skinB);
    } else {
        drawCube(0.0f, 1.84f, 0.0f, 0.52f, 0.52f, 0.48f, skinR, skinG, skinB);
    }
    if (!hasCap) {
        drawCube(0.0f, 2.05f, 0.02f, 0.48f, 0.10f, 0.34f, 0.10f, 0.06f, 0.03f);
        drawCube(0.0f, 1.98f, -0.20f, 0.38f, 0.12f, 0.10f, 0.10f, 0.06f, 0.03f);
        drawCube(-0.24f, 1.91f, -0.04f, 0.08f, 0.18f, 0.24f, 0.10f, 0.06f, 0.03f);
        drawCube(0.24f, 1.91f, -0.04f, 0.08f, 0.18f, 0.24f, 0.10f, 0.06f, 0.03f);
    }
    drawSphere(-0.30f, 1.84f, 0.0f, 0.055f, skinR * 0.92f, skinG * 0.92f, skinB * 0.92f);
    drawSphere(0.30f, 1.84f, 0.0f, 0.055f, skinR * 0.92f, skinG * 0.92f, skinB * 0.92f);
    drawSphere(-0.10f, 1.86f, -0.27f, 0.025f, 0.08f, 0.05f, 0.03f);
    drawSphere(0.10f, 1.86f, -0.27f, 0.025f, 0.08f, 0.05f, 0.03f);
    drawSphere(0.0f, 1.80f, -0.29f, 0.026f, skinR * 0.90f, skinG * 0.68f, skinB * 0.48f);

    glPushMatrix();
    glTranslatef(-0.43f, 1.48f, 0.0f);
    glRotatef(armSwing, 1.0f, 0.0f, 0.0f);
    drawCylinder(0.0f, -0.34f, 0.0f, 0.09f, 0.68f, bodyR * 0.9f, bodyG * 0.9f, bodyB * 0.9f, -90.0f);
    drawSphere(0.0f, -0.75f, 0.0f, 0.095f, skinR, skinG, skinB);
    glPopMatrix();

    glPushMatrix();
    glTranslatef(0.43f, 1.48f, 0.0f);
    glRotatef(-armSwing, 1.0f, 0.0f, 0.0f);
    drawCylinder(0.0f, -0.34f, 0.0f, 0.09f, 0.68f, bodyR * 0.9f, bodyG * 0.9f, bodyB * 0.9f, -90.0f);
    drawSphere(0.0f, -0.75f, 0.0f, 0.095f, skinR, skinG, skinB);
    glPopMatrix();

    glPushMatrix();
    glTranslatef(-0.16f, 0.72f, 0.0f);
    glRotatef(legSwing, 1.0f, 0.0f, 0.0f);
    drawCylinder(0.0f, -0.34f, 0.0f, 0.10f, 0.70f, pantsR, pantsG, pantsB, -90.0f);
    drawCube(0.0f, -0.72f, -0.06f, 0.30f, 0.14f, 0.42f, 0.05f, 0.05f, 0.06f);
    glPopMatrix();

    glPushMatrix();
    glTranslatef(0.16f, 0.72f, 0.0f);
    glRotatef(-legSwing, 1.0f, 0.0f, 0.0f);
    drawCylinder(0.0f, -0.34f, 0.0f, 0.10f, 0.70f, pantsR, pantsG, pantsB, -90.0f);
    drawCube(0.0f, -0.72f, -0.06f, 0.30f, 0.14f, 0.42f, 0.05f, 0.05f, 0.06f);
    glPopMatrix();

    drawCube(0.0f, 1.35f, -0.21f, 0.40f, 0.08f, 0.06f, accentR, accentG, accentB);
    drawCube(0.0f, 0.80f, -0.19f, 0.62f, 0.08f, 0.06f, 0.05f, 0.05f, 0.06f);

    if (hasCap) {
        drawCube(0.0f, 2.08f, -0.02f, 0.50f, 0.12f, 0.42f, accentR, accentG, accentB);
        drawCube(0.0f, 2.02f, -0.28f, 0.42f, 0.06f, 0.18f, accentR * 0.75f, accentG * 0.75f, accentB * 0.75f);
    }

    if (hasSuspiciousCoat) {
        drawCube(0.0f, 1.03f, -0.24f, 0.66f, 1.16f, 0.07f, 0.10f, 0.03f, 0.04f);
        drawCube(0.0f, 2.12f, 0.0f, 0.62f, 0.10f, 0.50f, 0.04f, 0.04f, 0.04f);
    }
}

void drawWorldText(float x, float y, float z, const std::string& text, void* font = GLUT_BITMAP_HELVETICA_18) {
    glDisable(GL_LIGHTING);
    glRasterPos3f(x, y, z);
    for (char c : text) {
        glutBitmapCharacter(font, c);
    }
    glEnable(GL_LIGHTING);
}

bool shouldShowWorldLabel(float x, float z, const std::string& text) {
    if (text == "KANTOR SAMSAT") {
        return true;
    }

    return isNear(player.x, player.z, x, z, 7.5f);
}

bool isIndoorScene(GameState scene) {
    switch (scene) {
        case INFORMATION_ROOM:
        case DIALOG_INFORMATION_OFFICER:
        case INVENTORY_CHECK:
        case FORM_COUNTER:
        case VERIFICATION_COUNTER:
        case PAYMENT_QUEUE:
        case PAYMENT_COUNTER:
        case VALIDATION_COUNTER:
        case VALIDATION_SUCCESS:
        case PHOTOCOPY_SHOP:
        case STAMP_QUEST:
        case FINAL_COUNTER_BOSS:
            return true;
        default:
            return false;
    }
}

float getIndoorFrontLimit(GameState scene) {
    switch (scene) {
        case INFORMATION_ROOM:
        case DIALOG_INFORMATION_OFFICER:
        case INVENTORY_CHECK:
            return 8.0f;
        case PAYMENT_QUEUE:
        case FINAL_COUNTER_BOSS:
            return 8.0f;
        default:
            return 7.0f;
    }
}

void draw3DLabel(float x,
                 float y,
                 float z,
                 const std::string& text,
                 float boardR = 0.06f,
                 float boardG = 0.07f,
                 float boardB = 0.10f,
                 float textR = 1.0f,
                 float textG = 1.0f,
                 float textB = 1.0f,
                 float width = 0.0f) {
    if (!shouldShowWorldLabel(x, z, text)) {
        return;
    }

    const float boardWidth = (width > 0.0f) ? width : std::max(2.0f, static_cast<float>(text.size()) * 0.16f);
    glDisable(GL_DEPTH_TEST);
    drawCube(x, y, z - 0.01f, boardWidth + 0.10f, 0.62f, 0.10f, 0.0f, 0.0f, 0.0f);
    drawCube(x, y, z, boardWidth, 0.52f, 0.08f, boardR, boardG, boardB);
    setColor(textR, textG, textB);
    drawWorldText(x - boardWidth * 0.42f, y - 0.08f, z + 0.08f, text, GLUT_BITMAP_HELVETICA_18);
    glEnable(GL_DEPTH_TEST);
}

void drawChair(float x, float z) {
    drawCube(x, 0.35f, z, 0.70f, 0.18f, 0.65f, 0.18f, 0.24f, 0.34f);
    drawCube(x, 0.82f, z + 0.28f, 0.70f, 0.75f, 0.14f, 0.14f, 0.18f, 0.26f);
    drawCube(x - 0.24f, 0.18f, z - 0.20f, 0.08f, 0.36f, 0.08f, 0.08f, 0.08f, 0.09f);
    drawCube(x + 0.24f, 0.18f, z - 0.20f, 0.08f, 0.36f, 0.08f, 0.08f, 0.08f, 0.09f);
}

void drawTrafficCone(float x, float z) {
    drawCube(x, 0.12f, z, 0.48f, 0.12f, 0.48f, 0.08f, 0.08f, 0.08f);
    drawCone(x, 0.16f, z, 0.26f, 0.70f, 0.95f, 0.36f, 0.08f, -90.0f);
    drawCylinder(x, 0.42f, z, 0.18f, 0.05f, 1.0f, 0.78f, 0.20f, -90.0f);
}

void drawFlagPole(float x, float z) {
    drawCube(x, 1.8f, z, 0.08f, 3.6f, 0.08f, 0.72f, 0.72f, 0.76f);
    drawCube(x + 0.45f, 3.0f, z, 0.90f, 0.28f, 0.04f, 0.85f, 0.05f, 0.05f);
    drawCube(x + 0.45f, 2.72f, z, 0.90f, 0.28f, 0.04f, 0.95f, 0.95f, 0.90f);
}

void drawGroundEllipse(float x, float z, float radiusX, float radiusZ, float y, float r, float g, float b, float a) {
    const bool wasTextureEnabled = glIsEnabled(GL_TEXTURE_2D);
    const bool wasLightingEnabled = glIsEnabled(GL_LIGHTING);
    const bool wasBlendEnabled = glIsEnabled(GL_BLEND);
    const bool wasCullEnabled = glIsEnabled(GL_CULL_FACE);
    GLboolean wasDepthMaskEnabled = GL_TRUE;
    glGetBooleanv(GL_DEPTH_WRITEMASK, &wasDepthMaskEnabled);

    glDisable(GL_TEXTURE_2D);
    glDisable(GL_LIGHTING);
    glDisable(GL_CULL_FACE);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glDepthMask(GL_FALSE);
    glColor4f(r, g, b, a);

    glBegin(GL_TRIANGLE_FAN);
    glVertex3f(x, y, z);
    for (int i = 0; i <= 40; ++i) {
        const float angle = static_cast<float>(i) * 2.0f * kPi / 40.0f;
        glVertex3f(x + std::cos(angle) * radiusX, y, z + std::sin(angle) * radiusZ);
    }
    glEnd();

    glDepthMask(wasDepthMaskEnabled);
    if (!wasBlendEnabled) {
        glDisable(GL_BLEND);
    }
    if (wasCullEnabled) {
        glEnable(GL_CULL_FACE);
    }
    if (wasLightingEnabled) {
        glEnable(GL_LIGHTING);
    }
    if (wasTextureEnabled) {
        glEnable(GL_TEXTURE_2D);
    }
}

void drawGroundDisk(float x, float z, float radius, float r, float g, float b) {
    drawGroundEllipse(x, z, radius, radius, 0.085f, r, g, b, 0.34f);
    drawGroundEllipse(x, z, radius * 0.86f, radius * 0.86f, 0.095f,
                      std::min(1.0f, r + 0.10f),
                      std::min(1.0f, g + 0.10f),
                      std::min(1.0f, b + 0.10f),
                      0.26f);
}

void drawCharacterShadowBlob(float x, float z, float radiusX, float radiusZ, float alpha = 0.44f) {
    drawGroundEllipse(x, z, radiusX, radiusZ, 0.075f, 0.0f, 0.0f, 0.0f, alpha);
}

void drawFloatingArrow(float x, float z, float r, float g, float b) {
    const float bob = std::sin(interactionPulse) * 0.15f;
    drawSphere(x, 2.62f + bob, z, 0.23f, r, g, b);
    drawCone(x, 2.42f + bob, z, 0.28f, 0.55f, r * 0.92f, g * 0.92f, b * 0.92f, 90.0f);
    drawCylinder(x, 2.38f + bob, z, 0.30f, 0.05f, r * 0.75f, g * 0.75f, b * 0.75f, -90.0f);
}

void drawPortalMarker(float x, float z, const std::string& label, float r, float g, float b, bool finalPortal = false) {
    (void)label;
    (void)finalPortal;
    const float pulse = 0.08f + (std::sin(interactionPulse) + 1.0f) * 0.04f;
    drawGroundDisk(x, z, 0.78f + pulse, r * 0.55f, g * 0.55f, b * 0.55f);
    drawGroundDisk(x, z, 0.42f + pulse * 0.30f, r, g, b);
    drawFloatingArrow(x, z, r, g, b);
}

void drawInteractionMarker(float x, float z, const std::string& label, bool completed = false) {
    (void)label;
    const bool nearby = isNear(player.x, player.z, x, z, 2.4f);
    const float pulse = nearby ? 0.35f : 0.12f;
    const float r = completed ? 0.20f : (nearby ? 1.0f : 0.75f);
    const float g = completed ? 0.78f : (nearby ? 0.86f : 0.70f);
    const float b = completed ? 0.25f : (nearby ? 0.18f : 0.22f);

    drawGroundDisk(x, z, nearby ? 0.70f : 0.48f, r, g, b);
    if (nearby) {
        drawSphere(x, 1.32f + std::sin(interactionPulse) * pulse, z, 0.08f, 1.0f, 0.95f, 0.45f);
    }
}

void drawObjectiveMarker(float x, float z, const std::string& label, bool finalMarker = false) {
    (void)label;
    const float r = finalMarker ? 0.95f : 0.20f;
    const float g = finalMarker ? 0.12f : 0.55f;
    const float b = finalMarker ? 0.12f : 1.00f;
    drawGroundDisk(x, z, finalMarker ? 0.92f : 0.76f, r * 0.55f, g * 0.55f, b * 0.55f);
    drawFloatingArrow(x, z, r, g, b);
}

void drawCompletionBadge(float x, float z, const std::string& label = "SELESAI") {
    (void)label;
    drawGroundDisk(x, z, 0.42f, 0.10f, 0.72f, 0.22f);
}

}  // namespace

void drawGround(float size, float r = 0.25f, float g = 0.45f, float b = 0.25f) {
    (void)r;
    (void)g;
    (void)b;
    drawTexturedGround(size, tileTextureId, 10.0f);
}

void drawPlayer3D(float x, float z) {
    glPushMatrix();
    glTranslatef(x, 0.0f, z);
    glRotatef(player.facingYaw, 0.0f, 1.0f, 0.0f);

    drawCharacterModel(0.10f, 0.20f, 0.90f,
                       0.08f, 0.10f, 0.22f,
                       1.00f, 0.86f, 0.20f,
                       false, false, true, player.isMoving, player.walkPhase);

    glPopMatrix();
}

void drawNPC3D(float x, float z, float r, float g, float b) {
    drawCharacterShadowBlob(x, z, 0.48f, 0.28f);

    glPushMatrix();
    glTranslatef(x, 0.0f, z);

    const bool securityUniform = g > r && g > b;
    const bool suspicious = r > 0.30f && g < 0.12f && b < 0.12f;
    const bool vendor = r > 0.75f && g > 0.35f && b < 0.20f;
    const bool formal = b > r && b > g;

    float pantsR = 0.10f;
    float pantsG = 0.11f;
    float pantsB = 0.13f;
    if (vendor) {
        pantsR = 0.25f;
        pantsG = 0.16f;
        pantsB = 0.08f;
    } else if (formal) {
        pantsR = 0.08f;
        pantsG = 0.10f;
        pantsB = 0.20f;
    }

    drawCharacterModel(r, g, b,
                       pantsR, pantsG, pantsB,
                       securityUniform ? 0.08f : 0.95f,
                       securityUniform ? 0.28f : 0.78f,
                       securityUniform ? 0.12f : 0.24f,
                       securityUniform, suspicious, true, false, 0.0f);

    glPopMatrix();
}

void drawSimpleCar(float x, float z) {
    drawGroundEllipse(x, z, 2.75f, 1.18f, 0.075f, 0.0f, 0.0f, 0.0f, 0.36f);
    drawCube(x, 0.70f, z, 4.80f, 0.90f, 1.85f, 0.18f, 0.24f, 0.70f);
    drawCube(x - 0.15f, 1.35f, z - 0.05f, 2.45f, 0.85f, 1.35f, 0.12f, 0.16f, 0.38f);

    drawCube(x - 0.15f, 1.55f, z - 0.78f, 2.00f, 0.42f, 0.06f, 0.05f, 0.10f, 0.16f);
    drawCube(x - 0.15f, 1.55f, z + 0.78f, 2.00f, 0.42f, 0.06f, 0.05f, 0.10f, 0.16f);
    drawCube(x - 1.35f, 1.45f, z, 0.08f, 0.48f, 1.00f, 0.04f, 0.09f, 0.15f);
    drawCube(x + 1.05f, 1.45f, z, 0.08f, 0.48f, 1.00f, 0.04f, 0.09f, 0.15f);

    drawCube(x - 2.48f, 0.64f, z, 0.18f, 0.45f, 1.70f, 0.10f, 0.12f, 0.18f);
    drawCube(x + 2.48f, 0.64f, z, 0.18f, 0.45f, 1.70f, 0.10f, 0.12f, 0.18f);
    drawSphere(x - 2.42f, 0.82f, z - 0.56f, 0.12f, 1.0f, 0.92f, 0.42f);
    drawSphere(x - 2.42f, 0.82f, z + 0.56f, 0.12f, 1.0f, 0.92f, 0.42f);
    drawSphere(x + 2.42f, 0.82f, z - 0.56f, 0.10f, 0.85f, 0.08f, 0.06f);
    drawSphere(x + 2.42f, 0.82f, z + 0.56f, 0.10f, 0.85f, 0.08f, 0.06f);

    const float wheelX[] = {x - 1.65f, x + 1.65f};
    const float wheelZ[] = {z - 0.98f, z + 0.98f};
    for (float wx : wheelX) {
        for (float wz : wheelZ) {
            drawCylinder(wx, 0.43f, wz, 0.42f, 0.28f, 0.03f, 0.03f, 0.04f);
            drawCylinder(wx, 0.43f, wz, 0.22f, 0.31f, 0.68f, 0.68f, 0.70f);
        }
    }

    drawCube(x - 0.80f, 1.10f, z - 1.02f, 0.12f, 0.12f, 0.12f, 0.05f, 0.05f, 0.06f);
    drawCube(x - 0.80f, 1.10f, z + 1.02f, 0.12f, 0.12f, 0.12f, 0.05f, 0.05f, 0.06f);
}

void drawCeilingLamp(float x, float y, float z, float width) {
    drawCube(x, y, z, width, 0.08f, 0.22f, 0.86f, 0.84f, 0.76f);
    drawCube(x, y - 0.07f, z, width * 0.86f, 0.04f, 0.16f, 1.0f, 0.92f, 0.58f);
    drawGroundEllipse(x, z, width * 0.34f, 0.48f, 0.085f, 1.0f, 0.82f, 0.34f, 0.12f);
}

void drawRoomFrame(float halfWidth, float halfDepth, float wallHeight, float r, float g, float b) {
    const float wallThickness = 0.30f;
    const float doorWidth = 2.8f;
    const float doorHeight = std::min(2.65f, wallHeight - 0.45f);
    const float ceilingThickness = 0.18f;
    const float sideFrontWidth = std::max(0.1f, halfWidth - doorWidth * 0.5f);

    drawTexturedBox(0.0f, 0.03f, 0.0f, halfWidth * 2.0f, 0.06f, halfDepth * 2.0f, tileTextureId, 0.46f, 0.46f, 0.48f, halfWidth, halfDepth);
    const float backWallRepeat = std::max(1.0f, halfWidth * 0.30f);
    const float sideWallRepeat = std::max(1.0f, halfDepth * 0.30f);
    drawTexturedBox(0.0f, wallHeight * 0.5f, -halfDepth, halfWidth * 2.0f, wallHeight, wallThickness, wallTextureId, r, g, b, backWallRepeat, 1.0f);
    drawTexturedBox(-halfWidth, wallHeight * 0.5f, 0.0f, wallThickness, wallHeight, halfDepth * 2.0f, wallTextureId, r * 0.94f, g * 0.94f, b * 0.94f, sideWallRepeat, 1.0f);
    drawTexturedBox(halfWidth, wallHeight * 0.5f, 0.0f, wallThickness, wallHeight, halfDepth * 2.0f, wallTextureId, r * 0.94f, g * 0.94f, b * 0.94f, sideWallRepeat, 1.0f);

    drawTexturedBox(-(doorWidth * 0.5f + sideFrontWidth * 0.5f), wallHeight * 0.5f, halfDepth, sideFrontWidth, wallHeight, wallThickness, wallTextureId, r, g, b, std::max(1.0f, sideFrontWidth * 0.30f), 1.0f);
    drawTexturedBox(doorWidth * 0.5f + sideFrontWidth * 0.5f, wallHeight * 0.5f, halfDepth, sideFrontWidth, wallHeight, wallThickness, wallTextureId, r, g, b, std::max(1.0f, sideFrontWidth * 0.30f), 1.0f);
    drawTexturedBox(0.0f, doorHeight + ((wallHeight - doorHeight) * 0.5f), halfDepth, doorWidth, wallHeight - doorHeight, wallThickness, wallTextureId, r * 0.92f, g * 0.92f, b * 0.92f, 1.0f, 0.5f);

    drawCube(-doorWidth * 0.5f, doorHeight * 0.5f, halfDepth - 0.03f, 0.12f, doorHeight, 0.18f, 0.20f, 0.18f, 0.14f);
    drawCube(doorWidth * 0.5f, doorHeight * 0.5f, halfDepth - 0.03f, 0.12f, doorHeight, 0.18f, 0.20f, 0.18f, 0.14f);
    drawCube(0.0f, doorHeight, halfDepth - 0.03f, doorWidth + 0.18f, 0.14f, 0.18f, 0.20f, 0.18f, 0.14f);

    drawTexturedBox(0.0f, wallHeight + ceilingThickness * 0.5f, 0.0f, halfWidth * 2.05f, ceilingThickness, halfDepth * 2.05f, wallTextureId, r * 0.82f, g * 0.82f, b * 0.82f, halfWidth * 0.25f, halfDepth * 0.25f);
    drawCylinder(0.0f, wallHeight - 0.08f, 0.0f, 0.42f, 0.08f, 0.95f, 0.90f, 0.62f, -90.0f);
    drawSphere(0.0f, wallHeight - 0.18f, 0.0f, 0.18f, 1.0f, 0.94f, 0.60f);
    drawCeilingLamp(-halfWidth * 0.42f, wallHeight - 0.08f, -halfDepth * 0.35f, 2.0f);
    drawCeilingLamp(halfWidth * 0.42f, wallHeight - 0.08f, -halfDepth * 0.35f, 2.0f);
    drawCeilingLamp(0.0f, wallHeight - 0.08f, halfDepth * 0.32f, 2.4f);
}

void drawQueueMarker(float x, float z) {
    drawCube(x, 0.085f, z - 0.44f, 0.80f, 0.018f, 0.08f, 0.95f, 0.90f, 0.25f);
    drawCube(x, 0.085f, z + 0.44f, 0.80f, 0.018f, 0.08f, 0.95f, 0.90f, 0.25f);
}

void drawPhotocopyMachine(float x, float z) {
    drawCube(x, 0.72f, z, 2.10f, 1.44f, 1.48f, 0.88f, 0.88f, 0.90f);
    drawCube(x - 0.12f, 1.52f, z - 0.10f, 1.72f, 0.16f, 1.04f, 0.12f, 0.13f, 0.15f);
    drawCube(x + 0.74f, 1.36f, z + 0.46f, 0.36f, 0.12f, 0.30f, 0.05f, 0.30f, 0.12f);
    drawCube(x + 0.60f, 0.96f, z - 0.82f, 0.96f, 0.08f, 0.22f, 0.82f, 0.82f, 0.84f);
    drawCube(x - 0.50f, 0.36f, z - 0.80f, 0.78f, 0.08f, 0.24f, 0.78f, 0.78f, 0.80f);
}

void drawCounterClerk(float x, float z, float r, float g, float b) {
    drawCube(x, 1.46f, z, 0.54f, 0.46f, 0.30f, r, g, b);
    drawCube(x, 1.68f, z, 0.24f, 0.14f, 0.20f, 0.92f, 0.72f, 0.54f);
    drawSphere(x, 1.88f, z, 0.25f, 1.0f, 0.80f, 0.60f);
    drawCube(x, 2.07f, z + 0.03f, 0.44f, 0.10f, 0.30f, 0.10f, 0.06f, 0.03f);
    drawCube(x, 1.96f, z + 0.21f, 0.36f, 0.16f, 0.08f, 0.10f, 0.06f, 0.03f);
    drawSphere(x - 0.27f, 1.88f, z, 0.05f, 0.92f, 0.72f, 0.54f);
    drawSphere(x + 0.27f, 1.88f, z, 0.05f, 0.92f, 0.72f, 0.54f);
}

void setupLighting() {
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
    glEnable(GL_LIGHT1);
    glEnable(GL_LIGHT2);
    glEnable(GL_LIGHT3);
    glEnable(GL_COLOR_MATERIAL);
    glEnable(GL_NORMALIZE);

    glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);

    const GLfloat globalAmbient[] = {0.18f, 0.19f, 0.23f, 1.0f};
    glLightModelfv(GL_LIGHT_MODEL_AMBIENT, globalAmbient);

    const GLfloat keyDiffuse[] = {1.00f, 0.88f, 0.62f, 1.0f};
    const GLfloat keySpecular[] = {0.55f, 0.50f, 0.42f, 1.0f};
    glLightfv(GL_LIGHT0, GL_DIFFUSE, keyDiffuse);
    glLightfv(GL_LIGHT0, GL_SPECULAR, keySpecular);

    const GLfloat fillDiffuse[] = {0.25f, 0.38f, 0.70f, 1.0f};
    glLightfv(GL_LIGHT1, GL_DIFFUSE, fillDiffuse);

    const GLfloat rimDiffuse[] = {0.65f, 0.78f, 1.00f, 1.0f};
    glLightfv(GL_LIGHT2, GL_DIFFUSE, rimDiffuse);

    const GLfloat roomDiffuse[] = {0.92f, 0.84f, 0.62f, 1.0f};
    const GLfloat roomSpecular[] = {0.20f, 0.18f, 0.12f, 1.0f};
    glLightfv(GL_LIGHT3, GL_DIFFUSE, roomDiffuse);
    glLightfv(GL_LIGHT3, GL_SPECULAR, roomSpecular);
    glLightf(GL_LIGHT3, GL_CONSTANT_ATTENUATION, 1.0f);
    glLightf(GL_LIGHT3, GL_LINEAR_ATTENUATION, 0.035f);

    const GLfloat materialSpecular[] = {0.28f, 0.28f, 0.30f, 1.0f};
    glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, materialSpecular);
    glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, 28.0f);
}

void updateSceneLights() {
    const GLfloat keyPosition[] = {-7.0f, 12.0f, 7.0f, 1.0f};
    const GLfloat fillPosition[] = {8.0f, 5.0f, -6.0f, 1.0f};
    const GLfloat rimPosition[] = {player.x - 3.0f, 4.5f, player.z + 4.0f, 1.0f};
    const GLfloat roomPosition[] = {0.0f, isIndoorScene(currentState) ? 3.8f : 8.0f, isIndoorScene(currentState) ? -1.0f : 1.5f, 1.0f};
    glLightfv(GL_LIGHT0, GL_POSITION, keyPosition);
    glLightfv(GL_LIGHT1, GL_POSITION, fillPosition);
    glLightfv(GL_LIGHT2, GL_POSITION, rimPosition);
    glLightfv(GL_LIGHT3, GL_POSITION, roomPosition);
}

void drawPlayerShadow3D(float x, float z) {
    const float squash = player.isMoving ? 0.08f * std::sin(player.walkPhase) : 0.0f;
    drawCharacterShadowBlob(x, z, 0.58f + squash, 0.36f, 0.48f);

    const float yaw = player.facingYaw * kPi / 180.0f;
    const float rightX = std::cos(yaw) * 0.16f;
    const float rightZ = -std::sin(yaw) * 0.16f;
    const float forwardX = std::sin(yaw) * 0.10f;
    const float forwardZ = std::cos(yaw) * 0.10f;
    const float step = player.isMoving ? std::sin(player.walkPhase) * 0.07f : 0.0f;
    drawGroundEllipse(x - rightX + forwardX + step, z - rightZ + forwardZ, 0.18f, 0.10f, 0.078f, 0.0f, 0.0f, 0.0f, 0.36f);
    drawGroundEllipse(x + rightX + forwardX - step, z + rightZ + forwardZ, 0.18f, 0.10f, 0.078f, 0.0f, 0.0f, 0.0f, 0.36f);
}

void setupCamera() {
    float targetX = player.x;
    float targetY = camera.targetHeight;
    float targetZ = player.z;
    float baseDistance = camera.distance;
    float baseHeight = camera.height;
    float effectivePitch = camera.pitch;

    if (isIndoorScene(currentState)) {
        targetY = 1.45f;
        baseDistance = clampFloat(camera.distance, 4.6f, 5.8f);
        baseHeight = 8.0f;
        effectivePitch = clampFloat(camera.pitch, -10.0f, -5.0f);
    }

    switch (currentState) {
        case SAMSAT_EXTERIOR:
        case TUTORIAL_CONTROL:
        case DIALOG_SECURITY_GUARD:
        case QUEUE_MACHINE:
        case MAP_VENDOR:
            targetY = 1.6f;
            break;

        case VEHICLE_CHECK_AREA:
            targetY = 1.3f;
            baseDistance = camera.distance + 2.0f;
            baseHeight = camera.height + 1.0f;
            break;

        case FINAL_COUNTER_BOSS:
            targetY = 1.45f;
            break;

        default:
            break;
    }

    const float yawRadians = camera.yaw * kPi / 180.0f;
    const float pitchRadians = effectivePitch * kPi / 180.0f;
    const float horizontalDistance = std::cos(pitchRadians) * baseDistance;
    const float verticalDistance = -std::sin(pitchRadians) * baseDistance;


    float camX = targetX + std::sin(yawRadians) * horizontalDistance;
    float camY = targetY + verticalDistance + (baseHeight - 8.0f);
    float camZ = targetZ + std::cos(yawRadians) * horizontalDistance;

    if (camY < 0.8f) {
        camY = 0.8f;
    }

    if (isIndoorScene(currentState)) {
        const float frontLimit = getIndoorFrontLimit(currentState);
        camX = clampFloat(camX, -7.35f, 7.35f);
        camZ = clampFloat(camZ, -7.35f, frontLimit - 0.55f);
        camY = clampFloat(camY, 1.75f, 3.05f);
    }

    gluLookAt(camX, camY, camZ, targetX, targetY, targetZ, 0.0f, 1.0f, 0.0f);
}

void drawSamsatOfficeFacade() {
    drawCube(0.0f, 0.18f, -7.08f, 23.2f, 0.36f, 0.72f, 0.48f, 0.48f, 0.50f);
    drawCube(0.0f, 2.70f, -10.4f, 22.0f, 5.40f, 6.0f, 0.62f, 0.63f, 0.66f);
    drawCube(0.0f, 5.46f, -10.4f, 23.0f, 0.36f, 6.35f, 0.42f, 0.10f, 0.08f);
    drawHouseRoof(0.0f, 5.60f, -10.4f, 24.4f, 1.15f, 7.2f, 0.46f, 0.10f, 0.08f);

    drawCube(0.0f, 2.45f, -6.85f, 6.4f, 4.4f, 0.42f, 0.74f, 0.74f, 0.70f);
    drawCube(0.0f, 4.75f, -6.72f, 7.3f, 0.48f, 0.58f, 0.25f, 0.27f, 0.30f);
    drawCube(0.0f, 1.35f, -6.60f, 2.30f, 2.70f, 0.20f, 0.08f, 0.09f, 0.12f);
    drawCube(-0.62f, 1.35f, -6.47f, 1.00f, 2.32f, 0.08f, 0.12f, 0.24f, 0.36f);
    drawCube(0.62f, 1.35f, -6.47f, 1.00f, 2.32f, 0.08f, 0.12f, 0.24f, 0.36f);
    drawCube(0.0f, 2.78f, -6.47f, 2.55f, 0.18f, 0.10f, 0.18f, 0.18f, 0.18f);
    drawCube(0.0f, 0.22f, -5.25f, 5.4f, 0.32f, 1.55f, 0.72f, 0.72f, 0.68f);
    drawCube(0.0f, 0.42f, -5.85f, 4.4f, 0.18f, 0.48f, 0.58f, 0.58f, 0.56f);

    for (float x : {-9.0f, -6.0f, -3.2f, 3.2f, 6.0f, 9.0f}) {
        drawCube(x, 3.10f, -6.90f, 1.55f, 1.10f, 0.16f, 0.15f, 0.32f, 0.45f);
        drawCube(x, 3.72f, -6.82f, 1.80f, 0.13f, 0.20f, 0.86f, 0.86f, 0.82f);
        drawCube(x, 2.48f, -6.82f, 1.80f, 0.13f, 0.20f, 0.86f, 0.86f, 0.82f);
        drawCube(x - 0.84f, 3.10f, -6.82f, 0.12f, 1.24f, 0.20f, 0.86f, 0.86f, 0.82f);
        drawCube(x + 0.84f, 3.10f, -6.82f, 0.12f, 1.24f, 0.20f, 0.86f, 0.86f, 0.82f);
    }

    for (float x : {-2.55f, 2.55f}) {
        drawCylinder(x, 2.05f, -6.42f, 0.16f, 3.75f, 0.82f, 0.82f, 0.76f, -90.0f);
        drawCylinder(x, 0.18f, -6.42f, 0.28f, 0.22f, 0.60f, 0.58f, 0.54f, -90.0f);
    }

    drawCube(0.0f, 5.20f, -6.45f, 6.0f, 0.72f, 0.18f, 0.02f, 0.08f, 0.16f);
    draw3DLabel(0.0f, 5.25f, -6.36f, "KANTOR SAMSAT", 0.02f, 0.08f, 0.16f, 1.0f, 0.92f, 0.24f, 5.4f);
    draw3DLabel(0.0f, 3.20f, -6.36f, "RUANG INFORMASI", 0.10f, 0.22f, 0.42f, 1.0f, 1.0f, 1.0f, 3.9f);
}

void drawExteriorServiceBooth(float x, float z, float width, float depth, float r, float g, float b) {
    drawGroundEllipse(x, z, width * 0.58f, depth * 0.54f, 0.075f, 0.0f, 0.0f, 0.0f, 0.22f);
    drawCube(x, 0.13f, z, width + 0.28f, 0.26f, depth + 0.28f, 0.38f, 0.38f, 0.38f);
    drawCube(x, 1.32f, z, width, 2.35f, depth, r, g, b);
    drawHouseRoof(x, 2.52f, z, width + 0.72f, 0.56f, depth + 0.70f, 0.35f, 0.16f, 0.08f);
    drawCube(x, 1.25f, z + depth * 0.50f + 0.04f, width * 0.76f, 0.58f, 0.12f, 0.16f, 0.11f, 0.08f);
    drawCube(x - width * 0.22f, 1.70f, z + depth * 0.51f + 0.08f, width * 0.24f, 0.38f, 0.09f, 0.06f, 0.12f, 0.18f);
    drawCube(x + width * 0.22f, 1.70f, z + depth * 0.51f + 0.08f, width * 0.24f, 0.38f, 0.09f, 0.06f, 0.12f, 0.18f);
    drawCylinder(x - width * 0.44f, 1.34f, z + depth * 0.51f, 0.07f, 2.25f, 0.78f, 0.76f, 0.70f, -90.0f);
    drawCylinder(x + width * 0.44f, 1.34f, z + depth * 0.51f, 0.07f, 2.25f, 0.78f, 0.76f, 0.70f, -90.0f);
}

void drawQueueMachine3D(float x, float z) {
    drawCube(x, 0.18f, z, 1.15f, 0.28f, 0.82f, 0.08f, 0.08f, 0.10f);
    drawCube(x, 1.05f, z, 0.88f, 1.70f, 0.62f, 0.18f, 0.20f, 0.24f);
    drawCube(x, 1.42f, z - 0.34f, 0.58f, 0.38f, 0.08f, 0.02f, 0.08f, 0.12f);
    drawCube(x, 0.98f, z - 0.35f, 0.56f, 0.10f, 0.08f, 0.88f, 0.88f, 0.78f);
    drawSphere(x + 0.28f, 0.62f, z - 0.35f, 0.08f, 0.90f, 0.20f, 0.12f);
}

void drawSamsatExterior3D() {
    drawCityBoundary(28.0f, 16.0f);
    drawTexturedGround(30.0f, rockTextureId, 6.0f);

    drawSamsatOfficeFacade();
    drawExteriorServiceBooth(-7.0f, -1.0f, 3.2f, 2.8f, 0.36f, 0.28f, 0.20f);
    drawExteriorServiceBooth(7.0f, -1.0f, 3.5f, 2.8f, 0.68f, 0.38f, 0.16f);
    drawExteriorServiceBooth(10.0f, -2.0f, 3.2f, 2.8f, 0.74f, 0.74f, 0.76f);
    drawQueueMachine3D(0.0f, 1.0f);
    draw3DLabel(0.0f, 2.75f, 0.42f, "MESIN ANTREAN", 0.05f, 0.08f, 0.12f, 1.0f, 0.9f, 0.2f, 3.1f);
    draw3DLabel(10.0f, 2.7f, -0.75f, "FOTOKOPI", 0.12f, 0.12f, 0.12f, 1.0f, 0.9f, 0.2f, 2.8f);
    draw3DLabel(-9.8f, 2.2f, -7.4f, "CEK FISIK", 0.07f, 0.18f, 0.10f, 1.0f, 1.0f, 1.0f, 2.7f);
    draw3DLabel(7.0f, 2.2f, -7.4f, "VERIFIKASI", 0.10f, 0.11f, 0.28f, 1.0f, 1.0f, 1.0f, 2.8f);
    draw3DLabel(-9.8f, 2.2f, 8.8f, "PEMBAYARAN", 0.35f, 0.20f, 0.05f, 1.0f, 0.95f, 0.25f, 3.0f);

    drawCube(-10.0f, 0.05f, 5.0f, 5.0f, 0.02f, 10.0f, 0.32f, 0.32f, 0.35f);
    drawCube(10.0f, 0.05f, 5.0f, 5.0f, 0.02f, 10.0f, 0.32f, 0.32f, 0.35f);
    drawSimpleCar(-9.0f, 5.0f);
    drawSimpleCar(9.0f, 5.0f);
    drawFlagPole(-13.5f, -7.5f);
    for (int i = 0; i < 5; ++i) {
        drawCube(-12.0f + static_cast<float>(i) * 1.1f, 0.45f, -2.8f, 0.10f, 0.90f, 0.10f, 0.85f, 0.85f, 0.78f);
        drawCube(-11.45f + static_cast<float>(i) * 1.1f, 0.82f, -2.8f, 1.0f, 0.10f, 0.10f, 0.85f, 0.85f, 0.78f);
    }
    drawTrafficCone(-4.0f, -4.2f);
    drawTrafficCone(4.0f, -4.2f);
    drawTrafficCone(-8.2f, -6.8f);
    drawTrafficCone(8.2f, -6.8f);

    drawPortalMarker(0.0f, -4.8f, "TEKAN E: MASUK", 0.15f, 0.42f, 1.0f);
    drawInteractionMarker(-7.0f, 1.0f, "E", metSecurityGuard);
    drawInteractionMarker(0.0f, 1.0f, "E", hasQueueNumber);
    drawInteractionMarker(7.0f, 1.0f, "E", hasCorrectMap);
    drawInteractionMarker(10.0f, -2.0f, "E", hasValidPhotocopy);
    if (hasQueueNumber) {
        drawCube(0.0f, 2.08f, 0.58f, 0.72f, 0.06f, 0.34f, 0.95f, 0.95f, 0.82f);
        drawCompletionBadge(0.0f, 1.0f, "TIKET OK");
    }
    if (hasCorrectMap) {
        drawCompletionBadge(7.0f, 1.0f, "MAP OK");
    }
    if (hasValidPhotocopy) {
        drawCompletionBadge(10.0f, -2.0f, "FOTO OK");
    }
    if (hasFilledForm) {
        drawPortalMarker(-9.8f, -8.3f, "CEK FISIK", 0.15f, 0.85f, 0.32f);
    }
    if (hasPhysicalCheckProof) {
        drawPortalMarker(7.0f, -8.3f, "VERIFIKASI", 0.15f, 0.42f, 1.0f);
    }
    if (hasVerificationStamp) {
        drawPortalMarker(-9.8f, 8.4f, "PEMBAYARAN", 0.15f, 0.85f, 0.32f);
    }

    if (!hasQueueNumber) {
        drawObjectiveMarker(0.0f, 1.0f, "TUJUAN: NOMOR");
    } else if (!hasCorrectMap) {
        drawObjectiveMarker(7.0f, 1.0f, "TUJUAN: MAP");
    } else if (!hasValidPhotocopy) {
        drawObjectiveMarker(10.0f, -2.0f, "TUJUAN: FOTOKOPI");
    } else if (!hasFilledForm) {
        drawObjectiveMarker(0.0f, -4.8f, "TUJUAN: INFORMASI");
    } else if (!hasPhysicalCheckProof) {
        drawObjectiveMarker(-9.8f, -8.3f, "TUJUAN: CEK FISIK");
    } else if (!hasVerificationStamp) {
        drawObjectiveMarker(7.0f, -8.3f, "TUJUAN: VERIFIKASI");
    } else if (!hasPaymentProof) {
        drawObjectiveMarker(-9.8f, 8.4f, "TUJUAN: BAYAR");
    }

    drawNPC3D(-7.0f, 1.0f, 0.10f, 0.60f, 0.20f);
    drawNPC3D(7.0f, 1.0f, 0.90f, 0.45f, 0.10f);
    drawPlayerShadow3D(player.x, player.z);
    drawPlayer3D(player.x, player.z);
}

void drawInformationRoom3D() {
    drawGround(16.0f, 0.55f, 0.55f, 0.58f);
    drawRoomFrame(9.0f, 8.0f, 4.5f, 0.72f, 0.72f, 0.76f);
    drawCube(0.0f, 0.08f, 6.2f, 3.0f, 0.04f, 0.8f, 0.18f, 0.48f, 0.90f);
    drawCube(5.0f, 0.08f, -3.0f, 1.8f, 0.04f, 1.8f, 0.20f, 0.36f, 0.78f);

    drawCube(0.0f, 0.8f, -3.0f, 5.0f, 1.0f, 1.4f, 0.38f, 0.28f, 0.18f);
    drawCube(0.0f, 1.5f, -4.6f, 6.0f, 2.0f, 0.3f, 0.35f, 0.40f, 0.48f);
    draw3DLabel(0.0f, 2.85f, -4.35f, "RUANG INFORMASI", 0.08f, 0.18f, 0.36f, 1.0f, 1.0f, 1.0f, 4.2f);
    draw3DLabel(5.0f, 2.25f, -2.25f, "LOKET FORMULIR", 0.10f, 0.18f, 0.34f, 1.0f, 1.0f, 1.0f, 3.6f);
    drawCube(-4.0f, 0.45f, 1.2f, 1.2f, 0.9f, 1.2f, 0.45f, 0.45f, 0.50f);
    drawCube(0.0f, 0.45f, 1.2f, 1.2f, 0.9f, 1.2f, 0.45f, 0.45f, 0.50f);
    drawCube(4.0f, 0.45f, 1.2f, 1.2f, 0.9f, 1.2f, 0.45f, 0.45f, 0.50f);
    drawChair(-4.0f, 1.2f);
    drawChair(0.0f, 1.2f);
    drawChair(4.0f, 1.2f);
    drawCube(5.0f, 1.0f, -3.0f, 1.4f, 2.0f, 1.4f, 0.36f, 0.32f, 0.28f);
    drawPortalMarker(0.0f, 6.2f, "KELUAR", 0.15f, 0.85f, 0.32f);
    drawPortalMarker(5.0f, -3.0f, "LOKET FORM", 0.15f, 0.42f, 1.0f);
    drawInteractionMarker(0.0f, -4.0f, "E", metInformationOfficer);
    if (metInformationOfficer) {
        drawCompletionBadge(0.0f, -4.0f, "INFO OK");
    }
    if (!hasFilledForm) {
        drawObjectiveMarker(5.0f, -3.0f, "TUJUAN: FORM");
    }

    drawNPC3D(0.0f, -4.0f, 0.25f, 0.45f, 0.92f);
    drawNPC3D(-4.0f, 2.5f, 0.70f, 0.70f, 0.20f);
    drawNPC3D(4.0f, 2.5f, 0.60f, 0.40f, 0.75f);
    drawPlayerShadow3D(player.x, player.z);
    drawPlayer3D(player.x, player.z);
}

void drawGenericCounterScene3D(float accentR, float accentG, float accentB, bool showPlayerCharacter = true) {
    drawGround(16.0f, 0.44f, 0.44f, 0.46f);
    drawRoomFrame(8.0f, 7.0f, 4.5f, 0.68f, 0.68f, 0.72f);
    drawCube(0.0f, 0.85f, -3.0f, 6.0f, 1.1f, 1.4f, 0.46f, 0.34f, 0.24f);
    drawCube(0.0f, 1.55f, -4.7f, 5.2f, 1.6f, 0.3f, accentR, accentG, accentB);
    draw3DLabel(0.0f, 2.65f, -4.45f, "VALIDASI SELESAI", 0.06f, 0.18f, 0.08f, 1.0f, 1.0f, 1.0f, 4.0f);
    drawPortalMarker(0.0f, 6.1f, "LOKET FINAL", 0.15f, 0.85f, 0.32f);
    drawObjectiveMarker(0.0f, 6.1f, "TUJUAN: FINAL");
    drawCounterClerk(0.0f, -3.95f, 0.18f, 0.36f, 0.80f);
    if (showPlayerCharacter) {
        drawPlayerShadow3D(player.x, player.z);
        drawPlayer3D(player.x, player.z);
    }
}

void drawFormCounter3D() {
    drawGround(16.0f, 0.52f, 0.52f, 0.56f);
    drawRoomFrame(8.0f, 7.0f, 4.5f, 0.70f, 0.70f, 0.74f);
    drawCube(0.0f, 0.08f, 6.1f, 3.0f, 0.04f, 0.8f, 0.20f, 0.36f, 0.78f);
    drawCube(0.0f, 0.85f, -3.0f, 6.4f, 1.1f, 1.4f, 0.44f, 0.32f, 0.22f);
    drawCube(0.0f, 1.70f, -4.7f, 5.4f, 1.4f, 0.3f, 0.20f, 0.36f, 0.78f);
    draw3DLabel(0.0f, 2.75f, -4.45f, "LOKET FORMULIR", 0.08f, 0.16f, 0.34f, 1.0f, 1.0f, 1.0f, 4.0f);
    drawPortalMarker(0.0f, 6.1f, hasFilledForm ? "LANJUT" : "KELUAR", 0.15f, 0.85f, 0.32f);
    drawInteractionMarker(0.0f, -4.0f, "E", hasFilledForm);
    if (hasFilledForm) {
        drawCompletionBadge(0.0f, -4.0f, "FORM OK");
    }
    if (!hasFilledForm) {
        drawObjectiveMarker(0.0f, -4.0f, "TUJUAN: FORM");
    }
    drawCounterClerk(0.0f, -3.95f, 0.18f, 0.36f, 0.82f);
    drawPlayerShadow3D(player.x, player.z);
    drawPlayer3D(player.x, player.z);
}

void drawVerificationCounter3D() {
    drawGround(16.0f, 0.42f, 0.44f, 0.48f);
    drawRoomFrame(8.0f, 7.0f, 4.5f, 0.64f, 0.66f, 0.70f);
    drawCube(0.0f, 0.08f, 6.1f, 3.0f, 0.04f, 0.8f, 0.58f, 0.58f, 0.86f);
    drawCube(0.0f, 0.90f, -3.0f, 6.2f, 1.2f, 1.4f, 0.42f, 0.34f, 0.24f);
    drawCube(0.0f, 1.70f, -4.7f, 5.4f, 1.5f, 0.3f, 0.32f, 0.42f, 0.52f);
    draw3DLabel(0.0f, 2.8f, -4.45f, "VERIFIKASI BERKAS", 0.10f, 0.12f, 0.22f, 1.0f, 1.0f, 1.0f, 4.5f);
    drawPortalMarker(0.0f, 6.1f, hasVerificationStamp ? "LANJUT" : "KELUAR", 0.15f, 0.85f, 0.32f);
    drawInteractionMarker(0.0f, -4.0f, "E", hasVerificationStamp);
    if (hasVerificationStamp) {
        drawCompletionBadge(0.0f, -4.0f, "CAP OK");
    }
    if (!hasVerificationStamp) {
        drawObjectiveMarker(0.0f, -4.0f, "TUJUAN: VERIFIKASI");
    }
    drawCounterClerk(0.0f, -3.95f, 0.16f, 0.34f, 0.74f);
    drawPlayerShadow3D(player.x, player.z);
    drawPlayer3D(player.x, player.z);
}

void drawPaymentCounter3D() {
    drawGround(16.0f, 0.50f, 0.48f, 0.44f);
    drawRoomFrame(8.0f, 7.0f, 4.5f, 0.68f, 0.66f, 0.62f);
    drawCube(0.0f, 0.08f, 6.1f, 3.0f, 0.04f, 0.8f, 0.86f, 0.58f, 0.28f);
    drawCube(0.0f, 0.90f, -3.0f, 6.4f, 1.2f, 1.4f, 0.48f, 0.36f, 0.22f);
    drawCube(0.0f, 1.72f, -4.7f, 5.4f, 1.5f, 0.3f, 0.58f, 0.38f, 0.16f);
    draw3DLabel(0.0f, 2.8f, -4.45f, "LOKET PEMBAYARAN", 0.28f, 0.16f, 0.04f, 1.0f, 0.95f, 0.25f, 4.4f);
    drawPortalMarker(0.0f, 6.1f, hasPaymentProof ? "VALIDASI" : "KEMBALI", 0.15f, 0.85f, 0.32f);
    drawInteractionMarker(0.0f, -4.0f, "E", hasPaymentProof);
    if (hasPaymentProof) {
        drawCompletionBadge(0.0f, -4.0f, "BAYAR OK");
    }
    if (!hasPaymentProof) {
        drawObjectiveMarker(0.0f, -4.0f, "TUJUAN: BAYAR");
    }
    drawCounterClerk(0.0f, -3.95f, 0.16f, 0.28f, 0.68f);
    drawPlayerShadow3D(player.x, player.z);
    drawPlayer3D(player.x, player.z);
}

void drawValidationCounter3D() {
    drawGround(16.0f, 0.44f, 0.40f, 0.40f);
    drawRoomFrame(8.0f, 7.0f, 4.5f, 0.66f, 0.62f, 0.62f);
    drawCube(0.0f, 0.08f, 6.1f, 3.0f, 0.04f, 0.8f, 0.80f, 0.18f, 0.18f);
    drawCube(0.0f, 0.90f, -3.0f, 6.4f, 1.2f, 1.4f, 0.42f, 0.32f, 0.22f);
    drawCube(0.0f, 1.72f, -4.7f, 5.4f, 1.5f, 0.3f, 0.62f, 0.10f, 0.10f);
    draw3DLabel(0.0f, 2.8f, -4.45f, "VALIDASI", 0.32f, 0.05f, 0.05f, 1.0f, 1.0f, 1.0f, 3.2f);
    drawPortalMarker(0.0f, 6.1f, receivedStampRequirement ? "METERAI" : "KELUAR", 0.15f, 0.85f, 0.32f);
    drawInteractionMarker(0.0f, -4.0f, "E", receivedStampRequirement);
    if (receivedStampRequirement) {
        drawCompletionBadge(0.0f, -4.0f, "VALID OK");
    }
    if (!receivedStampRequirement) {
        drawObjectiveMarker(0.0f, -4.0f, "TUJUAN: VALIDASI");
    }
    drawCounterClerk(0.0f, -3.95f, 0.14f, 0.26f, 0.62f);
    drawPlayerShadow3D(player.x, player.z);
    drawPlayer3D(player.x, player.z);
}

void drawPhotocopyShop3D() {
    drawGround(16.0f, 0.78f, 0.72f, 0.58f);
    drawRoomFrame(8.0f, 7.0f, 4.0f, 0.72f, 0.72f, 0.74f);
    drawCube(0.0f, 0.08f, 6.1f, 3.0f, 0.04f, 0.8f, 0.90f, 0.82f, 0.28f);
    drawPhotocopyMachine(-2.5f, -2.0f);
    drawCube(2.8f, 0.8f, -2.4f, 3.0f, 1.0f, 1.4f, 0.45f, 0.33f, 0.24f);
    drawCube(0.8f, 1.1f, -3.8f, 4.5f, 1.6f, 0.3f, 0.55f, 0.55f, 0.58f);
    draw3DLabel(0.8f, 2.35f, -3.55f, "FOTOKOPI", 0.15f, 0.15f, 0.12f, 1.0f, 0.9f, 0.2f, 3.0f);
    drawPortalMarker(0.0f, 6.1f, "KELUAR", 0.15f, 0.85f, 0.32f);
    drawInteractionMarker(4.8f, -2.0f, "E", hasValidPhotocopy);
    if (hasValidPhotocopy) {
        drawCompletionBadge(4.8f, -2.0f, "FOTO OK");
    }
    if (!hasValidPhotocopy) {
        drawObjectiveMarker(4.8f, -2.0f, "TUJUAN: FOTOKOPI");
    }
    drawNPC3D(4.8f, -2.0f, 0.55f, 0.55f, 0.55f);
    drawPlayerShadow3D(player.x, player.z);
    drawPlayer3D(player.x, player.z);
}

void drawVehicleCheck3D() {
    drawCityBoundary(28.0f, 16.0f);
    drawGround(28.0f, 0.40f, 0.48f, 0.40f);
    drawCube(0.0f, 0.05f, 0.0f, 12.4f, 0.03f, 15.4f, 0.24f, 0.24f, 0.26f);
    drawCube(0.0f, 0.085f, 1.5f, 1.35f, 0.025f, 10.8f, 0.12f, 0.12f, 0.13f);
    drawCube(0.0f, 0.11f, 1.5f, 0.62f, 0.02f, 9.8f, 0.06f, 0.06f, 0.07f);
    drawCube(0.0f, 0.12f, 1.5f, 0.30f, 0.03f, 10.2f, 0.95f, 0.90f, 0.25f);
    for (float z : {-4.0f, -1.4f, 1.2f, 3.8f, 6.0f}) {
        drawCube(-3.55f, 0.12f, z, 1.35f, 0.018f, 0.10f, 0.95f, 0.92f, 0.35f);
        drawCube(3.55f, 0.12f, z, 1.35f, 0.018f, 0.10f, 0.95f, 0.92f, 0.35f);
    }
    drawCylinder(-5.7f, 0.55f, -3.9f, 0.055f, 9.0f, 0.72f, 0.72f, 0.68f);
    drawCylinder(5.7f, 0.55f, -3.9f, 0.055f, 9.0f, 0.72f, 0.72f, 0.68f);
    drawCube(-5.7f, 0.72f, 0.5f, 0.12f, 0.12f, 8.8f, 0.70f, 0.70f, 0.66f);
    drawCube(5.7f, 0.72f, 0.5f, 0.12f, 0.12f, 8.8f, 0.70f, 0.70f, 0.66f);
    draw3DLabel(0.0f, 2.8f, -5.5f, "CEK FISIK KENDARAAN", 0.08f, 0.18f, 0.10f, 1.0f, 1.0f, 1.0f, 5.0f);
    drawSimpleCar(0.0f, 1.5f);
    drawCube(5.7f, 0.12f, -2.5f, 2.8f, 0.16f, 1.7f, 0.32f, 0.32f, 0.30f);
    drawCube(5.7f, 0.85f, -2.5f, 2.25f, 1.0f, 1.2f, 0.45f, 0.34f, 0.24f);
    drawCube(5.7f, 1.58f, -2.5f, 2.8f, 0.16f, 1.85f, 0.16f, 0.34f, 0.18f);
    drawCylinder(4.52f, 0.92f, -1.72f, 0.06f, 1.50f, 0.76f, 0.76f, 0.70f, -90.0f);
    drawCylinder(6.88f, 0.92f, -1.72f, 0.06f, 1.50f, 0.76f, 0.76f, 0.70f, -90.0f);
    drawCube(5.7f, 1.16f, -1.86f, 1.72f, 0.34f, 0.08f, 0.05f, 0.10f, 0.08f);
    drawTrafficCone(-4.5f, 6.0f);
    drawTrafficCone(4.5f, 6.0f);
    drawTrafficCone(-4.5f, -4.0f);
    drawTrafficCone(4.5f, -4.0f);
    drawPortalMarker(0.0f, 6.7f, "KELUAR", 0.15f, 0.85f, 0.32f);
    if (hasPhysicalCheckProof) {
        drawPortalMarker(0.0f, -6.4f, "VERIFIKASI", 0.15f, 0.42f, 1.0f);
    }
    drawInteractionMarker(0.0f, 1.5f, "E", hasPhysicalCheckProof);
    drawInteractionMarker(5.7f, -1.5f, "E", hasPhysicalCheckProof);
    if (hasPhysicalCheckProof) {
        drawCompletionBadge(0.0f, 1.5f, "CEK OK");
    }
    if (!hasPhysicalCheckProof) {
        drawObjectiveMarker(0.0f, 1.5f, "TUJUAN: CEK FISIK");
    }
    drawNPC3D(5.7f, -1.5f, 0.15f, 0.55f, 0.25f);
    drawPlayerShadow3D(player.x, player.z);
    drawPlayer3D(player.x, player.z);
}

void drawPaymentQueue3D() {
    drawGround(18.0f, 0.48f, 0.48f, 0.50f);
    drawRoomFrame(8.0f, 8.0f, 4.5f, 0.66f, 0.66f, 0.70f);
    drawCube(0.0f, 0.08f, 6.6f, 3.0f, 0.04f, 0.8f, 0.86f, 0.58f, 0.28f);
    drawCube(0.0f, 0.08f, -6.6f, 3.0f, 0.04f, 0.8f, 0.86f, 0.58f, 0.28f);
    drawCube(0.0f, 0.85f, -4.0f, 5.5f, 1.0f, 1.2f, 0.44f, 0.34f, 0.24f);
    draw3DLabel(0.0f, 2.0f, -3.55f, "ANTREAN PEMBAYARAN", 0.20f, 0.14f, 0.05f, 1.0f, 0.95f, 0.25f, 4.8f);
    drawPortalMarker(0.0f, 6.6f, "KELUAR", 0.15f, 0.85f, 0.32f);
    if (hasQueuedPaymentLine) {
        drawPortalMarker(0.0f, -6.6f, "LOKET BAYAR", 0.15f, 0.42f, 1.0f);
    }
    drawInteractionMarker(0.0f, 2.5f, "E", hasQueuedPaymentLine);
    if (hasQueuedPaymentLine) {
        drawCompletionBadge(0.0f, 2.5f, "ANTRE OK");
    }
    if (!hasQueuedPaymentLine) {
        drawObjectiveMarker(0.0f, 2.5f, "TUJUAN: ANTRE");
    }
    drawQueueMarker(-3.0f, 2.5f);
    drawQueueMarker(-1.5f, 2.5f);
    drawQueueMarker(0.0f, 2.5f);
    drawQueueMarker(1.5f, 2.5f);
    drawQueueMarker(3.0f, 2.5f);
    for (int i = 0; i < 5; ++i) {
        drawNPC3D(-3.0f + (static_cast<float>(i) * 1.5f), 2.5f, 0.65f, 0.62f, 0.22f);
    }
    drawPlayerShadow3D(player.x, player.z);
    drawPlayer3D(player.x, player.z);
}

void drawStampQuest3D() {
    drawGround(16.0f, 0.72f, 0.66f, 0.52f);
    drawRoomFrame(8.0f, 7.0f, 4.0f, 0.70f, 0.70f, 0.72f);
    drawCube(0.0f, 0.08f, 6.1f, 3.0f, 0.04f, 0.8f, 0.90f, 0.82f, 0.28f);
    drawCube(0.0f, 0.8f, -2.8f, 4.0f, 1.0f, 1.2f, 0.48f, 0.34f, 0.22f);
    draw3DLabel(0.0f, 2.0f, -2.45f, "METERAI", 0.34f, 0.18f, 0.04f, 1.0f, 0.95f, 0.25f, 2.6f);
    drawPortalMarker(0.0f, 6.1f, hasStampedDocument ? "VALIDASI" : "KELUAR", 0.15f, 0.85f, 0.32f);
    drawInteractionMarker(3.5f, -2.0f, "E", hasStampedDocument);
    if (hasStampedDocument) {
        drawCompletionBadge(3.5f, -2.0f, "METERAI OK");
    }
    if (!hasStampedDocument) {
        drawObjectiveMarker(3.5f, -2.0f, "TUJUAN: METERAI");
    }
    drawNPC3D(3.5f, -2.0f, 0.90f, 0.45f, 0.10f);
    drawPlayerShadow3D(player.x, player.z);
    drawPlayer3D(player.x, player.z);
}

void drawFinalBoss3D() {
    drawGround(18.0f, 0.18f, 0.18f, 0.22f);
    drawRoomFrame(9.0f, 8.0f, 5.0f, 0.28f, 0.28f, 0.32f);
    drawCube(0.0f, 1.0f, -4.5f, 8.0f, 1.3f, 1.6f, 0.35f, 0.20f, 0.20f);
    drawCube(0.0f, 4.2f, -7.4f, 6.5f, 0.25f, 0.4f, 0.55f, 0.18f, 0.18f);
    draw3DLabel(0.0f, 4.75f, -7.15f, "LOKET FINAL", 0.38f, 0.05f, 0.05f, 1.0f, 1.0f, 1.0f, 4.2f);
    drawPortalMarker(0.0f, 6.4f, "KEMBALI", 0.15f, 0.85f, 0.32f);
    drawInteractionMarker(0.0f, -5.5f, "E", hasFinalSTNK);
    if (hasFinalSTNK) {
        drawCompletionBadge(0.0f, -5.5f, "STNK OK");
    }
    if (!hasFinalSTNK) {
        drawObjectiveMarker(0.0f, -5.5f, "TUJUAN: SERAHKAN", true);
    }
    drawCounterClerk(0.0f, -4.05f, 0.10f, 0.20f, 0.60f);
    drawPlayerShadow3D(player.x, player.z);
    drawPlayer3D(player.x, player.z);
}

void renderCurrent3DState() {
    switch (currentState) {
        case SAMSAT_EXTERIOR:
        case TUTORIAL_CONTROL:
        case DIALOG_SECURITY_GUARD:
        case QUEUE_MACHINE:
        case MAP_VENDOR:
            drawSamsatExterior3D();
            break;

        case INFORMATION_ROOM:
        case DIALOG_INFORMATION_OFFICER:
        case INVENTORY_CHECK:
            drawInformationRoom3D();
            break;

        case PHOTOCOPY_SHOP:
            drawPhotocopyShop3D();
            break;

        case FORM_COUNTER:
            drawFormCounter3D();
            break;

        case VEHICLE_CHECK_AREA:
            drawVehicleCheck3D();
            break;

        case VERIFICATION_COUNTER:
            drawVerificationCounter3D();
            break;

        case PAYMENT_QUEUE:
            drawPaymentQueue3D();
            break;

        case PAYMENT_COUNTER:
            drawPaymentCounter3D();
            break;

        case VALIDATION_COUNTER:
            drawValidationCounter3D();
            break;

        case STAMP_QUEST:
            drawStampQuest3D();
            break;

        case VALIDATION_SUCCESS:
            drawGenericCounterScene3D(0.18f, 0.52f, 0.20f);
            break;

        case FINAL_COUNTER_BOSS:
            drawFinalBoss3D();
            break;

        default:
            break;
    }
}

void drawResourceUI() {
    const float margin = 12.0f;
    const float gap = 8.0f;
    const float boxH = 26.0f;
    const int columns = (windowWidth >= 1180) ? 6 : 3;
    const int rows = (columns == 6) ? 1 : 2;
    const float topBarH = margin + static_cast<float>(rows) * boxH + static_cast<float>(rows - 1) * gap + margin;

    setColor(0.02f, 0.02f, 0.02f);
    drawRect(0, static_cast<float>(windowHeight) - topBarH, static_cast<float>(windowWidth), topBarH);

    std::ostringstream time;
    if (timeHour < 10) time << "0";
    time << timeHour << ":";
    if (timeMinute < 10) time << "0";
    time << timeMinute;

    std::vector<std::string> blocks;
    blocks.push_back("Sabar " + std::to_string(patience));
    blocks.push_back("Stamina " + std::to_string(stamina));
    blocks.push_back("Uang " + std::to_string(money));
    blocks.push_back("Jam " + time.str());

    const float blockW = (static_cast<float>(windowWidth) - (margin * 2.0f) - (gap * static_cast<float>(columns - 1))) /
                         static_cast<float>(columns);
    for (std::size_t i = 0; i < blocks.size(); ++i) {
        const int col = static_cast<int>(i) % columns;
        const int row = static_cast<int>(i) / columns;
        const float x = margin + static_cast<float>(col) * (blockW + gap);
        const float y = static_cast<float>(windowHeight) - margin - boxH - static_cast<float>(row) * (boxH + gap);
        drawUiBox(x, y, blockW, boxH);
        setColor(1.0f, 1.0f, 1.0f);
        drawText(x + 12.0f, y + 8.0f, blocks[i], GLUT_BITMAP_8_BY_13);
    }
}

void drawDialogueBox() {
    if (!showDialogue) {
        return;
    }

    const float margin = std::max(44.0f, static_cast<float>(windowWidth) * 0.06f);
    const float panelW = static_cast<float>(windowWidth) - (margin * 2.0f);
    const float panelH = std::max(135.0f, static_cast<float>(windowHeight) * 0.20f);
    const float x = margin;
    const float y = std::max(28.0f, static_cast<float>(windowHeight) * 0.055f);
    drawPanel(x, y, panelW, panelH);

    setColor(1.0f, 0.9f, 0.2f);
    drawText(x + 24.0f, y + panelH - 32.0f, dialogueSpeaker, GLUT_BITMAP_HELVETICA_18);
    drawWrappedText(x + 24.0f, y + panelH - 66.0f, dialogueText, panelW - 48.0f, 24.0f,
                    GLUT_BITMAP_HELVETICA_18, 1.0f, 1.0f, 1.0f);

    setColor(0.8f, 0.8f, 0.8f);
    drawText(x + 24.0f, y + 20.0f, "Tekan SPACE untuk lanjut.", GLUT_BITMAP_8_BY_13);
}

void drawInventoryOverlay() {
    if (!showInventory) {
        return;
    }

    const float panelW = std::min(static_cast<float>(windowWidth) * 0.72f, 760.0f);
    const float panelH = std::min(static_cast<float>(windowHeight) * 0.68f, 520.0f);
    const float x = (static_cast<float>(windowWidth) - panelW) * 0.5f;
    const float y = (static_cast<float>(windowHeight) - panelH) * 0.5f;
    drawPanel(x, y, panelW, panelH);
    setColor(1.0f, 0.9f, 0.2f);
    drawText(x + 30.0f, y + panelH - 35.0f, "INVENTORY DOKUMEN", GLUT_BITMAP_HELVETICA_18);

    std::vector<std::string> items;
    items.push_back("KTP Asli: YES");
    items.push_back("BPKB Asli: YES");
    items.push_back("STNK Lama: YES");
    items.push_back("Nomor Antrean: " + boolText(hasQueueNumber));
    items.push_back("Map Benar: " + boolText(hasCorrectMap));
    items.push_back("Fotokopi Valid: " + boolText(hasValidPhotocopy));
    items.push_back("Formulir: " + boolText(hasFilledForm));
    items.push_back("Bukti Cek Fisik: " + boolText(hasPhysicalCheckProof));
    items.push_back("Stempel Verifikasi: " + boolText(hasVerificationStamp));
    items.push_back("Bukti Pembayaran: " + boolText(hasPaymentProof));
    items.push_back("Dokumen Bermeterai: " + boolText(hasStampedDocument));
    items.push_back("STNK Baru: " + boolText(hasFinalSTNK));

    setColor(1.0f, 1.0f, 1.0f);
    float itemY = y + panelH - 75.0f;
    const float lineH = std::max(23.0f, panelH * 0.052f);
    for (const std::string& item : items) {
        drawWrappedText(x + 40.0f, itemY, item, panelW - 80.0f, lineH, GLUT_BITMAP_HELVETICA_18,
                        1.0f, 1.0f, 1.0f);
        itemY -= lineH;
    }

    setColor(0.8f, 0.8f, 0.8f);
    drawText(x + 40.0f, y + 24.0f, "Tekan I untuk menutup inventory.", GLUT_BITMAP_8_BY_13);
}

void drawQuestLogOverlay() {
    if (!showQuestLog) {
        return;
    }

    const float panelW = std::min(static_cast<float>(windowWidth) * 0.74f, 820.0f);
    const float panelH = std::min(static_cast<float>(windowHeight) * 0.68f, 520.0f);
    const float x = (static_cast<float>(windowWidth) - panelW) * 0.5f;
    const float y = (static_cast<float>(windowHeight) - panelH) * 0.5f;
    drawPanel(x, y, panelW, panelH);
    setColor(1.0f, 0.9f, 0.2f);
    drawText(x + 30.0f, y + panelH - 35.0f, "QUEST LOG", GLUT_BITMAP_HELVETICA_18);

    std::vector<std::string> quests;
    quests.push_back(std::string(hasQueueNumber ? "[X] " : "[ ] ") + "Ambil nomor antrean yang benar.");
    quests.push_back(std::string(hasCorrectMap ? "[X] " : "[ ] ") + "Beli map yang sesuai.");
    quests.push_back(std::string(hasValidPhotocopy ? "[X] " : "[ ] ") + "Fotokopi dokumen sampai jelas.");
    quests.push_back(std::string(hasFilledForm ? "[X] " : "[ ] ") + "Ambil dan isi formulir.");
    quests.push_back(std::string(hasPhysicalCheckProof ? "[X] " : "[ ] ") + "Lakukan cek fisik kendaraan.");
    quests.push_back(std::string(hasVerificationStamp ? "[X] " : "[ ] ") + "Verifikasi dokumen.");
    quests.push_back(std::string(hasPaymentProof ? "[X] " : "[ ] ") + "Bayar biaya administrasi.");
    quests.push_back(std::string(hasStampedDocument ? "[X] " : "[ ] ") + "Cari dan tempel meterai.");
    quests.push_back(std::string(hasFinalSTNK ? "[X] " : "[ ] ") + "Serahkan berkas di loket final.");

    setColor(1.0f, 1.0f, 1.0f);
    float questY = y + panelH - 75.0f;
    const float lineH = std::max(26.0f, panelH * 0.06f);
    for (const std::string& quest : quests) {
        drawWrappedText(x + 30.0f, questY, quest, panelW - 60.0f, lineH * 0.75f,
                        GLUT_BITMAP_HELVETICA_18, 1.0f, 1.0f, 1.0f);
        questY -= lineH;
    }

    setColor(0.8f, 0.8f, 0.8f);
    drawText(x + 30.0f, y + 24.0f, "Tekan Q untuk menutup quest log.", GLUT_BITMAP_8_BY_13);
}

void drawDebugOverlay() {
    if (!showDebug) {
        return;
    }

    drawPanel(10.0f, 60.0f, 430.0f, 320.0f);
    setColor(0.2f, 1.0f, 0.2f);
    drawText(25.0f, 350.0f, "DEBUG OVERLAY", GLUT_BITMAP_HELVETICA_18);

    std::vector<std::string> lines;
    lines.push_back("State: " + stateToString(currentState));

    {
        std::stringstream ss;
        ss << "Player: (" << player.x << ", " << player.y << ", " << player.z << ")";
        lines.push_back(ss.str());
    }

    {
        std::stringstream ss;
        ss << "Patience: " << patience << " | Stamina: " << stamina;
        lines.push_back(ss.str());
    }

    {
        std::stringstream ss;
        ss << "Money: " << money;
        lines.push_back(ss.str());
    }

    {
        std::stringstream ss;
        ss << "TimeLeft: " << minutesUntilClosing() << " mins";
        lines.push_back(ss.str());
    }

    lines.push_back("Queue: " + boolText(hasQueueNumber) + " | Map: " + boolText(hasCorrectMap));
    lines.push_back("Photo: " + boolText(hasValidPhotocopy) + " | Form: " + boolText(hasFilledForm));
    lines.push_back("CekFisik: " + boolText(hasPhysicalCheckProof));
    lines.push_back("Verify: " + boolText(hasVerificationStamp) + " | Pay: " + boolText(hasPaymentProof));
    lines.push_back("Stamp: " + boolText(hasStampedDocument) + " | STNK: " + boolText(hasFinalSTNK));
    lines.push_back("Tutorial: " + boolText(hasSeenTutorial) + " | InventoryCheck: " + boolText(hasCheckedInventoryScene));
    lines.push_back("StampReq: " + boolText(receivedStampRequirement));
    lines.push_back("AllFinalReq: " + boolText(hasAllFinalRequirements()));
    lines.push_back("ProjectedEnding: " + getProjectedEndingText());

    setColor(1.0f, 1.0f, 1.0f);
    float y = 325.0f;
    for (const std::string& line : lines) {
        drawText(25.0f, y, line, GLUT_BITMAP_8_BY_13);
        y -= 22.0f;
    }
}

std::string getSceneTitle() {
    switch (currentState) {
        case SAMSAT_EXTERIOR:
        case DIALOG_SECURITY_GUARD:
        case QUEUE_MACHINE:
        case MAP_VENDOR:
            return "Halaman Samsat";
        case TUTORIAL_CONTROL:
            return "Tutorial Kontrol";
        case INFORMATION_ROOM:
        case DIALOG_INFORMATION_OFFICER:
            return "Ruang Informasi";
        case INVENTORY_CHECK:
            return "Pemeriksaan Dokumen";
        case PHOTOCOPY_SHOP:
            return "Fotokopi";
        case FORM_COUNTER:
            return "Loket Formulir";
        case VEHICLE_CHECK_AREA:
            return "Area Cek Fisik";
        case VERIFICATION_COUNTER:
            return "Loket Verifikasi";
        case PAYMENT_QUEUE:
            return "Antrean Pembayaran";
        case PAYMENT_COUNTER:
            return "Loket Pembayaran";
        case VALIDATION_COUNTER:
            return "Loket Validasi";
        case STAMP_QUEST:
            return "Warung Meterai";
        case VALIDATION_SUCCESS:
            return "Validasi Berhasil";
        case FINAL_COUNTER_BOSS:
            return "Loket Final";
        default:
            return "";
    }
}

std::string getSceneObjective() {
    switch (currentState) {
        case SAMSAT_EXTERIOR:
            if (!hasQueueNumber) {
                return "Ambil nomor antrean di mesin tengah halaman.";
            }
            if (!hasCorrectMap) {
                return "Beli map yang benar dari penjual map.";
            }
            if (!hasValidPhotocopy) {
                return "Datangi kios fotokopi dan gandakan berkas.";
            }
            if (!hasFilledForm) {
                return "Masuk ruang informasi lalu lanjut ke loket formulir.";
            }
            if (!hasPhysicalCheckProof) {
                return "Lanjut ke area cek fisik kendaraan.";
            }
            if (!hasVerificationStamp) {
                return "Datangi loket verifikasi untuk stempel berkas.";
            }
            if (!hasPaymentProof) {
                return "Masuk antrean pembayaran lalu ke loket.";
            }
            if (!hasStampedDocument) {
                return "Cari meterai, lalu tempelkan dokumen sebelum final.";
            }
            return "Semua syarat siap. Lanjut ke loket final.";

        case INFORMATION_ROOM:
            return "Bicara dengan petugas informasi atau lanjut ke loket formulir.";

        case QUEUE_MACHINE:
            return hasQueueNumber ? "Nomor antrean sudah aman. Tutup dialog, lalu lanjut jalan." :
                                    "Tekan E untuk mengambil nomor antrean.";

        case MAP_VENDOR:
            return hasCorrectMap ? "Map benar sudah dimiliki. Tutup dialog, lalu lanjut jalan." :
                                   "Tekan E untuk membeli map yang benar.";

        case PHOTOCOPY_SHOP:
            return hasValidPhotocopy ? "Fotokopi valid selesai. Tekan E di pintu untuk keluar." :
                                       "Tekan E untuk memproses fotokopi dokumen.";

        case FORM_COUNTER:
            return hasFilledForm ? "Formulir selesai. Tekan E di pintu untuk lanjut ke cek fisik." :
                                   "Dekati loket lalu tekan E untuk mengambil dan mengisi formulir.";

        case VEHICLE_CHECK_AREA:
            return hasPhysicalCheckProof ? "Cek fisik selesai. Tekan E di pintu depan untuk lanjut verifikasi." :
                                           "Dekati kendaraan atau petugas, lalu tekan E.";

        case VERIFICATION_COUNTER:
            return hasVerificationStamp ? "Verifikasi selesai. Tekan E di pintu untuk ke antrean pembayaran." :
                                          "Susun berkas lalu tekan E di loket verifikasi.";
        case PAYMENT_QUEUE:
            return hasQueuedPaymentLine ? "Antrean sudah diproses. Jalan ke ujung antrean lalu tekan E." :
                                          "Masuk ke antrean dan tekan E untuk menunggu giliran.";
        case PAYMENT_COUNTER:
            return hasPaymentProof ? "Pembayaran selesai. Tekan E di pintu untuk menuju validasi." :
                                     "Dekati loket pembayaran lalu tekan E.";
        case VALIDATION_COUNTER:
            if (!hasPaymentProof || !hasVerificationStamp) {
                return "Lengkapi bukti bayar dan verifikasi, lalu tekan E di loket.";
            }
            return receivedStampRequirement ?
                   "Validasi meminta meterai. Tekan E di pintu untuk mencari meterai." :
                   "Bicara dengan petugas validasi dulu agar kebutuhan meterai tercatat.";
        case STAMP_QUEST:
            return hasStampedDocument ? "Dokumen sudah bermeterai. Tekan E di pintu untuk validasi akhir." :
                                        "Dekati penjual meterai lalu tekan E.";
        case FINAL_COUNTER_BOSS:
            return hasFinalSTNK ? "STNK sudah terbit. Tekan SPACE untuk menentukan ending." :
                                  (hasAllFinalRequirements() ?
                                   "Berkas final lengkap. Dekati petugas loket final lalu tekan E." :
                                   "Lengkapi berkas final dulu, lalu kembali ke loket final.");

        case VALIDATION_SUCCESS:
            return "Jalan ke pintu loket final lalu tekan E.";

        default:
            return "";
    }
}

std::string getEndingStatsLine() {
    std::ostringstream ss;
    ss << "Kesabaran " << patience
       << " | Stamina " << stamina
       << " | Uang " << money;
    return ss.str();
}

void drawStateInstructionPanel() {
    if (currentState == TUTORIAL_CONTROL) {
        const float w = std::min(static_cast<float>(windowWidth) * 0.70f, 720.0f);
        const float h = std::min(static_cast<float>(windowHeight) * 0.28f, 200.0f);
        const float x = (static_cast<float>(windowWidth) - w) * 0.5f;
        const float y = (static_cast<float>(windowHeight) - h) * 0.5f;
        drawPanel(x, y, w, h);
        setColor(1.0f, 0.9f, 0.2f);
        drawText(x + 28.0f, y + h - 36.0f, "KONTROL DASAR", GLUT_BITMAP_HELVETICA_18);
        drawWrappedText(x + 28.0f, y + h - 70.0f,
                        "WASD / Arrow Keys: jalan mengikuti kamera. Mouse kanan drag / M: lihat sekitar. E: interaksi. I: inventory. Q: quest log. G: menyerah. R: reset kamera.",
                        w - 56.0f, 24.0f, GLUT_BITMAP_HELVETICA_18, 1.0f, 1.0f, 1.0f);
        setColor(0.8f, 0.8f, 0.8f);
        drawText(x + 28.0f, y + 24.0f, "Tekan SPACE untuk mulai menjelajah area Samsat", GLUT_BITMAP_8_BY_13);
        return;
    }

    if (currentState == INVENTORY_CHECK) {
        const float w = std::min(static_cast<float>(windowWidth) * 0.72f, 760.0f);
        const float h = std::min(static_cast<float>(windowHeight) * 0.34f, 250.0f);
        const float x = (static_cast<float>(windowWidth) - w) * 0.5f;
        const float y = (static_cast<float>(windowHeight) - h) * 0.5f;
        drawPanel(x, y, w, h);
        setColor(1.0f, 0.9f, 0.2f);
        drawText(x + 28.0f, y + h - 36.0f, "CHECKLIST AWAL BERKAS", GLUT_BITMAP_HELVETICA_18);
        drawWrappedText(x + 28.0f, y + h - 72.0f,
                        "Dokumen dasar sudah aman, tapi proses belum dimulai. Cari nomor antrean, map yang benar, dan fotokopi yang valid. Setelah itu kembali ke loket formulir untuk lanjut ke cek fisik.",
                        w - 56.0f, 24.0f, GLUT_BITMAP_HELVETICA_18, 1.0f, 1.0f, 1.0f);
        setColor(0.8f, 0.8f, 0.8f);
        drawText(x + 28.0f, y + 24.0f, "Tekan SPACE untuk kembali ke halaman depan", GLUT_BITMAP_8_BY_13);
    }
}

void drawInteractionPrompt() {
    const std::string prompt = getInteractionPrompt();
    if (prompt.empty()) {
        return;
    }

    const float w = std::min(static_cast<float>(windowWidth) * 0.70f, 700.0f);
    const float h = 50.0f;
    const float x = (static_cast<float>(windowWidth) - w) * 0.5f;
    const float y = std::max(94.0f, static_cast<float>(windowHeight) * 0.16f);
    drawUiBox(x, y, w, h);
    drawWrappedText(x + 12.0f, y + 30.0f, prompt, w - 24.0f, 18.0f,
                    GLUT_BITMAP_HELVETICA_18, 1.0f, 1.0f, 1.0f);
}

void drawSceneHeader() {
    const std::string title = getSceneTitle();
    if (title.empty()) {
        return;
    }

    const float margin = 12.0f;
    const float panelH = 56.0f;
    const float w = std::min(280.0f, static_cast<float>(windowWidth) * 0.30f);
    drawUiBox(margin, margin, w, panelH);
    setColor(1.0f, 0.9f, 0.2f);
    drawText(margin + 12.0f, margin + 23.0f, title, GLUT_BITMAP_HELVETICA_18);
}

void drawSceneObjective() {
    const std::string objective = getSceneObjective();
    if (objective.empty()) {
        return;
    }

    const float margin = 12.0f;
    const float panelH = 56.0f;
    const float titleW = std::min(280.0f, static_cast<float>(windowWidth) * 0.30f);
    const float x = margin + titleW + 12.0f;
    const float w = static_cast<float>(windowWidth) - x - margin;
    drawUiBox(x, margin, w, panelH);
    drawWrappedText(x + 12.0f, margin + 35.0f, objective, w - 24.0f, 17.0f,
                    GLUT_BITMAP_8_BY_13, 1.0f, 1.0f, 1.0f);
}

void drawNotificationToast() {
    if (notificationTimer <= 0.0f || notificationText.empty()) {
        return;
    }

    const float w = std::min(static_cast<float>(windowWidth) * 0.62f, 520.0f);
    const float h = 48.0f;
    const float x = (windowWidth - w) * 0.5f;
    const float y = static_cast<float>(windowHeight) - h - 62.0f;
    drawPanel(x, y, w, h);
    drawWrappedText(x + 18.0f, y + 28.0f, notificationText, w - 36.0f, 18.0f,
                    GLUT_BITMAP_HELVETICA_18, 1.0f, 0.86f, 0.35f);
}

void drawCameraReticle() {
    if (!camera.mouseLookEnabled || isOverlayOnlyState(currentState)) {
        return;
    }

    const float cx = windowWidth * 0.5f;
    const float cy = windowHeight * 0.5f;
    setColor(1.0f, 0.86f, 0.35f);
    drawLine(cx - 8.0f, cy, cx - 3.0f, cy);
    drawLine(cx + 3.0f, cy, cx + 8.0f, cy);
    drawLine(cx, cy - 8.0f, cx, cy - 3.0f);
    drawLine(cx, cy + 3.0f, cx, cy + 8.0f);
}

void drawTitleScreen() {
    setColor(0.02f, 0.03f, 0.08f);
    drawRect(0.0f, 0.0f, static_cast<float>(windowWidth), static_cast<float>(windowHeight));

    setColor(1.0f, 0.9f, 0.2f);
    drawCenteredText(static_cast<float>(windowHeight) * 0.68f, "SAMSAT QUEST", GLUT_BITMAP_TIMES_ROMAN_24);

    setColor(1.0f, 1.0f, 1.0f);
    drawCenteredText(static_cast<float>(windowHeight) * 0.62f, "STNK of Destiny", GLUT_BITMAP_HELVETICA_18);
    drawCenteredText(static_cast<float>(windowHeight) * 0.55f, "Urus STNK sebelum loket tutup", GLUT_BITMAP_HELVETICA_18);
    drawCenteredText(static_cast<float>(windowHeight) * 0.50f, "Lengkapi nomor antrean, map, fotokopi, formulir, cek fisik, pembayaran, dan validasi.", GLUT_BITMAP_8_BY_13);
    drawCenteredText(static_cast<float>(windowHeight) * 0.34f, "Tekan ENTER untuk mulai", GLUT_BITMAP_HELVETICA_18);
    drawCenteredText(static_cast<float>(windowHeight) * 0.29f, "Tekan ESC untuk keluar", GLUT_BITMAP_8_BY_13);
}

void drawOpeningNarration() {
    setColor(0.08f, 0.08f, 0.10f);
    drawRect(0.0f, 0.0f, static_cast<float>(windowWidth), static_cast<float>(windowHeight));

    setColor(1.0f, 0.9f, 0.2f);
    drawCenteredText(static_cast<float>(windowHeight) * 0.74f, "Opening", GLUT_BITMAP_TIMES_ROMAN_24);

    setColor(1.0f, 1.0f, 1.0f);
    const float textX = static_cast<float>(windowWidth) * 0.18f;
    const float textW = static_cast<float>(windowWidth) * 0.64f;
    const float startY = static_cast<float>(windowHeight) * 0.61f;
    if (openingLine == 0) {
        drawWrappedText(textX, startY, "STNK lama hampir habis masa berlakunya. Hari ini Anda harus mengurus perpanjangan di kantor Samsat.",
                        textW, 28.0f, GLUT_BITMAP_HELVETICA_18, 1.0f, 1.0f, 1.0f);
    } else if (openingLine == 1) {
        drawWrappedText(textX, startY, "Kantor tutup pukul 15:00. Uang, stamina, dan kesabaran Anda terbatas.",
                        textW, 28.0f, GLUT_BITMAP_HELVETICA_18, 1.0f, 1.0f, 1.0f);
    } else {
        drawWrappedText(textX, startY, "Tujuan utama: lengkapi berkas, ikuti alur loket, bayar, validasi, lalu pulang membawa STNK baru.",
                        textW, 28.0f, GLUT_BITMAP_HELVETICA_18, 1.0f, 1.0f, 1.0f);
    }

    setColor(0.8f, 0.8f, 0.8f);
    drawCenteredText(static_cast<float>(windowHeight) * 0.32f, "Tekan SPACE untuk lanjut", GLUT_BITMAP_8_BY_13);
}

void drawEndingScreen(const std::string& title, const std::string& line1, const std::string& line2) {
    setColor(0.04f, 0.04f, 0.08f);
    drawRect(0.0f, 0.0f, static_cast<float>(windowWidth), static_cast<float>(windowHeight));

    setColor(1.0f, 0.9f, 0.2f);
    drawCenteredText(static_cast<float>(windowHeight) * 0.70f, title, GLUT_BITMAP_TIMES_ROMAN_24);

    const float textX = static_cast<float>(windowWidth) * 0.18f;
    const float textW = static_cast<float>(windowWidth) * 0.64f;
    drawWrappedText(textX, static_cast<float>(windowHeight) * 0.57f, line1 + " " + line2,
                    textW, 28.0f, GLUT_BITMAP_HELVETICA_18, 1.0f, 1.0f, 1.0f);
    setColor(0.82f, 0.82f, 0.86f);
    drawCenteredText(static_cast<float>(windowHeight) * 0.38f, getEndingStatsLine(), GLUT_BITMAP_8_BY_13);

    setColor(0.8f, 0.8f, 0.8f);
    drawCenteredText(static_cast<float>(windowHeight) * 0.32f, "Tekan SPACE untuk menuju credit.", GLUT_BITMAP_8_BY_13);
}

void drawCreditScene() {
    setColor(0.02f, 0.02f, 0.05f);
    drawRect(0.0f, 0.0f, static_cast<float>(windowWidth), static_cast<float>(windowHeight));

    setColor(1.0f, 0.9f, 0.2f);
    drawCenteredText(static_cast<float>(windowHeight) * 0.70f, "SAMSAT QUEST", GLUT_BITMAP_TIMES_ROMAN_24);

    setColor(1.0f, 1.0f, 1.0f);
    drawCenteredText(static_cast<float>(windowHeight) * 0.60f, "OpenGL GLUT Edition", GLUT_BITMAP_HELVETICA_18);
    drawCenteredText(static_cast<float>(windowHeight) * 0.53f, "Terima kasih sudah mengurus STNK.", GLUT_BITMAP_HELVETICA_18);
    drawCenteredText(static_cast<float>(windowHeight) * 0.46f, "Post-credit: Mohon fotokopi rangkap tiga.", GLUT_BITMAP_HELVETICA_18);

    setColor(0.8f, 0.8f, 0.8f);
    drawCenteredText(static_cast<float>(windowHeight) * 0.32f, "Tekan ENTER untuk kembali ke title screen.", GLUT_BITMAP_8_BY_13);
}

void drawOverlayOnlyState() {
    switch (currentState) {
        case TITLE_SCREEN:
            drawTitleScreen();
            break;
        case OPENING_NARRATION:
            drawOpeningNarration();
            break;
        case ENDING_CLEAN_SUCCESS:
            drawEndingScreen("STNK BERHASIL SELESAI", "Semua berkas diterima.", "Anda pulang membawa STNK baru.");
            break;
        case ENDING_GIVE_UP:
            drawEndingScreen("PULANG TANPA STNK", "Proses belum selesai.", "Anda meninggalkan kantor tanpa STNK baru.");
            break;
        case CREDIT_SCENE:
            drawCreditScene();
            break;
        default:
            break;
    }
}

void drawCommonOverlay() {
    drawResourceUI();
    drawSceneHeader();
    drawSceneObjective();
    drawStateInstructionPanel();
    drawInteractionPrompt();
    drawDialogueBox();
    drawInventoryOverlay();
    drawQuestLogOverlay();
    drawNotificationToast();
    drawCameraReticle();
    drawDebugOverlay();
}

}  // namespace samsat
