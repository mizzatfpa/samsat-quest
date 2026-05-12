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

void drawText(float x, float y, const std::string& text, void* font = GLUT_BITMAP_HELVETICA_18) {
    glRasterPos2f(x, y);
    for (char c : text) {
        glutBitmapCharacter(font, c);
    }
}

void drawCenteredText(float y, const std::string& text, void* font = GLUT_BITMAP_HELVETICA_18) {
    const float approxWidth = static_cast<float>(text.size()) * 9.0f;
    drawText((windowWidth - approxWidth) * 0.5f, y, text, font);
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

    // Body, head, legs, and a small front marker so the facing direction is readable.
    drawCube(0.0f, 1.0f, 0.0f, 0.7f, 1.2f, 0.4f, 0.10f, 0.20f, 0.90f);
    drawCube(0.0f, 1.9f, 0.0f, 0.45f, 0.45f, 0.45f, 1.0f, 0.8f, 0.6f);
    drawCube(-0.2f, 0.35f, 0.0f, 0.25f, 0.7f, 0.25f, 0.1f, 0.1f, 0.1f);
    drawCube(0.2f, 0.35f, 0.0f, 0.25f, 0.7f, 0.25f, 0.1f, 0.1f, 0.1f);
    drawCube(0.0f, 1.15f, -0.28f, 0.36f, 0.28f, 0.14f, 1.0f, 0.86f, 0.20f);

    glPopMatrix();
}

void drawNPC3D(float x, float z, float r, float g, float b) {
    drawCube(x, 1.0f, z, 0.7f, 1.2f, 0.4f, r, g, b);
    drawCube(x, 1.9f, z, 0.45f, 0.45f, 0.45f, 1.0f, 0.8f, 0.6f);
}

