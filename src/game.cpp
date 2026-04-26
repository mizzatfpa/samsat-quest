#include "../include/game.hpp"

#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif

#include <algorithm>
#include <cmath>
#include <cstdlib>
#include <sstream>
#include <string>
#include <vector>

namespace {

constexpr float kPi = 3.1415926535f;

int windowWidth = 1000;
int windowHeight = 700;

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
};

GameState currentState = TITLE_SCREEN;
GameState previousState = TITLE_SCREEN;

Player player = {0.0f, 0.0f, 8.0f, 0.65f};

int patience = 100;
int stamina = 100;
int money = 50000;
int reputation = 0;
int moralScore = 100;
int timeHour = 8;
int timeMinute = 0;

bool hasQueueNumber = false;
bool hasCorrectMap = false;
bool hasWrongMap = false;
bool hasForm = false;
bool hasFilledForm = false;
bool hasValidPhotocopy = false;
bool hasPhysicalCheckProof = false;
bool hasVerificationStamp = false;
bool hasPaymentProof = false;
bool hasStampedDocument = false;
bool hasFinalSTNK = false;

bool usedInsider = false;
bool helpedNPCs = false;
bool systemFixed = false;

bool showInventory = false;
bool showQuestLog = false;
bool showDebug = false;
bool showDialogue = false;

std::string dialogueSpeaker;
std::string dialogueText;
int openingLine = 0;
bool hasSeenTutorial = false;
bool hasCheckedInventoryScene = false;

float cameraYaw = 0.0f;

int clampInt(int value, int minValue, int maxValue) {
    return std::max(minValue, std::min(value, maxValue));
}

float clampFloat(float value, float minValue, float maxValue) {
    return std::max(minValue, std::min(value, maxValue));
}

void resetPlayerForState(GameState state) {
    cameraYaw = 0.0f;

    switch (state) {
        case SAMSAT_EXTERIOR:
        case TUTORIAL_CONTROL:
        case DIALOG_SECURITY_GUARD:
        case QUEUE_MACHINE:
        case MAP_VENDOR:
        case PHOTOCOPY_SHOP:
            player.x = 0.0f;
            player.y = 0.0f;
            player.z = 8.0f;
            break;

        case INFORMATION_ROOM:
        case DIALOG_INFORMATION_OFFICER:
        case INVENTORY_CHECK:
        case FORM_COUNTER:
            player.x = 0.0f;
            player.y = 0.0f;
            player.z = 6.0f;
            break;

        case VEHICLE_CHECK_AREA:
            player.x = 0.0f;
            player.y = 0.0f;
            player.z = 7.0f;
            break;

        case PAYMENT_QUEUE:
        case PAYMENT_COUNTER:
        case VALIDATION_COUNTER:
        case STAMP_QUEST:
        case VALIDATION_SUCCESS:
            player.x = 0.0f;
            player.y = 0.0f;
            player.z = 6.0f;
            break;

        case FINAL_CORRIDOR:
        case FINAL_COUNTER_BOSS:
            player.x = 0.0f;
            player.y = 0.0f;
            player.z = 7.0f;
            break;

        default:
            break;
    }
}

void changeState(GameState nextState) {
    previousState = currentState;
    currentState = nextState;
    showDialogue = false;
    showInventory = false;
    showQuestLog = false;

    resetPlayerForState(nextState);
}

GameState getVerificationFallbackState() {
    if (!hasFilledForm) {
        return FORM_COUNTER;
    }
    if (!hasPhysicalCheckProof) {
        return VEHICLE_CHECK_AREA;
    }
    if (!hasValidPhotocopy || !hasCorrectMap) {
        return SAMSAT_EXTERIOR;
    }
    return VERIFICATION_COUNTER;
}

void addTime(int minutes) {
    timeMinute += minutes;

    while (timeMinute >= 60) {
        timeMinute -= 60;
        timeHour += 1;
    }

    if (timeHour >= 15) {
        patience = 0;
        currentState = ENDING_GIVE_UP;
    }
}

std::string boolText(bool value) {
    return value ? "YES" : "NO";
}

