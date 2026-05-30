#include "game.hpp"
#include "game_internal.hpp"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#include <cstdio>
#include <cstring>
#include <filesystem>
#include <vector>

using namespace samsat;

namespace {

std::string resolveAssetPath(const std::string& relativePath) {
    namespace fs = std::filesystem;
    const std::vector<fs::path> candidates = {
        fs::path(relativePath),
        fs::current_path() / relativePath,
        fs::current_path() / ".." / relativePath,
        fs::current_path() / "src" / relativePath,
        fs::current_path() / ".." / "src" / relativePath
    };

    for (const auto& candidate : candidates) {
        if (fs::exists(candidate)) {
            return candidate.string();
        }
    }
    return relativePath;
}

bool loadBmpTexture(const char* filePath, GLuint& textureId) {
    FILE* file = std::fopen(filePath, "rb");
    if (!file) {
        return false;
    }

    unsigned char header[54];
    if (std::fread(header, 1, 54, file) != 54) {
        std::fclose(file);
        return false;
    }

    if (header[0] != 'B' || header[1] != 'M') {
        std::fclose(file);
        return false;
    }

    int dataPos = *reinterpret_cast<int*>(&header[0x0A]);
    const int width = *reinterpret_cast<int*>(&header[0x12]);
    const int height = *reinterpret_cast<int*>(&header[0x16]);
    const unsigned short bitsPerPixel = *reinterpret_cast<unsigned short*>(&header[0x1C]);
    const int compression = *reinterpret_cast<int*>(&header[0x1E]);

    if ((bitsPerPixel != 24 && bitsPerPixel != 32) || compression != 0 || width <= 0 || height == 0) {
        std::fclose(file);
        return false;
    }

    const int absHeight = std::abs(height);
    if (dataPos <= 0) {
        dataPos = 54;
    }
    const int bytesPerPixel = bitsPerPixel / 8;
    const int paddedRowSize = ((width * bytesPerPixel + 3) / 4) * 4;
    const int dataSize = paddedRowSize * absHeight;

    std::vector<unsigned char> rawData(dataSize);
    std::fseek(file, dataPos, SEEK_SET);
    if (std::fread(rawData.data(), 1, dataSize, file) != static_cast<size_t>(dataSize)) {
        std::fclose(file);
        return false;
    }
    std::fclose(file);

    std::vector<unsigned char> rgbData(width * absHeight * 3);
    const bool bottomUp = height > 0;
    for (int row = 0; row < absHeight; ++row) {
        const int srcRow = bottomUp ? (absHeight - 1 - row) : row;
        const unsigned char* src = rawData.data() + (srcRow * paddedRowSize);
        unsigned char* dst = rgbData.data() + (row * width * 3);
        for (int x = 0; x < width; ++x) {
            dst[x * 3 + 0] = src[x * bytesPerPixel + 2];
            dst[x * 3 + 1] = src[x * bytesPerPixel + 1];
            dst[x * 3 + 2] = src[x * bytesPerPixel + 0];
        }
    }

    glGenTextures(1, &textureId);
    glBindTexture(GL_TEXTURE_2D, textureId);
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, absHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, rgbData.data());
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    return true;
}

bool loadTextureFromImage(const char* filePath, GLuint& textureId) {
    int width = 0;
    int height = 0;
    int channels = 0;
    stbi_set_flip_vertically_on_load(false);
    unsigned char* imageData = stbi_load(filePath, &width, &height, &channels, STBI_rgb);
    if (!imageData || width <= 0 || height <= 0) {
        if (imageData) {
            stbi_image_free(imageData);
        }
        return false;
    }

    glGenTextures(1, &textureId);
    glBindTexture(GL_TEXTURE_2D, textureId);
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, imageData);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    stbi_image_free(imageData);
    return true;
}

