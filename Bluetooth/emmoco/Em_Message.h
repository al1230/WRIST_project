#ifndef Em_Message_H_
#define Em_Message_H_

#include "Em_Types.h"

#ifdef __cplusplus
extern "C" {
#endif

/* protocolLevel #4 */
#define Em_Message_INDSIZE 4

typedef uint8_t Em_Message_Size;
typedef uint8_t Em_Message_Kind;
/* protocolLevel #12 -- split 16-bit resId into <resId, chan> */
typedef int8_t Em_Message_ResId;
typedef uint8_t Em_Message_Chan;

#define Em_Message_NOP 0
#define Em_Message_FETCH 1
#define Em_Message_FETCH_DONE 2
#define Em_Message_STORE 3
#define Em_Message_STORE_DONE 4
#define Em_Message_INDICATOR 5
#define Em_Message_CONNECT 6
#define Em_Message_DISCONNECT 7
#define Em_Message_ECHO 8
/* protocolLevel #3 */
/* protocolLevel #6  -- rename from BROADCAST to PAIRING */
#define Em_Message_PAIRING 9
#define Em_Message_PAIRING_DONE 10
/* protocolLevel #7 */
#define Em_Message_OFFLINE 11
/* protocolLevel #8 */
#define Em_Message_ACCEPT 12

typedef struct Em_Message_Header {
    Em_Message_Size size;
    Em_Message_Kind kind;
    Em_Message_ResId resId;
    Em_Message_Chan chan;
} Em_Message_Header;

typedef uint16_t Em_Message_protocolLevel_t;

/* protocolLevel #1 */

/* protocolLevel #10 */
/* #define Em_Message_SYS_SCHEMA_UUID -1 */

#define Em_Message_SYS_MCM_PROTOCOL_LEVEL -2
#define Em_Message_SYS_EAP_PROTOCOL_LEVEL -3
#define Em_Message_SYS_EAP_BUILD_DATE -4

/* protocolLevel #2 */
#define Em_Message_SYS_FILE_INDEX_RESET -5

/* protocolLevel #5 */
#define Em_Message_SYS_SCHEMA_HASH -6

/* protocolLevel #7 */
#define Em_Message_SYS_RESOURCE_COUNT -7

/* protocolLevel #9 */
#define Em_Message_SYS_MOBILE_RSSI -8

/* protocolLevel #11 */
#define Em_Message_SYS_MCM_DISCONNECT -9

extern void Em_Message_init(void);

extern bool Em_Message_addByte(uint8_t b);
extern void Em_Message_dispatch(void);
extern bool Em_Message_getByte(uint8_t *bp);
extern uint8_t Em_Message_lock(void);
extern void Em_Message_startSend(void);
extern void Em_Message_unlock(uint8_t key);

#ifdef __cplusplus
}
#endif

#endif /*Em_Message_H_*/
