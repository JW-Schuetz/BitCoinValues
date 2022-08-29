
#include <signal.h>

#include "bitcoinkurs.h"


extern int oldTableIndex;

struct MemoryStruct content;

void signalhandler( int sig, siginfo_t *si, void *uc )
{
    // 1: falls ein neuer Eintrag im Datenfile, 0: sonst
    int file_written = 0;
    char *filename = NULL;
    double actprice = 0.0;
    CURLM *easy_handle;

#ifndef NOPOLL
    // Seite "https://www.bitcoin.de" pollen
    easy_handle = curl_easy_init();
    if( easy_handle )
    {
        struct curl_slist *headers = NULL;

        // Parameter für das Pollen der URL
        setpoll( easy_handle, &headers, &content );

        BitCoinCode ret = dopoll( easy_handle, &file_written, &filename, &actprice );
        if( ret != BITCOIN_OK ) exitfun( ret, 0 );

        curl_slist_free_all( headers );
        curl_easy_cleanup( easy_handle );
    } else exitfun( BITCOIN_CURL, 0 );
#endif

#ifndef NOFTP
    // Gelesene Daten (in filename) auf Diskstation kopieren (FTP)
    if( file_written != 0 )
    {
        easy_handle = curl_easy_init();
        if( easy_handle )
        {
            // Parameter für FTP-Filetransfer zur Diskstation
            FILE *file = setftp( easy_handle, filename );
            CURLcode cres = curl_easy_perform( easy_handle );
            fclose( file );

            if( cres != CURLE_OK ) return exitfun( BITCOIN_CURL, 0 );

            printf( "File %s updated\n", filename );

            curl_easy_cleanup( easy_handle );
        } else exitfun( BITCOIN_CURL, 0 );
    }
#endif

#ifndef NOEMAIL
#ifndef TESTMODE
    // Mail senden, falls sich Preisstufe ändert
    if( actprice != 0.0 )
    {
        int actTableIndex = findInTable( actprice );

        if( actTableIndex != NOINDEX )
        {
            if( oldTableIndex != actTableIndex )
            {
                if( oldTableIndex != NOINDEX )  // keine EMail nach Programmstart
                {
#endif
                    easy_handle = curl_easy_init();

                    struct curl_slist *recipients = NULL;
                    struct SmtpWrite state;

                    createhdr( actprice, &state );

                    // Parameter für Senden einer EMail
                    setsmtp( easy_handle, &recipients, &state );

                    CURLcode cres = curl_easy_perform( easy_handle );
                    if( cres != CURLE_OK ) return exitfun( BITCOIN_CURL, 0 );

                    delhdr( &state );

                    printf( "\nEMail sent\n" );

                    curl_slist_free_all( recipients );
                    curl_easy_cleanup( easy_handle );
#ifndef TESTMODE
                }

                oldTableIndex = actTableIndex;
            }
        }
    }
#endif
#endif

#ifndef NOAPIHISTORY
    // Seite "https://api.bitcoin.de/v2/trades/history" abfragen
    easy_handle = curl_easy_init();
    if( easy_handle )
    {
        struct curl_slist *headers = NULL;

        // Parameter für das Abfragen der URL
        setapihistory( easy_handle, &headers, &content );

        BitCoinCode ret = doapihistory( easy_handle );
        if( ret != BITCOIN_OK ) exitfun( ret, 0 );

        curl_slist_free_all( headers );
        curl_easy_cleanup( easy_handle );
    } else exitfun( BITCOIN_CURL, 1 );
#endif

    // den evtl. in dopoll() belegten Speicherplatz freigeben
    free( filename );
}

int findInTable( double price )
{
    extern double *tabelle;

    int N = MAXBITCOINPRICE / BITCOINPRICESTEP + 1;
    for( int n = 0; n < N; ++n )
        if( tabelle[n] > price )
            return n;

    return NOINDEX;
}