std::string stateToString(GameState state) {
    switch (state) {
        case TITLE_SCREEN: return "TITLE_SCREEN";
        case OPENING_NARRATION: return "OPENING_NARRATION";
        case SAMSAT_EXTERIOR: return "SAMSAT_EXTERIOR";
        case TUTORIAL_CONTROL: return "TUTORIAL_CONTROL";
        case DIALOG_SECURITY_GUARD: return "DIALOG_SECURITY_GUARD";
        case INFORMATION_ROOM: return "INFORMATION_ROOM";
        case DIALOG_INFORMATION_OFFICER: return "DIALOG_INFORMATION_OFFICER";
        case INVENTORY_CHECK: return "INVENTORY_CHECK";
        case QUEUE_MACHINE: return "QUEUE_MACHINE";
        case MAP_VENDOR: return "MAP_VENDOR";
        case PHOTOCOPY_SHOP: return "PHOTOCOPY_SHOP";
        case FORM_COUNTER: return "FORM_COUNTER";
        case VEHICLE_CHECK_AREA: return "VEHICLE_CHECK_AREA";
        case VERIFICATION_COUNTER: return "VERIFICATION_COUNTER";
        case PAYMENT_QUEUE: return "PAYMENT_QUEUE";
        case PAYMENT_COUNTER: return "PAYMENT_COUNTER";
        case VALIDATION_COUNTER: return "VALIDATION_COUNTER";
        case STAMP_QUEST: return "STAMP_QUEST";
        case VALIDATION_SUCCESS: return "VALIDATION_SUCCESS";
        case FINAL_CORRIDOR: return "FINAL_CORRIDOR";
        case FINAL_COUNTER_BOSS: return "FINAL_COUNTER_BOSS";
        case ENDING_CLEAN_SUCCESS: return "ENDING_CLEAN_SUCCESS";
        case ENDING_FAST_SUCCESS: return "ENDING_FAST_SUCCESS";
        case ENDING_SAMSAT_LEGEND: return "ENDING_SAMSAT_LEGEND";
        case ENDING_GIVE_UP: return "ENDING_GIVE_UP";
        case ENDING_MAP_REVOLUTION: return "ENDING_MAP_REVOLUTION";
        case CREDIT_SCENE: return "CREDIT_SCENE";
        default: return "UNKNOWN";
    }
}

bool isOverlayOnlyState(GameState state) {
    return state == TITLE_SCREEN ||
           state == OPENING_NARRATION ||
           state == ENDING_CLEAN_SUCCESS ||
           state == ENDING_FAST_SUCCESS ||
           state == ENDING_SAMSAT_LEGEND ||
           state == ENDING_GIVE_UP ||
           state == ENDING_MAP_REVOLUTION ||
           state == CREDIT_SCENE;
}

bool isWalkable3DState(GameState state) {
    switch (state) {
        case SAMSAT_EXTERIOR:
        case TUTORIAL_CONTROL:
        case INFORMATION_ROOM:
        case FORM_COUNTER:
        case VEHICLE_CHECK_AREA:
        case VERIFICATION_COUNTER:
        case PAYMENT_QUEUE:
        case PAYMENT_COUNTER:
        case VALIDATION_COUNTER:
        case STAMP_QUEST:
        case VALIDATION_SUCCESS:
        case FINAL_CORRIDOR:
        case FINAL_COUNTER_BOSS:
            return true;
        default:
            return false;
    }
}

void setDialogue(const std::string& speaker, const std::string& text) {
    dialogueSpeaker = speaker;
    dialogueText = text;
    showDialogue = true;
}

void startSecurityDialogue() {
    setDialogue(
        "Satpam Filosofis",
        "Mau urus STNK? Tanyakan dulu ke ruang informasi. Di sini arah selalu datang setelah bingung."
    );
}

void startInformationDialogue() {
    reputation = clampInt(reputation + 5, 0, 100);
    setDialogue(
        "Petugas Informasi",
        "Urutannya nomor antrean, map, fotokopi, formulir, cek fisik, pembayaran, validasi, lalu meterai."
    );
}

bool hasAllFinalRequirements() {
    return hasQueueNumber &&
           hasCorrectMap &&
           hasFilledForm &&
           hasValidPhotocopy &&
           hasPhysicalCheckProof &&
           hasVerificationStamp &&
           hasPaymentProof &&
           hasStampedDocument;
}

GameState determineEnding() {
    if (patience <= 0 || timeHour >= 15) {
        return ENDING_GIVE_UP;
    }

    if (usedInsider) {
        hasFinalSTNK = true;
        return ENDING_FAST_SUCCESS;
    }

    if (hasFinalSTNK && reputation >= 80 && moralScore >= 90 && systemFixed) {
        return ENDING_MAP_REVOLUTION;
    }

    if (hasFinalSTNK && reputation >= 60 && helpedNPCs) {
        return ENDING_SAMSAT_LEGEND;
    }

    if (hasFinalSTNK) {
        return ENDING_CLEAN_SUCCESS;
    }

    return ENDING_GIVE_UP;
}

void processQueueMachine() {
    if (!hasQueueNumber) {
        hasQueueNumber = true;
        reputation = clampInt(reputation + 3, 0, 100);
        addTime(10);
        setDialogue("Mesin Antrean", "Nomor B-047 keluar. Kertas kecil, konsekuensi panjang.");
        return;
    }

    setDialogue("Mesin Antrean", "Nomor antrean Anda masih sah. Sistem belum lupa, hanya lambat.");
}

void processMapVendor() {
    if (hasCorrectMap) {
        setDialogue("Penjual Map", "Map sudah ada. Jangan menimbun harapan warga lain.");
        return;
    }

    if (money >= 3000) {
        money -= 3000;
        hasCorrectMap = true;
        hasWrongMap = false;
        setDialogue("Penjual Map", "Ini map biru yang benar. Untuk hari ini, warna masih menentukan takdir.");
        return;
    }

    patience = clampInt(patience - 10, 0, 100);
    setDialogue("Penjual Map", "Uangnya kurang. Bahkan map pun menuntut kepastian.");
}

