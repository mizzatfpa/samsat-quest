#include "game_internal.hpp"

namespace samsat {

void setColor(float r, float g, float b) {
    glColor3f(r, g, b);
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

void drawCube(float x, float y, float z, float sx, float sy, float sz, float r, float g, float b) {
    glPushMatrix();
    setColor(r, g, b);
    glTranslatef(x, y, z);
    glScalef(sx, sy, sz);
    glutSolidCube(1.0f);
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
                        bool hasSuspiciousCoat) {
    const float skinR = 1.0f;
    const float skinG = 0.80f;
    const float skinB = 0.60f;

    drawCube(0.0f, 1.14f, 0.0f, 0.70f, 1.12f, 0.42f, bodyR, bodyG, bodyB);
    drawCube(0.0f, 1.86f, 0.0f, 0.48f, 0.48f, 0.48f, skinR, skinG, skinB);

    drawCube(-0.52f, 1.12f, 0.0f, 0.22f, 0.86f, 0.24f, bodyR * 0.9f, bodyG * 0.9f, bodyB * 0.9f);
    drawCube(0.52f, 1.12f, 0.0f, 0.22f, 0.86f, 0.24f, bodyR * 0.9f, bodyG * 0.9f, bodyB * 0.9f);
    drawCube(-0.52f, 0.62f, 0.0f, 0.22f, 0.20f, 0.22f, skinR, skinG, skinB);
    drawCube(0.52f, 0.62f, 0.0f, 0.22f, 0.20f, 0.22f, skinR, skinG, skinB);

    drawCube(-0.20f, 0.45f, 0.0f, 0.24f, 0.78f, 0.26f, pantsR, pantsG, pantsB);
    drawCube(0.20f, 0.45f, 0.0f, 0.24f, 0.78f, 0.26f, pantsR, pantsG, pantsB);
    drawCube(-0.20f, 0.08f, -0.05f, 0.32f, 0.16f, 0.42f, 0.05f, 0.05f, 0.06f);
    drawCube(0.20f, 0.08f, -0.05f, 0.32f, 0.16f, 0.42f, 0.05f, 0.05f, 0.06f);

    drawCube(0.0f, 1.35f, -0.25f, 0.46f, 0.10f, 0.08f, accentR, accentG, accentB);
    drawCube(0.0f, 0.82f, -0.24f, 0.76f, 0.10f, 0.08f, accentR * 0.75f, accentG * 0.75f, accentB * 0.75f);

    if (hasCap) {
        drawCube(0.0f, 2.16f, -0.03f, 0.56f, 0.16f, 0.50f, accentR, accentG, accentB);
        drawCube(0.0f, 2.08f, -0.31f, 0.48f, 0.08f, 0.20f, accentR * 0.75f, accentG * 0.75f, accentB * 0.75f);
    }

    if (hasSuspiciousCoat) {
        drawCube(0.0f, 1.04f, -0.28f, 0.82f, 1.28f, 0.08f, 0.10f, 0.03f, 0.04f);
        drawCube(0.0f, 2.18f, 0.0f, 0.70f, 0.12f, 0.55f, 0.04f, 0.04f, 0.04f);
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
    const float boardWidth = (width > 0.0f) ? width : std::max(2.0f, static_cast<float>(text.size()) * 0.16f);
    drawCube(x, y, z, boardWidth, 0.52f, 0.08f, boardR, boardG, boardB);
    setColor(textR, textG, textB);
    drawWorldText(x - boardWidth * 0.42f, y - 0.08f, z + 0.08f, text, GLUT_BITMAP_HELVETICA_18);
}

void drawSmallPoster(float x, float y, float z, const std::string& text, float r, float g, float b) {
    drawCube(x, y, z, 2.5f, 1.1f, 0.08f, r, g, b);
    setColor(0.02f, 0.02f, 0.02f);
    drawWorldText(x - 1.05f, y + 0.12f, z + 0.08f, text, GLUT_BITMAP_8_BY_13);
}

void drawChair(float x, float z) {
    drawCube(x, 0.35f, z, 0.70f, 0.18f, 0.65f, 0.18f, 0.24f, 0.34f);
    drawCube(x, 0.82f, z + 0.28f, 0.70f, 0.75f, 0.14f, 0.14f, 0.18f, 0.26f);
    drawCube(x - 0.24f, 0.18f, z - 0.20f, 0.08f, 0.36f, 0.08f, 0.08f, 0.08f, 0.09f);
    drawCube(x + 0.24f, 0.18f, z - 0.20f, 0.08f, 0.36f, 0.08f, 0.08f, 0.08f, 0.09f);
}

void drawTrafficCone(float x, float z) {
    drawCube(x, 0.12f, z, 0.48f, 0.12f, 0.48f, 0.08f, 0.08f, 0.08f);
    drawCube(x, 0.36f, z, 0.34f, 0.36f, 0.34f, 0.95f, 0.36f, 0.08f);
    drawCube(x, 0.58f, z, 0.20f, 0.22f, 0.20f, 1.0f, 0.78f, 0.20f);
}

void drawFlagPole(float x, float z) {
    drawCube(x, 1.8f, z, 0.08f, 3.6f, 0.08f, 0.72f, 0.72f, 0.76f);
    drawCube(x + 0.45f, 3.0f, z, 0.90f, 0.28f, 0.04f, 0.85f, 0.05f, 0.05f);
    drawCube(x + 0.45f, 2.72f, z, 0.90f, 0.28f, 0.04f, 0.95f, 0.95f, 0.90f);
}

void drawGroundDisk(float x, float z, float radius, float r, float g, float b) {
    glDisable(GL_LIGHTING);
    setColor(r, g, b);
    glBegin(GL_TRIANGLE_FAN);
    glVertex3f(x, 0.07f, z);
    for (int i = 0; i <= 32; ++i) {
        const float angle = static_cast<float>(i) * 2.0f * kPi / 32.0f;
        glVertex3f(x + std::cos(angle) * radius, 0.07f, z + std::sin(angle) * radius);
    }
    glEnd();
    glEnable(GL_LIGHTING);
}

void drawFloatingArrow(float x, float z, float r, float g, float b) {
    const float bob = std::sin(interactionPulse) * 0.15f;
    drawCube(x, 2.65f + bob, z, 0.22f, 0.72f, 0.22f, r, g, b);
    drawCube(x, 2.18f + bob, z, 0.70f, 0.24f, 0.70f, r, g, b);
}

void drawPortalMarker(float x, float z, const std::string& label, float r, float g, float b, bool finalPortal = false) {
    const float pulse = 0.18f + (std::sin(interactionPulse) + 1.0f) * 0.10f;
    drawGroundDisk(x, z, 1.15f + pulse, r * 0.55f, g * 0.55f, b * 0.55f);
    drawGroundDisk(x, z, 0.65f + pulse * 0.35f, r, g, b);
    drawFloatingArrow(x, z, r, g, b);
    draw3DLabel(x, finalPortal ? 3.18f : 1.55f, z + 0.72f, label,
                finalPortal ? 0.38f : 0.04f,
                finalPortal ? 0.05f : 0.09f,
                finalPortal ? 0.05f : 0.16f,
                1.0f, 1.0f, 1.0f, std::max(2.4f, static_cast<float>(label.size()) * 0.18f));
}

void drawInteractionMarker(float x, float z, const std::string& label, bool completed = false) {
    const bool nearby = isNear(player.x, player.z, x, z, 2.4f);
    const float pulse = nearby ? 0.35f : 0.12f;
    const float r = completed ? 0.20f : (nearby ? 1.0f : 0.75f);
    const float g = completed ? 0.78f : (nearby ? 0.86f : 0.70f);
    const float b = completed ? 0.25f : (nearby ? 0.18f : 0.22f);

    drawGroundDisk(x, z, nearby ? 1.05f : 0.72f, r, g, b);
    drawCube(x, 2.38f + std::sin(interactionPulse) * pulse, z, 0.46f, 0.46f, 0.08f, r, g, b);
    draw3DLabel(x, 2.90f, z + 0.15f, completed ? "SELESAI" : label,
                0.04f, 0.05f, 0.07f, 1.0f, 1.0f, 1.0f,
                completed ? 2.4f : std::max(1.6f, static_cast<float>(label.size()) * 0.15f));
}

void drawObjectiveMarker(float x, float z, const std::string& label, bool finalMarker = false) {
    const float r = finalMarker ? 0.95f : 0.20f;
    const float g = finalMarker ? 0.12f : 0.55f;
    const float b = finalMarker ? 0.12f : 1.00f;
    drawGroundDisk(x, z, 1.35f, r * 0.55f, g * 0.55f, b * 0.55f);
    drawFloatingArrow(x, z, r, g, b);
    draw3DLabel(x, 3.35f, z + 0.10f, label, 0.02f, 0.04f, 0.08f, 1.0f, 1.0f, 1.0f,
                std::max(2.4f, static_cast<float>(label.size()) * 0.17f));
}

}  // namespace

void drawGround(float size, float r = 0.25f, float g = 0.45f, float b = 0.25f) {
    setColor(r, g, b);
    glBegin(GL_QUADS);
    glNormal3f(0.0f, 1.0f, 0.0f);
    glVertex3f(-size, 0.0f, -size);
    glVertex3f(size, 0.0f, -size);
    glVertex3f(size, 0.0f, size);
    glVertex3f(-size, 0.0f, size);
    glEnd();
}

void drawPlayer3D(float x, float z) {
    glPushMatrix();
    glTranslatef(x, 0.0f, z);
    glRotatef(player.facingYaw, 0.0f, 1.0f, 0.0f);

    drawCharacterModel(0.10f, 0.20f, 0.90f,
                       0.08f, 0.10f, 0.22f,
                       1.00f, 0.86f, 0.20f,
                       false, false);

    glPopMatrix();
}

void drawNPC3D(float x, float z, float r, float g, float b) {
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
                       securityUniform, suspicious);

    glPopMatrix();
}

void drawSimpleCar(float x, float z) {
    drawCube(x, 0.5f, z, 2.2f, 0.7f, 1.2f, 0.18f, 0.24f, 0.70f);
    drawCube(x, 1.0f, z - 0.1f, 1.2f, 0.6f, 0.9f, 0.10f, 0.12f, 0.32f);
    drawCube(x - 0.75f, 0.20f, z - 0.62f, 0.42f, 0.42f, 0.18f, 0.03f, 0.03f, 0.04f);
    drawCube(x + 0.75f, 0.20f, z - 0.62f, 0.42f, 0.42f, 0.18f, 0.03f, 0.03f, 0.04f);
    drawCube(x - 0.75f, 0.20f, z + 0.62f, 0.42f, 0.42f, 0.18f, 0.03f, 0.03f, 0.04f);
    drawCube(x + 0.75f, 0.20f, z + 0.62f, 0.42f, 0.42f, 0.18f, 0.03f, 0.03f, 0.04f);
}

void drawRoomFrame(float halfWidth, float halfDepth, float wallHeight, float r, float g, float b) {
    drawCube(0.0f, wallHeight * 0.5f, -halfDepth, halfWidth * 2.0f, wallHeight, 0.3f, r, g, b);
    drawCube(-halfWidth, wallHeight * 0.5f, 0.0f, 0.3f, wallHeight, halfDepth * 2.0f, r, g, b);
    drawCube(halfWidth, wallHeight * 0.5f, 0.0f, 0.3f, wallHeight, halfDepth * 2.0f, r, g, b);
}

void drawQueueMarker(float x, float z) {
    drawCube(x, 0.05f, z, 0.9f, 0.02f, 2.2f, 0.95f, 0.90f, 0.25f);
}

void drawDocumentPile(float x, float y, float z, float r, float g, float b) {
    drawCube(x, y, z, 0.9f, 0.10f, 0.7f, r, g, b);
    drawCube(x + 0.10f, y + 0.10f, z - 0.05f, 0.9f, 0.08f, 0.7f, r + 0.05f, g + 0.05f, b + 0.05f);
}

void drawCounterStamp(float x, float z, float r, float g, float b) {
    drawCube(x, 1.10f, z, 0.45f, 0.75f, 0.45f, r, g, b);
    drawCube(x, 0.72f, z, 0.85f, 0.18f, 0.85f, r * 0.8f, g * 0.8f, b * 0.8f);
}

void setupLighting() {
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
    glEnable(GL_LIGHT1);
    glEnable(GL_LIGHT2);
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

    const GLfloat materialSpecular[] = {0.28f, 0.28f, 0.30f, 1.0f};
    glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, materialSpecular);
    glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, 28.0f);
}