void drawSkyCube(GLuint textureId, float halfSize = 25.0f) {
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, textureId);
    glColor3f(1.0f, 1.0f, 1.0f);

    glDepthMask(GL_FALSE);
    glDisable(GL_DEPTH_TEST);
    glDisable(GL_LIGHTING);
    glDisable(GL_CULL_FACE);

    glPushMatrix();
    glRotatef(-camera.yaw, 0.0f, 1.0f, 0.0f);
    glRotatef(-camera.pitch, 1.0f, 0.0f, 0.0f);

    const auto skyTexCoord = [](float u, float v) {
        glTexCoord2f(1.0f - v, u);
    };

    glBegin(GL_QUADS);
    // Back face
    skyTexCoord(0.0f, 0.0f); glVertex3f(-halfSize, -halfSize, -halfSize);
    skyTexCoord(1.0f, 0.0f); glVertex3f(halfSize, -halfSize, -halfSize);
    skyTexCoord(1.0f, 1.0f); glVertex3f(halfSize, halfSize, -halfSize);
    skyTexCoord(0.0f, 1.0f); glVertex3f(-halfSize, halfSize, -halfSize);

    // Front face
    skyTexCoord(0.0f, 0.0f); glVertex3f(halfSize, -halfSize, halfSize);
    skyTexCoord(1.0f, 0.0f); glVertex3f(-halfSize, -halfSize, halfSize);
    skyTexCoord(1.0f, 1.0f); glVertex3f(-halfSize, halfSize, halfSize);
    skyTexCoord(0.0f, 1.0f); glVertex3f(halfSize, halfSize, halfSize);

    // Left face
    skyTexCoord(0.0f, 0.0f); glVertex3f(-halfSize, -halfSize, halfSize);
    skyTexCoord(1.0f, 0.0f); glVertex3f(-halfSize, -halfSize, -halfSize);
    skyTexCoord(1.0f, 1.0f); glVertex3f(-halfSize, halfSize, -halfSize);
    skyTexCoord(0.0f, 1.0f); glVertex3f(-halfSize, halfSize, halfSize);

    // Right face
    skyTexCoord(0.0f, 0.0f); glVertex3f(halfSize, -halfSize, -halfSize);
    skyTexCoord(1.0f, 0.0f); glVertex3f(halfSize, -halfSize, halfSize);
    skyTexCoord(1.0f, 1.0f); glVertex3f(halfSize, halfSize, halfSize);
    skyTexCoord(0.0f, 1.0f); glVertex3f(halfSize, halfSize, -halfSize);

    // Bottom face
    skyTexCoord(0.0f, 0.0f); glVertex3f(-halfSize, -halfSize, halfSize);
    skyTexCoord(1.0f, 0.0f); glVertex3f(halfSize, -halfSize, halfSize);
    skyTexCoord(1.0f, 1.0f); glVertex3f(halfSize, -halfSize, -halfSize);
    skyTexCoord(0.0f, 1.0f); glVertex3f(-halfSize, -halfSize, -halfSize);

    // Top face
    skyTexCoord(0.0f, 0.0f); glVertex3f(-halfSize, halfSize, -halfSize);
    skyTexCoord(1.0f, 0.0f); glVertex3f(halfSize, halfSize, -halfSize);
    skyTexCoord(1.0f, 1.0f); glVertex3f(halfSize, halfSize, halfSize);
    skyTexCoord(0.0f, 1.0f); glVertex3f(-halfSize, halfSize, halfSize);
    glEnd();

    glPopMatrix();
    glBindTexture(GL_TEXTURE_2D, 0);
    glDisable(GL_TEXTURE_2D);
    glEnable(GL_CULL_FACE);
    glEnable(GL_LIGHTING);
    glEnable(GL_DEPTH_TEST);
    glDepthMask(GL_TRUE);
}

// void drawSkyBackground(GLuint textureId) {
//     glMatrixMode(GL_PROJECTION);
//     glPushMatrix();
//     glLoadIdentity();
//     gluPerspective(60.0f, static_cast<float>(windowWidth) / static_cast<float>(windowHeight), 0.1f, 100.0f);

//     glMatrixMode(GL_MODELVIEW);
//     glPushMatrix();
//     glLoadIdentity();