void processPhotocopyShop() {
    if (hasValidPhotocopy) {
        setDialogue("Tukang Fotokopi", "Fotokopi sudah jelas. Toner hari ini sedang berbaik hati.");
        return;
    }

    if (money >= 5000) {
        money -= 5000;
        hasValidPhotocopy = true;
        addTime(15);
        setDialogue("Tukang Fotokopi", "Fotokopi selesai. Tidak buram, jadi jelas terasa langka.");
        return;
    }

    patience = clampInt(patience - 10, 0, 100);
    setDialogue("Tukang Fotokopi", "Uang kurang. Mesin ini cuma paham kertas dan nominal.");
}

void processFormCounter() {
    if (hasQueueNumber && hasCorrectMap && hasValidPhotocopy) {
        hasForm = true;
        hasFilledForm = true;
        addTime(20);
        setDialogue("Petugas Formulir", "Formulir selesai. Tulisan Anda cukup rapi untuk dipercaya.");
        return;
    }

    patience = clampInt(patience - 10, 0, 100);
    setDialogue("Petugas Formulir", "Syarat belum lengkap. Formulir hanya datang pada map dan fotokopi yang sah.");
}

void processVehicleCheck() {
    if (hasFilledForm) {
        hasPhysicalCheckProof = true;
        stamina = clampInt(stamina - 10, 0, 100);
        addTime(20);
        setDialogue("Petugas Cek Fisik", "Nomor mesin ditemukan. Ia bersembunyi, tapi prosedur lebih sabar.");
        return;
    }

    patience = clampInt(patience - 10, 0, 100);
    setDialogue("Petugas Cek Fisik", "Formulir dulu. Kendaraan juga butuh konteks administratif.");
}

void processVerification() {
    if (hasFilledForm && hasValidPhotocopy && hasPhysicalCheckProof && hasCorrectMap) {
        hasVerificationStamp = true;
        addTime(20);
        setDialogue("Petugas Verifikasi", "Berkas lengkap. Susunannya bahkan tampak sudah menyerah.");
        return;
    }

    patience = clampInt(patience - 10, 0, 100);
    setDialogue("Petugas Verifikasi", "Masih ada syarat yang hilang. Biasanya yang paling penting.");
}

void processPaymentQueue() {
    if (hasVerificationStamp) {
        patience = clampInt(patience - 5, 0, 100);
        stamina = clampInt(stamina - 10, 0, 100);
        addTime(30);
        setDialogue("Antrean", "Antrean maju sedikit. Secara emosional, itu sudah pencapaian.");
        return;
    }

    patience = clampInt(patience - 10, 0, 100);
    setDialogue("Antrean", "Anda belum diverifikasi. Bahkan antrean punya standar.");
}

void processPaymentCounter() {
    if (!hasVerificationStamp) {
        patience = clampInt(patience - 10, 0, 100);
        setDialogue("Petugas Pembayaran", "Verifikasi dulu. Uang tidak bisa melompati urutan.");
        return;
    }

    if (hasPaymentProof) {
        setDialogue("Petugas Pembayaran", "Bukti bayar sudah ada. Membayar dua kali bukan speedrun.");
        return;
    }

    if (money >= 25000) {
        money -= 25000;
        hasPaymentProof = true;
        addTime(10);
        setDialogue("Petugas Pembayaran", "Pembayaran diterima. Dompet dan sistem sama-sama kehilangan energi.");
        return;
    }

    patience = clampInt(patience - 15, 0, 100);
    setDialogue("Petugas Pembayaran", "Saldo tidak cukup. Administrasi berhenti di nominal.");
}

void processValidationCounter() {
    if (hasPaymentProof && hasVerificationStamp) {
        setDialogue("Petugas Validasi", "Semua bagus. Tinggal satu hal kecil yang besar: meterai.");
        return;
    }

    patience = clampInt(patience - 10, 0, 100);
    setDialogue("Petugas Validasi", "Bukti bayar dan stempel verifikasi harus hadir bersama.");
}

void processStampQuest() {
    if (hasStampedDocument) {
        setDialogue("Penjual Warung", "Meterai sudah tertempel. Jangan digeser, nanti jadi debat nasional.");
        return;
    }

    if (money >= 10000) {
        money -= 10000;
        hasStampedDocument = true;
        helpedNPCs = true;
        reputation = clampInt(reputation + 10, 0, 100);
        addTime(15);
        setDialogue("Penjual Warung", "Meterai terpasang rapi. Cukup lurus untuk menenangkan prosedur.");
        return;
    }

    patience = clampInt(patience - 15, 0, 100);
    setDialogue("Penjual Warung", "Uang tidak cukup. Takdir tertahan di warung meterai.");
}