void drawSimpleCar(float x, float z) {
    drawCube(x, 0.5f, z, 2.2f, 0.7f, 1.2f, 0.18f, 0.24f, 0.70f);
    drawCube(x, 1.0f, z - 0.1f, 1.2f, 0.6f, 0.9f, 0.10f, 0.12f, 0.32f);
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
    drawCube(0.0f, 0.08f, -4.8f, 2.8f, 0.04f, 1.0f, 0.18f, 0.48f, 0.90f);  // information room portal
    drawCube(10.0f, 0.08f, -8.2f, 2.8f, 0.04f, 1.0f, 0.90f, 0.82f, 0.28f); // photocopy portal
    drawCube(-9.8f, 0.08f, -8.3f, 2.8f, 0.04f, 1.0f, 0.22f, 0.68f, 0.35f); // vehicle check portal
    drawCube(7.0f, 0.08f, -8.3f, 2.8f, 0.04f, 1.0f, 0.58f, 0.58f, 0.86f);  // verification portal
    drawCube(-9.8f, 0.08f, 8.4f, 2.8f, 0.04f, 1.0f, 0.86f, 0.58f, 0.28f);  // payment portal
    drawCube(-7.0f, 1.2f, -1.0f, 2.6f, 2.4f, 2.5f, 0.35f, 0.25f, 0.16f);
    drawCube(7.0f, 1.2f, -1.0f, 3.2f, 2.4f, 2.5f, 0.65f, 0.35f, 0.15f);
    drawCube(10.0f, 1.2f, -2.0f, 2.8f, 2.4f, 2.4f, 0.72f, 0.72f, 0.76f);
    drawCube(0.0f, 1.0f, 1.0f, 1.0f, 2.0f, 0.8f, 0.15f, 0.15f, 0.18f);

    drawCube(-10.0f, 0.05f, 5.0f, 5.0f, 0.02f, 10.0f, 0.32f, 0.32f, 0.35f);
    drawCube(10.0f, 0.05f, 5.0f, 5.0f, 0.02f, 10.0f, 0.32f, 0.32f, 0.35f);
    drawSimpleCar(-9.0f, 5.0f);
    drawSimpleCar(9.0f, 5.0f);

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
    drawCube(-4.0f, 0.45f, 1.2f, 1.2f, 0.9f, 1.2f, 0.45f, 0.45f, 0.50f);
    drawCube(0.0f, 0.45f, 1.2f, 1.2f, 0.9f, 1.2f, 0.45f, 0.45f, 0.50f);
    drawCube(4.0f, 0.45f, 1.2f, 1.2f, 0.9f, 1.2f, 0.45f, 0.45f, 0.50f);
    drawCube(5.0f, 1.0f, -3.0f, 1.4f, 2.0f, 1.4f, 0.36f, 0.32f, 0.28f);

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
    drawCube(-4.8f, 1.1f, -2.5f, 1.0f, 1.8f, 1.0f, 0.55f, 0.55f, 0.58f);
    drawCube(4.8f, 1.1f, -2.5f, 1.0f, 1.8f, 1.0f, 0.55f, 0.55f, 0.58f);
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
    drawDocumentPile(-2.2f, 1.15f, -2.7f, 0.90f, 0.90f, 0.92f);
    drawDocumentPile(2.0f, 1.15f, -2.8f, 0.86f, 0.86f, 0.88f);
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
    drawDocumentPile(-2.4f, 1.12f, -2.7f, 0.88f, 0.84f, 0.65f);
    drawDocumentPile(2.3f, 1.12f, -2.8f, 0.84f, 0.90f, 0.84f);
    drawCounterStamp(0.0f, -2.5f, 0.55f, 0.05f, 0.05f);
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
    drawCube(-1.8f, 1.18f, -2.7f, 0.8f, 0.45f, 0.8f, 0.14f, 0.14f, 0.16f);
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
    drawCounterStamp(-1.2f, -2.5f, 0.65f, 0.00f, 0.00f);
    drawCounterStamp(1.6f, -2.6f, 0.12f, 0.12f, 0.12f);
    drawDocumentPile(2.8f, 1.10f, -2.7f, 0.90f, 0.88f, 0.82f);
    drawCube(-4.8f, 1.7f, -4.4f, 1.2f, 3.4f, 0.9f, 0.48f, 0.42f, 0.34f);
    drawNPC3D(0.0f, -4.0f, 0.14f, 0.26f, 0.62f);
    drawPlayer3D(player.x, player.z);
}

void drawPhotocopyShop3D() {
    drawGround(16.0f, 0.78f, 0.72f, 0.58f);
    drawRoomFrame(8.0f, 7.0f, 4.0f, 0.72f, 0.72f, 0.74f);
    drawCube(0.0f, 0.08f, 6.1f, 3.0f, 0.04f, 0.8f, 0.90f, 0.82f, 0.28f);
    drawCube(-2.5f, 0.9f, -2.0f, 2.2f, 1.8f, 1.6f, 0.90f, 0.90f, 0.92f);
    drawCube(2.8f, 0.8f, -2.4f, 3.0f, 1.0f, 1.4f, 0.45f, 0.33f, 0.24f);
    drawCube(0.8f, 1.1f, -3.8f, 4.5f, 1.6f, 0.3f, 0.55f, 0.55f, 0.58f);
    drawNPC3D(4.8f, -2.0f, 0.55f, 0.55f, 0.55f);
    drawPlayer3D(player.x, player.z);
}

void drawVehicleCheck3D() {
    drawGround(20.0f, 0.40f, 0.48f, 0.40f);
    drawCube(0.0f, 0.05f, 0.0f, 7.5f, 0.02f, 12.0f, 0.22f, 0.22f, 0.24f);
    drawSimpleCar(0.0f, 1.5f);
    drawCube(4.5f, 0.8f, -2.5f, 2.0f, 1.0f, 1.2f, 0.45f, 0.34f, 0.24f);
    drawCube(-5.5f, 1.0f, -3.5f, 2.6f, 2.0f, 2.2f, 0.32f, 0.42f, 0.52f);
    drawNPC3D(4.5f, -1.5f, 0.15f, 0.55f, 0.25f);
    drawPlayer3D(player.x, player.z);
}

