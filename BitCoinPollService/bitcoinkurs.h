
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>

#include <curl/curl.h>


// Datenquellen/Datensenken:
#define URLAPIHISTORY       "https://api.bitcoin.de/v2/trades/history"
#define URL                 "https://www.bitcoin.de"
#define DISKSTATION         "ftp://womser-schuetz.diskstation.me/homes/Juergen/BitCoinPrice/"

#define MAXBITCOINPRICE     20000             // max. Preis für Tabelle EMail-Benachrichtigung
#define NOINDEX             -1                // kein gültiger Index
#define NOAPIHISTORY

#define DEBUG                                 // debugging enabled
#define NOEMAIL                               // dont send EMAILS

//#define TESTMODE
#ifdef TESTMODE
  #define NOFTP
//  #define NOPOLL
  #define NOEMAIL
  #define BITCOINPRICESTEP    500             // Preisstufe für EMail-Benachrichtigung
  #define TIMER_REPEAT_TIME   5*60            // Pollzeit [sec]
#else
  #define BITCOINPRICESTEP    500               // Preisstufe für EMail-Benachrichtigung
  #define TIMER_REPEAT_TIME   5 * 60            // Pollzeit [sec]
#endif

#define FILENAME_PREFIX     "BitCoinPrice-"
#define FILENAME_POSTFIX    "*.csv"

// #define USEGMXDE
#define USEWEBDE

#define FTPCREDENTIALS      ""	// siehe credentials.zip
#define SMTPGMXCREDENTIALS  ""	// siehe credentials.zip
#define SMTPWEBCREDENTIALS  ""	// siehe credentials.zip

typedef enum
{
  BITCOIN_OK = 0,               // alles OK
  BITCOIN_PARSE,                // Fehler beim Parsen
  BITCOIN_CURL,                 // Curl hat ein Problem
  BITCOIN_SIG,                  // SIG hat Probleme
  BITCOIN_TIMER,                // Timer hat Problem
} BitCoinCode;

struct MemoryStruct
{
  char *memory;
  size_t size;
};

struct Result
{
  char *ticker;
  char *time;
  int state;
};

struct SmtpWrite
{
  char **data;        // message data
  int linendx;        // Index zu lesendr Zeile
};

// Prototypen
void init();

BitCoinCode inittimer();
BitCoinCode starttimer();

void setpoll( CURLM *, struct curl_slist **, struct MemoryStruct * );
size_t write_data( void *, size_t, size_t, void * );
BitCoinCode dopoll( CURLM *, int *, char **, double * );
struct Result *parse( struct MemoryStruct * );
int findInTable( double );

void createhdr( double, struct SmtpWrite * );
void delhdr( struct SmtpWrite * );
void setsmtp( CURLM *, struct curl_slist **, struct SmtpWrite * );
size_t write_smtp( void *, size_t, size_t, void * );
void debugsmtp( char * );

FILE *setftp( CURLM *, char * );
BitCoinCode doftp( CURLM * );

void setapihistory( CURLM *, struct curl_slist **, struct MemoryStruct * );
BitCoinCode doapihistory( CURLM * );

void exitfun( BitCoinCode, int );
