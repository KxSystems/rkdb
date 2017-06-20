
ZK rexec(int type, K x);
ZK kintv(J len, int *val);
ZK kinta(J len, int rank, int *shape, int *val);
ZK kdoublev(J len, double *val);
ZK kdoublea(J len, int rank, int *shape, double *val);
ZK from_any_robject(SEXP sxp);

__thread int ROPEN= 0; // initialise thread-local. Will fail in other threads.
                       // Ideally need to check if on q main thread.

/*
 * convert R SEXP into K object.
 */
ZK from_any_robject(SEXP);
ZK error_broken_robject(SEXP);
ZK from_null_robject(SEXP);
ZK from_symbol_robject(SEXP);
ZK from_pairlist_robject(SEXP);
ZK from_closure_robject(SEXP);
ZK from_language_robject(SEXP);
ZK from_char_robject(SEXP);
ZK from_logical_robject(SEXP);
ZK from_integer_robject(SEXP);
ZK from_double_robject(SEXP);
ZK from_character_robject(SEXP);
ZK from_vector_robject(SEXP);
ZK from_raw_robject(SEXP sxp);
ZK from_nyi_robject(S m, SEXP sxp);

ZK from_any_robject(SEXP sxp) {
  K result= 0;
  int type= TYPEOF(sxp);
  switch(type) {
  case NILSXP:
    return from_null_robject(sxp);
    break; /* nil = NULL */
  case SYMSXP:
    return from_symbol_robject(sxp);
    break; /* symbols */
  case LISTSXP:
    return from_pairlist_robject(sxp);
    break; /* lists of dotted pairs */
  case CLOSXP:
    return from_closure_robject(sxp);
    break; /* closures */
  case ENVSXP:
    return from_nyi_robject("environment", sxp);
    break; /* environments */
  case PROMSXP:
    return from_nyi_robject("promise", sxp);
    break; /* promises: [un]evaluated closure arguments */
  case LANGSXP:
    return from_language_robject(sxp);
    break; /* language constructs (special lists) */
  case SPECIALSXP:
    return from_nyi_robject("special", sxp);
    break; /* special forms */
  case BUILTINSXP:
    return from_nyi_robject("builtin", sxp);
    break; /* builtin non-special forms */
  case CHARSXP:
    return from_char_robject(sxp);
    break; /* "scalar" string type (internal only)*/
  case LGLSXP:
    return from_logical_robject(sxp);
    break; /* logical vectors */
  case INTSXP:
    return from_integer_robject(sxp);
    break; /* integer vectors */
  case REALSXP:
    return from_double_robject(sxp);
    break; /* real variables */
  case CPLXSXP:
    return from_nyi_robject("complex", sxp);
    break; /* complex variables */
  case STRSXP:
    return from_character_robject(sxp);
    break; /* string vectors */
  case DOTSXP:
    return from_nyi_robject("dot", sxp);
    break; /* dot-dot-dot object */
  case ANYSXP:
    return error_broken_robject(sxp);
    break; /* make "any" args work */
  case VECSXP:
    return from_vector_robject(sxp);
    break; /* generic vectors */
  case EXPRSXP:
    return from_nyi_robject("exprlist", sxp);
    break; /* sxps vectors */
  case BCODESXP:
    return from_nyi_robject("bcode", sxp);
    break; /* byte code */
  case EXTPTRSXP:
    return from_nyi_robject("external", sxp);
    break; /* external pointer */
  case WEAKREFSXP:
    return error_broken_robject(sxp);
    break; /* weak reference */
  case RAWSXP:
    return from_raw_robject(sxp);
    break; /* raw bytes */
  case S4SXP:
    return from_nyi_robject("s4", sxp);
    break; /* S4 non-vector */

  case NEWSXP:
    return error_broken_robject(sxp);
    break; /* fresh node created in new page */
  case FREESXP:
    return error_broken_robject(sxp);
    break; /* node released by GC */
  case FUNSXP:
    return from_nyi_robject("fun", sxp);
    break; /* Closure or Builtin */
  }
  return result;
}

