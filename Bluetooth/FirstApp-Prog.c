#include "Em/FirstApp.h"
#include "Hal/Hal.h"

static FirstApp_data_t dataVal = "kim sucks";

void main() {
    
    Hal_init();
    FirstApp_start();
    Hal_idleLoop();
}

/* -------- SCHEMA CALLBACKS -------- */

void FirstApp_connectHandler(void) {
    Hal_connected();
}

void FirstApp_disconnectHandler(void) {
    Hal_disconnected();
}

void FirstApp_data_fetch(FirstApp_data_t output) {
    memcpy(output, dataVal, sizeof(FirstApp_data_t));
}

void FirstApp_data_store(FirstApp_data_t input) {
    memcpy(dataVal, input, sizeof(FirstApp_data_t));
}
