
#include "bitcoinkurs.h"


extern struct Result *prev_res;


BitCoinCode doapihistory( CURLM *easy_handle )
{
  extern struct Result *prev_res;
  extern struct MemoryStruct content;

  content.memory = (char *)malloc( 1 );    // Memory wächst durch realloc in Funktion write_data()
  content.size = 0;                        // z.Z. noch keine Daten

  CURLcode cres = curl_easy_perform( easy_handle );
  if( cres != CURLE_OK ) return BITCOIN_CURL;

  return BITCOIN_OK;
}