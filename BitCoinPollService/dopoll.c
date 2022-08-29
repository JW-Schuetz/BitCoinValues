
#include "bitcoinkurs.h"


extern struct Result *prev_res;

struct Result *cloneResult( struct Result *res )
{
    struct Result *ret = (struct Result *)malloc( sizeof( struct Result ) );
    ret->ticker = strdup( res->ticker );
    ret->time = strdup( res->time );
    ret->state = res->state;
    return ret;
}

void freeResult( struct Result *res )
{
  if( res != NULL )
  {
    free( res-> ticker );
    free( res->time );
    free( res );
  }
}

BitCoinCode dopoll( CURLM *easy_handle, int *file_written, char **filename, double *price )
{
  extern struct Result *prev_res;
  extern struct MemoryStruct content;

  content.memory = (char *)malloc( 1 );    // Memory wächst durch realloc in Funktion write_data()
  content.size = 0;                        // z.Z. noch keine Daten

  *price = 0.0;

  CURLcode cres = curl_easy_perform( easy_handle );
  if( cres != CURLE_OK )
  {
    free( content.memory );
    return BITCOIN_CURL;
  }

  struct Result *res = parse( &content );

  free( content.memory ); // content Speicher wieder freigeben

  if( res->state != BITCOIN_OK )
  {
    freeResult( res );  // in parse() allozierten Speicher freigeben
    return BITCOIN_PARSE;
  }
 
  // neuer Datensatz mit anderer Zeitmarke?
  if( ( prev_res == NULL ) || strcmp( res->time, prev_res->time ) != 0 )
  {
      // String duplizieren wg. Modifikation durch strtok()
      char *duptime = strdup( res->time );
      char *datum = strtok( duptime, " " );

      // Filename generieren
      int size = strlen( FILENAME_PREFIX ) + strlen( datum ) + strlen( FILENAME_POSTFIX ) + 1;
      *filename = (char *)malloc( size );
      memset( *filename, 0, size );

      *filename = strcat( *filename, FILENAME_PREFIX );
      *filename = strcat( *filename, datum );
      *filename = strcat( *filename, ".csv" );

      FILE *file = fopen( *filename, "a" );

      fwrite( res->time, strlen( res->time ), 1, file );
      fwrite( ", ", 2, 1, file );
      fwrite( res->ticker, strlen( res->ticker ), 1, file );
      fwrite( "\n", strlen( "\n" ), 1, file );

      fclose( file );

      free( duptime );

      *price = atof( res->ticker );
      *file_written = 1;  // Merker: neuer Eintrag im File

      // vorherige Ergebnisse evtl. freigeben
      freeResult( prev_res );

      // neues Result merken
      prev_res = res;
  }

  return BITCOIN_OK;
}