//     if (textureId == 0) {
//         glDisable(GL_DEPTH_TEST);
//         glDisable(GL_LIGHTING);

//         glBegin(GL_QUADS);
//         glColor3f(0.42f, 0.72f, 0.94f);
//         glVertex3f(-1.0f, -1.0f, -1.0f);
//         glVertex3f(1.0f, -1.0f, -1.0f);
//         glVertex3f(1.0f, 1.0f, -1.0f);
//         glVertex3f(-1.0f, 1.0f, -1.0f);
//         glEnd();

//         glEnable(GL_LIGHTING);
//         glEnable(GL_DEPTH_TEST);
//     } else {
//         drawSkyCube(textureId, 30.0f);
//     }

//     glMatrixMode(GL_MODELVIEW);
//     glPopMatrix();
//     glMatrixMode(GL_PROJECTION);
//     glPopMatrix();
//     glMatrixMode(GL_MODELVIEW);
// }

void drawSkyBackground(GLuint) {
    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();
    gluPerspective(60.0f, static_cast<float>(windowWidth) / static_cast<float>(windowHeight), 0.1f, 100.0f);

    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glLoadIdentity();

    glDisable(GL_DEPTH_TEST);
    glDisable(GL_LIGHTING);

    glBegin(GL_QUADS);
    glColor3f(0.42f, 0.72f, 0.94f); 
    glVertex3f(-1.0f, -1.0f, -1.0f);
    glVertex3f(1.0f, -1.0f, -1.0f);
    glVertex3f(1.0f, 1.0f, -1.0f);
    glVertex3f(-1.0f, 1.0f, -1.0f);
    glEnd();

    glEnable(GL_LIGHTING);
    glEnable(GL_DEPTH_TEST);

    glMatrixMode(GL_MODELVIEW);
    glPopMatrix();
    glMatrixMode(GL_PROJECTION);
    glPopMatrix();
    glMatrixMode(GL_MODELVIEW);
}

} // namespace

namespace samsat {
GLuint skyTextureId = 0;
GLuint rockTextureId = 0;
GLuint tileTextureId = 0;
GLuint wallTextureId = 0;
GLuint texCityFront = 0;
GLuint texCityBack = 0;
GLuint texCityRight = 0;
GLuint texCityLeft = 0;
GLuint texCityTop = 0;
} // namespace samsat

