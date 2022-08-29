
#include <signal.h>
#include <time.h>

#include "bitcoinkurs.h"


#define SIG SIGPROF

extern void signalhandler( int, siginfo_t *, void * );

timer_t timerid;

BitCoinCode inittimer()
{
    // Signalhandler initialisieren
    struct sigaction sa;
    sa.sa_flags = SA_SIGINFO;
    sa.sa_sigaction = signalhandler;
    sigemptyset( &sa.sa_mask );
    if( sigaction( SIG, &sa, NULL ) == -1 ) return BITCOIN_SIG;

    // Timer initialisieren
    struct sigevent sev;
    sev.sigev_notify = SIGEV_SIGNAL;
    sev.sigev_signo = SIG;
    sev.sigev_value.sival_ptr = timerid;

    if( timer_create( CLOCK_REALTIME, &sev, &timerid ) == -1 ) return BITCOIN_TIMER;

    return BITCOIN_OK;
}

BitCoinCode starttimer()
{
    struct itimerspec its;
    its.it_interval.tv_sec = TIMER_REPEAT_TIME;
    its.it_interval.tv_nsec = 0;
    its.it_value.tv_sec = TIMER_REPEAT_TIME;
    its.it_value.tv_nsec = 0;

    if( timer_settime( timerid, 0, &its, NULL ) == -1 ) return BITCOIN_SIG;

    return BITCOIN_OK;
}