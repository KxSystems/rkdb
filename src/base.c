/*
 * Q server for R
 */

/*
 * The public interface used from R.
 */

#ifdef WIN32
#define EXPORT __declspec(dllexport)
#else
#define EXPORT
#endif

EXPORT SEXP kx_r_open_connection(SEXP);
EXPORT SEXP kx_r_close_connection(SEXP);
EXPORT SEXP kx_r_execute(SEXP c, SEXP, SEXP);

/*
 * Open a connection to an existing kdb+ process.
 *
 * Defaults are passed from R level. Always 5 params.
 * If we have a host, port, "username:password" we call instead khpu.
 */
SEXP kx_r_open_connection(SEXP whence) {
  SEXP result;
  int connection, port, timeout,tls,cap;
  char *host,*user;
  int length= GET_LENGTH(whence);
  if(length != 5)
    error("Expecting 5 parameters: host, port, user, timeout, tls. Got %d.",length);

  host= (char *) CHARACTER_VALUE(VECTOR_ELT(whence, 0));
  port= INTEGER_POINTER(VECTOR_ELT(whence, 1))[0];
  user= (char *) CHARACTER_VALUE(VECTOR_ELT(whence, 2));
  timeout= INTEGER_POINTER(VECTOR_ELT(whence, 3))[0];
  tls= INTEGER_POINTER(VECTOR_ELT(whence, 4))[0];
  cap = tls<<1|1;
  connection= khpunc(host, port, user,timeout,cap);
  if(!connection)
    error("Could not authenticate.");
  else if(connection == -3)
    error("OpenSSL initialisation error.");
  else if(connection ==-2 )
    error("Connection timed out.");
  else if(connection ==-1 ) {
#ifdef WIN32
    char buf[256];
    FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM, NULL, GetLastError(),
                  MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), buf, 256, NULL);
    error(buf);
#else
    error(strerror(errno));
#endif
  }
  PROTECT(result= NEW_INTEGER(1));
  INTEGER_POINTER(result)[0]= connection;
  UNPROTECT(1);
  return result;
}

/*
 * Close a connection to an existing kdb+ process.
 */
SEXP kx_r_close_connection(SEXP connection) {
  SEXP result;

  /* Close the connection. */
  kclose(INTEGER_VALUE(connection));

  PROTECT(result= NEW_INTEGER(1));
  INTEGER_POINTER(result)[0]= 0;
  UNPROTECT(1);
  return result;
}

/*
 * Execute a kdb+ query over the given connection.
 */
SEXP kx_r_execute(SEXP connection, SEXP query, SEXP args) {
  K result;
  SEXP s;
  char *query_str;
  kx_connection= INTEGER_VALUE(connection);
  size_t nargs= LENGTH(args);

  if(nargs > 8) {
    error("kdb+ functions take a maximum of 8 parameters.");
  }
  if(TYPEOF(query) != STRSXP) {
    error("Supplied query or function name must be a string.");
  }
  query_str= (char *) CHARACTER_VALUE(query);
  K kargs[8]= { (K) 0 };
  for(size_t i= 0; i < nargs; i++) {
    kargs[i]= from_any_robject(VECTOR_ELT(args, i));
  }
  result= k(kx_connection, query_str, kargs[0], kargs[1], kargs[2], kargs[3],
            kargs[4], kargs[5], kargs[6], kargs[7], (K) 0);

  if(0 == result) {
    error("Not connected to kdb+ server.");
  } else if(kx_connection < 0) { // async IPC
    return R_NilValue;
  } else if(-128 == result->t) {
    char *e= calloc(strlen(result->s) + 1, 1);
    strcpy(e, result->s);
    r0(result);
    error("kdb+ : %s.", e);
  }
  s= from_any_kobject(result);
  r0(result);
  return s;
}

static const R_CallMethodDef callMethods[]= {
  { "kx_r_open_connection", (DL_FUNC) &kx_r_open_connection, -1 },
  { "kx_r_close_connection", (DL_FUNC) &kx_r_close_connection, -1 },
  { "kx_r_execute", (DL_FUNC) &kx_r_execute, -1 },
  { NULL, NULL, 0 }
};

void R_init_rkdb(DllInfo *info) {
  R_registerRoutines(info, NULL, callMethods, NULL, NULL);
  R_useDynamicSymbols(info, FALSE);
}
