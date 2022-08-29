
#include "bitcoinkurs.h"


int main( void )
{
  // BitCoinKurs-App initialisieren
  init();

  // Curl initialisieren
  curl_global_init( CURL_GLOBAL_ALL );

  // Timer initialisieren
  BitCoinCode ret = inittimer();
  if( ret != BITCOIN_OK ) exitfun( ret, 1 );

  // Timer starten
  ret = starttimer();
  if( ret != BITCOIN_OK ) exitfun( ret, 1 );

  // Warteschleife
  while( 1 ) sleep( 5 * TIMER_REPEAT_TIME );
 
  // der nachfolgende Code wird nie erreicht
  curl_global_cleanup();

  return BITCOIN_CURL;
}