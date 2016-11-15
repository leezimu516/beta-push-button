#include "NewPingCWrapper.h"
#include "NewPing.h"
#include "global.h"
#define MAX_DISTANCE 200

extern "C" {

    CNewPing * new_ping_c_wrapper_init(int trigger_pin, int echo_pin) {
        NewPing *t = new NewPing(trigger_pin, echo_pin, MAX_DISTANCE);

        return (CNewPing *)t;
    }

    int new_ping_c_wrapper_sonar_ping(const CNewPing *new_ping) {
       NewPing *t = (NewPing *)new_ping;
       int value = t->ping() / US_ROUNDTRIP_CM;
       LOG_PRINT(LOGGER_DEBUG, "ping value %d", value);
       return value;
    }
}
