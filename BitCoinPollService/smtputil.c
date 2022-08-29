
#include "bitcoinkurs.h"


#define HDRLINES 9     // Anzahl der Header-Zeilen + 1 (wg. NULL)

void delhdr( struct SmtpWrite *state )
{
    if( state != NULL )
    {
        for( int n = 0; n < HDRLINES; ++n ) free( state->data[n] );
        free( state->data );
    }
}

void createhdr( double actprice, struct SmtpWrite *state )
{
    int size   = HDRLINES * sizeof( char * );
    state->data = (char **)malloc( size );
    memset( state->data, 0, size );
    state->linendx = 0;                 // Lesezeilenindex initialisieren

    // Zeitmarke besorgen
    time_t t = time( NULL );
    char *buffer = malloc( 26 + 1 );    // 26: siehe Doko von ctime_r(), +1 wg. Zeile "ct[ strlen( ct ) + 1 ] = 0;"
    char * ct = ctime_r( &t, buffer );

    ct[ strlen( ct ) - 1 ] = '\r';      // '\n' durch '\r\n' ersetzen
    ct[ strlen( ct ) ] = '\n';
    ct[ strlen( ct ) + 1 ] = 0;

    // 1. Zeile: Zeitmarke
    char *line = "Date: ";
    size = strlen( line ) + strlen( ct ) + 1;
    char *hdr = malloc( size );
    *hdr = 0;                           // String 0-terminieren
    hdr = strcat( hdr, line );
    hdr = strcat( hdr, ct );
    state->data[0] = hdr;

    free( buffer );

    // 2. Zeile: Emfänger
    state->data[1] = strdup( "To: <jwschuetz1954@icloud.com>\r\n" );

    // 3. Zeile: Absender
    state->data[2] = strdup( "From: <jws1954@web.de>\r\n" );

    // 4. Zeile: CC
    state->data[3] = strdup( "Cc: \r\n" );

    // 5. Zeile: Message-ID
    line = "Message-ID: <";
    char *line1 = "@JWS-Cloud.com>\r\n";
    size = strlen( line ) + strlen( line1 ) + 10 + 1;
    buffer = malloc( size );
    snprintf( buffer, size, "%s%10d%s", line, (int)t, line1 );
    state->data[4] = strdup( buffer );
    free( buffer );

    // 6. Zeile: Subject
    state->data[5] = strdup( "Subject: BITCOIN Kurs RasPi\r\n" );

    // 7. Zeile: Trenner Header/Body
    state->data[6] = strdup( "\r\n" );

    // 8. Zeile: Body mit Kursinfo
    line = "Kurs: ";
    size = strlen( line ) + 10 + 1;   // z.B. "10027.52" + "\r\n" sind 10 Zeichen 
    buffer = malloc( size );
    snprintf( buffer, size, "%s%8.2f\r\n", line, actprice );
    state->data[7] = strdup( buffer );
    free( buffer );
}

size_t write_smtp( void *buffer, size_t size, size_t nmemb, void *userp )
{
    // size*nmemb=16k
    int maxsize = size * nmemb;
    if( ( size == 0 ) || ( nmemb == 0 ) || ( maxsize < 1 ) ) return 0;

    struct SmtpWrite *state = (struct SmtpWrite *)userp;
 
    char *line = state->data[ state->linendx ];
 
    if( line )
    {
        size_t len = strlen( line );
        if( len > maxsize ) return CURL_READFUNC_ABORT;
        memcpy( buffer, line, len + 1 );
        ++( state->linendx );
 
#ifdef DEBUG
        debugsmtp( buffer );
#endif

        return len;
    }

#ifdef DEBUG
    debugsmtp( buffer );
#endif

    return 0;
}

void debugsmtp( char *buffer )
{
    FILE *f = fopen( "debug.out", "a" );
    fwrite( buffer, strlen( buffer ), 1, f );
    fwrite( "\r\n", strlen( "\r\n" ), 1, f );
    fclose( f );
}