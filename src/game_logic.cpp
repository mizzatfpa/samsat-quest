#include "game_internal.hpp"

#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#endif

namespace samsat {

int clampInt(int value, int minValue, int maxValue) {
    return std::max(minValue, std::min(value, maxValue));
}

float clampFloat(float value, float minValue, float maxValue) {
    return std::max(minValue, std::min(value, maxValue));
}


void playTone(int frequency, int durationMs) {
#ifdef _WIN32
    Beep(frequency, durationMs);
#else
    (void)frequency;
    (void)durationMs;
    std::cout << "\a" << std::flush;
#endif
}

void playUiBeep() {
    playTone(660, 45);
}

void playSuccessBeep() {
    playTone(784, 55);
    playTone(988, 70);
}

void playErrorBeep() {
    playTone(220, 90);
}

void showNotification(const std::string& message) {
    notificationText = message;
    notificationTimer = 2.4f;
}

void resetCamera() {
    camera.yaw = 0.0f;
    camera.pitch = -18.0f;
    camera.distance = 12.0f;
    camera.height = 8.0f;
    camera.targetHeight = 1.5f;
    camera.mouseLookEnabled = false;
    camera.dragging = false;
    camera.ignoreNextWarp = false;
    camera.lastMouseX = windowWidth / 2;
    camera.lastMouseY = windowHeight / 2;
}

void rotateCamera(float degrees) {
    camera.yaw += degrees;
    while (camera.yaw >= 360.0f) camera.yaw -= 360.0f;
    while (camera.yaw < 0.0f) camera.yaw += 360.0f;
}

void pitchCamera(float degrees) {
    camera.pitch = clampFloat(camera.pitch + degrees, -75.0f, 20.0f);
}

void zoomCamera(float amount) {
    camera.distance = clampFloat(camera.distance + amount, 5.0f, 24.0f);
}

void adjustCameraHeight(float amount) {
    camera.height = clampFloat(camera.height + amount, 3.5f, 16.0f);
}

void centerMousePointer() {
    camera.ignoreNextWarp = true;
    camera.lastMouseX = windowWidth / 2;
    camera.lastMouseY = windowHeight / 2;
    glutWarpPointer(camera.lastMouseX, camera.lastMouseY);
}

void handleMouseLook(int x, int y, bool forceRelative) {
    if (camera.ignoreNextWarp) {
        camera.ignoreNextWarp = false;
        return;
    }

    if (!forceRelative && !camera.dragging && !camera.mouseLookEnabled) {
        camera.lastMouseX = x;
        camera.lastMouseY = y;
        return;
    }

    int dx = x - camera.lastMouseX;
    int dy = y - camera.lastMouseY;

    if (camera.mouseLookEnabled) {
        dx = x - (windowWidth / 2);
        dy = y - (windowHeight / 2);
    }

    const float sensitivity = 0.16f;

    // Natural mouse-look: moving the mouse right turns the view/player right,
    // moving it up looks upward. The previous build felt reversed on yaw.
    rotateCamera(-dx * sensitivity);
    pitchCamera(dy * sensitivity);

    if (camera.mouseLookEnabled) {
        centerMousePointer();
    } else {
        camera.lastMouseX = x;
        camera.lastMouseY = y;
    }
}

void resetPlayerForState(GameState state) {
    resetCamera();

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

namespace {

enum SceneArea {
    AREA_OVERLAY,
    AREA_EXTERIOR,
    AREA_INFORMATION,
    AREA_PHOTOCOPY,
    AREA_FORM,
    AREA_VEHICLE_CHECK,
    AREA_VERIFICATION,
    AREA_PAYMENT_QUEUE,
    AREA_PAYMENT_COUNTER,
    AREA_VALIDATION,
    AREA_STAMP,
    AREA_VALIDATION_SUCCESS,
    AREA_FINAL_CORRIDOR,
    AREA_FINAL_BOSS
};

SceneArea getSceneArea(GameState state) {
    switch (state) {
        case SAMSAT_EXTERIOR:
        case TUTORIAL_CONTROL:
        case DIALOG_SECURITY_GUARD:
        case QUEUE_MACHINE:
        case MAP_VENDOR:
            return AREA_EXTERIOR;

        case INFORMATION_ROOM:
        case DIALOG_INFORMATION_OFFICER:
        case INVENTORY_CHECK:
            return AREA_INFORMATION;

        case PHOTOCOPY_SHOP:
            return AREA_PHOTOCOPY;
        case FORM_COUNTER:
            return AREA_FORM;
        case VEHICLE_CHECK_AREA:
            return AREA_VEHICLE_CHECK;
        case VERIFICATION_COUNTER:
            return AREA_VERIFICATION;
        case PAYMENT_QUEUE:
            return AREA_PAYMENT_QUEUE;
        case PAYMENT_COUNTER:
            return AREA_PAYMENT_COUNTER;
        case VALIDATION_COUNTER:
            return AREA_VALIDATION;
        case STAMP_QUEST:
            return AREA_STAMP;
        case VALIDATION_SUCCESS:
            return AREA_VALIDATION_SUCCESS;
        case FINAL_CORRIDOR:
            return AREA_FINAL_CORRIDOR;
        case FINAL_COUNTER_BOSS:
            return AREA_FINAL_BOSS;
        default:
            return AREA_OVERLAY;
    }
}

} // namespace

void changeState(GameState nextState) {
    const SceneArea oldArea = getSceneArea(currentState);
    const SceneArea newArea = getSceneArea(nextState);

    previousState = currentState;
    currentState = nextState;
    showDialogue = false;
    showInventory = false;
    showQuestLog = false;

    // Only reposition the player when they actually move to a different map area.
    // Talking to NPCs or finishing a task now keeps the player standing there.
    if (oldArea != newArea) {
        resetPlayerForState(nextState);
    } else {
        clampPlayerToScene();
    }

    playUiBeep();
    showNotification(getSceneTitle().empty() ? "Scene updated" : getSceneTitle());
}

void changeStateAt(GameState nextState, float x, float z, float facingYaw) {
    changeState(nextState);
    player.x = x;
    player.y = 0.0f;
    player.z = z;
    player.facingYaw = facingYaw;
    clampPlayerToScene();
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
    showNotification("+" + std::to_string(minutes) + " menit berlalu");
    timeMinute += minutes;

    while (timeMinute >= 60) {
        timeMinute -= 60;
        timeHour += 1;
    }

    if (timeHour >= 15) {
        timeHour = 15;
        timeMinute = 0;
        patience = 0;
        if (currentState != ENDING_GIVE_UP &&
            currentState != CREDIT_SCENE &&
            currentState != TITLE_SCREEN) {
            changeState(ENDING_GIVE_UP);
        }
    }
}

int minutesUntilClosing() {
    const int minutesLeft = ((15 - timeHour) * 60) - timeMinute;
    return std::max(0, minutesLeft);
}

bool canQualifyForFastEnding() {
    return usedInsider;
}

bool canQualifyForLegendEnding() {
    return helpedNPCs && reputation >= 60;
}

bool canQualifyForRevolutionEnding() {
    return !usedInsider &&
           reputation >= 80 &&
           moralScore >= 90 &&
           metSecurityGuard &&
           metInformationOfficer &&
           receivedCorridorAdvice;
}

bool handleBackNavigation() {
    switch (currentState) {
        case DIALOG_SECURITY_GUARD:
        case QUEUE_MACHINE:
        case MAP_VENDOR:
        case PHOTOCOPY_SHOP:
        case TUTORIAL_CONTROL:
            changeState(SAMSAT_EXTERIOR);
            return true;

        case INFORMATION_ROOM:
            changeState(SAMSAT_EXTERIOR);
            return true;

        case DIALOG_INFORMATION_OFFICER:
        case INVENTORY_CHECK:
            changeState(INFORMATION_ROOM);
            return true;

        case FORM_COUNTER:
            changeState(INFORMATION_ROOM);
            return true;

        case VEHICLE_CHECK_AREA:
            changeState(FORM_COUNTER);
            return true;

        case VERIFICATION_COUNTER:
            changeState(VEHICLE_CHECK_AREA);
            return true;

        case PAYMENT_QUEUE:
            changeState(VERIFICATION_COUNTER);
            return true;

        case PAYMENT_COUNTER:
            changeState(PAYMENT_QUEUE);
            return true;

        case VALIDATION_COUNTER:
            changeState(PAYMENT_COUNTER);
            return true;

        case STAMP_QUEST:
            changeState(VALIDATION_COUNTER);
            return true;

        case VALIDATION_SUCCESS:
            changeState(STAMP_QUEST);
            return true;

        case FINAL_CORRIDOR:
            changeState(VALIDATION_SUCCESS);
            return true;

        case FINAL_COUNTER_BOSS:
            changeState(FINAL_CORRIDOR);
            return true;

        default:
            return false;
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

bool isEndingState(GameState state) {
    return state == ENDING_CLEAN_SUCCESS ||
           state == ENDING_FAST_SUCCESS ||
           state == ENDING_SAMSAT_LEGEND ||
           state == ENDING_GIVE_UP ||
           state == ENDING_MAP_REVOLUTION;
}

bool isWalkable3DState(GameState state) {
    switch (state) {
        case SAMSAT_EXTERIOR:
        case TUTORIAL_CONTROL:
        case PHOTOCOPY_SHOP:
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
    if (!metSecurityGuard) {
        metSecurityGuard = true;
        reputation = clampInt(reputation + 5, 0, 100);
    }

    setDialogue(
        "Satpam Filosofis",
        "Mau urus STNK? Tanyakan dulu ke ruang informasi. Di sini arah selalu datang setelah bingung."
    );
}

void startInformationDialogue() {
    if (!metInformationOfficer) {
        metInformationOfficer = true;
        reputation = clampInt(reputation + 5, 0, 100);
    }

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

    if (usedInsider && hasFinalSTNK) {
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

std::string getProjectedEndingText() {
    if (!hasFinalSTNK) {
        return "N/A";
    }
    return stateToString(determineEnding());
}

std::string buildMissingRequirementText() {
    std::vector<std::string> missing;

    if (!hasQueueNumber) missing.push_back("nomor antrean");
    if (!hasCorrectMap) missing.push_back("map yang benar");
    if (!hasFilledForm) missing.push_back("formulir");
    if (!hasValidPhotocopy) missing.push_back("fotokopi valid");
    if (!hasPhysicalCheckProof) missing.push_back("bukti cek fisik");
    if (!hasVerificationStamp) missing.push_back("stempel verifikasi");
    if (!hasPaymentProof) missing.push_back("bukti pembayaran");
    if (!hasStampedDocument) missing.push_back("dokumen bermeterai");

    if (missing.empty()) {
        return "tidak ada";
    }

    std::ostringstream ss;
    for (std::size_t i = 0; i < missing.size(); ++i) {
        if (i > 0) {
            ss << (i + 1 == missing.size() ? " dan " : ", ");
        }
        ss << missing[i];
    }

    return ss.str();
}

void processQueueMachine() {
    if (!hasQueueNumber) {
        hasQueueNumber = true;
        playSuccessBeep();
        showNotification("Objective Complete: Nomor antrean didapat");
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
        playSuccessBeep();
        showNotification("Objective Complete: Map benar dibeli");
        hasWrongMap = false;
        reputation = clampInt(reputation + 5, 0, 100);
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
        playSuccessBeep();
        showNotification("Objective Complete: Fotokopi selesai");
        reputation = clampInt(reputation + 5, 0, 100);
        addTime(15);
        setDialogue("Tukang Fotokopi", "Fotokopi selesai. Tidak buram, jadi jelas terasa langka.");
        return;
    }

    patience = clampInt(patience - 10, 0, 100);
    setDialogue("Tukang Fotokopi", "Uang kurang. Mesin ini cuma paham kertas dan nominal.");
}

void processFormCounter() {
    if (hasFilledForm) {
        setDialogue("Petugas Formulir", "Formulir Anda sudah rapi. Tidak perlu menguji kesabaran loket lagi.");
        return;
    }

    if (hasQueueNumber && hasCorrectMap && hasValidPhotocopy) {
        hasForm = true;
        hasFilledForm = true;
        playSuccessBeep();
        showNotification("Objective Complete: Formulir lengkap");
        reputation = clampInt(reputation + 10, 0, 100);
        addTime(20);
        setDialogue("Petugas Formulir", "Formulir selesai. Tulisan Anda cukup rapi untuk dipercaya.");
        return;
    }

    patience = clampInt(patience - 10, 0, 100);
    setDialogue("Petugas Formulir", "Syarat belum lengkap. Formulir hanya datang pada map dan fotokopi yang sah.");
}

void processVehicleCheck() {
    if (hasPhysicalCheckProof) {
        setDialogue("Petugas Cek Fisik", "Bukti cek fisik sudah ada. Kendaraan Anda sudah resmi melewati tatapan curiga.");
        return;
    }

    if (hasFilledForm) {
        hasPhysicalCheckProof = true;
        playSuccessBeep();
        showNotification("Objective Complete: Cek fisik selesai");
        stamina = clampInt(stamina - 10, 0, 100);
        reputation = clampInt(reputation + 5, 0, 100);
        addTime(20);
        setDialogue("Petugas Cek Fisik", "Nomor mesin ditemukan. Ia bersembunyi, tapi prosedur lebih sabar.");
        return;
    }

    patience = clampInt(patience - 10, 0, 100);
    setDialogue("Petugas Cek Fisik", "Formulir dulu. Kendaraan juga butuh konteks administratif.");
}

void processVerification() {
    if (hasVerificationStamp) {
        setDialogue("Petugas Verifikasi", "Stempel sudah turun. Berkas Anda kini punya aura resmi.");
        return;
    }

    if (hasFilledForm && hasValidPhotocopy && hasPhysicalCheckProof && hasCorrectMap) {
        hasVerificationStamp = true;
        playSuccessBeep();
        showNotification("Objective Complete: Verifikasi selesai");
        reputation = clampInt(reputation + 10, 0, 100);
        addTime(20);
        setDialogue("Petugas Verifikasi", "Berkas lengkap. Susunannya bahkan tampak sudah menyerah.");
        return;
    }

    patience = clampInt(patience - 10, 0, 100);
    setDialogue("Petugas Verifikasi", "Masih ada syarat yang hilang. Biasanya yang paling penting.");
}

void processPaymentQueue() {
    if (!hasVerificationStamp) {
        patience = clampInt(patience - 10, 0, 100);
        setDialogue("Antrean", "Anda belum diverifikasi. Bahkan antrean punya standar.");
        return;
    }

    if (hasQueuedPaymentLine) {
        setDialogue("Antrean", "Anda sudah antre dengan benar. Tekan SPACE untuk lanjut ke loket pembayaran.");
        return;
    }

    hasQueuedPaymentLine = true;
        playSuccessBeep();
        showNotification("Objective Complete: Antrean pembayaran aman");
    patience = clampInt(patience - 5, 0, 100);
    stamina = clampInt(stamina - 10, 0, 100);
    addTime(30);
    setDialogue("Antrean", "Antrean maju sedikit. Secara emosional, itu sudah pencapaian.");
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
        playSuccessBeep();
        showNotification("Objective Complete: Pembayaran selesai");
        reputation = clampInt(reputation + 5, 0, 100);
        addTime(10);
        setDialogue("Petugas Pembayaran", "Pembayaran diterima. Dompet dan sistem sama-sama kehilangan energi.");
        return;
    }

    patience = clampInt(patience - 15, 0, 100);
    setDialogue("Petugas Pembayaran", "Saldo tidak cukup. Administrasi berhenti di nominal.");
}

void processValidationCounter() {
    if (hasPaymentProof && hasVerificationStamp) {
        if (!receivedStampRequirement && !hasStampedDocument) {
            receivedStampRequirement = true;
            playSuccessBeep();
            showNotification("Objective Complete: Validasi awal selesai");
            reputation = clampInt(reputation + 10, 0, 100);
        }
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
        playSuccessBeep();
        showNotification("Objective Complete: Meterai terpasang");
        reputation = clampInt(reputation + 10, 0, 100);
        addTime(15);
        setDialogue("Penjual Warung", "Meterai terpasang rapi. Cukup lurus untuk menenangkan prosedur.");
        return;
    }

    patience = clampInt(patience - 15, 0, 100);
    setDialogue("Penjual Warung", "Uang tidak cukup. Takdir tertahan di warung meterai.");
}

void processCorridorAdvice() {
    if (!receivedCorridorAdvice) {
        receivedCorridorAdvice = true;
        helpedNPCs = true;
        playSuccessBeep();
        showNotification("Objective Complete: Info final didapat");
        reputation = clampInt(reputation + 5, 0, 100);
        setDialogue("Antrean Senior", "Pastikan berkasmu rapi. Loket final menyukai urutan lebih dari manusia.");
        return;
    }

    setDialogue("Antrean Senior", "Kalau panik, cek lagi map, stempel, dan bukti bayar. Boss final hidup dari detail.");
}

void processInsiderOffer() {
    if (usedInsider) {
        setDialogue("Orang Dalam", "Jalur cepat sudah dibuka. Tinggal lihat apakah hati nuranimu ikut antre.");
        return;
    }

    usedInsider = true;
    playSuccessBeep();
    showNotification("Objective Complete: Jalur cepat terbuka");
    moralScore = clampInt(moralScore - 40, 0, 100);
    reputation = clampInt(reputation - 5, 0, 100);
    setDialogue("Orang Dalam", "Saya bisa bantu percepat. Berkas nanti dianggap 'sudah dipahami sistem'.");
}

void processFinalBoss() {
    if (hasFinalSTNK) {
        setDialogue("Petugas Loket Final", "STNK Anda sudah jadi. Yang tersisa sekarang hanya makna dari perjalanan ini.");
        return;
    }

    if (hasAllFinalRequirements()) {
        hasFinalSTNK = true;
        playSuccessBeep();
        showNotification("Objective Complete: STNK selesai!");
        reputation = clampInt(reputation + 5, 0, 100);
        if (reputation >= 80 &&
            moralScore >= 90 &&
            metSecurityGuard &&
            metInformationOfficer &&
            receivedCorridorAdvice) {
            systemFixed = true;
        }
        setDialogue("Petugas Loket Final", "Selamat. STNK selesai. Untuk hari ini, Anda lebih kuat dari sistem.");
        return;
    }

    if (usedInsider) {
        hasFinalSTNK = true;
        setDialogue("Petugas Loket Final", "Berkas Anda... diproses khusus. Cepat sekali. Mencurigakan, tapi cepat.");
        return;
    }

    patience = clampInt(patience - 20, 0, 100);
    setDialogue("Petugas Loket Final", "Masih kurang: " + buildMissingRequirementText() + ".");
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

    clampPlayerToScene();
}

void movePlayerRelative(float forwardAmount, float strafeAmount) {
    const float yawRadians = camera.yaw * kPi / 180.0f;

    // Match the actual camera look direction used by setupCamera().
    // At yaw 0, the camera looks toward -Z, so W walks into the screen.
    const float forwardX = -std::sin(yawRadians);
    const float forwardZ = -std::cos(yawRadians);
    const float rightX = std::cos(yawRadians);
    const float rightZ = -std::sin(yawRadians);

    const float dx = forwardX * forwardAmount + rightX * strafeAmount;
    const float dz = forwardZ * forwardAmount + rightZ * strafeAmount;

    if (std::fabs(dx) > 0.0001f || std::fabs(dz) > 0.0001f) {
        // Convert movement vector into the same yaw convention used for drawing.
        player.facingYaw = std::atan2(-dx, -dz) * 180.0f / kPi;
    }

    movePlayer(dx, dz);
}

void setKeyState(unsigned char key, bool isDown) {
    switch (key) {
        case 'w': case 'W': keyForward = isDown; break;
        case 's': case 'S': keyBackward = isDown; break;
        case 'a': case 'A': keyLeft = isDown; break;
        case 'd': case 'D': keyRight = isDown; break;
        default: break;
    }
}

void setSpecialKeyState(int key, bool isDown) {
    switch (key) {
        case GLUT_KEY_UP: specialForward = isDown; break;
        case GLUT_KEY_DOWN: specialBackward = isDown; break;
        case GLUT_KEY_LEFT: specialLeft = isDown; break;
        case GLUT_KEY_RIGHT: specialRight = isDown; break;
        default: break;
    }
}

void updateContinuousMovement(float deltaSeconds) {
    if (notificationTimer > 0.0f) {
        notificationTimer = std::max(0.0f, notificationTimer - deltaSeconds);
    }
    interactionPulse += deltaSeconds * 4.0f;

    float forward = 0.0f;
    float strafe = 0.0f;

    if (keyForward || specialForward) forward += 1.0f;
    if (keyBackward || specialBackward) forward -= 1.0f;
    if (keyRight || specialRight) strafe += 1.0f;
    if (keyLeft || specialLeft) strafe -= 1.0f;

    if (forward == 0.0f && strafe == 0.0f) {
        return;
    }

    const float length = std::sqrt(forward * forward + strafe * strafe);
    if (length > 1.0f) {
        forward /= length;
        strafe /= length;
    }

    const float unitsPerSecond = 5.0f;
    movePlayerRelative(forward * unitsPerSecond * deltaSeconds, strafe * unitsPerSecond * deltaSeconds);
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
        if (player.x > 8.0f && player.z < -7.0f) {
            return "Tekan E untuk masuk kios fotokopi";
        }
        if (hasFilledForm && player.x < -8.0f && player.z < -7.0f) {
            return "Tekan E untuk masuk area cek fisik";
        }
        if (hasPhysicalCheckProof && player.x > 5.5f && player.z < -7.0f) {
            return "Tekan E untuk masuk loket verifikasi";
        }
        if (hasVerificationStamp && player.x < -8.0f && player.z > 6.5f) {
            return "Tekan E untuk masuk antrean pembayaran";
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
            return "Tekan E untuk keluar ke halaman Samsat";
        }
        return "";
    }

    if (currentState == INVENTORY_CHECK) {
        return "Tekan SPACE untuk kembali dan mulai mengumpulkan berkas";
    }

    if (currentState == PHOTOCOPY_SHOP) {
        if (player.z > 5.2f) {
            return "Tekan E untuk keluar ke halaman Samsat";
        }
        if (isNear(player.x, player.z, 4.8f, -2.0f, 2.3f)) {
            return "Tekan E untuk memproses fotokopi";
        }
        return "Dekati mesin fotokopi atau pintu keluar";
    }

    if (currentState == FORM_COUNTER) {
        if (player.z > 5.2f) {
            return hasFilledForm ? "Tekan E untuk keluar menuju area cek fisik" : "Tekan E untuk kembali ke ruang informasi";
        }
        if (isNear(player.x, player.z, 0.0f, -4.0f, 2.2f)) {
            return "Tekan E di loket formulir";
        }
        return "Dekati loket formulir untuk memproses berkas";
    }

    if (currentState == VEHICLE_CHECK_AREA) {
        if (player.z > 6.2f) {
            return "Tekan E untuk keluar ke halaman Samsat";
        }
        if (player.z < -6.2f && hasPhysicalCheckProof) {
            return "Tekan E untuk lanjut ke loket verifikasi";
        }
        if (isNear(player.x, player.z, 4.5f, -1.5f, 2.5f) || isNear(player.x, player.z, 0.0f, 1.5f, 2.5f)) {
            return "Tekan E untuk proses cek fisik kendaraan";
        }
        return "Dekati kendaraan atau petugas cek fisik";
    }

    if (currentState == VERIFICATION_COUNTER) {
        if (player.z > 5.2f) {
            return hasVerificationStamp ? "Tekan E untuk lanjut ke antrean pembayaran" : "Tekan E untuk keluar ke halaman Samsat";
        }
        if (isNear(player.x, player.z, 0.0f, -4.0f, 2.2f)) {
            return "Tekan E di loket verifikasi";
        }
        return "Dekati loket verifikasi";
    }

    if (currentState == PAYMENT_QUEUE) {
        if (player.z > 5.5f) {
            return "Tekan E untuk keluar ke halaman Samsat";
        }
        if (hasQueuedPaymentLine && player.z < -5.5f) {
            return "Tekan E untuk maju ke loket pembayaran";
        }
        if (isNear(player.x, player.z, 0.0f, 2.5f, 3.6f)) {
            return "Tekan E untuk ikut antre pembayaran";
        }
        return hasQueuedPaymentLine ? "Pergi ke ujung antrean lalu tekan E" : "Dekati garis antrean pembayaran";
    }

    if (currentState == PAYMENT_COUNTER) {
        if (player.z > 5.2f) {
            return hasPaymentProof ? "Tekan E untuk lanjut ke loket validasi" : "Tekan E untuk kembali ke antrean pembayaran";
        }
        if (isNear(player.x, player.z, 0.0f, -4.0f, 2.2f)) {
            return "Tekan E di loket pembayaran";
        }
        return "Dekati loket pembayaran";
    }

    if (currentState == VALIDATION_COUNTER) {
        if (player.z > 5.2f) {
            return receivedStampRequirement ? "Tekan E untuk mencari meterai" : "Tekan E untuk keluar ke halaman Samsat";
        }
        if (isNear(player.x, player.z, 0.0f, -4.0f, 2.2f)) {
            return "Tekan E di loket validasi";
        }
        return "Dekati loket validasi";
    }

    if (currentState == STAMP_QUEST) {
        if (player.z > 5.2f) {
            return hasStampedDocument ? "Tekan E untuk menuju validasi akhir" : "Tekan E untuk keluar ke halaman Samsat";
        }
        if (isNear(player.x, player.z, 3.5f, -2.0f, 2.3f)) {
            return "Tekan E untuk membeli dan menempel meterai";
        }
        return "Dekati penjual meterai";
    }

    if (currentState == VALIDATION_SUCCESS) {
        if (player.z < -5.5f) {
            return "Tekan E untuk masuk lorong final";
        }
        return "Berkas siap. Jalan ke pintu lorong final lalu tekan E";
    }
    if (currentState == FINAL_CORRIDOR) {
        if (isNear(player.x, player.z, -2.5f, -2.0f, 2.3f)) {
            return "Tekan E untuk bicara dengan Antrean Senior";
        }
        if (isNear(player.x, player.z, 2.8f, -3.8f, 2.3f)) {
            return usedInsider ? "Tekan E untuk cek jalur cepat" : "Tekan E untuk bicara dengan Orang Dalam";
        }
        if (player.z < -6.0f) {
            return "Tekan E untuk menghadapi loket final";
        }
        return "Berjalan ke ujung lorong lalu tekan E";
    }

    if (currentState == FINAL_COUNTER_BOSS) {
        if (player.z > 6.0f) {
            return "Tekan E untuk kembali ke lorong final";
        }
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
            changeStateAt(PHOTOCOPY_SHOP, -5.5f, 5.4f, 180.0f);
            return;
        }

        if (player.x > -1.8f && player.x < 1.8f && player.z > -6.0f && player.z < -3.5f) {
            changeStateAt(INFORMATION_ROOM, 0.0f, 5.6f, 180.0f);
            return;
        }

        if (player.x > 8.0f && player.z < -7.0f) {
            changeStateAt(PHOTOCOPY_SHOP, -5.5f, 5.4f, 180.0f);
            return;
        }

        if (hasFilledForm && player.x < -8.0f && player.z < -7.0f) {
            changeStateAt(VEHICLE_CHECK_AREA, 0.0f, 6.6f, 180.0f);
            return;
        }

        if (hasPhysicalCheckProof && player.x > 5.5f && player.z < -7.0f) {
            changeStateAt(VERIFICATION_COUNTER, 0.0f, 5.6f, 180.0f);
            return;
        }

        if (hasVerificationStamp && player.x < -8.0f && player.z > 6.5f) {
            changeStateAt(PAYMENT_QUEUE, 0.0f, 6.2f, 180.0f);
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
            changeStateAt(SAMSAT_EXTERIOR, 0.0f, -4.2f, 0.0f);
            return;
        }

        setDialogue("Sistem", "Dekati petugas, loket formulir, atau pintu keluar.");
        return;
    }


    if (currentState == PHOTOCOPY_SHOP) {
        if (player.z > 5.2f) {
            changeStateAt(SAMSAT_EXTERIOR, 10.0f, -7.5f, 0.0f);
            return;
        }
        if (isNear(player.x, player.z, 4.8f, -2.0f, 2.3f)) {
            processPhotocopyShop();
            return;
        }

        setDialogue("Sistem", "Dekati mesin fotokopi atau pintu keluar.");
        return;
    }

    if (currentState == FORM_COUNTER) {
        if (player.z > 5.2f) {
            if (hasFilledForm) {
                changeStateAt(VEHICLE_CHECK_AREA, 0.0f, 6.6f, 180.0f);
            } else {
                changeStateAt(INFORMATION_ROOM, 5.2f, -2.2f, -90.0f);
            }
            return;
        }
        if (isNear(player.x, player.z, 0.0f, -4.0f, 2.2f)) {
            processFormCounter();
            return;
        }

        setDialogue("Sistem", "Dekati loket formulir dulu atau pintu keluar.");
        return;
    }

    if (currentState == VEHICLE_CHECK_AREA) {
        if (player.z > 6.2f) {
            changeStateAt(SAMSAT_EXTERIOR, -9.5f, -7.5f, 0.0f);
            return;
        }
        if (player.z < -6.2f && hasPhysicalCheckProof) {
            changeStateAt(VERIFICATION_COUNTER, 0.0f, 5.6f, 180.0f);
            return;
        }
        if (isNear(player.x, player.z, 4.5f, -1.5f, 2.5f) || isNear(player.x, player.z, 0.0f, 1.5f, 2.5f)) {
            processVehicleCheck();
            return;
        }

        setDialogue("Sistem", "Dekati kendaraan, petugas cek fisik, atau pintu keluar.");
        return;
    }

    if (currentState == VERIFICATION_COUNTER) {
        if (player.z > 5.2f) {
            if (hasVerificationStamp) {
                changeStateAt(PAYMENT_QUEUE, 0.0f, 6.2f, 180.0f);
            } else {
                changeStateAt(SAMSAT_EXTERIOR, 7.0f, -7.5f, 0.0f);
            }
            return;
        }
        if (isNear(player.x, player.z, 0.0f, -4.0f, 2.2f)) {
            processVerification();
            return;
        }

        setDialogue("Sistem", "Dekati loket verifikasi atau pintu keluar.");
        return;
    }

    if (currentState == PAYMENT_QUEUE) {
        if (player.z > 5.5f) {
            changeStateAt(SAMSAT_EXTERIOR, -9.5f, 7.2f, 0.0f);
            return;
        }
        if (hasQueuedPaymentLine && player.z < -5.5f) {
            changeStateAt(PAYMENT_COUNTER, 0.0f, 5.6f, 180.0f);
            return;
        }
        if (isNear(player.x, player.z, 0.0f, 2.5f, 3.6f)) {
            processPaymentQueue();
            return;
        }

        setDialogue("Sistem", "Masuk dulu ke area antrean pembayaran, atau cari pintu keluar.");
        return;
    }

    if (currentState == PAYMENT_COUNTER) {
        if (player.z > 5.2f) {
            if (hasPaymentProof) {
                changeStateAt(VALIDATION_COUNTER, 0.0f, 5.6f, 180.0f);
            } else {
                changeStateAt(PAYMENT_QUEUE, 0.0f, -5.8f, 0.0f);
            }
            return;
        }
        if (isNear(player.x, player.z, 0.0f, -4.0f, 2.2f)) {
            processPaymentCounter();
            return;
        }

        setDialogue("Sistem", "Dekati loket pembayaran atau pintu keluar.");
        return;
    }

    if (currentState == VALIDATION_COUNTER) {
        if (player.z > 5.2f) {
            if (receivedStampRequirement) {
                changeStateAt(STAMP_QUEST, -4.8f, 5.6f, 180.0f);
            } else {
                changeStateAt(SAMSAT_EXTERIOR, 0.0f, 8.5f, 0.0f);
            }
            return;
        }
        if (isNear(player.x, player.z, 0.0f, -4.0f, 2.2f)) {
            processValidationCounter();
            return;
        }

        setDialogue("Sistem", "Dekati loket validasi atau pintu keluar.");
        return;
    }

    if (currentState == STAMP_QUEST) {
        if (player.z > 5.2f) {
            if (hasStampedDocument) {
                changeStateAt(VALIDATION_SUCCESS, 0.0f, 5.6f, 180.0f);
            } else {
                changeStateAt(SAMSAT_EXTERIOR, 0.0f, 8.5f, 0.0f);
            }
            return;
        }
        if (isNear(player.x, player.z, 3.5f, -2.0f, 2.3f)) {
            processStampQuest();
            return;
        }

        setDialogue("Sistem", "Dekati penjual meterai atau pintu keluar.");
        return;
    }

    if (currentState == VALIDATION_SUCCESS) {
        if (player.z < -5.5f) {
            changeStateAt(FINAL_CORRIDOR, 0.0f, 7.0f, 180.0f);
            return;
        }

        setDialogue("Sistem", "Jalan ke pintu lorong final lalu tekan E.");
        return;
    }

    if (currentState == FINAL_CORRIDOR) {
        if (isNear(player.x, player.z, -2.5f, -2.0f, 2.3f)) {
            processCorridorAdvice();
            return;
        }

        if (isNear(player.x, player.z, 2.8f, -3.8f, 2.3f)) {
            processInsiderOffer();
            return;
        }

        if (player.z < -6.0f) {
            changeState(FINAL_COUNTER_BOSS);
            return;
        }

        setDialogue("Sistem", "Ujung lorong final masih di depan.");
        return;
    }

    if (currentState == FINAL_COUNTER_BOSS) {
        if (player.z > 6.0f) {
            changeState(FINAL_CORRIDOR);
            return;
        }

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
            changeState(SAMSAT_EXTERIOR);
            return;
        }

        if (currentState == DIALOG_INFORMATION_OFFICER) {
            hasCheckedInventoryScene = true;
            changeState(INFORMATION_ROOM);
            return;
        }

        if (currentState == QUEUE_MACHINE ||
            currentState == MAP_VENDOR) {
            changeState(SAMSAT_EXTERIOR);
            return;
        }

        if (currentState == PHOTOCOPY_SHOP) {
            return;
        }

        if (currentState == FORM_COUNTER) {
            return;
        }

        if (currentState == VEHICLE_CHECK_AREA) {
            return;
        }

        if (currentState == VERIFICATION_COUNTER) {
            return;
        }

        if (currentState == PAYMENT_QUEUE) {
            return;
        }

        if (currentState == PAYMENT_COUNTER) {
            return;
        }

        if (currentState == VALIDATION_COUNTER) {
            return;
        }

        if (currentState == STAMP_QUEST) {
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
            changeState(INFORMATION_ROOM);
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

}  // namespace samsat
