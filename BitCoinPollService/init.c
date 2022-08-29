
#include "bitcoinkurs.h"


double *tabelle;
int oldTableIndex;
struct Result *prev_res;

void init()
{
  // Preistabelle initialisieren, 0, 100, 200, 300, ..., MAXBITCOINPRICE
  int N = MAXBITCOINPRICE / BITCOINPRICESTEP + 1;
  tabelle = (double *)malloc( N * sizeof( double ) );
  for( int n = 0; n < N; ++n ) tabelle[n] = n * BITCOINPRICESTEP;

  oldTableIndex = NOINDEX;

  // Result Struktur initialisieren
  prev_res = NULL;
}