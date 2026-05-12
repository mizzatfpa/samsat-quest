#include "game.hpp"
#include "game_internal.hpp"

using namespace samsat;

void initGame() {
    glClearColor(0.05f, 0.05f, 0.08f, 1.0f);
    glEnable(GL_DEPTH_TEST);
    glShadeModel(GL_SMOOTH);
    setupLighting();

    currentState = TITLE_SCREEN;
    previousState = TITLE_SCREEN;

    patience = 100;
    stamina = 100;
    money = 50000;
    reputation = 0;
    moralScore = 100;
    timeHour = 8;
    timeMinute = 0;

    hasQueueNumber = false;
    hasCorrectMap = false;
    hasWrongMap = false;
    hasForm = false;
    hasFilledForm = false;
    hasValidPhotocopy = false;
    hasPhysicalCheckProof = false;
    hasVerificationStamp = false;
    hasPaymentProof = false;
    hasStampedDocument = false;
    hasFinalSTNK = false;
    hasQueuedPaymentLine = false;

    usedInsider = false;
    helpedNPCs = false;
    systemFixed = false;

    showInventory = false;
    showQuestLog = false;
    showDebug = false;
    showDialogue = false;
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
    receivedCorridorAdvice = false;
    metSecurityGuard = false;
    metInformationOfficer = false;
    receivedStampRequirement = false;

    player.speed = 0.65f;
    resetPlayerForState(SAMSAT_EXTERIOR);
}

void display() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

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
            if (showInventory) {
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
            reputation = 55;
            moralScore = 100;
            timeHour = 9;
            timeMinute = 0;
            hasQueueNumber = true;
            hasCorrectMap = true;
            hasValidPhotocopy = true;
            hasForm = true;
            hasFilledForm = true;
            hasPhysicalCheckProof = true;
            hasVerificationStamp = true;
            hasQueuedPaymentLine = true;
            hasPaymentProof = true;
            hasStampedDocument = true;
            hasFinalSTNK = false;
            usedInsider = false;
            helpedNPCs = false;
            systemFixed = false;
            receivedCorridorAdvice = false;
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