void drawPaymentQueue3D() {
    drawGround(18.0f, 0.48f, 0.48f, 0.50f);
    drawRoomFrame(8.0f, 8.0f, 4.5f, 0.66f, 0.66f, 0.70f);
    drawCube(0.0f, 0.08f, 6.6f, 3.0f, 0.04f, 0.8f, 0.86f, 0.58f, 0.28f);
    drawCube(0.0f, 0.08f, -6.6f, 3.0f, 0.04f, 0.8f, 0.86f, 0.58f, 0.28f);
    drawCube(0.0f, 0.85f, -4.0f, 5.5f, 1.0f, 1.2f, 0.44f, 0.34f, 0.24f);
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
    drawCube(0.0f, 0.8f, -2.8f, 4.0f, 1.0f, 1.2f, 0.48f, 0.34f, 0.22f);
    drawCube(0.0f, 1.05f, -1.0f, 0.4f, 0.02f, 0.3f, 0.9f, 0.1f, 0.1f);
    drawCube(4.5f, 1.2f, -3.8f, 1.5f, 2.4f, 1.0f, 0.55f, 0.42f, 0.26f);
    drawNPC3D(3.5f, -2.0f, 0.90f, 0.45f, 0.10f);
    drawPlayer3D(player.x, player.z);
}

void drawFinalCorridor3D() {
    drawGround(18.0f, 0.20f, 0.20f, 0.24f);
    drawCube(-4.5f, 2.0f, -1.0f, 0.5f, 4.0f, 16.0f, 0.32f, 0.32f, 0.36f);
    drawCube(4.5f, 2.0f, -1.0f, 0.5f, 4.0f, 16.0f, 0.32f, 0.32f, 0.36f);
    drawCube(0.0f, 3.8f, -1.0f, 9.5f, 0.4f, 16.0f, 0.26f, 0.26f, 0.30f);
    drawCube(0.0f, 1.2f, -8.0f, 4.0f, 2.4f, 0.3f, 0.75f, 0.70f, 0.45f);
    drawCube(-3.7f, 1.8f, -4.6f, 1.0f, 3.6f, 0.9f, 0.40f, 0.34f, 0.28f);
    drawCube(3.7f, 1.8f, -4.6f, 1.0f, 3.6f, 0.9f, 0.40f, 0.34f, 0.28f);
    drawCube(-3.5f, 3.3f, -1.5f, 0.5f, 0.3f, 0.5f, 0.85f, 0.82f, 0.60f);
    drawCube(3.5f, 3.3f, -1.5f, 0.5f, 0.3f, 0.5f, 0.85f, 0.82f, 0.60f);
    drawCube(-3.5f, 3.3f, -6.0f, 0.5f, 0.3f, 0.5f, 0.85f, 0.82f, 0.60f);
    drawCube(3.5f, 3.3f, -6.0f, 0.5f, 0.3f, 0.5f, 0.85f, 0.82f, 0.60f);
    drawCube(0.0f, 0.03f, -6.5f, 3.6f, 0.02f, 1.2f, 0.90f, 0.82f, 0.20f);
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
    drawCube(1.6f, 1.6f, -3.9f, 1.2f, 0.7f, 0.8f, 0.12f, 0.12f, 0.15f);
    drawCube(3.3f, 0.75f, -3.7f, 1.0f, 0.25f, 1.0f, 0.55f, 0.00f, 0.00f);
    drawCube(-2.5f, 1.15f, -3.8f, 1.0f, 0.4f, 0.8f, 0.72f, 0.72f, 0.76f);
    drawDocumentPile(-0.8f, 1.12f, -3.6f, 0.90f, 0.88f, 0.82f);
    drawDocumentPile(-5.9f, 0.8f, -2.8f, 0.84f, 0.84f, 0.82f);
    drawDocumentPile(5.8f, 0.8f, -2.8f, 0.84f, 0.84f, 0.82f);
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
    setColor(0.02f, 0.02f, 0.02f);
    drawRect(0, windowHeight - 48, static_cast<float>(windowWidth), 48.0f);

    std::stringstream ss;
    ss << "Kesabaran: " << patience
       << " | Stamina: " << stamina
       << " | Uang: " << money
       << " | Reputasi: " << reputation
       << " | Moral: " << moralScore
       << " | Jam: ";

    if (timeHour < 10) {
        ss << "0";
    }
    ss << timeHour << ":";
    if (timeMinute < 10) {
        ss << "0";
    }
    ss << timeMinute;

    setColor(1.0f, 1.0f, 1.0f);
    drawText(16.0f, windowHeight - 28.0f, ss.str(), GLUT_BITMAP_HELVETICA_18);
}