ZK dictpairlist(SEXP sxp) {
  K k= ktn(0, length(sxp));
  K v= ktn(0, length(sxp));
  SEXP s= sxp;
  for(int i= 0; i < length(sxp); i++) {
    kK(k)[i]= from_any_robject(TAG(s));
    kK(v)[i]= from_any_robject(CAR(s));
    s= CDR(s);
  }
  return xD(k, v);
}

/* add attribute */
ZK addattR(K x, SEXP att) {
  // attrs are pairlists: LISTSXP
  K u= dictpairlist(att);
  return knk(2, u, x);
}

/* add attribute if any */
ZK attR(K x, SEXP sxp) {
  SEXP att= ATTRIB(sxp);
  if(isNull(att))
    return x;
  return addattR(x, att);
}

ZK error_broken_robject(SEXP sxp) { return krr("Broken R object."); }

ZK from_nyi_robject(S marker, SEXP sxp) {
  return attR(kp((S) Rf_type2char(TYPEOF(sxp))), sxp);
}

ZK from_raw_robject(SEXP sxp) {
  K x= ktn(KG, XLENGTH(sxp));
  DO(xn, kG(x)[i]= RAW(sxp)[i])
  return x;
}

// NULL in R(R_NilValue): often used as generic zero length vector
// NULL objects cannot have attributes and attempting to assign one by attr
// gives an error
ZK from_null_robject(SEXP sxp) { return knk(0); }

ZK from_symbol_robject(SEXP sxp) {
  const char *t= CHAR(PRINTNAME(sxp));
  K x= ks((S) t);
  return attR(x, sxp);
}

ZK from_pairlist_robject(SEXP sxp) {
  K x= ktn(0, 2 * length(sxp));
  SEXP s= sxp;
  for(J i= 0; i < x->n; i+= 2) {
    kK(x)[i]= from_any_robject(CAR(s));
    kK(x)[i + 1]= from_any_robject(TAG(s));
    s= CDR(s);
  }
  return attR(x, sxp);
}

ZK from_closure_robject(SEXP sxp) {
  K x= from_any_robject(FORMALS(sxp));
  K y= from_any_robject(BODY(sxp));
  return attR(knk(2, x, y), sxp);
}

ZK from_language_robject(SEXP sxp) {
  K x= knk(0);
  SEXP s= sxp;
  while(0 < length(s)) {
    x= jk(&x, from_any_robject(CAR(s)));
    s= CDR(s);
  }
  return attR(x, sxp);
}

ZK from_char_robject(SEXP sxp) {
  K x= kpn((S) CHAR(STRING_ELT(sxp, 0)), LENGTH(sxp));
  return attR(x, sxp);
}

ZK from_logical_robject(SEXP sxp) {
  K x;
  J len= XLENGTH(sxp);
  int *s= malloc(len * sizeof(int));
  DO(len, s[i]= LOGICAL_POINTER(sxp)[i]);
  SEXP dim= GET_DIM(sxp);
  if(isNull(dim)) {
    x= kintv(len, s);
    free(s);
    return attR(x, sxp);
  }
  x= kinta(len, length(dim), INTEGER(dim), s);
  free(s);
  SEXP dimnames= GET_DIMNAMES(sxp);
  if(!isNull(dimnames))
    return attR(x, sxp);
  SEXP e;
  PROTECT(e= duplicate(sxp));
  SET_DIM(e, R_NilValue);
  x= attR(x, e);
  UNPROTECT(1);
  return x;
}

ZK from_integer_robject(SEXP sxp) {
  K x;
  J len= XLENGTH(sxp);
  int *s= malloc(len * sizeof(int));
  DO(len, s[i]= INTEGER_POINTER(sxp)[i]);
  SEXP dim= GET_DIM(sxp);
  if(isNull(dim)) {
    x= kintv(len, s);
    free(s);
    return attR(x, sxp);
  }
  x= kinta(len, length(dim), INTEGER(dim), s);
  free(s);
  SEXP dimnames= GET_DIMNAMES(sxp);
  if(!isNull(dimnames))
    return attR(x, sxp);
  SEXP e;
  PROTECT(e= duplicate(sxp));
  SET_DIM(e, R_NilValue);
  x= attR(x, e);
  UNPROTECT(1);
  return x;
}