void updateSceneLights() {
    const GLfloat keyPosition[] = {-7.0f, 12.0f, 7.0f, 1.0f};
    const GLfloat fillPosition[] = {8.0f, 5.0f, -6.0f, 1.0f};
    const GLfloat rimPosition[] = {player.x - 3.0f, 4.5f, player.z + 4.0f, 1.0f};
    glLightfv(GL_LIGHT0, GL_POSITION, keyPosition);
    glLightfv(GL_LIGHT1, GL_POSITION, fillPosition);
    glLightfv(GL_LIGHT2, GL_POSITION, rimPosition);
}

void setupCamera() {
    float targetX = player.x;
    float targetY = camera.targetHeight;
    float targetZ = player.z;
    float baseDistance = camera.distance;
    float baseHeight = camera.height;

    switch (currentState) {
        case SAMSAT_EXTERIOR:
        case TUTORIAL_CONTROL:
        case DIALOG_SECURITY_GUARD:
        case QUEUE_MACHINE:
        case MAP_VENDOR:
        case PHOTOCOPY_SHOP:
            targetX = player.x;
            targetY = 1.6f;
            targetZ = player.z;
            break;

        case VEHICLE_CHECK_AREA:
            targetX = 0.0f;
            targetY = 1.3f;
            targetZ = 0.0f;
            baseDistance = camera.distance + 2.0f;
            baseHeight = camera.height + 1.0f;
            break;

        case FINAL_CORRIDOR:
        case FINAL_COUNTER_BOSS:
            targetX = 0.0f;
            targetY = 1.6f;
            targetZ = -3.0f;
            baseDistance = camera.distance + 3.0f;
            break;

        default:
            break;
    }

    const float yawRadians = camera.yaw * kPi / 180.0f;
    const float pitchRadians = camera.pitch * kPi / 180.0f;
    const float horizontalDistance = std::cos(pitchRadians) * baseDistance;
    const float verticalDistance = -std::sin(pitchRadians) * baseDistance;

    const float camX = targetX + std::sin(yawRadians) * horizontalDistance;
    const float camY = targetY + verticalDistance + (baseHeight - 8.0f);
    const float camZ = targetZ + std::cos(yawRadians) * horizontalDistance;

    gluLookAt(camX, camY, camZ, targetX, targetY, targetZ, 0.0f, 1.0f, 0.0f);
}