void processFinalBoss() {
    if (hasAllFinalRequirements()) {
        hasFinalSTNK = true;
        if (reputation >= 80 && moralScore >= 90) {
            systemFixed = true;
        }
        setDialogue("Petugas Loket Final", "Selamat. STNK selesai. Untuk hari ini, Anda lebih kuat dari sistem.");
        return;
    }

    patience = clampInt(patience - 20, 0, 100);
    setDialogue("Petugas Loket Final", "Berkas belum lengkap. Boss loket belum bisa ditaklukkan.");
}

float distanceSquared(float ax, float az, float bx, float bz) {
    const float dx = ax - bx;
    const float dz = az - bz;
    return dx * dx + dz * dz;
}

bool isNear(float px, float pz, float ox, float oz, float radius) {
    return distanceSquared(px, pz, ox, oz) <= radius * radius;
}

void clampPlayerToScene() {
    switch (currentState) {
        case SAMSAT_EXTERIOR:
        case TUTORIAL_CONTROL:
        case DIALOG_SECURITY_GUARD:
        case QUEUE_MACHINE:
        case MAP_VENDOR:
        case PHOTOCOPY_SHOP:
            player.x = clampFloat(player.x, -12.0f, 12.0f);
            player.z = clampFloat(player.z, -12.0f, 10.0f);
            break;

        case INFORMATION_ROOM:
        case DIALOG_INFORMATION_OFFICER:
        case INVENTORY_CHECK:
        case FORM_COUNTER:
            player.x = clampFloat(player.x, -8.0f, 8.0f);
            player.z = clampFloat(player.z, -6.0f, 7.0f);
            break;

        case VEHICLE_CHECK_AREA:
            player.x = clampFloat(player.x, -10.0f, 10.0f);
            player.z = clampFloat(player.z, -8.0f, 8.0f);
            break;

        case PAYMENT_QUEUE:
        case PAYMENT_COUNTER:
        case VALIDATION_COUNTER:
        case STAMP_QUEST:
        case VALIDATION_SUCCESS:
            player.x = clampFloat(player.x, -8.0f, 8.0f);
            player.z = clampFloat(player.z, -7.0f, 7.0f);
            break;

        case FINAL_CORRIDOR:
        case FINAL_COUNTER_BOSS:
            player.x = clampFloat(player.x, -6.0f, 6.0f);
            player.z = clampFloat(player.z, -9.0f, 8.0f);
            break;

        default:
            break;
    }
}

void movePlayer(float dx, float dz) {
    if (showDialogue || showInventory || showQuestLog) {
        return;
    }

    if (!isWalkable3DState(currentState)) {
        return;
    }

    player.x += dx;
    player.z += dz;
    player.y = 0.0f;

    if (std::fabs(dx) > 0.0f || std::fabs(dz) > 0.0f) {
        cameraYaw = std::atan2(dx, -dz) * 180.0f / kPi;
    }

    clampPlayerToScene();
}

std::string getInteractionPrompt() {
    if (currentState == SAMSAT_EXTERIOR || currentState == TUTORIAL_CONTROL) {
        if (currentState == TUTORIAL_CONTROL) {
            return "Tekan SPACE untuk melihat kontrol, lalu mulai eksplorasi";
        }
        if (isNear(player.x, player.z, -7.0f, 1.0f, 2.2f)) {
            return "Tekan E untuk bicara dengan Satpam";
        }
        if (isNear(player.x, player.z, 0.0f, 1.0f, 2.0f)) {
            return "Tekan E untuk ambil nomor antrean";
        }
        if (isNear(player.x, player.z, 7.0f, 1.0f, 2.2f)) {
            return "Tekan E untuk membeli map";
        }
        if (isNear(player.x, player.z, 10.0f, -2.0f, 2.2f)) {
            return "Tekan E untuk ke fotokopi";
        }
        if (player.x > -1.8f && player.x < 1.8f && player.z > -6.0f && player.z < -3.5f) {
            return "Tekan E untuk masuk ke ruang informasi";
        }
        return "";
    }

    if (currentState == INFORMATION_ROOM) {
        if (isNear(player.x, player.z, 0.0f, -4.0f, 2.2f)) {
            return "Tekan E untuk bicara dengan Petugas Informasi";
        }
        if (isNear(player.x, player.z, 5.0f, -3.0f, 2.2f)) {
            return "Tekan E untuk ke loket formulir";
        }
        if (player.z > 5.2f) {
            return "Tekan E untuk kembali ke halaman Samsat";
        }
        return "";
    }

    if (currentState == INVENTORY_CHECK) {
        return "Tekan SPACE untuk kembali dan mulai mengumpulkan berkas";
    }

    if (currentState == FORM_COUNTER) {
        if (isNear(player.x, player.z, 0.0f, -4.0f, 2.2f)) {
            return "Tekan E di loket formulir";
        }
        return "Dekati loket formulir untuk memproses berkas";
    }

    if (currentState == VEHICLE_CHECK_AREA) {
        if (isNear(player.x, player.z, 4.5f, -1.5f, 2.5f) || isNear(player.x, player.z, 0.0f, 1.5f, 2.5f)) {
            return "Tekan E untuk proses cek fisik kendaraan";
        }
        return "Dekati kendaraan atau petugas cek fisik";
    }

    if (currentState == VERIFICATION_COUNTER) {
        if (isNear(player.x, player.z, 0.0f, -4.0f, 2.2f)) {
            return "Tekan E di loket verifikasi";
        }
        return "Dekati loket verifikasi";
    }

    if (currentState == PAYMENT_QUEUE) {
        if (isNear(player.x, player.z, 0.0f, 2.5f, 3.6f)) {
            return "Tekan E untuk ikut antre pembayaran";
        }
        return "Dekati garis antrean pembayaran";
    }

    if (currentState == PAYMENT_COUNTER) {
        if (isNear(player.x, player.z, 0.0f, -4.0f, 2.2f)) {
            return "Tekan E di loket pembayaran";
        }
        return "Dekati loket pembayaran";
    }

    if (currentState == VALIDATION_COUNTER) {
        if (isNear(player.x, player.z, 0.0f, -4.0f, 2.2f)) {
            return "Tekan E di loket validasi";
        }
        return "Dekati loket validasi";
    }

    if (currentState == STAMP_QUEST) {
        if (isNear(player.x, player.z, 3.5f, -2.0f, 2.3f)) {
            return "Tekan E untuk membeli dan menempel meterai";
        }
        return "Dekati penjual meterai";
    }

    if (currentState == VALIDATION_SUCCESS) {
        return "Tekan SPACE untuk lanjut ke lorong final";
    }

    if (currentState == FINAL_CORRIDOR) {
        if (player.z < -6.0f) {
            return "Tekan E untuk menghadapi loket final";
        }
        return "Berjalan ke ujung lorong lalu tekan E";
    }

    if (currentState == FINAL_COUNTER_BOSS) {
        if (isNear(player.x, player.z, 0.0f, -5.5f, 2.3f)) {
            return "Tekan E di loket final";
        }
        return "Dekati boss loket final";
    }

    return "";
}

