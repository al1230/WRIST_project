/**** DO NOT EDIT -- this file has been automatically generated from @emmoco.com.FirstApp on 2014-05-20T11:50:16 ****/
/**** protocolLevel = 12, toolsVersion = 12.0.0.201211010336 ****/

#ifndef FirstApp__H
#define FirstApp__H

#include "Em_Types.h"
#include "Em_Message.h"

#ifdef __cplusplus
extern "C" {
#endif

/* resource data */
typedef struct FirstApp_data_t { char elems[13]; } FirstApp_data_t;
#define FirstApp_data_length 13
extern void FirstApp_data_fetch(FirstApp_data_t* const output);
extern void FirstApp_data_store(FirstApp_data_t* const input);

void FirstApp_reset(void);
void FirstApp_run(void);

void FirstApp_accept(bool enable);
void FirstApp_disconnect(void);
void FirstApp_pairingOn(uint8_t secs, void(*handler)(void));
void FirstApp_pairingOff(void(*handler)(void));

void FirstApp_connectHandler(void);
void FirstApp_disconnectHandler(void);

#ifdef __cplusplus
}
#endif

#endif /* FirstApp__H */
