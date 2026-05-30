#include "game_internal.hpp"

namespace samsat {

int windowWidth = 1000;
int windowHeight = 700;

GameState currentState = TITLE_SCREEN;
GameState previousState = TITLE_SCREEN;

Player player = {0.0f, 0.0f, 8.0f, 0.65f, 0.0f, false, 0.0f};

int patience = 100;
int stamina = 100;
int money = 50000;
int timeHour = 8;
int timeMinute = 0;

bool hasQueueNumber = false;
bool hasCorrectMap = false;
bool hasFilledForm = false;
bool hasValidPhotocopy = false;
bool hasPhysicalCheckProof = false;
bool hasVerificationStamp = false;
bool hasPaymentProof = false;
bool hasStampedDocument = false;
bool hasFinalSTNK = false;

bool hasQueuedPaymentLine = false;

bool showInventory = false;
bool showQuestLog = false;
bool showDebug = false;
bool showDialogue = false;
bool pendingGiveUpConfirm = false;
bool keyForward = false;
bool keyBackward = false;
bool keyLeft = false;
bool keyRight = false;
bool specialForward = false;
bool specialBackward = false;
bool specialLeft = false;
bool specialRight = false;
int lastUpdateTimeMs = 0;
float interactionPulse = 0.0f;
float notificationTimer = 0.0f;
std::string notificationText;

std::string dialogueSpeaker;
std::string dialogueText;
int openingLine = 0;
bool hasSeenTutorial = false;
bool hasCheckedInventoryScene = false;
bool metSecurityGuard = false;
bool metInformationOfficer = false;
bool receivedStampRequirement = false;

Camera camera = {0.0f, -18.0f, 12.0f, 8.0f, 1.5f, false, false, false, 500, 350};

}  // namespace samsat