void drawSamsatExterior3D() {
    drawGround(30.0f);

    drawCube(0.0f, 2.5f, -10.0f, 14.0f, 5.0f, 4.0f, 0.58f, 0.58f, 0.62f);
    drawCube(0.0f, 1.3f, -7.7f, 2.0f, 2.6f, 0.2f, 0.10f, 0.10f, 0.15f);
    draw3DLabel(0.0f, 5.05f, -7.85f, "KANTOR SAMSAT", 0.02f, 0.08f, 0.16f, 1.0f, 0.92f, 0.24f, 5.0f);
    draw3DLabel(0.0f, 2.9f, -7.55f, "RUANG INFORMASI", 0.10f, 0.22f, 0.42f, 1.0f, 1.0f, 1.0f, 3.9f);
    drawCube(0.0f, 0.08f, -4.8f, 2.8f, 0.04f, 1.0f, 0.18f, 0.48f, 0.90f);  // information room portal
    drawCube(10.0f, 0.08f, -8.2f, 2.8f, 0.04f, 1.0f, 0.90f, 0.82f, 0.28f); // photocopy portal
    drawCube(-9.8f, 0.08f, -8.3f, 2.8f, 0.04f, 1.0f, 0.22f, 0.68f, 0.35f); // vehicle check portal
    drawCube(7.0f, 0.08f, -8.3f, 2.8f, 0.04f, 1.0f, 0.58f, 0.58f, 0.86f);  // verification portal
    drawCube(-9.8f, 0.08f, 8.4f, 2.8f, 0.04f, 1.0f, 0.86f, 0.58f, 0.28f);  // payment portal
    drawCube(-7.0f, 1.2f, -1.0f, 2.6f, 2.4f, 2.5f, 0.35f, 0.25f, 0.16f);
    drawCube(7.0f, 1.2f, -1.0f, 3.2f, 2.4f, 2.5f, 0.65f, 0.35f, 0.15f);
    drawCube(10.0f, 1.2f, -2.0f, 2.8f, 2.4f, 2.4f, 0.72f, 0.72f, 0.76f);
    drawCube(0.0f, 1.0f, 1.0f, 1.0f, 2.0f, 0.8f, 0.15f, 0.15f, 0.18f);
    draw3DLabel(0.0f, 2.25f, 0.62f, "MESIN ANTREAN", 0.05f, 0.08f, 0.12f, 1.0f, 0.9f, 0.2f, 3.1f);
    draw3DLabel(-7.0f, 2.65f, 0.25f, "SATPAM", 0.06f, 0.18f, 0.08f, 1.0f, 1.0f, 1.0f, 2.2f);
    draw3DLabel(7.0f, 2.75f, 0.15f, "PENJUAL MAP", 0.30f, 0.14f, 0.04f, 1.0f, 0.95f, 0.25f, 2.9f);
    draw3DLabel(10.0f, 2.7f, -0.75f, "FOTOKOPI", 0.12f, 0.12f, 0.12f, 1.0f, 0.9f, 0.2f, 2.8f);
    draw3DLabel(-9.8f, 2.2f, -7.4f, "CEK FISIK", 0.07f, 0.18f, 0.10f, 1.0f, 1.0f, 1.0f, 2.7f);
    draw3DLabel(7.0f, 2.2f, -7.4f, "VERIFIKASI", 0.10f, 0.11f, 0.28f, 1.0f, 1.0f, 1.0f, 2.8f);
    draw3DLabel(-9.8f, 2.2f, 8.8f, "PEMBAYARAN", 0.35f, 0.20f, 0.05f, 1.0f, 0.95f, 0.25f, 3.0f);

    drawCube(-10.0f, 0.05f, 5.0f, 5.0f, 0.02f, 10.0f, 0.32f, 0.32f, 0.35f);
    drawCube(10.0f, 0.05f, 5.0f, 5.0f, 0.02f, 10.0f, 0.32f, 0.32f, 0.35f);
    draw3DLabel(-10.0f, 0.25f, 0.4f, "PARKIR", 0.12f, 0.12f, 0.14f, 1.0f, 1.0f, 1.0f, 2.0f);
    draw3DLabel(10.0f, 0.25f, 0.4f, "PARKIR", 0.12f, 0.12f, 0.14f, 1.0f, 1.0f, 1.0f, 2.0f);
    drawSimpleCar(-9.0f, 5.0f);
    drawSimpleCar(9.0f, 5.0f);
    drawFlagPole(-13.5f, -7.5f);
    drawSmallPoster(-3.8f, 1.6f, -7.55f, "AMBIL NOMOR", 0.95f, 0.88f, 0.28f);
    drawSmallPoster(3.8f, 1.6f, -7.55f, "SIAPKAN KTP", 0.95f, 0.88f, 0.28f);
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
    drawNPC3D(0.0f, 1.0f, 0.40f, 0.40f, 0.40f);
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
    draw3DLabel(0.0f, 1.25f, 6.55f, "KELUAR", 0.07f, 0.22f, 0.11f, 1.0f, 1.0f, 1.0f, 2.0f);
    drawCube(-4.0f, 0.45f, 1.2f, 1.2f, 0.9f, 1.2f, 0.45f, 0.45f, 0.50f);
    drawCube(0.0f, 0.45f, 1.2f, 1.2f, 0.9f, 1.2f, 0.45f, 0.45f, 0.50f);
    drawCube(4.0f, 0.45f, 1.2f, 1.2f, 0.9f, 1.2f, 0.45f, 0.45f, 0.50f);
    drawChair(-4.0f, 1.2f);
    drawChair(0.0f, 1.2f);
    drawChair(4.0f, 1.2f);
    drawSmallPoster(-5.5f, 2.1f, -7.75f, "ALUR LAYANAN", 0.85f, 0.92f, 1.0f);
    drawSmallPoster(5.5f, 2.1f, -7.75f, "BERKAS LENGKAP", 0.95f, 0.88f, 0.28f);
    drawCube(5.0f, 1.0f, -3.0f, 1.4f, 2.0f, 1.4f, 0.36f, 0.32f, 0.28f);
    drawPortalMarker(0.0f, 6.2f, "KELUAR", 0.15f, 0.85f, 0.32f);
    drawPortalMarker(5.0f, -3.0f, "LOKET FORM", 0.15f, 0.42f, 1.0f);
    drawInteractionMarker(0.0f, -4.0f, "E", metInformationOfficer);
    if (!hasFilledForm) {
        drawObjectiveMarker(5.0f, -3.0f, "TUJUAN: FORM");
    }

    drawNPC3D(0.0f, -4.0f, 0.25f, 0.45f, 0.92f);
    drawNPC3D(-4.0f, 2.5f, 0.70f, 0.70f, 0.20f);
    drawNPC3D(4.0f, 2.5f, 0.60f, 0.40f, 0.75f);
    drawPlayer3D(player.x, player.z);
}

