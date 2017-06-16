/*
 * This library provides a R client for kdb+
 *
 */
#include <errno.h>
#include <string.h>
#include <R.h>
#include <Rinternals.h>
#include <Rdefines.h>
#include <R_ext/Rdynload.h>
#ifdef WIN32
#include <windows.h>
#include <winbase.h>
#endif
#define KXVER 3
#include "k.h"
#include "sexp2k.c"
#include "common.c"
#include "base.c"