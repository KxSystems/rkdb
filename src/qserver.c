/*
 * This library provides a Q server for R
 *
 * See kx wiki https://code.kx.com/trac/wiki/Cookbook/IntegratingWithR
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
#include "k.h"

#include "common.c"
#include "base.c"