void drawGenericCounterScene3D(float accentR, float accentG, float accentB, bool showPlayerCharacter = true) {
    drawGround(16.0f, 0.44f, 0.44f, 0.46f);
    drawRoomFrame(8.0f, 7.0f, 4.5f, 0.68f, 0.68f, 0.72f);
    drawCube(0.0f, 0.85f, -3.0f, 6.0f, 1.1f, 1.4f, 0.46f, 0.34f, 0.24f);
    drawCube(0.0f, 1.55f, -4.7f, 5.2f, 1.6f, 0.3f, accentR, accentG, accentB);
    draw3DLabel(0.0f, 2.65f, -4.45f, "VALIDASI SELESAI", 0.06f, 0.18f, 0.08f, 1.0f, 1.0f, 1.0f, 4.0f);
    draw3DLabel(0.0f, 1.25f, 6.35f, "LORONG FINAL", 0.06f, 0.12f, 0.08f, 1.0f, 1.0f, 1.0f, 3.4f);
    drawCube(-4.8f, 1.1f, -2.5f, 1.0f, 1.8f, 1.0f, 0.55f, 0.55f, 0.58f);
    drawCube(4.8f, 1.1f, -2.5f, 1.0f, 1.8f, 1.0f, 0.55f, 0.55f, 0.58f);
    drawPortalMarker(0.0f, -6.0f, "LORONG FINAL", 0.15f, 0.85f, 0.32f);
    drawObjectiveMarker(0.0f, -6.0f, "TUJUAN: FINAL");
    drawNPC3D(0.0f, -4.0f, 0.18f, 0.36f, 0.80f);
    if (showPlayerCharacter) {
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
    draw3DLabel(0.0f, 1.25f, 6.45f, "KELUAR / CEK FISIK", 0.06f, 0.20f, 0.10f, 1.0f, 1.0f, 1.0f, 4.2f);
    drawDocumentPile(-2.2f, 1.15f, -2.7f, 0.90f, 0.90f, 0.92f);
    drawDocumentPile(2.0f, 1.15f, -2.8f, 0.86f, 0.86f, 0.88f);
    drawDocumentPile(-0.7f, 1.22f, -2.4f, 0.75f, 0.78f, 0.95f);
    drawCube(2.9f, 1.22f, -2.55f, 0.48f, 0.14f, 0.70f, 0.18f, 0.36f, 0.82f);
    drawCube(3.2f, 1.26f, -2.55f, 0.48f, 0.14f, 0.70f, 0.18f, 0.36f, 0.82f);
    drawPortalMarker(0.0f, 6.1f, hasFilledForm ? "LANJUT" : "KELUAR", 0.15f, 0.85f, 0.32f);
    drawInteractionMarker(0.0f, -4.0f, "E", hasFilledForm);
    if (!hasFilledForm) {
        drawObjectiveMarker(0.0f, -4.0f, "TUJUAN: FORM");
    }
    drawCube(4.8f, 1.6f, -4.5f, 1.1f, 3.2f, 0.8f, 0.55f, 0.55f, 0.60f);
    drawCube(-4.8f, 1.6f, -4.5f, 1.1f, 3.2f, 0.8f, 0.55f, 0.55f, 0.60f);
    drawNPC3D(0.0f, -4.0f, 0.18f, 0.36f, 0.82f);
    drawPlayer3D(player.x, player.z);
}

void drawVerificationCounter3D() {
    drawGround(16.0f, 0.42f, 0.44f, 0.48f);
    drawRoomFrame(8.0f, 7.0f, 4.5f, 0.64f, 0.66f, 0.70f);
    drawCube(0.0f, 0.08f, 6.1f, 3.0f, 0.04f, 0.8f, 0.58f, 0.58f, 0.86f);
    drawCube(0.0f, 0.90f, -3.0f, 6.2f, 1.2f, 1.4f, 0.42f, 0.34f, 0.24f);
    drawCube(0.0f, 1.70f, -4.7f, 5.4f, 1.5f, 0.3f, 0.32f, 0.42f, 0.52f);
    draw3DLabel(0.0f, 2.8f, -4.45f, "VERIFIKASI BERKAS", 0.10f, 0.12f, 0.22f, 1.0f, 1.0f, 1.0f, 4.5f);
    draw3DLabel(0.0f, 1.25f, 6.45f, "PEMBAYARAN", 0.20f, 0.16f, 0.05f, 1.0f, 0.95f, 0.25f, 3.3f);
    drawDocumentPile(-2.4f, 1.12f, -2.7f, 0.88f, 0.84f, 0.65f);
    drawDocumentPile(2.3f, 1.12f, -2.8f, 0.84f, 0.90f, 0.84f);
    drawCounterStamp(0.0f, -2.5f, 0.55f, 0.05f, 0.05f);
    drawCube(1.1f, 1.32f, -2.55f, 0.80f, 0.10f, 0.80f, 0.95f, 0.15f, 0.15f);
    drawPortalMarker(0.0f, 6.1f, hasVerificationStamp ? "LANJUT" : "KELUAR", 0.15f, 0.85f, 0.32f);
    drawInteractionMarker(0.0f, -4.0f, "E", hasVerificationStamp);
    if (!hasVerificationStamp) {
        drawObjectiveMarker(0.0f, -4.0f, "TUJUAN: VERIFIKASI");
    }
    drawCube(-5.2f, 2.0f, -4.4f, 1.3f, 4.0f, 1.0f, 0.44f, 0.40f, 0.34f);
    drawCube(5.2f, 2.0f, -4.4f, 1.3f, 4.0f, 1.0f, 0.44f, 0.40f, 0.34f);
    drawNPC3D(0.0f, -4.0f, 0.16f, 0.34f, 0.74f);
    drawPlayer3D(player.x, player.z);
}

void drawPaymentCounter3D() {
    drawGround(16.0f, 0.50f, 0.48f, 0.44f);
    drawRoomFrame(8.0f, 7.0f, 4.5f, 0.68f, 0.66f, 0.62f);
    drawCube(0.0f, 0.08f, 6.1f, 3.0f, 0.04f, 0.8f, 0.86f, 0.58f, 0.28f);
    drawCube(0.0f, 0.90f, -3.0f, 6.4f, 1.2f, 1.4f, 0.48f, 0.36f, 0.22f);
    drawCube(0.0f, 2.05f, -3.5f, 5.2f, 0.15f, 0.1f, 0.76f, 0.84f, 0.90f);
    drawCube(0.0f, 1.72f, -4.7f, 5.4f, 1.5f, 0.3f, 0.58f, 0.38f, 0.16f);
    draw3DLabel(0.0f, 2.8f, -4.45f, "LOKET PEMBAYARAN", 0.28f, 0.16f, 0.04f, 1.0f, 0.95f, 0.25f, 4.4f);
    draw3DLabel(0.0f, 1.25f, 6.45f, "VALIDASI", 0.30f, 0.06f, 0.06f, 1.0f, 1.0f, 1.0f, 2.8f);
    drawCube(-1.8f, 1.18f, -2.7f, 0.8f, 0.45f, 0.8f, 0.14f, 0.14f, 0.16f);
    drawCube(-1.8f, 1.48f, -2.98f, 0.90f, 0.10f, 0.12f, 0.10f, 0.60f, 0.18f);
    drawCube(0.9f, 1.18f, -2.65f, 0.70f, 0.10f, 0.40f, 0.12f, 0.55f, 0.18f);
    drawWorldText(0.62f, 1.28f, -2.43f, "RP", GLUT_BITMAP_8_BY_13);
    drawPortalMarker(0.0f, 6.1f, hasPaymentProof ? "VALIDASI" : "KEMBALI", 0.15f, 0.85f, 0.32f);
    drawInteractionMarker(0.0f, -4.0f, "E", hasPaymentProof);
    if (!hasPaymentProof) {
        drawObjectiveMarker(0.0f, -4.0f, "TUJUAN: BAYAR");
    }
    drawDocumentPile(2.0f, 1.12f, -2.7f, 0.92f, 0.92f, 0.88f);
    drawCube(4.7f, 1.10f, -2.8f, 1.2f, 1.8f, 1.0f, 0.54f, 0.52f, 0.48f);
    drawNPC3D(0.0f, -4.0f, 0.16f, 0.28f, 0.68f);
    drawPlayer3D(player.x, player.z);
}

void drawValidationCounter3D() {
    drawGround(16.0f, 0.44f, 0.40f, 0.40f);
    drawRoomFrame(8.0f, 7.0f, 4.5f, 0.66f, 0.62f, 0.62f);
    drawCube(0.0f, 0.08f, 6.1f, 3.0f, 0.04f, 0.8f, 0.80f, 0.18f, 0.18f);
    drawCube(0.0f, 0.90f, -3.0f, 6.4f, 1.2f, 1.4f, 0.42f, 0.32f, 0.22f);
    drawCube(0.0f, 1.72f, -4.7f, 5.4f, 1.5f, 0.3f, 0.62f, 0.10f, 0.10f);
    draw3DLabel(0.0f, 2.8f, -4.45f, "VALIDASI", 0.32f, 0.05f, 0.05f, 1.0f, 1.0f, 1.0f, 3.2f);
    draw3DLabel(0.0f, 1.25f, 6.45f, "CARI METERAI", 0.34f, 0.20f, 0.04f, 1.0f, 0.95f, 0.25f, 3.5f);
    drawCounterStamp(-1.2f, -2.5f, 0.65f, 0.00f, 0.00f);
    drawCounterStamp(1.6f, -2.6f, 0.12f, 0.12f, 0.12f);
    draw3DLabel(-1.2f, 2.0f, -2.25f, "CAP VALID", 0.62f, 0.08f, 0.08f, 1.0f, 1.0f, 1.0f, 2.4f);
    drawPortalMarker(0.0f, 6.1f, receivedStampRequirement ? "METERAI" : "KELUAR", 0.15f, 0.85f, 0.32f);
    drawInteractionMarker(0.0f, -4.0f, "E", receivedStampRequirement);
    if (!receivedStampRequirement) {
        drawObjectiveMarker(0.0f, -4.0f, "TUJUAN: VALIDASI");
    }
    drawDocumentPile(2.8f, 1.10f, -2.7f, 0.90f, 0.88f, 0.82f);
    drawCube(-4.8f, 1.7f, -4.4f, 1.2f, 3.4f, 0.9f, 0.48f, 0.42f, 0.34f);
    drawNPC3D(0.0f, -4.0f, 0.14f, 0.26f, 0.62f);
    drawPlayer3D(player.x, player.z);
}

void drawPhotocopyShop3D() {
    drawGround(16.0f, 0.78f, 0.72f, 0.58f);
    drawRoomFrame(8.0f, 7.0f, 4.0f, 0.72f, 0.72f, 0.74f);
    drawCube(0.0f, 0.08f, 6.1f, 3.0f, 0.04f, 0.8f, 0.90f, 0.82f, 0.28f);
    draw3DLabel(0.0f, 1.25f, 6.45f, "KELUAR", 0.08f, 0.20f, 0.10f, 1.0f, 1.0f, 1.0f, 2.0f);
    drawCube(-2.5f, 0.9f, -2.0f, 2.2f, 1.8f, 1.6f, 0.90f, 0.90f, 0.92f);
    draw3DLabel(-2.5f, 2.15f, -1.05f, "MESIN FOTOKOPI", 0.10f, 0.10f, 0.12f, 1.0f, 0.92f, 0.25f, 3.6f);
    drawCube(2.8f, 0.8f, -2.4f, 3.0f, 1.0f, 1.4f, 0.45f, 0.33f, 0.24f);
    drawCube(0.8f, 1.1f, -3.8f, 4.5f, 1.6f, 0.3f, 0.55f, 0.55f, 0.58f);
    draw3DLabel(0.8f, 2.35f, -3.55f, "FOTOKOPI", 0.15f, 0.15f, 0.12f, 1.0f, 0.9f, 0.2f, 3.0f);
    drawDocumentPile(2.1f, 1.35f, -2.2f, 0.94f, 0.94f, 0.90f);
    drawDocumentPile(3.1f, 1.35f, -2.5f, 0.12f, 0.32f, 0.82f);
    drawPortalMarker(0.0f, 6.1f, "KELUAR", 0.15f, 0.85f, 0.32f);
    drawInteractionMarker(4.8f, -2.0f, "E", hasValidPhotocopy);
    if (!hasValidPhotocopy) {
        drawObjectiveMarker(4.8f, -2.0f, "TUJUAN: FOTOKOPI");
    }
    drawNPC3D(4.8f, -2.0f, 0.55f, 0.55f, 0.55f);
    drawPlayer3D(player.x, player.z);
}

void drawVehicleCheck3D() {
    drawGround(20.0f, 0.40f, 0.48f, 0.40f);
    drawCube(0.0f, 0.05f, 0.0f, 7.5f, 0.02f, 12.0f, 0.22f, 0.22f, 0.24f);
    draw3DLabel(0.0f, 2.8f, -5.5f, "CEK FISIK KENDARAAN", 0.08f, 0.18f, 0.10f, 1.0f, 1.0f, 1.0f, 5.0f);
    draw3DLabel(0.0f, 1.25f, 6.85f, "KELUAR", 0.08f, 0.20f, 0.10f, 1.0f, 1.0f, 1.0f, 2.0f);
    draw3DLabel(0.0f, 1.25f, -6.4f, "VERIFIKASI", 0.10f, 0.11f, 0.28f, 1.0f, 1.0f, 1.0f, 2.8f);
    drawSimpleCar(0.0f, 1.5f);
    drawCube(0.0f, 0.08f, 1.5f, 1.0f, 0.03f, 9.5f, 0.95f, 0.90f, 0.25f);
    drawCube(4.5f, 0.8f, -2.5f, 2.0f, 1.0f, 1.2f, 0.45f, 0.34f, 0.24f);
    drawCube(-5.5f, 1.0f, -3.5f, 2.6f, 2.0f, 2.2f, 0.32f, 0.42f, 0.52f);
    drawTrafficCone(-2.8f, 5.0f);
    drawTrafficCone(2.8f, 5.0f);
    drawTrafficCone(-2.8f, -3.0f);
    drawTrafficCone(2.8f, -3.0f);
    drawPortalMarker(0.0f, 6.7f, "KELUAR", 0.15f, 0.85f, 0.32f);
    if (hasPhysicalCheckProof) {
        drawPortalMarker(0.0f, -6.4f, "VERIFIKASI", 0.15f, 0.42f, 1.0f);
    }
    drawInteractionMarker(0.0f, 1.5f, "E", hasPhysicalCheckProof);
    drawInteractionMarker(4.5f, -1.5f, "E", hasPhysicalCheckProof);
    if (!hasPhysicalCheckProof) {
        drawObjectiveMarker(0.0f, 1.5f, "TUJUAN: CEK FISIK");
    }
    drawNPC3D(4.5f, -1.5f, 0.15f, 0.55f, 0.25f);
    drawPlayer3D(player.x, player.z);
}

void drawPaymentQueue3D() {
    drawGround(18.0f, 0.48f, 0.48f, 0.50f);
    drawRoomFrame(8.0f, 8.0f, 4.5f, 0.66f, 0.66f, 0.70f);
    drawCube(0.0f, 0.08f, 6.6f, 3.0f, 0.04f, 0.8f, 0.86f, 0.58f, 0.28f);
    drawCube(0.0f, 0.08f, -6.6f, 3.0f, 0.04f, 0.8f, 0.86f, 0.58f, 0.28f);
    draw3DLabel(0.0f, 1.25f, 6.95f, "KELUAR", 0.08f, 0.20f, 0.10f, 1.0f, 1.0f, 1.0f, 2.0f);
    draw3DLabel(0.0f, 1.25f, -6.25f, "LOKET BAYAR", 0.28f, 0.16f, 0.04f, 1.0f, 0.95f, 0.25f, 3.4f);
    drawCube(0.0f, 0.85f, -4.0f, 5.5f, 1.0f, 1.2f, 0.44f, 0.34f, 0.24f);
    draw3DLabel(0.0f, 2.0f, -3.55f, "ANTREAN PEMBAYARAN", 0.20f, 0.14f, 0.05f, 1.0f, 0.95f, 0.25f, 4.8f);
    drawPortalMarker(0.0f, 6.6f, "KELUAR", 0.15f, 0.85f, 0.32f);
    if (hasQueuedPaymentLine) {
        drawPortalMarker(0.0f, -6.6f, "LOKET BAYAR", 0.15f, 0.42f, 1.0f);
    }
    drawInteractionMarker(0.0f, 2.5f, "E", hasQueuedPaymentLine);
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
    drawPlayer3D(player.x, player.z);
}

void drawStampQuest3D() {
    drawGround(16.0f, 0.72f, 0.66f, 0.52f);
    drawRoomFrame(8.0f, 7.0f, 4.0f, 0.70f, 0.70f, 0.72f);
    drawCube(0.0f, 0.08f, 6.1f, 3.0f, 0.04f, 0.8f, 0.90f, 0.82f, 0.28f);
    draw3DLabel(0.0f, 1.25f, 6.45f, "VALIDASI AKHIR", 0.30f, 0.06f, 0.06f, 1.0f, 1.0f, 1.0f, 3.8f);
    drawCube(0.0f, 0.8f, -2.8f, 4.0f, 1.0f, 1.2f, 0.48f, 0.34f, 0.22f);
    drawCube(0.0f, 1.05f, -1.0f, 0.4f, 0.02f, 0.3f, 0.9f, 0.1f, 0.1f);
    drawCube(4.5f, 1.2f, -3.8f, 1.5f, 2.4f, 1.0f, 0.55f, 0.42f, 0.26f);
    draw3DLabel(0.0f, 2.0f, -2.45f, "METERAI", 0.34f, 0.18f, 0.04f, 1.0f, 0.95f, 0.25f, 2.6f);
    drawPortalMarker(0.0f, 6.1f, hasStampedDocument ? "VALIDASI" : "KELUAR", 0.15f, 0.85f, 0.32f);
    drawInteractionMarker(3.5f, -2.0f, "E", hasStampedDocument);
    if (!hasStampedDocument) {
        drawObjectiveMarker(3.5f, -2.0f, "TUJUAN: METERAI");
    }
    drawNPC3D(3.5f, -2.0f, 0.90f, 0.45f, 0.10f);
    drawPlayer3D(player.x, player.z);
}

void drawFinalCorridor3D() {
    drawGround(18.0f, 0.20f, 0.20f, 0.24f);
    drawCube(-4.5f, 2.0f, -1.0f, 0.5f, 4.0f, 16.0f, 0.32f, 0.32f, 0.36f);
    drawCube(4.5f, 2.0f, -1.0f, 0.5f, 4.0f, 16.0f, 0.32f, 0.32f, 0.36f);
    drawCube(0.0f, 3.8f, -1.0f, 9.5f, 0.4f, 16.0f, 0.26f, 0.26f, 0.30f);
    drawCube(0.0f, 1.2f, -8.0f, 4.0f, 2.4f, 0.3f, 0.75f, 0.70f, 0.45f);
    draw3DLabel(0.0f, 2.8f, -7.75f, "LOKET FINAL", 0.38f, 0.05f, 0.05f, 1.0f, 1.0f, 1.0f, 3.6f);
    drawCube(-3.7f, 1.8f, -4.6f, 1.0f, 3.6f, 0.9f, 0.40f, 0.34f, 0.28f);
    drawCube(3.7f, 1.8f, -4.6f, 1.0f, 3.6f, 0.9f, 0.40f, 0.34f, 0.28f);
    drawCube(-3.5f, 3.3f, -1.5f, 0.5f, 0.3f, 0.5f, 0.85f, 0.82f, 0.60f);
    drawCube(3.5f, 3.3f, -1.5f, 0.5f, 0.3f, 0.5f, 0.85f, 0.82f, 0.60f);
    drawCube(-3.5f, 3.3f, -6.0f, 0.5f, 0.3f, 0.5f, 0.85f, 0.82f, 0.60f);
    drawCube(3.5f, 3.3f, -6.0f, 0.5f, 0.3f, 0.5f, 0.85f, 0.82f, 0.60f);
    drawCube(0.0f, 0.03f, -6.5f, 3.6f, 0.02f, 1.2f, 0.90f, 0.82f, 0.20f);
    draw3DLabel(-2.5f, 2.55f, -1.55f, "ANTREAN SENIOR", 0.16f, 0.12f, 0.24f, 1.0f, 1.0f, 1.0f, 3.8f);
    draw3DLabel(2.8f, 2.55f, -3.35f, "ORANG DALAM", 0.08f, 0.02f, 0.03f, 1.0f, 0.82f, 0.82f, 3.2f);
    drawPortalMarker(0.0f, -6.5f, "LOKET FINAL", 1.0f, 0.16f, 0.16f, true);
    drawInteractionMarker(-2.5f, -2.0f, "E", receivedCorridorAdvice);
    drawInteractionMarker(2.8f, -3.8f, "E", usedInsider);
    drawObjectiveMarker(0.0f, -6.5f, "TUJUAN: FINAL", true);
    drawNPC3D(-2.5f, -2.0f, 0.55f, 0.42f, 0.70f);
    drawNPC3D(2.8f, -3.8f, 0.35f, 0.05f, 0.05f);
    drawPlayer3D(player.x, player.z);
}

void drawFinalBoss3D() {
    drawGround(18.0f, 0.18f, 0.18f, 0.22f);
    drawRoomFrame(9.0f, 8.0f, 5.0f, 0.28f, 0.28f, 0.32f);
    drawCube(0.0f, 1.0f, -4.5f, 8.0f, 1.3f, 1.6f, 0.35f, 0.20f, 0.20f);
    drawCube(-6.2f, 2.0f, -4.2f, 1.5f, 4.0f, 2.0f, 0.38f, 0.32f, 0.26f);
    drawCube(6.2f, 2.0f, -4.2f, 1.5f, 4.0f, 2.0f, 0.38f, 0.32f, 0.26f);
    drawCube(0.0f, 4.2f, -7.4f, 6.5f, 0.25f, 0.4f, 0.55f, 0.18f, 0.18f);
    draw3DLabel(0.0f, 4.75f, -7.15f, "LOKET FINAL", 0.38f, 0.05f, 0.05f, 1.0f, 1.0f, 1.0f, 4.2f);
    drawCube(1.6f, 1.6f, -3.9f, 1.2f, 0.7f, 0.8f, 0.12f, 0.12f, 0.15f);
    drawCube(3.3f, 0.75f, -3.7f, 1.0f, 0.25f, 1.0f, 0.55f, 0.00f, 0.00f);
    drawCube(-2.5f, 1.15f, -3.8f, 1.0f, 0.4f, 0.8f, 0.72f, 0.72f, 0.76f);
    drawDocumentPile(-0.8f, 1.12f, -3.6f, 0.90f, 0.88f, 0.82f);
    drawDocumentPile(-5.9f, 0.8f, -2.8f, 0.84f, 0.84f, 0.82f);
    drawDocumentPile(5.8f, 0.8f, -2.8f, 0.84f, 0.84f, 0.82f);
    drawDocumentPile(-4.8f, 1.0f, -3.4f, 0.90f, 0.88f, 0.82f);
    drawDocumentPile(4.7f, 1.0f, -3.4f, 0.90f, 0.88f, 0.82f);
    drawPortalMarker(0.0f, 6.4f, "KEMBALI", 0.15f, 0.85f, 0.32f);
    drawInteractionMarker(0.0f, -5.5f, "E", hasFinalSTNK);
    if (!hasFinalSTNK) {
        drawObjectiveMarker(0.0f, -5.5f, "TUJUAN: SERAHKAN", true);
    }
    drawCounterStamp(3.1f, -3.7f, 0.60f, 0.00f, 0.00f);
    drawNPC3D(0.0f, -5.5f, 0.10f, 0.20f, 0.60f);
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

        case FINAL_CORRIDOR:
            drawFinalCorridor3D();
            break;

        case FINAL_COUNTER_BOSS:
            drawFinalBoss3D();
            break;

        default:
            break;
    }
}

