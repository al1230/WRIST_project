/**** DO NOT EDIT -- this file has been automatically generated from @emmoco.com.FirstApp on 2014-05-20T11:50:16 ****/
/**** protocolLevel = 12, toolsVersion = 12.0.0.201211010336 ****/

#include "Em_Message.h"
#include "FirstApp.h"

#ifdef __cplusplus
extern "C" {
#endif

#define Em_Message_protocolLevel 12

typedef struct Em_App_Message {
    uint8_t dummy;
    uint8_t sot;
    Em_Message_Header hdr;
    uint8_t data[20];
} Em_App_Message;

const uint8_t Em_App_hash[] = {80, 120, 25, 12, 77, 226, 176, 233, 107, 51, 16, 254, 25, 47, 161, 229, 12, 0, ((sizeof(struct{uint8_t f1; uint16_t f2;}) - sizeof(uint16_t)) << 4) | (sizeof(struct{uint8_t f1; uint32_t f2;}) - sizeof(uint32_t))};

const uint8_t Em_App_build[] = {113, 68, 248, 26, 70, 1, 0, 0};

#define Em_App_APP_RESOURCE_COUNT 1
#define Em_App_SYS_RESOURCE_COUNT 8

#define Em_App_ACCEPT FirstApp_accept
#define Em_App_DISCONNECT FirstApp_disconnect
#define Em_App_PAIRINGON FirstApp_pairingOn
#define Em_App_PAIRINGOFF FirstApp_pairingOff
#define Em_App_RESET FirstApp_reset
#define Em_App_RUN FirstApp_run

#define Em_App_CONNECTHANDLER FirstApp_connectHandler
#define Em_App_DISCONNECTHANDLER FirstApp_disconnectHandler

#define Em_App_MAX_INDICATOR 1

enum {Em_App_IDLE, Em_App_DISCONNECTED, Em_App_CONNECTED};

typedef struct Em_App_Indicator {
    uint8_t dummy;
    uint8_t sot;
    Em_Message_Header hdr;
    uint8_t data[Em_Message_INDSIZE];
} Em_App_Indicator;

void (*Em_App_pdHdlr)(void);

const uint16_t Em_App_endian = 0x0100;

Em_Message_Size Em_App_recvIdx;
Em_Message_Size Em_App_recvSize;
Em_Message_Size Em_App_xmitIdx;
Em_Message_Size Em_App_xmitSize;

uint8_t Em_App_state = Em_App_IDLE;
Em_App_Message Em_App_msg;
Em_App_Indicator Em_App_ind;
bool Em_App_moreData = false;
bool Em_App_readIdle = true;
int32_t Em_App_fileIndex;
uint32_t Em_App_xmitMask;

uint8_t* Em_App_inBuf = (uint8_t*)&Em_App_msg.hdr;
uint8_t* Em_App_outBuf;

void Em_App_fetchDispatch(void);
void Em_App_storeDispatch(void);
void Em_App_sendIndicator(Em_Message_ResId indId);
void Em_App_sendResponse(Em_Message_Kind kind, Em_Message_Size size);
void Em_App_startIndSend(void);
void Em_App_startResSend(void);
void Em_App_sysFetchDispatch(void);
void Em_App_sysStoreDispatch(void);
bool Em_App_xmitReady(Em_Message_ResId indId);

bool Em_Message_addByte(uint8_t b) {
    if (Em_App_readIdle && b) {
        Em_App_recvSize = Em_App_inBuf[0] = b;
        Em_App_recvIdx = 1; 
        Em_App_readIdle = false;
        return false;
    }
    Em_App_inBuf[Em_App_recvIdx++] = b;
    if (Em_App_recvIdx < Em_App_recvSize) {
        return false;
    }
    Em_App_readIdle = true;
    return true;
}

bool Em_Message_getByte(uint8_t* bp) {
    uint8_t key = Em_Message_lock();
    Em_App_moreData = (Em_App_xmitSize != 0);
    if (!Em_App_moreData && (Em_App_xmitMask != 0)) {
        uint8_t i;
        uint32_t m;
        for (i = 0, m = 0x1; i < Em_App_MAX_INDICATOR; i++, m <<= 1) {
            if (Em_App_xmitMask & m) {
                Em_App_xmitMask &= ~m;
                if (i == 0) {
                    Em_App_startResSend();
                }
                else {
                    Em_App_sendIndicator(i);
                }
                break;
            }
        }
        Em_Message_unlock(key);
        return false;
    }
    else if (Em_App_moreData) {
        *bp = Em_App_outBuf[Em_App_xmitIdx++];
        Em_App_xmitSize -= 1;
    }
    Em_Message_unlock(key);
    return Em_App_moreData;
}

void Em_App_ACCEPT(bool enable) {
    if (Em_App_state == Em_App_CONNECTED) {
        return;
    }
    Em_App_ind.sot = 0;
    Em_App_ind.hdr.kind = Em_Message_ACCEPT;
    Em_App_ind.hdr.size = sizeof (Em_Message_Header);
    Em_App_ind.hdr.resId = enable;
    Em_App_startIndSend();
}

void Em_App_DISCONNECT(void) {
    if (Em_App_state != Em_App_CONNECTED) {
        return;
    }
    Em_App_state = Em_App_DISCONNECTED;
    Em_App_ind.sot = 0;
    Em_App_ind.hdr.kind = Em_Message_DISCONNECT;
    Em_App_ind.hdr.size = sizeof (Em_Message_Header);
    Em_App_ind.hdr.resId = 0;
    Em_App_startIndSend();
}

void Em_Message_dispatch(void) {
    if (Em_App_state == Em_App_IDLE) {
        return;
    }
    switch (Em_App_msg.hdr.kind) {
        case Em_Message_CONNECT:
            Em_App_state = Em_App_CONNECTED;
            Em_App_CONNECTHANDLER();
            break;
        case Em_Message_DISCONNECT:
            Em_App_state = Em_App_DISCONNECTED;
            Em_App_DISCONNECTHANDLER();
            break;
        case Em_Message_PAIRING_DONE:
            if (Em_App_pdHdlr) {
                (*Em_App_pdHdlr)();
            }
            break;
        case Em_Message_FETCH:
            if (Em_App_msg.hdr.resId > 0) {
                Em_App_fetchDispatch();
            }
            else {
                Em_App_sysFetchDispatch();
            }
            break;
        case Em_Message_STORE:
            if (Em_App_msg.hdr.resId > 0) {
                Em_App_storeDispatch();
            }
            else {
                Em_App_sysStoreDispatch();
            }
            break;
    }
}

void Em_App_PAIRINGOFF(void(*handler)(void)) {
    Em_App_PAIRINGON(0, handler);
}

void Em_App_PAIRINGON(uint8_t secs, void(*handler)(void)) {
    if (Em_App_state != Em_App_DISCONNECTED) {
        return;
    }
    Em_App_pdHdlr = handler;
    Em_App_ind.sot = 0;
    Em_App_ind.hdr.kind = Em_Message_PAIRING;
    Em_App_ind.hdr.size = sizeof (Em_Message_Header);
    Em_App_ind.hdr.resId = secs;
    Em_App_startIndSend();
}

void Em_App_RESET(void) {
    Em_App_recvIdx = Em_App_recvSize = Em_App_xmitIdx = Em_App_xmitSize = 0;
    Em_App_state = Em_App_IDLE;
    Em_App_moreData = false;
    Em_App_readIdle = true;
    Em_App_fileIndex = 0;
    Em_App_xmitMask = 0;
}

void Em_App_RUN(void) {
    Em_App_state = Em_App_DISCONNECTED;
}

void Em_App_sendResponse(Em_Message_Kind kind, Em_Message_Size size) {
    if (Em_App_state != Em_App_CONNECTED) {
        return;
    }
    Em_App_msg.sot = 0;
    Em_App_msg.hdr.kind = kind;
    Em_App_msg.hdr.size = size + sizeof (Em_Message_Header);
    if (Em_App_xmitReady(0)) {
        Em_App_startResSend();
    }
}

void Em_App_startIndSend(void) {
    Em_App_outBuf = (uint8_t*)&Em_App_ind.sot;
    Em_App_xmitSize = Em_App_ind.hdr.size + 1;
    Em_App_xmitIdx = 0;
    Em_Message_startSend();
}

void Em_App_startResSend(void) {
    Em_App_outBuf = (uint8_t*)&Em_App_msg.sot;
    Em_App_xmitSize = Em_App_msg.hdr.size + 1;
    Em_App_xmitIdx = 0;
    Em_Message_startSend();
}

void Em_App_sysFetchDispatch(void) {
    uint8_t size = 0;
    int i;
    switch (Em_App_msg.hdr.resId) {
        case Em_Message_SYS_SCHEMA_HASH:
            for (i = 0; i < sizeof (Em_App_hash); i++) {
                Em_App_msg.data[i] = Em_App_hash[i];
            }
            Em_App_msg.data[sizeof (Em_App_hash)] = *((uint8_t*)&Em_App_endian);
            size = sizeof (Em_App_hash) + 1;
            break;
        case Em_Message_SYS_EAP_PROTOCOL_LEVEL:
            *((Em_Message_protocolLevel_t*)Em_App_msg.data) = Em_Message_protocolLevel;
            size = sizeof (Em_Message_protocolLevel_t);
            break;
        case Em_Message_SYS_EAP_BUILD_DATE:
            for (i = 0; i < sizeof (Em_App_build); i++) {
                Em_App_msg.data[i] = Em_App_build[i];
            }
            size = sizeof (Em_App_build);
            break;
        case Em_Message_SYS_RESOURCE_COUNT:
            Em_App_msg.data[0] = Em_App_APP_RESOURCE_COUNT;
            Em_App_msg.data[1] = Em_App_SYS_RESOURCE_COUNT;
            size = 2;
            break;
    }
    Em_App_sendResponse(Em_Message_FETCH_DONE, size);
}

void Em_App_sysStoreDispatch(void) {
    switch (Em_App_msg.hdr.resId) {
        case Em_Message_SYS_FILE_INDEX_RESET:
            Em_App_fileIndex = 0;
            break;
    }
    Em_App_sendResponse(Em_Message_STORE_DONE, 0);
}

bool Em_App_xmitReady(Em_Message_ResId indId) {
    uint8_t key = Em_Message_lock();
    bool res = !Em_App_moreData && Em_App_xmitMask == 0;
    if (!res) {
        Em_App_xmitMask |= (1 << indId);
    }
    Em_Message_unlock(key);
    return res;    
}

void Em_App_fetchDispatch(void) {
    uint8_t size = 0;
    switch (Em_App_msg.hdr.resId) {
    case 0:
        break;
    case 1:
        FirstApp_data_fetch((FirstApp_data_t*)Em_App_msg.data);
        size = 13;
        break;
    }
    Em_App_sendResponse(Em_Message_FETCH_DONE, size);
}

void Em_App_storeDispatch(void) {
    switch (Em_App_msg.hdr.resId) {
    case 0:
        break;
    case 1:
        FirstApp_data_store((FirstApp_data_t*)Em_App_msg.data);
        break;
    }
    Em_App_sendResponse(Em_Message_STORE_DONE, 0);
}

void Em_App_sendIndicator(Em_Message_ResId indId) {
    Em_Message_Size resId = 0;
    Em_Message_Size size = 0;
    Em_App_ind.sot = 0;
    Em_App_ind.hdr.kind = Em_Message_INDICATOR;
    Em_App_ind.hdr.size = sizeof (Em_Message_Header) + size;
    Em_App_ind.hdr.resId = resId;
    Em_App_startIndSend();
}

#ifdef __cplusplus
}
#endif