void interactCurrentScene() {
    if (currentState == SAMSAT_EXTERIOR || currentState == TUTORIAL_CONTROL) {
        if (isNear(player.x, player.z, -7.0f, 1.0f, 2.2f)) {
            changeState(DIALOG_SECURITY_GUARD);
            startSecurityDialogue();
            return;
        }

        if (isNear(player.x, player.z, 0.0f, 1.0f, 2.0f)) {
            changeState(QUEUE_MACHINE);
            processQueueMachine();
            return;
        }

        if (isNear(player.x, player.z, 7.0f, 1.0f, 2.2f)) {
            changeState(MAP_VENDOR);
            processMapVendor();
            return;
        }

        if (isNear(player.x, player.z, 10.0f, -2.0f, 2.2f)) {
            changeState(PHOTOCOPY_SHOP);
            processPhotocopyShop();
            return;
        }

        if (player.x > -1.8f && player.x < 1.8f && player.z > -6.0f && player.z < -3.5f) {
            changeState(INFORMATION_ROOM);
            return;
        }

        setDialogue("Sistem", "Tidak ada objek penting di dekat sini.");
        return;
    }

    if (currentState == INFORMATION_ROOM) {
        if (isNear(player.x, player.z, 0.0f, -4.0f, 2.2f)) {
            changeState(DIALOG_INFORMATION_OFFICER);
            startInformationDialogue();
            return;
        }

        if (isNear(player.x, player.z, 5.0f, -3.0f, 2.2f)) {
            changeState(FORM_COUNTER);
            return;
        }

        if (player.z > 5.2f) {
            changeState(SAMSAT_EXTERIOR);
            return;
        }

        setDialogue("Sistem", "Dekati petugas atau loket formulir.");
        return;
    }

    if (currentState == FORM_COUNTER) {
        if (isNear(player.x, player.z, 0.0f, -4.0f, 2.2f)) {
            processFormCounter();
            return;
        }

        setDialogue("Sistem", "Dekati loket formulir dulu.");
        return;
    }

    if (currentState == VEHICLE_CHECK_AREA) {
        if (isNear(player.x, player.z, 4.5f, -1.5f, 2.5f) || isNear(player.x, player.z, 0.0f, 1.5f, 2.5f)) {
            processVehicleCheck();
            return;
        }

        setDialogue("Sistem", "Dekati kendaraan atau petugas cek fisik.");
        return;
    }

    if (currentState == VERIFICATION_COUNTER) {
        if (isNear(player.x, player.z, 0.0f, -4.0f, 2.2f)) {
            processVerification();
            return;
        }

        setDialogue("Sistem", "Dekati loket verifikasi terlebih dahulu.");
        return;
    }

    if (currentState == PAYMENT_QUEUE) {
        if (isNear(player.x, player.z, 0.0f, 2.5f, 3.6f)) {
            processPaymentQueue();
            return;
        }

        setDialogue("Sistem", "Masuk dulu ke area antrean pembayaran.");
        return;
    }

    if (currentState == PAYMENT_COUNTER) {
        if (isNear(player.x, player.z, 0.0f, -4.0f, 2.2f)) {
            processPaymentCounter();
            return;
        }

        setDialogue("Sistem", "Dekati loket pembayaran terlebih dahulu.");
        return;
    }

    if (currentState == VALIDATION_COUNTER) {
        if (isNear(player.x, player.z, 0.0f, -4.0f, 2.2f)) {
            processValidationCounter();
            return;
        }

        setDialogue("Sistem", "Dekati loket validasi terlebih dahulu.");
        return;
    }

    if (currentState == STAMP_QUEST) {
        if (isNear(player.x, player.z, 3.5f, -2.0f, 2.3f)) {
            processStampQuest();
            return;
        }

        setDialogue("Sistem", "Dekati penjual meterai.");
        return;
    }

    if (currentState == FINAL_CORRIDOR) {
        if (player.z < -6.0f) {
            changeState(FINAL_COUNTER_BOSS);
            return;
        }

        setDialogue("Sistem", "Ujung lorong final masih di depan.");
        return;
    }

    if (currentState == FINAL_COUNTER_BOSS) {
        if (isNear(player.x, player.z, 0.0f, -5.5f, 2.3f)) {
            processFinalBoss();
            return;
        }

        setDialogue("Sistem", "Dekati loket final untuk menyerahkan semua berkas.");
        return;
    }
}