void drawResourceUI() {
    const float margin = std::max(12.0f, static_cast<float>(windowWidth) * 0.012f);
    const float topBarH = (windowWidth < 1150) ? 76.0f : 52.0f;
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
    blocks.push_back("Rep " + std::to_string(reputation));
    blocks.push_back("Moral " + std::to_string(moralScore));
    blocks.push_back("Jam " + time.str());

    const int columns = (windowWidth < 1150) ? 3 : 6;
    const float gap = 8.0f;
    const float blockW = (static_cast<float>(windowWidth) - (margin * 2.0f) - (gap * static_cast<float>(columns - 1))) /
                         static_cast<float>(columns);
    for (std::size_t i = 0; i < blocks.size(); ++i) {
        const int col = static_cast<int>(i) % columns;
        const int row = static_cast<int>(i) / columns;
        const float x = margin + static_cast<float>(col) * (blockW + gap);
        const float y = static_cast<float>(windowHeight) - 32.0f - static_cast<float>(row) * 30.0f;
        setColor(0.10f, 0.12f, 0.18f);
        drawRect(x, y - 14.0f, blockW, 24.0f);
        setColor(1.0f, 1.0f, 1.0f);
        drawText(x + 8.0f, y - 5.0f, blocks[i], GLUT_BITMAP_8_BY_13);
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
    items.push_back("Map Salah: " + boolText(hasWrongMap));
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
    quests.push_back(std::string(hasFinalSTNK ? "[X] " : "[ ] ") + "Hadapi loket final.");

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
        ss << "Money: " << money << " | Rep: " << reputation << " | Moral: " << moralScore;
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
    lines.push_back("CorridorAdvice: " + boolText(receivedCorridorAdvice));
    lines.push_back("StampReq: " + boolText(receivedStampRequirement));
    lines.push_back("Insider: " + boolText(usedInsider) + " | HelpedNPC: " + boolText(helpedNPCs));
    lines.push_back("SystemFixed: " + boolText(systemFixed));
    lines.push_back("AllFinalReq: " + boolText(hasAllFinalRequirements()));
    lines.push_back("CanFast: " + boolText(canQualifyForFastEnding()) +
                    " | CanLegend: " + boolText(canQualifyForLegendEnding()));
    lines.push_back("CanRevolution: " + boolText(canQualifyForRevolutionEnding()));
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
            return "Quest Meterai";
        case VALIDATION_SUCCESS:
            return "Validasi Berhasil";
        case FINAL_CORRIDOR:
            return "Lorong Final";
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
            return "Semua syarat siap. Lanjut ke lorong final.";

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
            return (hasPaymentProof && hasVerificationStamp) ?
                   "Validasi meminta meterai. Tekan E di pintu untuk mencari meterai." :
                   "Lengkapi bukti bayar dan verifikasi, lalu tekan E di loket.";
        case STAMP_QUEST:
            return hasStampedDocument ? "Dokumen sudah bermeterai. Tekan E di pintu untuk validasi akhir." :
                                        "Dekati penjual meterai lalu tekan E.";
        case FINAL_CORRIDOR:
            return usedInsider ? "Jalur cepat terbuka. Anda masih bisa bicara dengan senior atau maju ke loket final." :
                                 "Cari info terakhir atau jalur cepat sebelum ke loket final.";
        case FINAL_COUNTER_BOSS:
            return hasFinalSTNK ? "STNK sudah terbit. Tekan SPACE untuk menentukan ending." :
                                  (usedInsider ? "Dekati loket final untuk memproses jalur cepat." :
                                                 (hasAllFinalRequirements() ?
                                                  "Berkas final lengkap. Dekati loket final lalu tekan E." :
                                                  "Lengkapi berkas final dulu, lalu kembali ke loket final."));

        case VALIDATION_SUCCESS:
            return "Jalan ke pintu lorong final lalu tekan E.";

        default:
            return "";
    }
}