void drawDialogueBox() {
    if (!showDialogue) {
        return;
    }

    drawPanel(60.0f, 40.0f, static_cast<float>(windowWidth - 120), 135.0f);

    setColor(1.0f, 0.9f, 0.2f);
    drawText(85.0f, 145.0f, dialogueSpeaker, GLUT_BITMAP_HELVETICA_18);

    setColor(1.0f, 1.0f, 1.0f);
    drawText(85.0f, 110.0f, dialogueText, GLUT_BITMAP_HELVETICA_18);

    setColor(0.8f, 0.8f, 0.8f);
    drawText(85.0f, 72.0f, "Tekan SPACE untuk lanjut.", GLUT_BITMAP_8_BY_13);
}

void drawInventoryOverlay() {
    if (!showInventory) {
        return;
    }

    drawPanel(180.0f, 100.0f, 640.0f, 460.0f);
    setColor(1.0f, 0.9f, 0.2f);
    drawText(210.0f, 525.0f, "INVENTORY DOKUMEN", GLUT_BITMAP_HELVETICA_18);

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
    float y = 485.0f;
    for (const std::string& item : items) {
        drawText(220.0f, y, item, GLUT_BITMAP_HELVETICA_18);
        y -= 28.0f;
    }

    setColor(0.8f, 0.8f, 0.8f);
    drawText(220.0f, 125.0f, "Tekan I untuk menutup inventory.", GLUT_BITMAP_8_BY_13);
}

void drawQuestLogOverlay() {
    if (!showQuestLog) {
        return;
    }

    drawPanel(150.0f, 100.0f, 700.0f, 460.0f);
    setColor(1.0f, 0.9f, 0.2f);
    drawText(180.0f, 525.0f, "QUEST LOG", GLUT_BITMAP_HELVETICA_18);

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
    float y = 485.0f;
    for (const std::string& quest : quests) {
        drawText(180.0f, y, quest, GLUT_BITMAP_HELVETICA_18);
        y -= 32.0f;
    }

    setColor(0.8f, 0.8f, 0.8f);
    drawText(180.0f, 125.0f, "Tekan Q untuk menutup quest log.", GLUT_BITMAP_8_BY_13);
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
        drawPanel(180.0f, 240.0f, 640.0f, 170.0f);
        setColor(1.0f, 0.9f, 0.2f);
        drawCenteredText(375.0f, "KONTROL DASAR", GLUT_BITMAP_HELVETICA_18);
        setColor(1.0f, 1.0f, 1.0f);
        drawCenteredText(340.0f, "WASD / Arrow Keys: jalan mulus mengikuti kamera", GLUT_BITMAP_HELVETICA_18);
        drawCenteredText(310.0f, "Mouse kanan drag / M: lihat sekitar | E: interaksi", GLUT_BITMAP_HELVETICA_18);
        drawCenteredText(280.0f, "I: inventory | Q: quest log | R: reset kamera", GLUT_BITMAP_HELVETICA_18);
        setColor(0.8f, 0.8f, 0.8f);
        drawCenteredText(255.0f, "Tekan SPACE untuk mulai menjelajah area Samsat", GLUT_BITMAP_8_BY_13);
        return;
    }

    if (currentState == INVENTORY_CHECK) {
        drawPanel(170.0f, 210.0f, 660.0f, 220.0f);
        setColor(1.0f, 0.9f, 0.2f);
        drawCenteredText(395.0f, "CHECKLIST AWAL BERKAS", GLUT_BITMAP_HELVETICA_18);
        setColor(1.0f, 1.0f, 1.0f);
        drawCenteredText(360.0f, "Dokumen dasar sudah aman, tapi proses belum dimulai.", GLUT_BITMAP_HELVETICA_18);
        drawCenteredText(330.0f, "Cari nomor antrean, map yang benar, dan fotokopi yang valid.", GLUT_BITMAP_HELVETICA_18);
        drawCenteredText(300.0f, "Setelah itu kembali ke loket formulir untuk lanjut ke cek fisik.", GLUT_BITMAP_HELVETICA_18);
        setColor(0.8f, 0.8f, 0.8f);
        drawCenteredText(265.0f, "Tekan SPACE untuk kembali ke halaman depan", GLUT_BITMAP_8_BY_13);
    }
}

