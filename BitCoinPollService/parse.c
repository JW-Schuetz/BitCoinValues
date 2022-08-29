
#include "bitcoinkurs.h"


char *beautify( char *t )
{
  int size = strlen( t ) + 1;
  char *clone = malloc( size );
  memset( clone, 0, size );

  int n = 0;
  char *p = t;

  while( *p != 0 )
  {
    if( *p != '.' )
    {
      if( *p != ',' ) clone[n] = *p;
      else            clone[n] = '.';

      ++n; ++p;
    }
    else ++p;
  }
  return clone;
}

extern int errno;

struct Result *parse( struct MemoryStruct *content )
{
  struct Result *res = (struct Result *)malloc( sizeof( struct Result ) );
  res->ticker = NULL;
  res->time = NULL;
  res->state = BITCOIN_PARSE;

  // Tickerpreis (Annahme: zuerst Tickerpreis, dann Zeitstempel)
  char *match = "<strong id=\"ticker_price\" data-value=\"";
  char *start1 = strstr( content->memory, match );
  if( start1 == NULL ) return res;  // nix gefunden -> raus

  char *m1 = start1 + strlen( match );
  char *clone = strdup( m1 );
  char *t1 = strtok( m1, "\"" );  // t1 zeigt in content->memory
  if( t1 == NULL )  // nix gefunden -> raus
  {
    free( clone );
    return res;
  }
  char *ticker = beautify( t1 );  // beautify() liefert malloc-Pointer

  // Zeitstempel
  match = "<div class=\"d-block d-lg-inline\">(Stand: ";
  char *start2 = strstr( clone + strlen( ticker ) + 1, match );
  if( start2 == NULL )  // nix gefunden -> raus
  {
    free( clone );
    free( ticker );
    return res;
  }

  char *m2 = start2 + strlen( match );
  char *t2 = strtok( m2, ")" );  // TT.MM.YY hh:mm, t2 zeigt in content->memory
  if( t2 == NULL )  // nix gefunden -> raus
  {
    free( clone );
    free( ticker );
    return res;
  }

  // 2 Bytes mehr reservieren für "20" (plus 0-Terminierung 0 macht 3)
  int len = strlen( t2 ) + 3;
  char *time = (char *)malloc( len );
  memset( time, 0, len );

  time = strncat( time, t2, strlen( t2 ) - 8 ); // TT.MM.
  time = strcat( time, "20" ); // TT.MM.20
  time = strncat( time, t2 + 6, 2 ); // TT.MM.20YY
  time = strcat( time, t2 + 8 ); // TT.MM.20YY hh:mm

  free( clone );

  res->ticker = ticker;
  res->time = time;
  res->state = BITCOIN_OK;

  return res;
}
