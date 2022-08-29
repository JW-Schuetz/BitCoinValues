
#include "bitcoinkurs.h"


void exitfun( BitCoinCode state, int doexit )
{
  extern int errno;

  int ret = 1;
  char *errstr;
  switch( state )
  {
    case BITCOIN_OK:
      errstr = "Programm erfogreich beendet";
      ret = 0;
      break;
    case BITCOIN_PARSE:
      errstr = "Laufzeitfehler: Fehler beim Parsen";
      break;
    case BITCOIN_CURL:
      errstr = "Laufzeitfehler: Curl hat ein Problem";
      break;
    case BITCOIN_SIG:
      errstr = "Laufzeitfehler: Signal hat ein Problem";
      break;
    case BITCOIN_TIMER:
      errstr = "Laufzeitfehler: Timer hat ein Problem";
      break;
  }

  printf( "%s\n\n", errstr );
  if( doexit != 0 ) exit( ret );
}