void samsat::drawCityBoundary(float size, float height) {
    glEnable(GL_TEXTURE_2D);
    glColor3f(1.0f, 1.0f, 1.0f);
    glDisable(GL_LIGHTING);
    
    glPushMatrix();

    // SISI DEPAN (Z = -size)
    if (texCityBack != 0) { 
        glBindTexture(GL_TEXTURE_2D, texCityBack);
        glBegin(GL_QUADS);
        glTexCoord2f(0.0f, 1.0f); glVertex3f(-size, 0.0f, -size);
        glTexCoord2f(1.0f, 1.0f); glVertex3f( size, 0.0f, -size);
        glTexCoord2f(1.0f, 0.0f); glVertex3f( size, height, -size);
        glTexCoord2f(0.0f, 0.0f); glVertex3f(-size, height, -size);
        glEnd();
    }

    // SISI BELAKANG (Z = size)
    if (texCityBack != 0) {
        glBindTexture(GL_TEXTURE_2D, texCityBack);
        glBegin(GL_QUADS);
        glTexCoord2f(0.0f, 1.0f); glVertex3f( size, 0.0f,  size);
        glTexCoord2f(1.0f, 1.0f); glVertex3f(-size, 0.0f,  size);
        glTexCoord2f(1.0f, 0.0f); glVertex3f(-size, height, size);
        glTexCoord2f(0.0f, 0.0f); glVertex3f( size, height, size);
        glEnd();
    }

    // SISI KANAN (X = size)
    if (texCityRight != 0) {
        glBindTexture(GL_TEXTURE_2D, texCityRight);
        glBegin(GL_QUADS);
        glTexCoord2f(0.0f, 1.0f); glVertex3f( size, 0.0f, -size);
        glTexCoord2f(1.0f, 1.0f); glVertex3f( size, 0.0f,  size);
        glTexCoord2f(1.0f, 0.0f); glVertex3f( size, height, size);
        glTexCoord2f(0.0f, 0.0f); glVertex3f( size, height,-size);
        glEnd();
    }

    // SISI KIRI (X = -size)
    if (texCityRight != 0) {
        glBindTexture(GL_TEXTURE_2D, texCityRight);
        glBegin(GL_QUADS);
        glTexCoord2f(0.0f, 1.0f); glVertex3f(-size, 0.0f,  size);
        glTexCoord2f(1.0f, 1.0f); glVertex3f(-size, 0.0f, -size);
        glTexCoord2f(1.0f, 0.0f); glVertex3f(-size, height,-size);
        glTexCoord2f(0.0f, 0.0f); glVertex3f(-size, height, size);
        glEnd();
    }

    // SISI ATAS / LANGIT (Y = height)
    if (texCityTop != 0) {
        glBindTexture(GL_TEXTURE_2D, texCityTop);
        glBegin(GL_QUADS);
        glTexCoord2f(0.0f, 1.0f); glVertex3f(-size, height, -size);
        glTexCoord2f(1.0f, 1.0f); glVertex3f( size, height, -size);
        glTexCoord2f(1.0f, 0.0f); glVertex3f( size, height,  size);
        glTexCoord2f(0.0f, 0.0f); glVertex3f(-size, height,  size);
        glEnd();
    }

    glPopMatrix();
    glBindTexture(GL_TEXTURE_2D, 0);
    glDisable(GL_TEXTURE_2D);
    glEnable(GL_LIGHTING);
}

void initGame() {
    glClearColor(0.05f, 0.05f, 0.08f, 1.0f);
    glEnable(GL_DEPTH_TEST);
    glShadeModel(GL_SMOOTH);
    setupLighting();

    const std::string skyPath = resolveAssetPath("assets/skyblue.jpg");
    if (!loadTextureFromImage(skyPath.c_str(), skyTextureId)) {
        std::cerr << "Warning: failed to load sky texture from " << skyPath << std::endl;
    }

    const std::string rockPath = resolveAssetPath("assets/rock.jpg");
    if (!loadTextureFromImage(rockPath.c_str(), rockTextureId)) {
        std::cerr << "Warning: failed to load rock texture from " << rockPath << std::endl;
    }

    const std::string tilePath = resolveAssetPath("assets/tile.jpg");
    if (!loadTextureFromImage(tilePath.c_str(), tileTextureId)) {
        std::cerr << "Warning: failed to load tile texture from " << tilePath << std::endl;
    }

    const std::string wallPath = resolveAssetPath("assets/office_wall_generated.png");
    if (!loadTextureFromImage(wallPath.c_str(), wallTextureId)) {
        std::cerr << "Warning: failed to load wall texture from " << wallPath << std::endl;
    }

    if (!loadTextureFromImage(resolveAssetPath("assets/city_front.png").c_str(), texCityFront)) {
        std::cerr << "Warning: failed to load city_front.png" << std::endl;
    }
    if (!loadTextureFromImage(resolveAssetPath("assets/city_back.png").c_str(), texCityBack)) {
        std::cerr << "Warning: failed to load city_back.png" << std::endl;
    }
    if (!loadTextureFromImage(resolveAssetPath("assets/city_right.png").c_str(), texCityRight)) {
        std::cerr << "Warning: failed to load city_right.png" << std::endl;
    }
    if (!loadTextureFromImage(resolveAssetPath("assets/city_left.png").c_str(), texCityLeft)) {
        std::cerr << "Warning: failed to load city_left.png" << std::endl;
    }
    if (!loadTextureFromImage(resolveAssetPath("assets/city_top.png").c_str(), texCityTop)) {
        std::cerr << "Warning: failed to load city_top.png" << std::endl;
    }

    currentState = TITLE_SCREEN;
    previousState = TITLE_SCREEN;

    patience = 100;
    stamina = 100;
    money = 50000;
    timeHour = 8;
    timeMinute = 0;

    hasQueueNumber = false;
    hasCorrectMap = false;
    hasFilledForm = false;
    hasValidPhotocopy = false;
    hasPhysicalCheckProof = false;
    hasVerificationStamp = false;
    hasPaymentProof = false;
    hasStampedDocument = false;
    hasFinalSTNK = false;
    hasQueuedPaymentLine = false;

    showInventory = false;
    showQuestLog = false;
    showDebug = false;
    showDialogue = false;
    pendingGiveUpConfirm = false;
    keyForward = keyBackward = keyLeft = keyRight = false;
    specialForward = specialBackward = specialLeft = specialRight = false;
    lastUpdateTimeMs = glutGet(GLUT_ELAPSED_TIME);
    notificationTimer = 0.0f;
    notificationText.clear();

    dialogueSpeaker.clear();
    dialogueText.clear();
    openingLine = 0;
    hasSeenTutorial = false;
    hasCheckedInventoryScene = false;
    metSecurityGuard = false;
    metInformationOfficer = false;
    receivedStampRequirement = false;

    player.speed = 0.65f;
    resetPlayerForState(SAMSAT_EXTERIOR);
}