void progressWithSpace() {
    if (showInventory || showQuestLog) {
        return;
    }

    if (showDialogue) {
        showDialogue = false;

        if (currentState == DIALOG_SECURITY_GUARD) {
            changeState(INFORMATION_ROOM);
            return;
        }

        if (currentState == DIALOG_INFORMATION_OFFICER) {
            changeState(INVENTORY_CHECK);
            return;
        }

        if (currentState == QUEUE_MACHINE ||
            currentState == MAP_VENDOR ||
            currentState == PHOTOCOPY_SHOP) {
            changeState(SAMSAT_EXTERIOR);
            return;
        }

        if (currentState == FORM_COUNTER) {
            if (hasFilledForm) {
                changeState(VEHICLE_CHECK_AREA);
            } else {
                changeState(INFORMATION_ROOM);
            }
            return;
        }

        if (currentState == VEHICLE_CHECK_AREA) {
            if (hasPhysicalCheckProof) {
                changeState(VERIFICATION_COUNTER);
            } else {
                changeState(FORM_COUNTER);
            }
            return;
        }

        if (currentState == VERIFICATION_COUNTER) {
            if (hasVerificationStamp) {
                changeState(PAYMENT_QUEUE);
            } else {
                changeState(getVerificationFallbackState());
            }
            return;
        }

        if (currentState == PAYMENT_QUEUE) {
            if (hasVerificationStamp) {
                changeState(PAYMENT_COUNTER);
            } else {
                changeState(VERIFICATION_COUNTER);
            }
            return;
        }

        if (currentState == PAYMENT_COUNTER) {
            if (hasPaymentProof) {
                changeState(VALIDATION_COUNTER);
            }
            return;
        }

        if (currentState == VALIDATION_COUNTER) {
            if (hasPaymentProof && hasVerificationStamp) {
                changeState(STAMP_QUEST);
            } else {
                changeState(hasPaymentProof ? VERIFICATION_COUNTER : PAYMENT_COUNTER);
            }
            return;
        }

        if (currentState == STAMP_QUEST) {
            if (hasStampedDocument) {
                changeState(VALIDATION_SUCCESS);
            }
            return;
        }

        if (currentState == FINAL_COUNTER_BOSS) {
            if (hasFinalSTNK) {
                changeState(determineEnding());
            }
            return;
        }
    }

    switch (currentState) {
        case OPENING_NARRATION:
            openingLine += 1;
            if (openingLine > 2) {
                changeState(TUTORIAL_CONTROL);
            }
            break;

        case TUTORIAL_CONTROL:
            hasSeenTutorial = true;
            changeState(SAMSAT_EXTERIOR);
            break;

        case INVENTORY_CHECK:
            hasCheckedInventoryScene = true;
            changeState(SAMSAT_EXTERIOR);
            break;

        case FORM_COUNTER:
            break;

        case VEHICLE_CHECK_AREA:
            break;

        case VERIFICATION_COUNTER:
            break;

        case PAYMENT_QUEUE:
            break;

        case PAYMENT_COUNTER:
            break;

        case VALIDATION_COUNTER:
            break;

        case STAMP_QUEST:
            break;

        case VALIDATION_SUCCESS:
            changeState(FINAL_CORRIDOR);
            break;

        case FINAL_CORRIDOR:
            break;

        case FINAL_COUNTER_BOSS:
            break;

        case ENDING_CLEAN_SUCCESS:
        case ENDING_FAST_SUCCESS:
        case ENDING_SAMSAT_LEGEND:
        case ENDING_GIVE_UP:
        case ENDING_MAP_REVOLUTION:
            changeState(CREDIT_SCENE);
            break;

        default:
            break;
    }
}

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
}

