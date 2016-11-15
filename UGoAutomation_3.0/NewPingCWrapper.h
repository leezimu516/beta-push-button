#ifndef NewPingCWrapper_h
#define NewPingCWrapper_h

typedef void CNewPing;

#ifdef __cplusplus
extern "C" {
#endif

CNewPing * new_ping_c_wrapper_init(int, int);
int new_ping_c_wrapper_sonar_ping(const CNewPing *t);
#ifdef __cplusplus
}
#endif

#endif