void display() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    drawSkyBackground(skyTextureId);

    if (!isOverlayOnlyState(currentState)) {
        glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();
        setupCamera();
        updateSceneLights();
        renderCurrent3DState();
    }

    begin2DOverlay();
    if (isOverlayOnlyState(currentState)) {
        drawOverlayOnlyState();
    } else {
        drawCommonOverlay();
    }
    end2DOverlay();

    glutSwapBuffers();
}

void reshape(int width, int height) {
    windowWidth = width;
    windowHeight = std::max(1, height);

    glViewport(0, 0, windowWidth, windowHeight);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(60.0f, static_cast<float>(windowWidth) / static_cast<float>(windowHeight), 0.1f, 100.0f);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
}

void keyboard(unsigned char key, int, int) {
    switch (key) {
        case 27:
            if (pendingGiveUpConfirm) {
                pendingGiveUpConfirm = false;
                showDialogue = false;
            } else if (showInventory) {
                showInventory = false;
            } else if (showQuestLog) {
                showQuestLog = false;
            } else if (handleBackNavigation()) {
                showDialogue = false;
            } else if (currentState == CREDIT_SCENE || isEndingState(currentState)) {
                initGame();
            } else {
                std::exit(0);
            }
            break;

        case 13:
            if (currentState == TITLE_SCREEN) {
                changeState(OPENING_NARRATION);
            } else if (currentState == CREDIT_SCENE) {
                initGame();
            }
            break;

        case ' ':
            progressWithSpace();
            break;

        case 'e':
        case 'E':
            interactCurrentScene();
            break;

        case 'i':
        case 'I':
            showInventory = !showInventory;
            showQuestLog = false;
            break;

        case 'q':
        case 'Q':
            showQuestLog = !showQuestLog;
            showInventory = false;
            break;

        case 'g':
        case 'G':
            if (currentState != TITLE_SCREEN &&
                currentState != OPENING_NARRATION &&
                currentState != CREDIT_SCENE &&
                !isEndingState(currentState)) {
                if (pendingGiveUpConfirm) {
                    pendingGiveUpConfirm = false;
                    patience = 0;
                    changeState(ENDING_GIVE_UP);
                } else {
                    pendingGiveUpConfirm = true;
                    setDialogue("Sistem", "Yakin pulang tanpa STNK? Tekan G sekali lagi untuk menyerah, atau SPACE untuk batal.");
                }
            }
            break;

        case 'w': case 'W':
        case 's': case 'S':
        case 'a': case 'A':
        case 'd': case 'D':
            setKeyState(key, true);
            break;

        case 'm':
        case 'M':
            camera.mouseLookEnabled = !camera.mouseLookEnabled;
            camera.dragging = false;
            if (camera.mouseLookEnabled) {
                centerMousePointer();
                glutSetCursor(GLUT_CURSOR_NONE);
            } else {
                glutSetCursor(GLUT_CURSOR_INHERIT);
            }
            break;

        case 'r':
        case 'R':
            resetCamera();
            glutSetCursor(GLUT_CURSOR_INHERIT);
            break;

        case '+':
        case '=':
            zoomCamera(-1.0f);
            break;

        case '-':
        case '_':
            zoomCamera(1.0f);
            break;

        case '[':
            adjustCameraHeight(-0.8f);
            break;

        case ']':
            adjustCameraHeight(0.8f);
            break;

        default:
            break;
    }

    if ((patience <= 0 || timeHour >= 15) &&
        currentState != ENDING_GIVE_UP &&
        currentState != CREDIT_SCENE &&
        currentState != TITLE_SCREEN) {
        changeState(ENDING_GIVE_UP);
    }

    glutPostRedisplay();
}

