
#include "bitcoinkurs.h"


void setsmtp( CURLM *easy_handle, struct curl_slist **recipients, struct SmtpWrite *state )
{
//  curl_easy_setopt( easy_handle, CURLOPT_VERBOSE, 1L );
    curl_easy_setopt( easy_handle, CURLOPT_USE_SSL, (long)CURLUSESSL_TRY );

#ifdef USEWEBDE
    curl_easy_setopt( easy_handle, CURLOPT_URL, "smtp://mail.web.de:587" );
    curl_easy_setopt( easy_handle, CURLOPT_MAIL_FROM, "<jws1954@web.de>" );
    curl_easy_setopt( easy_handle, CURLOPT_USERPWD, SMTPWEBCREDENTIALS );
#endif

#ifdef USEGMXDE
    curl_easy_setopt( easy_handle, CURLOPT_URL, "smtp://mail.gmx.de:465" );
    curl_easy_setopt( easy_handle, CURLOPT_MAIL_FROM, "<jws1954@gmx.de>" );
    curl_easy_setopt( easy_handle, CURLOPT_USERPWD, SMTPGMXCREDENTIALS );
#endif

    curl_easy_setopt( easy_handle, CURLOPT_SSL_VERIFYPEER, 0L );
    curl_easy_setopt( easy_handle, CURLOPT_SSL_VERIFYHOST, 0L );

    *recipients = curl_slist_append( *recipients, "<jwschuetz1954@icloud.com>" );
    curl_easy_setopt( easy_handle, CURLOPT_MAIL_RCPT, *recipients );

    curl_easy_setopt( easy_handle, CURLOPT_READDATA, state );
    curl_easy_setopt( easy_handle, CURLOPT_READFUNCTION, write_smtp );
    curl_easy_setopt( easy_handle, CURLOPT_UPLOAD, 1L );
}

FILE *setftp( CURLM *easy_handle, char *filename )
{
    void *ret;
    int n = strlen( DISKSTATION ) + strlen( filename ) + 1;
    char *remotefilename = (char *)malloc( n );
    memset( remotefilename, 0, n );

    remotefilename = strcat( remotefilename, DISKSTATION );
    remotefilename = strcat( remotefilename, filename );
 
    curl_easy_setopt( easy_handle, CURLOPT_URL, remotefilename );

    free( remotefilename );

    FILE *file = fopen( filename, "r" );
    curl_easy_setopt( easy_handle, CURLOPT_READDATA, (void *)file );

    curl_easy_setopt( easy_handle, CURLOPT_UPLOAD, 1L );
    curl_easy_setopt( easy_handle, CURLOPT_FTP_CREATE_MISSING_DIRS, CURLFTP_CREATE_DIR );

    curl_easy_setopt( easy_handle, CURLOPT_USERPWD, FTPCREDENTIALS );

    return( file );
}

void setpoll( CURLM *easy_handle, struct curl_slist **headers, struct MemoryStruct *content )
{
    curl_easy_setopt( easy_handle, CURLOPT_URL, URL );
    curl_easy_setopt( easy_handle, CURLOPT_HTTPGET, 1L );
    curl_easy_setopt( easy_handle, CURLOPT_HTTP_VERSION, CURL_HTTP_VERSION_1_1 );
    curl_easy_setopt( easy_handle, CURLOPT_WRITEFUNCTION, write_data );
    curl_easy_setopt( easy_handle, CURLOPT_WRITEDATA, (void *)content );

    *headers = curl_slist_append( *headers, "Host: www.bitcoin.de" );
    *headers = curl_slist_append( *headers, "User-Agent: MATLAB 9.5.0.1067069 (R2018b) Update 4" );
    *headers = curl_slist_append( *headers, "Connection: keep-alive" );
    *headers = curl_slist_append( *headers, "CharacterEncoding: auto" );
    *headers = curl_slist_append( *headers, "ContentType: text/html" );
    *headers = curl_slist_append( *headers, "MediaType: application/x-www-form-urlencoded" );
    *headers = curl_slist_append( *headers, "CharacterSet: utf-8" );

    // pass our list of custom made headers
    curl_easy_setopt( easy_handle, CURLOPT_HTTPHEADER, *headers );
}

void setapihistory( CURLM *easy_handle, struct curl_slist **headers, struct MemoryStruct *content )
{
    curl_easy_setopt( easy_handle, CURLOPT_URL, URLAPIHISTORY );
    curl_easy_setopt( easy_handle, CURLOPT_HTTPGET, 1L );
    curl_easy_setopt( easy_handle, CURLOPT_HTTP_VERSION, CURL_HTTP_VERSION_1_1 );
    curl_easy_setopt( easy_handle, CURLOPT_WRITEFUNCTION, write_data );
    curl_easy_setopt( easy_handle, CURLOPT_WRITEDATA, (void *)content );

    *headers = curl_slist_append( *headers, "Host: www.bitcoin.de" );
    *headers = curl_slist_append( *headers, "User-Agent: MATLAB 9.5.0.1067069 (R2018b) Update 4" );
    *headers = curl_slist_append( *headers, "Connection: keep-alive" );
    *headers = curl_slist_append( *headers, "CharacterEncoding: auto" );
    *headers = curl_slist_append( *headers, "ContentType: text/html" );
    *headers = curl_slist_append( *headers, "MediaType: application/x-www-form-urlencoded" );
    *headers = curl_slist_append( *headers, "CharacterSet: utf-8" );

    // pass our list of custom made headers
    curl_easy_setopt( easy_handle, CURLOPT_HTTPHEADER, *headers );
}