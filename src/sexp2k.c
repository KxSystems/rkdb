
ZK kintv(J len, int *val);
ZK kinta(J len, int rank, int *shape, int *val);
ZK klonga(J len, int rank, int *shape, J*val);
ZK kdoublev(J len, double *val);
ZK kdoublea(J len, int rank, int *shape, double *val);
ZK from_any_robject(SEXP sxp);

/*
 * convert R SEXP into K object.
 */
ZK from_any_robject(SEXP);
ZK from_null_robject(SEXP);
ZK from_symbol_robject(SEXP);
ZK dictpairlist(SEXP);
ZK from_closure_robject(SEXP);
ZK from_language_robject(SEXP);
ZK from_char_robject(SEXP);
ZK from_logical_robject(SEXP);
ZK from_integer_robject(SEXP);
ZK from_double_robject(SEXP);
ZK from_character_robject(SEXP);
ZK from_vector_robject(SEXP);
ZK from_raw_robject(SEXP);
ZK from_nyi_robject(SEXP);
ZK from_frame_robject(SEXP);
ZK from_factor_robject(SEXP);

Rboolean isClass(const char *class_, SEXP s) {
  SEXP klass;
  int i;
  if(OBJECT(s)) {
    klass= getAttrib(s, R_ClassSymbol);
    for(i= 0; i < length(klass); i++)
      if(!strcmp(CHAR(STRING_ELT(klass, i)), class_))
        return TRUE;
  }
  return FALSE;
}

ZK from_any_robject(SEXP sxp) {
  if(isClass("data.frame", sxp)) {
    return from_frame_robject(sxp);
  }
  if(isClass("factor", sxp)) {
    return from_factor_robject(sxp);
  }
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
    return dictpairlist(sxp);
    break; /* lists of dotted pairs */
  case CLOSXP:
    return from_closure_robject(sxp);
    break; /* closures */
  case LANGSXP:
    return from_language_robject(sxp);
    break; /* language constructs (special lists) */
  case CHARSXP:
    return from_char_robject(sxp);
    break; /* "scalar" string type (internal only)*/
  case LGLSXP:
    return from_logical_robject(sxp);
    break; /* logical vectors */
  case RAWSXP:
    return from_raw_robject(sxp);
    break; /* raw bytes */
  case INTSXP:
    return from_integer_robject(sxp);
    break; /* integer vectors */
  case REALSXP:
    return from_double_robject(sxp);
    break; /* real variables */
  case STRSXP:
    return from_character_robject(sxp);
    break; /* string vectors */
  case VECSXP:
    return from_vector_robject(sxp);
    break; /* generic vectors */
  case EXPRSXP:
  case BCODESXP: /* byte code */
  case EXTPTRSXP: /* external pointer */
  case WEAKREFSXP: /* weak reference */
  case S4SXP: /* S4 non-vector */
  case NEWSXP: /* fresh node created in new page */
  case FREESXP: /* node released by GC */
  case FUNSXP: /* Closure or Builtin */
  case PROMSXP: /* promises: [un]evaluated closure arguments */
  case SPECIALSXP: /* special forms */
  case BUILTINSXP: /* builtin non-special forms */
  case ENVSXP: /* environments */
  case CPLXSXP: /* complex variables */
  case DOTSXP: /* dot-dot-dot object */
  case ANYSXP: /* make "any" args work */
    return from_nyi_robject(sxp);
    break;
  }
  return result;
}