void keyboardUp(unsigned char key, int, int) {
    setKeyState(key, false);
}

void specialInput(int key, int, int) {
    switch (key) {
        case GLUT_KEY_UP:
        case GLUT_KEY_DOWN:
        case GLUT_KEY_LEFT:
        case GLUT_KEY_RIGHT:
            setSpecialKeyState(key, true);
            break;

        case GLUT_KEY_F1:
            showDebug = !showDebug;
            break;

        case GLUT_KEY_F2:
            patience = 100;
            stamina = 100;
            break;

        case GLUT_KEY_F3:
            money += 50000;
            break;

        case GLUT_KEY_F4:
            patience = 100;
            stamina = 100;
            money = std::max(money, 50000);
            timeHour = 9;
            timeMinute = 0;
            hasQueueNumber = true;
            hasCorrectMap = true;
            hasValidPhotocopy = true;
            hasFilledForm = true;
            hasPhysicalCheckProof = true;
            hasVerificationStamp = true;
            hasQueuedPaymentLine = true;
            hasPaymentProof = true;
            hasStampedDocument = true;
            hasFinalSTNK = false;
            changeState(FINAL_COUNTER_BOSS);
            break;

        case GLUT_KEY_F5:
            hasFinalSTNK = true;
            changeState(ENDING_CLEAN_SUCCESS);
            break;

        case GLUT_KEY_F6:
            patience = 0;
            changeState(ENDING_GIVE_UP);
            break;

        default:
            break;
    }

    glutPostRedisplay();
}

void specialInputUp(int key, int, int) {
    setSpecialKeyState(key, false);
}

void mouseButton(int button, int state, int x, int y) {
    if (button == GLUT_RIGHT_BUTTON) {
        camera.dragging = (state == GLUT_DOWN);
        camera.lastMouseX = x;
        camera.lastMouseY = y;
    }

    // FreeGLUT reports wheel up/down as buttons 3/4 on many Windows builds.
    if (state == GLUT_DOWN) {
        if (button == 3) {
            zoomCamera(-1.0f);
        } else if (button == 4) {
            zoomCamera(1.0f);
        }
    }

    glutPostRedisplay();
}

void mouseMotion(int x, int y) {
    handleMouseLook(x, y, false);
    glutPostRedisplay();
}

void passiveMouseMotion(int x, int y) {
    handleMouseLook(x, y, false);
    glutPostRedisplay();
}

void update(int value) {
    (void)value;
    const int now = glutGet(GLUT_ELAPSED_TIME);
    float deltaSeconds = (lastUpdateTimeMs > 0) ? (now - lastUpdateTimeMs) / 1000.0f : 0.016f;
    lastUpdateTimeMs = now;
    deltaSeconds = clampFloat(deltaSeconds, 0.0f, 0.05f);

    updateContinuousMovement(deltaSeconds);

    glutPostRedisplay();
    glutTimerFunc(16, update, 0);
}