void drawInteractionPrompt() {
    const std::string prompt = getInteractionPrompt();
    if (prompt.empty()) {
        return;
    }

    drawPanel(240.0f, 185.0f, 520.0f, 44.0f);
    setColor(1.0f, 1.0f, 1.0f);
    drawCenteredText(202.0f, prompt, GLUT_BITMAP_HELVETICA_18);
}

void drawSceneHeader() {
    const std::string title = getSceneTitle();
    if (title.empty()) {
        return;
    }

    drawPanel(16.0f, 16.0f, 235.0f, 52.0f);
    setColor(1.0f, 0.9f, 0.2f);
    drawText(28.0f, 45.0f, title, GLUT_BITMAP_HELVETICA_18);
}

void drawSceneObjective() {
    const std::string objective = getSceneObjective();
    if (objective.empty()) {
        return;
    }

    drawPanel(270.0f, 16.0f, 470.0f, 52.0f);
    setColor(1.0f, 1.0f, 1.0f);
    drawText(286.0f, 45.0f, objective, GLUT_BITMAP_8_BY_13);
}

void drawNotificationToast() {
    if (notificationTimer <= 0.0f || notificationText.empty()) {
        return;
    }

    const float w = 360.0f;
    const float h = 42.0f;
    const float x = (windowWidth - w) * 0.5f;
    const float y = static_cast<float>(windowHeight) - 92.0f;
    drawPanel(x, y, w, h);
    setColor(1.0f, 0.86f, 0.35f);
    drawCenteredText(y + 16.0f, notificationText, GLUT_BITMAP_HELVETICA_18);
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
    drawCenteredText(430.0f, "SAMSAT QUEST", GLUT_BITMAP_TIMES_ROMAN_24);

    setColor(1.0f, 1.0f, 1.0f);
    drawCenteredText(395.0f, "STNK of Destiny", GLUT_BITMAP_HELVETICA_18);
    drawCenteredText(350.0f, "Prototype 3D OpenGL GLUT", GLUT_BITMAP_HELVETICA_18);
    drawCenteredText(230.0f, "Tekan ENTER untuk mulai", GLUT_BITMAP_HELVETICA_18);
    drawCenteredText(200.0f, "Tekan ESC untuk keluar", GLUT_BITMAP_8_BY_13);
}