ZK dictpairlist(SEXP sxp) {
  K k= knk(0);
  K v= knk(0);
  SEXP s= sxp;
  while(s!=R_NilValue){
    jk(&k,from_any_robject(TAG(s)));
    jk(&v,from_any_robject(CAR(s)));
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

ZK from_nyi_robject(SEXP sxp) {
  return attR(kp((S)type2char(TYPEOF(sxp))), sxp);
}

ZK from_frame_robject(SEXP sxp) {
  J length= XLENGTH(sxp);
  SEXP colNames= getAttrib(sxp, R_NamesSymbol);
  
  K k= ktn(KS, length), v= ktn(0, length);
  for(J i= 0; i < length; i++) {
    kK(v)[i]= from_any_robject(VECTOR_ELT(sxp, i));
    const char *colName= CHAR(STRING_ELT(colNames, i));
    kS(k)[i]= ss((S) colName);
  }

  K tbl= xT(xD(k, v));
  return tbl;
}

ZK from_factor_robject(SEXP sxp) {
  J length= XLENGTH(sxp);
  SEXP levels= asCharacterFactor(sxp);
  K x= ktn(KS, length);
  for(J i= 0; i < length; i++) {
    const char *sym= CHAR(STRING_ELT(levels, i));
    kS(x)[i]= ss((S) sym);
  }
  return x;
}

ZK from_raw_robject(SEXP sxp) {
  K x= ktn(KG, XLENGTH(sxp));
  G*r=RAW(sxp);
  DO(xn, kG(x)[i]= r[i])
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
  K x= kpn((S) CHAR(STRING_ELT(sxp, 0)), XLENGTH(sxp));
  return attR(x, sxp);
}

ZK from_logical_robject(SEXP sxp) {
  K x;
  J len= XLENGTH(sxp);
  if(!isMatrix(sxp)) {
    x= kintv(len, LOGICAL(sxp));
    return attR(x, sxp);
  }
  SEXP dim= getAttrib(sxp, R_DimSymbol);
  x= kinta(len, length(dim), INTEGER(dim), LOGICAL(sxp));
  SEXP dimnames= getAttrib(sxp, R_DimNamesSymbol);
  if(!isNull(dimnames))
    return attR(x, sxp);
  SEXP e;
  PROTECT(e= duplicate(sxp));
  setAttrib(e, R_DimSymbol, R_NilValue);
  x= attR(x, e);
  UNPROTECT(1);
  return x;
}

ZK from_integer_robject(SEXP sxp) {
  K x;
  J len= XLENGTH(sxp);
  if(!isMatrix(sxp)) {
    x= kintv(len, INTEGER(sxp));
    return attR(x, sxp);
  }
  SEXP dim= getAttrib(sxp, R_DimSymbol);
  x= kinta(len, length(dim), INTEGER(dim), INTEGER(sxp));
  SEXP dimnames= getAttrib(sxp, R_DimNamesSymbol);
  if(!isNull(dimnames))
    return attR(x, sxp);
  SEXP e;
  PROTECT(e= duplicate(sxp));
  setAttrib(e, R_DimSymbol, R_NilValue);
  x= attR(x, e);
  UNPROTECT(1);
  return x;
}

ZK from_double_robject(SEXP sxp) {
  K x;I nano,bit64=isClass("integer64",sxp);
  J len= XLENGTH(sxp);
  if(!isMatrix(sxp)) {
    nano = isClass("nanotime",sxp);
    if(nano || bit64) {
      x=ktn(nano?KP:KJ,len);
      DO(len,kJ(x)[i]=INT64(sxp)[i])
      if(nano)
        DO(len,if(kJ(x)[i]!=nj)kJ(x)[i]-=epoch_offset)
      return x;
    }
    x= kdoublev(len, REAL(sxp));
    return attR(x, sxp);  
  }
  SEXP dim= getAttrib(sxp, R_DimSymbol);
  if(bit64){
    x= klonga(len, length(dim), INTEGER(dim), (J*)REAL(sxp));
  }else{
    x= kdoublea(len, length(dim), INTEGER(dim), REAL(sxp));
  }
  SEXP dimnames= getAttrib(sxp, R_DimNamesSymbol);
  if(!isNull(dimnames))
    return attR(x, sxp);
  SEXP e;
  PROTECT(e= duplicate(sxp));
  setAttrib(e, R_DimSymbol, R_NilValue);
  if(bit64) classgets(e,R_NilValue);
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
      kK(x)[i]= kp((char *) CHAR(STRING_ELT(sxp, i)));
    }
  }
  return attR(x, sxp);
}

ZK from_vector_robject(SEXP sxp) {
  J i, length= XLENGTH(sxp);
  K x= ktn(0, length);
  for(i= 0; i < length; i++) {
    kK(x)[i]= from_any_robject(VECTOR_ELT(sxp, i));
  }
  SEXP colNames= getAttrib(sxp, R_NamesSymbol);
  if(!isNull(colNames)&&length==XLENGTH(colNames)){
    K k= ktn(KS, length);
    for(i= 0; i < length; i++) {
      const char *colName= CHAR(STRING_ELT(colNames, i));
      kS(k)[i]= ss((S) colName);
    }
    return xD(k,x);
  }
  return attR(x, sxp);
}

/*
 * various utilities
 */

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

ZK klonga(J len, int rank, int *shape, J*val) {
  K x, y;
  J i, j, r, c, k;
  switch(rank) {
  case 1:
    x= ktn(KJ,len);
    DO(len, kJ(x)[i]=val[i])
    break;
  case 2:
    r= shape[0];
    c= shape[1];
    x= ktn(0,r);
    for(i= 0; i < r; i++) {
      y= ktn(KJ, c);
      for(j= 0; j < c; j++)
        kJ(y)[j]= val[i + r * j];
      kK(x)[i]=y;
    };
    break;
  default:
    k= rank - 1;
    r= shape[k];
    c= len / r;
    x= ktn(0,r);
    for(i= 0; i < r; i++)
      kK(x)[i] = klonga(c, k, shape, val + c * i);
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