ZK from_double_robject(SEXP sxp) {
  K x;
  J len= XLENGTH(sxp);
  double *s= malloc(len * sizeof(double));
  DO(len, s[i]= REAL(sxp)[i]);
  SEXP dim= GET_DIM(sxp);
  if(isNull(dim)) {
    x= kdoublev(len, s);
    free(s);
    return attR(x, sxp);
  }
  x= kdoublea(len, length(dim), INTEGER(dim), s);
  free(s);
  SEXP dimnames= GET_DIMNAMES(sxp);
  if(!isNull(dimnames))
    return attR(x, sxp);
  SEXP e;
  PROTECT(e= duplicate(sxp));
  SET_DIM(e, R_NilValue);
  x= attR(x, e);
  UNPROTECT(1);
  return x;
}

ZK from_character_robject(SEXP sxp) {
  K x;
  J i, length= XLENGTH(sxp);
  if(length == 1)
    x= kp((char *) CHAR(STRING_ELT(sxp, 0)));
  else {
    x= ktn(0, length);
    for(i= 0; i < length; i++) {
      xK[i]= kp((char *) CHAR(STRING_ELT(sxp, i)));
    }
  }
  return attR(x, sxp);
}

ZK from_vector_robject(SEXP sxp) {
  J i, length= LENGTH(sxp);
  K x= ktn(0, length);
  for(i= 0; i < length; i++) {
    xK[i]= from_any_robject(VECTOR_ELT(sxp, i));
  }
  return attR(x, sxp);
}

/*
 * various utilities
 */

/* get k string or symbol name */
static char *getkstring(K x) {
  char *s= NULL;
  int len;
  switch(xt) {
  case -KC:
    s= calloc(2, 1);
    s[0]= xg;
    break;
  case KC:
    s= calloc(1 + xn, 1);
    memcpy(s, xG, xn);
    break;
  case -KS:
    len= 1 + strlen(xs);
    s= calloc(len, 1);
    memcpy(s, xs, len);
    break;
  default:
    krr("invalid name");
  }
  return s;
}

/*
 * convert R arrays to K lists
 * done for int, double
 */

ZK kintv(J len, int *val) {
  K x= ktn(KI, len);
  DO(len, kI(x)[i]= (val)[i]);
  return x;
}

ZK kinta(J len, int rank, int *shape, int *val) {
  K x, y;
  J i, j, r, c, k;
  switch(rank) {
  case 1:
    x= kintv(len, val);
    break;
  case 2:
    r= shape[0];
    c= shape[1];
    x= knk(0);
    for(i= 0; i < r; i++) {
      y= ktn(KI, c);
      for(j= 0; j < c; j++)
        kI(y)[j]= val[i + r * j];
      x= jk(&x, y);
    };
    break;
  default:
    k= rank - 1;
    r= shape[k];
    c= len / r;
    x= knk(0);
    for(i= 0; i < r; i++)
      x= jk(&x, kinta(c, k, shape, val + c * i));
  }
  return x;
}

ZK kdoublev(J len, double *val) {
  K x= ktn(KF, len);
  DO(len, kF(x)[i]= (val)[i]);
  return x;
}

ZK kdoublea(J len, int rank, int *shape, double *val) {
  K x, y;
  J i, j, r, c, k;
  switch(rank) {
  case 1:
    x= kdoublev(len, val);
    break;
  case 2:
    r= shape[0];
    c= shape[1];
    x= knk(0);
    for(i= 0; i < r; i++) {
      y= ktn(KF, c);
      for(j= 0; j < c; j++)
        kF(y)[j]= val[i + r * j];
      x= jk(&x, y);
    };
    break;
  default:
    k= rank - 1;
    r= shape[k];
    c= len / r;
    x= knk(0);
    for(i= 0; i < r; i++)
      x= jk(&x, kdoublea(c, k, shape, val + c * i));
  }
  return x;
}