std::string getEndingStatsLine() {
    std::ostringstream ss;
    ss << "Kesabaran " << patience
       << " | Stamina " << stamina
       << " | Uang " << money
       << " | Reputasi " << reputation
       << " | Moral " << moralScore;
    return ss.str();
}

std::string getEndingRouteLine() {
    if (usedInsider) {
        return "Rute: Jalur cepat orang dalam";
    }
    if (systemFixed) {
        return "Rute: Reformasi prosedur dari dalam";
    }
    if (helpedNPCs) {
        return "Rute: Warga teladan yang membantu sekitar";
    }
    return "Rute: Jalur bersih standar birokrasi";
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
                        "WASD / Arrow Keys: jalan mengikuti kamera. Mouse kanan drag / M: lihat sekitar. E: interaksi. I: inventory. Q: quest log. R: reset kamera.",
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

    const float w = std::min(static_cast<float>(windowWidth) * 0.68f, 680.0f);
    const float h = 48.0f;
    const float x = (static_cast<float>(windowWidth) - w) * 0.5f;
    const float y = std::max(92.0f, static_cast<float>(windowHeight) * 0.16f);
    drawPanel(x, y, w, h);
    drawWrappedText(x + 18.0f, y + 28.0f, prompt, w - 36.0f, 18.0f,
                    GLUT_BITMAP_HELVETICA_18, 1.0f, 1.0f, 1.0f);
}