void end2DOverlay() {
    glEnable(GL_DEPTH_TEST);

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
    setColor(0.08f, 0.08f, 0.10f);
    drawRect(x, y, w, h);
    setColor(1.0f, 1.0f, 1.0f);
    drawLine(x, y, x + w, y);
    drawLine(x + w, y, x + w, y + h);
    drawLine(x + w, y + h, x, y + h);
    drawLine(x, y + h, x, y);
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
    glVertex3f(-size, 0.0f, -size);
    glVertex3f(size, 0.0f, -size);
    glVertex3f(size, 0.0f, size);
    glVertex3f(-size, 0.0f, size);
    glEnd();
}

void drawPlayer3D(float x, float z) {
    drawCube(x, 1.0f, z, 0.7f, 1.2f, 0.4f, 0.10f, 0.20f, 0.90f);
    drawCube(x, 1.9f, z, 0.45f, 0.45f, 0.45f, 1.0f, 0.8f, 0.6f);
    drawCube(x - 0.2f, 0.35f, z, 0.25f, 0.7f, 0.25f, 0.1f, 0.1f, 0.1f);
    drawCube(x + 0.2f, 0.35f, z, 0.25f, 0.7f, 0.25f, 0.1f, 0.1f, 0.1f);
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

void setupCamera() {
    float camX = 0.0f;
    float camY = 8.0f;
    float camZ = 12.0f;
    float targetX = 0.0f;
    float targetY = 1.5f;
    float targetZ = 0.0f;

    switch (currentState) {
        case SAMSAT_EXTERIOR:
        case TUTORIAL_CONTROL:
        case DIALOG_SECURITY_GUARD:
        case QUEUE_MACHINE:
        case MAP_VENDOR:
        case PHOTOCOPY_SHOP:
            camX = player.x;
            camY = 8.0f;
            camZ = player.z + 12.0f;
            targetX = player.x;
            targetY = 1.6f;
            targetZ = player.z - 1.0f;
            break;

        case INFORMATION_ROOM:
        case DIALOG_INFORMATION_OFFICER:
        case INVENTORY_CHECK:
        case FORM_COUNTER:
            camX = 0.0f;
            camY = 8.0f;
            camZ = 12.0f;
            targetX = 0.0f;
            targetY = 1.3f;
            targetZ = -1.0f;
            break;

        case VEHICLE_CHECK_AREA:
            camX = 0.0f;
            camY = 9.0f;
            camZ = 14.0f;
            targetX = 0.0f;
            targetY = 1.3f;
            targetZ = 0.0f;
            break;

        case PAYMENT_QUEUE:
        case PAYMENT_COUNTER:
        case VALIDATION_COUNTER:
        case STAMP_QUEST:
        case VALIDATION_SUCCESS:
            camX = 0.0f;
            camY = 7.0f;
            camZ = 13.0f;
            targetX = 0.0f;
            targetY = 1.4f;
            targetZ = -1.0f;
            break;

        case FINAL_CORRIDOR:
        case FINAL_COUNTER_BOSS:
            camX = 0.0f;
            camY = 7.0f;
            camZ = 15.0f;
            targetX = 0.0f;
            targetY = 1.6f;
            targetZ = -3.0f;
            break;

        default:
            break;
    }

    gluLookAt(camX, camY, camZ, targetX, targetY, targetZ, 0.0f, 1.0f, 0.0f);
}

void drawSamsatExterior3D() {
    drawGround(30.0f);

    drawCube(0.0f, 2.5f, -10.0f, 14.0f, 5.0f, 4.0f, 0.58f, 0.58f, 0.62f);
    drawCube(0.0f, 1.3f, -7.7f, 2.0f, 2.6f, 0.2f, 0.10f, 0.10f, 0.15f);
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

void drawPhotocopyShop3D() {
    drawGround(16.0f, 0.78f, 0.72f, 0.58f);
    drawRoomFrame(8.0f, 7.0f, 4.0f, 0.72f, 0.72f, 0.74f);
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
    drawNPC3D(-2.5f, -2.0f, 0.55f, 0.42f, 0.70f);
    drawPlayer3D(player.x, player.z);
}

void drawFinalBoss3D() {
    drawGround(18.0f, 0.18f, 0.18f, 0.22f);
    drawRoomFrame(9.0f, 8.0f, 5.0f, 0.28f, 0.28f, 0.32f);
    drawCube(0.0f, 1.0f, -4.5f, 7.0f, 1.2f, 1.5f, 0.35f, 0.20f, 0.20f);
    drawCube(-6.0f, 2.0f, -4.0f, 1.5f, 4.0f, 2.0f, 0.38f, 0.32f, 0.26f);
    drawCube(6.0f, 2.0f, -4.0f, 1.5f, 4.0f, 2.0f, 0.38f, 0.32f, 0.26f);
    drawCube(1.8f, 1.5f, -3.9f, 1.0f, 0.6f, 0.7f, 0.12f, 0.12f, 0.15f);
    drawCube(3.2f, 0.7f, -3.7f, 0.9f, 0.2f, 0.9f, 0.55f, 0.00f, 0.00f);
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
            drawGenericCounterScene3D(0.20f, 0.36f, 0.78f);
            break;

        case VEHICLE_CHECK_AREA:
            drawVehicleCheck3D();
            break;

        case VERIFICATION_COUNTER:
            drawGenericCounterScene3D(0.32f, 0.42f, 0.52f);
            break;

        case PAYMENT_QUEUE:
            drawPaymentQueue3D();
            break;

        case PAYMENT_COUNTER:
            drawGenericCounterScene3D(0.58f, 0.38f, 0.16f);
            break;

        case VALIDATION_COUNTER:
            drawGenericCounterScene3D(0.62f, 0.10f, 0.10f);
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

    lines.push_back("Queue: " + boolText(hasQueueNumber) + " | Map: " + boolText(hasCorrectMap));
    lines.push_back("Photo: " + boolText(hasValidPhotocopy) + " | Form: " + boolText(hasFilledForm));
    lines.push_back("CekFisik: " + boolText(hasPhysicalCheckProof));
    lines.push_back("Verify: " + boolText(hasVerificationStamp) + " | Pay: " + boolText(hasPaymentProof));
    lines.push_back("Stamp: " + boolText(hasStampedDocument) + " | STNK: " + boolText(hasFinalSTNK));
    lines.push_back("Tutorial: " + boolText(hasSeenTutorial) + " | InventoryCheck: " + boolText(hasCheckedInventoryScene));
    lines.push_back("Insider: " + boolText(usedInsider) + " | HelpedNPC: " + boolText(helpedNPCs));
    lines.push_back("SystemFixed: " + boolText(systemFixed));

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

void drawStateInstructionPanel() {
    if (currentState == TUTORIAL_CONTROL) {
        drawPanel(180.0f, 240.0f, 640.0f, 170.0f);
        setColor(1.0f, 0.9f, 0.2f);
        drawCenteredText(375.0f, "KONTROL DASAR", GLUT_BITMAP_HELVETICA_18);
        setColor(1.0f, 1.0f, 1.0f);
        drawCenteredText(340.0f, "WASD / Arrow Keys: bergerak pada bidang X-Z", GLUT_BITMAP_HELVETICA_18);
        drawCenteredText(310.0f, "E: interaksi | I: inventory | Q: quest log", GLUT_BITMAP_HELVETICA_18);
        drawCenteredText(280.0f, "F1-F6: debug shortcut sesuai AGENTS.md", GLUT_BITMAP_HELVETICA_18);
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
    drawStateInstructionPanel();
    drawInteractionPrompt();
    drawDialogueBox();
    drawInventoryOverlay();
    drawQuestLogOverlay();
    drawDebugOverlay();
}

}  // namespace

void initGame() {
    glClearColor(0.05f, 0.05f, 0.08f, 1.0f);
    glEnable(GL_DEPTH_TEST);
    glShadeModel(GL_SMOOTH);

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

    usedInsider = false;
    helpedNPCs = false;
    systemFixed = false;

    showInventory = false;
    showQuestLog = false;
    showDebug = false;
    showDialogue = false;

    dialogueSpeaker.clear();
    dialogueText.clear();
    openingLine = 0;
    hasSeenTutorial = false;
    hasCheckedInventoryScene = false;

    player.speed = 0.65f;
    resetPlayerForState(SAMSAT_EXTERIOR);
}

void display() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    if (!isOverlayOnlyState(currentState)) {
        glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();
        setupCamera();
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

        case 'w':
        case 'W':
            movePlayer(0.0f, -player.speed);
            break;

        case 's':
        case 'S':
            movePlayer(0.0f, player.speed);
            break;

        case 'a':
        case 'A':
            movePlayer(-player.speed, 0.0f);
            break;

        case 'd':
        case 'D':
            movePlayer(player.speed, 0.0f);
            break;

        default:
            break;
    }

    if (patience <= 0 &&
        currentState != ENDING_GIVE_UP &&
        currentState != CREDIT_SCENE &&
        currentState != TITLE_SCREEN) {
        changeState(ENDING_GIVE_UP);
    }

    glutPostRedisplay();
}

void specialInput(int key, int, int) {
    switch (key) {
        case GLUT_KEY_UP:
            movePlayer(0.0f, -player.speed);
            break;

        case GLUT_KEY_DOWN:
            movePlayer(0.0f, player.speed);
            break;

        case GLUT_KEY_LEFT:
            movePlayer(-player.speed, 0.0f);
            break;

        case GLUT_KEY_RIGHT:
            movePlayer(player.speed, 0.0f);
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
            hasQueueNumber = true;
            hasCorrectMap = true;
            hasValidPhotocopy = true;
            hasForm = true;
            hasFilledForm = true;
            hasPhysicalCheckProof = true;
            hasVerificationStamp = true;
            hasPaymentProof = true;
            hasStampedDocument = true;
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

void update(int value) {
    glutPostRedisplay();
    glutTimerFunc(16, update, 0);
}
