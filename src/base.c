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
EXPORT SEXP kx_r_execute(SEXP c, SEXP);

/*
 * Open a connection to an existing kdb+ process.
 *
 * If we just have a host and port we call khp from the kdb+ interface.
 * If we have a host, port, "username:password" we call instead khpu.
 */
SEXP kx_r_open_connection(SEXP whence) {
  SEXP result;
  int connection, port;
  char *host;
  int length= GET_LENGTH(whence);
  if(length < 2)
    error("Can't connect with so few parameters..");

  port= INTEGER_POINTER(VECTOR_ELT(whence, 1))[0];
  host= (char *) CHARACTER_VALUE(VECTOR_ELT(whence, 0));

  if(2 == length)
    connection= khp(host, port);
  else {
    char *user= (char *) CHARACTER_VALUE(VECTOR_ELT(whence, 2));
    connection= khpu(host, port, user);
  }
  if(!connection)
    error("Could not authenticate");
  else if(connection < 0) {
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
SEXP kx_r_execute0(SEXP connection, SEXP query)
{
  K result;
  SEXP s;
  kx_connection = INTEGER_VALUE(connection);

  result = k(kx_connection, (char*) CHARACTER_VALUE(query), (K)0);
  if(kx_connection < 0){
    PROTECT(s = NEW_INTEGER(1));
    INTEGER_POINTER(s)[0] = 0;
    UNPROTECT(1);
    return s;   
  }
  if (0 == result) {
    error("Error: not connected to kdb+ server\n");
  }
  else if (-128 == result->t) {
    char *e = calloc(strlen(result->s) + 1, 1);
    strcpy(e, result->s);
    r0(result);
    error("Error from kdb+: `%s\n", e);
  }
  s = from_any_kobject(result);
  r0(result);
  return s;
}

SEXP kx_r_execute1(SEXP connection,SEXP query, SEXP arg1)
{
  K result;
  SEXP s;
  kx_connection = INTEGER_VALUE(connection);

  K rarg1=from_any_robject(arg1);

  result = k(kx_connection,(char*) CHARACTER_VALUE(query),rarg1,(K)0 );
  if(kx_connection < 0){
    PROTECT(s = NEW_INTEGER(1));
    INTEGER_POINTER(s)[0] = 0;
    UNPROTECT(1);
    return s;   
  } 
  if (0 == result) {
    error("Error: not connected to kdb+ server\n");
  }
  else if (-128 == result->t) {
    char *e = calloc(strlen(result->s) + 1, 1);
    strcpy(e, result->s);
    r0(result);
    error("Error from kdb+: `%s\n", e);
  }
  s = from_any_kobject(result);
  r0(result);
  return s;
}

SEXP kx_r_execute2(SEXP connection,SEXP query, SEXP arg1, SEXP arg2)
{
  K result;
  SEXP s;
  kx_connection = INTEGER_VALUE(connection);

  K rarg1=from_any_robject(arg1);
  K rarg2=from_any_robject(arg2); 

  result = k(kx_connection,(char*) CHARACTER_VALUE(query),rarg1,rarg2,(K)0 );
  if(kx_connection < 0){
    PROTECT(s = NEW_INTEGER(1));
    INTEGER_POINTER(s)[0] = 0;
    UNPROTECT(1);
    return s;   
  } 
  if (0 == result) {
    error("Error: not connected to kdb+ server\n");
  }
  else if (-128 == result->t) {
    char *e = calloc(strlen(result->s) + 1, 1);
    strcpy(e, result->s);
    r0(result);
    error("Error from kdb+: `%s\n", e);
  }
  s = from_any_kobject(result);
  r0(result);
  return s;
}

SEXP kx_r_execute3(SEXP connection,SEXP query, SEXP arg1, SEXP arg2, SEXP arg3)
{
  K result;
  SEXP s;
  kx_connection = INTEGER_VALUE(connection);

  K rarg1=from_any_robject(arg1);
  K rarg2=from_any_robject(arg2);
  K rarg3=from_any_robject(arg3);   

  result = k(kx_connection,(char*) CHARACTER_VALUE(query),rarg1,rarg2,rarg3,(K)0 );
  if(kx_connection < 0){
    PROTECT(s = NEW_INTEGER(1));
    INTEGER_POINTER(s)[0] = 0;
    UNPROTECT(1);
    return s;   
  } 
  if (0 == result) {
    error("Error: not connected to kdb+ server\n");
  }
  else if (-128 == result->t) {
    char *e = calloc(strlen(result->s) + 1, 1);
    strcpy(e, result->s);
    r0(result);
    error("Error from kdb+: `%s\n", e);
  }
  s = from_any_kobject(result);
  r0(result);
  return s;
}

SEXP kx_r_execute4(SEXP connection,SEXP query, SEXP arg1, SEXP arg2, SEXP arg3, SEXP arg4)
{
  K result;
  SEXP s;
  kx_connection = INTEGER_VALUE(connection);

  K rarg1=from_any_robject(arg1);
  K rarg2=from_any_robject(arg2);
  K rarg3=from_any_robject(arg3);
  K rarg4=from_any_robject(arg4);     

  result = k(kx_connection,(char*) CHARACTER_VALUE(query),rarg1,rarg2,rarg3,rarg4,(K)0 );
  if(kx_connection < 0){
    PROTECT(s = NEW_INTEGER(1));
    INTEGER_POINTER(s)[0] = 0;
    UNPROTECT(1);
    return s;   
  } 
  if (0 == result) {
    error("Error: not connected to kdb+ server\n");
  }
  else if (-128 == result->t) {
    char *e = calloc(strlen(result->s) + 1, 1);
    strcpy(e, result->s);
    r0(result);
    error("Error from kdb+: `%s\n", e);
  }
  s = from_any_kobject(result);
  r0(result);
  return s;
}

SEXP kx_r_execute5(SEXP connection,SEXP query, SEXP arg1, SEXP arg2, SEXP arg3, SEXP arg4, SEXP arg5)
{
  K result;
  SEXP s;
  kx_connection = INTEGER_VALUE(connection);

  K rarg1=from_any_robject(arg1);
  K rarg2=from_any_robject(arg2);
  K rarg3=from_any_robject(arg3);
  K rarg4=from_any_robject(arg4);
  K rarg5=from_any_robject(arg5);       

  result = k(kx_connection,(char*) CHARACTER_VALUE(query),rarg1,rarg2,rarg3,rarg4,rarg5,(K)0 );
  if(kx_connection < 0){
    PROTECT(s = NEW_INTEGER(1));
    INTEGER_POINTER(s)[0] = 0;
    UNPROTECT(1);
    return s;   
  } 
  if (0 == result) {
    error("Error: not connected to kdb+ server\n");
  }
  else if (-128 == result->t) {
    char *e = calloc(strlen(result->s) + 1, 1);
    strcpy(e, result->s);
    r0(result);
    error("Error from kdb+: `%s\n", e);
  }
  s = from_any_kobject(result);
  r0(result);
  return s;
}

SEXP kx_r_execute6(SEXP connection,SEXP query, SEXP arg1, SEXP arg2, SEXP arg3, SEXP arg4, SEXP arg5, SEXP arg6)
{
  K result;
  SEXP s;
  kx_connection = INTEGER_VALUE(connection);

  K rarg1=from_any_robject(arg1);
  K rarg2=from_any_robject(arg2);
  K rarg3=from_any_robject(arg3);
  K rarg4=from_any_robject(arg4);
  K rarg5=from_any_robject(arg5);
  K rarg6=from_any_robject(arg6);         

  result = k(kx_connection,(char*) CHARACTER_VALUE(query),rarg1,rarg2,rarg3,rarg4,rarg5,rarg6,(K)0 );
  if(kx_connection < 0){
    PROTECT(s = NEW_INTEGER(1));
    INTEGER_POINTER(s)[0] = 0;
    UNPROTECT(1);
    return s;   
  } 
  if (0 == result) {
    error("Error: not connected to kdb+ server\n");
  }
  else if (-128 == result->t) {
    char *e = calloc(strlen(result->s) + 1, 1);
    strcpy(e, result->s);
    r0(result);
    error("Error from kdb+: `%s\n", e);
  }
  s = from_any_kobject(result);
  r0(result);
  return s;
}

SEXP kx_r_execute7(SEXP connection,SEXP query, SEXP arg1, SEXP arg2, SEXP arg3, SEXP arg4, SEXP arg5, SEXP arg6, SEXP arg7)
{
  K result;
  SEXP s;
  kx_connection = INTEGER_VALUE(connection);

  K rarg1=from_any_robject(arg1);
  K rarg2=from_any_robject(arg2);
  K rarg3=from_any_robject(arg3);
  K rarg4=from_any_robject(arg4);
  K rarg5=from_any_robject(arg5);
  K rarg6=from_any_robject(arg6);
  K rarg7=from_any_robject(arg7);           

  result = k(kx_connection,(char*) CHARACTER_VALUE(query),rarg1,rarg2,rarg3,rarg4,rarg5,rarg6,rarg7,(K)0 );
  if(kx_connection < 0){
    PROTECT(s = NEW_INTEGER(1));
    INTEGER_POINTER(s)[0] = 0;
    UNPROTECT(1);
    return s;   
  } 
  if (0 == result) {
    error("Error: not connected to kdb+ server\n");
  }
  else if (-128 == result->t) {
    char *e = calloc(strlen(result->s) + 1, 1);
    strcpy(e, result->s);
    r0(result);
    error("Error from kdb+: `%s\n", e);
  }
  s = from_any_kobject(result);
  r0(result);
  return s;
}

static const
R_CallMethodDef callMethods[] = {
  {"kx_r_open_connection", (DL_FUNC) &kx_r_open_connection, -1},
  {"kx_r_close_connection", (DL_FUNC) &kx_r_close_connection, -1},
  {"kx_r_execute0", (DL_FUNC) &kx_r_execute0, -1},
  {"kx_r_execute1", (DL_FUNC) &kx_r_execute1, -1},
  {"kx_r_execute2", (DL_FUNC) &kx_r_execute2, -1},
  {"kx_r_execute3", (DL_FUNC) &kx_r_execute3, -1},
  {"kx_r_execute4", (DL_FUNC) &kx_r_execute4, -1},
  {"kx_r_execute5", (DL_FUNC) &kx_r_execute5, -1},
  {"kx_r_execute6", (DL_FUNC) &kx_r_execute6, -1},
  {"kx_r_execute7", (DL_FUNC) &kx_r_execute7, -1},              
  {NULL, NULL, 0}
};

void R_init_qserver(DllInfo *info)
{
    R_registerRoutines(info, NULL, callMethods, NULL, NULL);
    R_useDynamicSymbols(info, FALSE);
}
