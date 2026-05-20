#ifndef GAME_INTERNAL_HPP
#define GAME_INTERNAL_HPP

#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif

#ifndef GL_CLAMP_TO_EDGE
#define GL_CLAMP_TO_EDGE 0x812F
#endif

#include <algorithm>
#include <cmath>
#include <cstdlib>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

namespace samsat {

constexpr float kPi = 3.1415926535f;

enum GameState {
    TITLE_SCREEN,
    OPENING_NARRATION,
    SAMSAT_EXTERIOR,
    TUTORIAL_CONTROL,
    DIALOG_SECURITY_GUARD,
    INFORMATION_ROOM,
    DIALOG_INFORMATION_OFFICER,
    INVENTORY_CHECK,
    QUEUE_MACHINE,
    MAP_VENDOR,
    PHOTOCOPY_SHOP,
    FORM_COUNTER,
    VEHICLE_CHECK_AREA,
    VERIFICATION_COUNTER,
    PAYMENT_QUEUE,
    PAYMENT_COUNTER,
    VALIDATION_COUNTER,
    STAMP_QUEST,
    VALIDATION_SUCCESS,
    FINAL_CORRIDOR,
    FINAL_COUNTER_BOSS,
    ENDING_CLEAN_SUCCESS,
    ENDING_FAST_SUCCESS,
    ENDING_SAMSAT_LEGEND,
    ENDING_GIVE_UP,
    ENDING_MAP_REVOLUTION,
    CREDIT_SCENE
};

struct Player {
    float x;
    float y;
    float z;
    float speed;
    float facingYaw;
};

struct Camera {
    float yaw;
    float pitch;
    float distance;
    float height;
    float targetHeight;
    bool mouseLookEnabled;
    bool dragging;
    bool ignoreNextWarp;
    int lastMouseX;
    int lastMouseY;
};

extern int windowWidth;
extern int windowHeight;
extern GameState currentState;
extern GameState previousState;
extern Player player;
extern Camera camera;
extern int patience;
extern int stamina;
extern int money;
extern int reputation;
extern int moralScore;
extern int timeHour;
extern int timeMinute;
extern bool hasQueueNumber;
extern bool hasCorrectMap;
extern bool hasWrongMap;
extern bool hasForm;
extern bool hasFilledForm;
extern bool hasValidPhotocopy;
extern bool hasPhysicalCheckProof;
extern bool hasVerificationStamp;
extern bool hasPaymentProof;
extern bool hasStampedDocument;
extern bool hasFinalSTNK;
extern bool usedInsider;
extern bool helpedNPCs;
extern bool systemFixed;
extern bool hasQueuedPaymentLine;
extern bool showInventory;
extern bool showQuestLog;
extern bool showDebug;
extern bool showDialogue;
extern bool keyForward;
extern bool keyBackward;
extern bool keyLeft;
extern bool keyRight;
extern bool specialForward;
extern bool specialBackward;
extern bool specialLeft;
extern bool specialRight;
extern int lastUpdateTimeMs;
extern float interactionPulse;
extern float notificationTimer;
extern std::string notificationText;
extern std::string dialogueSpeaker;
extern std::string dialogueText;
extern int openingLine;
extern bool hasSeenTutorial;
extern bool hasCheckedInventoryScene;
extern bool receivedCorridorAdvice;
extern bool metSecurityGuard;
extern bool metInformationOfficer;
extern bool receivedStampRequirement;

extern GLuint skyTextureId;
extern GLuint rockTextureId;
extern GLuint tileTextureId;
extern GLuint wallTextureId;

int clampInt(int value, int minValue, int maxValue);
float clampFloat(float value, float minValue, float maxValue);
void resetCamera();
void rotateCamera(float degrees);
void zoomCamera(float amount);
void adjustCameraHeight(float amount);
void movePlayerRelative(float forwardAmount, float strafeAmount);
void updateContinuousMovement(float deltaSeconds);
void setKeyState(unsigned char key, bool isDown);
void setSpecialKeyState(int key, bool isDown);
void playUiBeep();
void playSuccessBeep();
void playErrorBeep();
void showNotification(const std::string& message);
void handleMouseLook(int x, int y, bool forceRelative);
void centerMousePointer();
void resetPlayerForState(GameState state);
void changeState(GameState nextState);
void changeStateAt(GameState nextState, float x, float z, float facingYaw = 0.0f);
GameState getVerificationFallbackState();
void addTime(int minutes);
int minutesUntilClosing();
bool canQualifyForFastEnding();
bool canQualifyForLegendEnding();
bool canQualifyForRevolutionEnding();
bool handleBackNavigation();
std::string boolText(bool value);
std::string stateToString(GameState state);
bool isOverlayOnlyState(GameState state);
bool isEndingState(GameState state);
bool isWalkable3DState(GameState state);
void setDialogue(const std::string& speaker, const std::string& text);
void startSecurityDialogue();
void startInformationDialogue();
bool hasAllFinalRequirements();
GameState determineEnding();
std::string getProjectedEndingText();
std::string buildMissingRequirementText();
void processQueueMachine();
void processMapVendor();
void processPhotocopyShop();
void processFormCounter();
void processVehicleCheck();
void processVerification();
void processPaymentQueue();
void processPaymentCounter();
void processValidationCounter();
void processStampQuest();
void processCorridorAdvice();
void processInsiderOffer();
void processFinalBoss();
float distanceSquared(float ax, float az, float bx, float bz);
bool isNear(float px, float pz, float ox, float oz, float radius);
void clampPlayerToScene();
void movePlayer(float dx, float dz);
std::string getInteractionPrompt();
void interactCurrentScene();
void progressWithSpace();
void setColor(float r, float g, float b);
void begin2DOverlay();
void end2DOverlay();
void drawRect(float x, float y, float w, float h);
void drawLine(float x1, float y1, float x2, float y2);
float getTextWidth(const std::string& text, void* font);
void drawText(float x, float y, const std::string& text, void* font);
void drawCenteredText(float y, const std::string& text, void* font);
int drawWrappedText(float x, float y, const std::string& text, float maxWidth, float lineHeight, void* font, float r, float g, float b);
void drawPanel(float x, float y, float w, float h);
void drawCube(float x, float y, float z, float sx, float sy, float sz, float r, float g, float b);
void drawGround(float size, float r, float g, float b);
void drawPlayer3D(float x, float z);
void drawNPC3D(float x, float z, float r, float g, float b);
void drawSimpleCar(float x, float z);
void drawRoomFrame(float halfWidth, float halfDepth, float wallHeight, float r, float g, float b);
void drawQueueMarker(float x, float z);
void drawDocumentPile(float x, float y, float z, float r, float g, float b);
void drawCounterStamp(float x, float z, float r, float g, float b);
void setupLighting();
void updateSceneLights();
void setupCamera();
void drawSamsatExterior3D();
void drawInformationRoom3D();
void drawGenericCounterScene3D(float accentR, float accentG, float accentB, bool showPlayerCharacter);
void drawFormCounter3D();
void drawVerificationCounter3D();
void drawPaymentCounter3D();
void drawValidationCounter3D();
void drawPhotocopyShop3D();
void drawVehicleCheck3D();
void drawPaymentQueue3D();
void drawStampQuest3D();
void drawFinalCorridor3D();
void drawFinalBoss3D();
void renderCurrent3DState();
void drawResourceUI();
void drawDialogueBox();
void drawInventoryOverlay();
void drawQuestLogOverlay();
void drawDebugOverlay();
std::string getSceneTitle();
std::string getSceneObjective();
std::string getEndingStatsLine();
std::string getEndingRouteLine();
void drawStateInstructionPanel();
void drawInteractionPrompt();
void drawSceneHeader();
void drawSceneObjective();
void drawNotificationToast();
void drawCameraReticle();
void drawTitleScreen();
void drawOpeningNarration();
void drawEndingScreen(const std::string& title, const std::string& line1, const std::string& line2);
void drawCreditScene();
void drawOverlayOnlyState();
void drawCommonOverlay();

}  // namespace samsat

#endif