void drawOpeningNarration() {
    setColor(0.08f, 0.08f, 0.10f);
    drawRect(0.0f, 0.0f, static_cast<float>(windowWidth), static_cast<float>(windowHeight));

    setColor(1.0f, 0.9f, 0.2f);
    drawCenteredText(500.0f, "Opening", GLUT_BITMAP_TIMES_ROMAN_24);

    setColor(1.0f, 1.0f, 1.0f);
    if (openingLine == 0) {
        drawCenteredText(410.0f, "Di sebuah pagi yang tampak biasa,", GLUT_BITMAP_HELVETICA_18);
        drawCenteredText(380.0f, "seorang warga menyadari satu kenyataan besar:", GLUT_BITMAP_HELVETICA_18);
        drawCenteredText(350.0f, "STNK harus segera diurus.", GLUT_BITMAP_HELVETICA_18);
    } else if (openingLine == 1) {
        drawCenteredText(410.0f, "Tujuannya tampak sederhana:", GLUT_BITMAP_HELVETICA_18);
        drawCenteredText(380.0f, "datang, menyerahkan berkas, lalu pulang.", GLUT_BITMAP_HELVETICA_18);
        drawCenteredText(350.0f, "Namun birokrasi punya questline sendiri.", GLUT_BITMAP_HELVETICA_18);
    } else {
        drawCenteredText(410.0f, "Hari itu, Samsat bukan sekadar kantor.", GLUT_BITMAP_HELVETICA_18);
        drawCenteredText(380.0f, "Ia adalah dungeon administratif.", GLUT_BITMAP_HELVETICA_18);
        drawCenteredText(350.0f, "Dan STNK adalah artefak takdir.", GLUT_BITMAP_HELVETICA_18);
    }

    setColor(0.8f, 0.8f, 0.8f);
    drawCenteredText(220.0f, "Tekan SPACE untuk lanjut", GLUT_BITMAP_8_BY_13);
}

void drawEndingScreen(const std::string& title, const std::string& line1, const std::string& line2) {
    setColor(0.04f, 0.04f, 0.08f);
    drawRect(0.0f, 0.0f, static_cast<float>(windowWidth), static_cast<float>(windowHeight));

    setColor(1.0f, 0.9f, 0.2f);
    drawCenteredText(470.0f, title, GLUT_BITMAP_TIMES_ROMAN_24);

    setColor(1.0f, 1.0f, 1.0f);
    drawCenteredText(380.0f, line1, GLUT_BITMAP_HELVETICA_18);
    drawCenteredText(345.0f, line2, GLUT_BITMAP_HELVETICA_18);
    drawCenteredText(300.0f, getEndingRouteLine(), GLUT_BITMAP_HELVETICA_18);

    setColor(0.82f, 0.82f, 0.86f);
    drawCenteredText(265.0f, getEndingStatsLine(), GLUT_BITMAP_8_BY_13);

    setColor(0.8f, 0.8f, 0.8f);
    drawCenteredText(230.0f, "Tekan SPACE untuk menuju credit.", GLUT_BITMAP_8_BY_13);
}

void drawCreditScene() {
    setColor(0.02f, 0.02f, 0.05f);
    drawRect(0.0f, 0.0f, static_cast<float>(windowWidth), static_cast<float>(windowHeight));

    setColor(1.0f, 0.9f, 0.2f);
    drawCenteredText(470.0f, "SAMSAT QUEST", GLUT_BITMAP_TIMES_ROMAN_24);

    setColor(1.0f, 1.0f, 1.0f);
    drawCenteredText(410.0f, "Prototype OpenGL GLUT", GLUT_BITMAP_HELVETICA_18);
    drawCenteredText(370.0f, "Terima kasih sudah mengurus STNK.", GLUT_BITMAP_HELVETICA_18);
    drawCenteredText(330.0f, "Post-credit: Mohon fotokopi rangkap tiga.", GLUT_BITMAP_HELVETICA_18);

    setColor(0.8f, 0.8f, 0.8f);
    drawCenteredText(230.0f, "Tekan ENTER untuk kembali ke title screen.", GLUT_BITMAP_8_BY_13);
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