void drawSceneHeader() {
    const std::string title = getSceneTitle();
    if (title.empty()) {
        return;
    }

    const float margin = std::max(14.0f, static_cast<float>(windowWidth) * 0.014f);
    const float w = std::min(280.0f, static_cast<float>(windowWidth) * 0.28f);
    drawPanel(margin, margin, w, 52.0f);
    setColor(1.0f, 0.9f, 0.2f);
    drawText(margin + 12.0f, margin + 29.0f, title, GLUT_BITMAP_HELVETICA_18);
}

void drawSceneObjective() {
    const std::string objective = getSceneObjective();
    if (objective.empty()) {
        return;
    }

    const float margin = std::max(14.0f, static_cast<float>(windowWidth) * 0.014f);
    const float titleW = std::min(280.0f, static_cast<float>(windowWidth) * 0.28f);
    const float x = margin + titleW + 16.0f;
    const float w = static_cast<float>(windowWidth) - x - margin;
    drawPanel(x, margin, w, 58.0f);
    drawWrappedText(x + 14.0f, margin + 37.0f, objective, w - 28.0f, 17.0f,
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
    drawCenteredText(static_cast<float>(windowHeight) * 0.55f, "Prototype 3D OpenGL GLUT", GLUT_BITMAP_HELVETICA_18);
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
        drawWrappedText(textX, startY, "Di sebuah pagi yang tampak biasa, seorang warga menyadari satu kenyataan besar: STNK harus segera diurus.",
                        textW, 28.0f, GLUT_BITMAP_HELVETICA_18, 1.0f, 1.0f, 1.0f);
    } else if (openingLine == 1) {
        drawWrappedText(textX, startY, "Tujuannya tampak sederhana: datang, menyerahkan berkas, lalu pulang. Namun birokrasi punya questline sendiri.",
                        textW, 28.0f, GLUT_BITMAP_HELVETICA_18, 1.0f, 1.0f, 1.0f);
    } else {
        drawWrappedText(textX, startY, "Hari itu, Samsat bukan sekadar kantor. Ia adalah dungeon administratif. Dan STNK adalah artefak takdir.",
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
    drawWrappedText(textX, static_cast<float>(windowHeight) * 0.46f, getEndingRouteLine(),
                    textW, 24.0f, GLUT_BITMAP_HELVETICA_18, 1.0f, 1.0f, 1.0f);

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
    drawCenteredText(static_cast<float>(windowHeight) * 0.60f, "Prototype OpenGL GLUT", GLUT_BITMAP_HELVETICA_18);
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
            drawEndingScreen("ENDING: SUKSES BERSIH", "Semua berkas benar.", "Anda menang tanpa jalan pintas.");
            break;
        case ENDING_FAST_SUCCESS:
            drawEndingScreen("ENDING: SUKSES CEPAT", "Jalur cepat berhasil.", "Moral turun, waktu terselamatkan.");
            break;
        case ENDING_SAMSAT_LEGEND:
            drawEndingScreen("ENDING: LEGENDA SAMSAT", "Semua orang mengenal nama Anda.", "Antrean pun memberi hormat.");
            break;
        case ENDING_GIVE_UP:
            drawEndingScreen("ENDING: MENYERAH", "Kesabaran atau waktu habis.", "Sistem menang hari ini.");
            break;
        case ENDING_MAP_REVOLUTION:
            drawEndingScreen("ENDING: REVOLUSI MAP", "Anda memperbaiki sistem dari dalam.", "Map akhirnya tidak lagi mistis.");
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
