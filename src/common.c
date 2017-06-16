/*
 * common code for Q/R interface
 */

int kx_connection= 0;

/*
 * A (readable type name, R data type number) pair.
 */
struct data_types {
  char *name;
  Sint id;
};

/*
 * A mapping from readable names to R data type numbers.
 */
const struct data_types r_data_types[]= { { "unknown", -1 },
                                          { "NULL", NILSXP },
                                          { "symbol", SYMSXP },
                                          { "pairlist", LISTSXP },
                                          { "closure", CLOSXP },
                                          { "environment", ENVSXP },
                                          { "promise", PROMSXP },
                                          { "language", LANGSXP },
                                          { "special", SPECIALSXP },
                                          { "builtin", BUILTINSXP },
                                          { "char", CHARSXP },
                                          { "logical", LGLSXP },
                                          { "integer", INTSXP },
                                          { "double", REALSXP },
                                          { "complex", CPLXSXP },
                                          { "character", STRSXP },
                                          { "...", DOTSXP },
                                          { "any", ANYSXP },
                                          { "expression", EXPRSXP },
                                          { "list", VECSXP },
                                          { "numeric", REALSXP },
                                          { "name", SYMSXP },
                                          { 0, -1 } };

/*
 * Brute force search of R type table.
 * eg. 	get_type_name(LISTSXP)
 */
char *get_type_name(Sint type) {
  int i;
  for(i= 1; r_data_types[i].name != 0; i++) {
    if(type == r_data_types[i].id)
      return r_data_types[i].name;
  }
  return r_data_types[0].name;
}

/*
 * Given the appropriate names, types, and lengths, create an R named list.
 */
SEXP make_named_list(char **names, SEXPTYPE *types, Sint *lengths, Sint n) {
  SEXP output, output_names, object= NULL_USER_OBJECT;
  Sint elements;
  int i;

  PROTECT(output= NEW_LIST(n));
  PROTECT(output_names= NEW_CHARACTER(n));

  for(i= 0; i < n; i++) {
    elements= lengths[i];
    switch((int) types[i]) {
    case LGLSXP:
      PROTECT(object= NEW_LOGICAL(elements));
      break;
    case INTSXP:
      PROTECT(object= NEW_INTEGER(elements));
      break;
    case REALSXP:
      PROTECT(object= NEW_NUMERIC(elements));
      break;
    case STRSXP:
      PROTECT(object= NEW_CHARACTER(elements));
      break;
    case VECSXP:
      PROTECT(object= NEW_LIST(elements));
      break;
    default:
      error("Unsupported data type at %d %s\n", __LINE__, __FILE__);
    }
    SET_VECTOR_ELT(output, (Sint) i, object);
    SET_STRING_ELT(output_names, i, COPY_TO_USER_STRING(names[i]));
  }
  SET_NAMES(output, output_names);
  UNPROTECT(n + 2);
  return output;
}

/*
 * Make a data.frame from a named list by adding row.names, and class
 * attribute. Uses "1", "2", .. as row.names.
 */
void make_data_frame(SEXP data) {
  SEXP class_name, row_names;
  Sint n;
  PROTECT(data);
  PROTECT(class_name= NEW_CHARACTER((Sint) 1));
  SET_STRING_ELT(class_name, 0, COPY_TO_USER_STRING("data.frame"));

  /* Set the row.names. */
  n= GET_LENGTH(VECTOR_ELT(data, 0));
  PROTECT(row_names= NEW_INTEGER(2));
  INTEGER(row_names)[0]= NA_INTEGER;
  INTEGER(row_names)[1]= -n;
  setAttrib(data, R_RowNamesSymbol, row_names);
  SET_CLASS(data, class_name);
  UNPROTECT(3);
}

/* for datetime, timestamp */
static void setdatetimeclass(SEXP sxp) {
  SEXP datetimeclass= PROTECT(allocVector(STRSXP, 2));
  SET_STRING_ELT(datetimeclass, 0, mkChar("POSIXt"));
  SET_STRING_ELT(datetimeclass, 1, mkChar("POSIXct"));
  setAttrib(sxp, R_ClassSymbol, datetimeclass);
  UNPROTECT(2);
}

/*
 * We have functions that turn any K object into the appropriate R SEXP.
 */
static SEXP from_any_kobject(K object);
static SEXP error_broken_kobject(K);
static SEXP from_list_of_kobjects(K);
static SEXP from_bool_kobject(K);
static SEXP from_byte_kobject(K);
static SEXP from_guid_kobject(K);
static SEXP from_string_kobject(K);
static SEXP from_string_column_kobject(K);
static SEXP from_short_kobject(K);
static SEXP from_int_kobject(K);
static SEXP from_long_kobject(K);
static SEXP from_float_kobject(K);
static SEXP from_double_kobject(K);
static SEXP from_symbol_kobject(K);
static SEXP from_month_kobject(K);
static SEXP from_date_kobject(K);
static SEXP from_datetime_kobject(K);
static SEXP from_minute_kobject(K);
static SEXP from_second_kobject(K);
static SEXP from_time_kobject(K);
static SEXP from_timespan_kobject(K);
static SEXP from_timestamp_kobject(K);
static SEXP from_columns_kobject(K object);
static SEXP from_dictionary_kobject(K);
static SEXP from_table_kobject(K);

/*
 * An array of functions that deal with kdbplus data types. Note that the order
 * is very important as we index it based on the kdb+ type number in the K
 * object.
 */
typedef SEXP (*conversion_function)(K);

conversion_function kdbplus_types[]= {
  from_list_of_kobjects,  from_bool_kobject,     from_guid_kobject,
  error_broken_kobject,   from_byte_kobject,     from_short_kobject,
  from_int_kobject,       from_long_kobject,     from_float_kobject,
  from_double_kobject,    from_string_kobject,   from_symbol_kobject,
  from_timestamp_kobject, from_month_kobject,    from_date_kobject,
  from_datetime_kobject,  from_timespan_kobject, from_minute_kobject,
  from_second_kobject,    from_time_kobject
};

/*
 * Convert K object to R object
 */
static SEXP from_any_kobject(K x) {
  SEXP result;
  int type= abs(x->t);
  if(XT == type)
    result= from_table_kobject(x);
  else if(XD == type)
    result= from_dictionary_kobject(x);
  else if(105 == type || 101 == type)
    result= from_int_kobject(ki(0));
  else if(type <= KT)
    result= kdbplus_types[type](x);
  else if(KT < type && type < 77) {
    K t= k(0, "value", r1(x), (K) 0);
    if(t && t->t != -128) {
      result= from_any_kobject(t);
      r0(t);
    } else
      result= error_broken_kobject(x);
  } else if(77 < type && type < XT) {
    K t= k(0, "{(::) each x}", r1(x), (K) 0);
    if(t && t->t != -128) {
      result= from_any_kobject(t);
      r0(t);
    } else
      result= error_broken_kobject(x);
  } else
    result= error_broken_kobject(x);
  return result;
}

/*
 * Convert K columns to R object
 */
static SEXP from_columns_kobject(K x) {
  SEXP col, result;
  int i, type, length= x->n;
  K c;
  PROTECT(result= NEW_LIST(length));
  for(i= 0; i < length; i++) {
    c= kK(x)[i];
    type= abs(c->t);
    if(type == KC)
      col= from_string_column_kobject(c);
    else
      col= from_any_kobject(c);
    SET_VECTOR_ELT(result, i, col);
  }
  UNPROTECT(1);
  return result;
}

/*
 * Complain that the given K object is not valid and return "unknown".
 */
static SEXP error_broken_kobject(K broken) {
  error("Value is not a valid kdb+ object; unknown type %d\n", broken->t);
  return mkChar(r_data_types[0].name);
}

/*
 * An R list from a K list object.
 */
static SEXP from_list_of_kobjects(K x) {
  SEXP result;
  int i, length= x->n;
  PROTECT(result= NEW_LIST(length));
  for(i= 0; i < length; i++) {
    SET_VECTOR_ELT(result, i, from_any_kobject(kK(x)[i]));
  }
  UNPROTECT(1);
  return result;
}

/*
 * These next functions have 2 main control flow paths. One for scalars and
 * one for vectors. Because of the way the data is laid out in k objects, its
 * not possible to combine them.
 *
 * We always decrement the reference count of the object as it will have been
 * incremented in the initial dispatch.
 *
 * We promote shorts and floats to larger types when converting to R (ints and
 * doubles respectively).
 */

static I scalar(K x){return x->t < 0;}

static SEXP from_bool_kobject(K x) {
  SEXP result;
  int length= x->n;
  if(scalar(x)) {
    PROTECT(result= NEW_LOGICAL(1));
    LOGICAL_POINTER(result)[0]= x->g;
  } else {
    int i;
    PROTECT(result= NEW_LOGICAL(length));
    for(i= 0; i < length; i++)
      LOGICAL_POINTER(result)[i]= kG(x)[i];
  }
  UNPROTECT(1);
  return result;
}

static SEXP from_byte_kobject(K x) {
  SEXP result;
  int i, length= x->n;
  if(scalar(x)) {
    PROTECT(result= NEW_INTEGER(1));
    INTEGER_POINTER(result)[0]= (int) x->g;
  } else {
    PROTECT(result= NEW_INTEGER(length));
    for(i= 0; i < length; i++)
      INTEGER_POINTER(result)[i]= kG(x)[i];
  }
  UNPROTECT(1);
  return result;
}

static SEXP from_guid_kobject(K x) {
  K y= k(kx_connection, "string", r1(x), (K) 0);
  SEXP r= from_any_kobject(y);
  r0(y);
  return r;
}

static SEXP from_short_kobject(K x) {
  SEXP result;
  int i, length= x->n;
  if(scalar(x)) {
    PROTECT(result= NEW_INTEGER(1));
    INTEGER_POINTER(result)[0]= (int) x->h;
  } else {
    PROTECT(result= NEW_INTEGER(x->n));
    for(i= 0; i < length; i++)
      INTEGER_POINTER(result)[i]= (int) kH(x)[i];
  }
  UNPROTECT(1);
  return result;
}

static SEXP from_int_kobject(K x) {
  SEXP result;
  int i, length= x->n;
  if(scalar(x)) {
    PROTECT(result= NEW_INTEGER(1));
    INTEGER_POINTER(result)[0]= x->i;
  } else {
    PROTECT(result= NEW_INTEGER(length));
    for(i= 0; i < length; i++)
      INTEGER_POINTER(result)[i]= (int) kI(x)[i];
  }
  UNPROTECT(1);
  return result;
}

static SEXP from_long_kobject(K x) {
  SEXP result;
  int i, length= x->n;
  if(scalar(x)) {
    PROTECT(result= NEW_NUMERIC(1));
    NUMERIC_POINTER(result)[0]= (double) x->j;
  } else {
    PROTECT(result= NEW_NUMERIC(length));
    for(i= 0; i < length; i++)
      NUMERIC_POINTER(result)[i]= (double) kJ(x)[i];
  }
  UNPROTECT(1);
  return result;
}

static SEXP from_float_kobject(K x) {
  SEXP result;
  int i, length= x->n;
  if(scalar(x)) {
    PROTECT(result= NEW_NUMERIC(1));
    NUMERIC_POINTER(result)[0]= (double) x->e;
  } else {
    PROTECT(result= NEW_NUMERIC(length));
    for(i= 0; i < length; i++)
      NUMERIC_POINTER(result)[i]= (double) kE(x)[i];
  }
  UNPROTECT(1);
  return result;
}

static SEXP from_double_kobject(K x) {
  SEXP result;
  int i, length= x->n;
  if(scalar(x)) {
    PROTECT(result= NEW_NUMERIC(1));
    NUMERIC_POINTER(result)[0]= x->f;
  } else {
    PROTECT(result= NEW_NUMERIC(length));
    for(i= 0; i < length; i++)
      NUMERIC_POINTER(result)[i]= kF(x)[i];
  }
  UNPROTECT(1);
  return result;
}

static SEXP from_string_kobject(K x) {
  SEXP result;
  int length= x->n;
  if(scalar(x)) {
    PROTECT(result= NEW_CHARACTER(1));
    SET_STRING_ELT(result, 0, mkCharLen((S) &x->g, 1));
  } else {
    PROTECT(result= allocVector(STRSXP, 1));
    SET_STRING_ELT(result, 0, mkCharLen((S) kC(x), length));
  };
  UNPROTECT(1);
  return result;
}

static SEXP from_string_column_kobject(K x) {
  SEXP result;
  int i, length= x->n;
  PROTECT(result= NEW_CHARACTER(length));
  for(i= 0; i < length; i++) {
    SET_STRING_ELT(result, i, mkCharLen((S) &kC(x)[i], 1));
  }
  UNPROTECT(1);
  return result;
}

static SEXP from_symbol_kobject(K x) {
  SEXP result;
  int i, length= x->n;
  if(scalar(x)) {
    PROTECT(result= NEW_CHARACTER(1));
    SET_STRING_ELT(result, 0, mkChar(x->s));
  } else {
    PROTECT(result= NEW_CHARACTER(length));
    for(i= 0; i < length; i++)
      SET_STRING_ELT(result, i, mkChar(kS(x)[i]));
  }
  UNPROTECT(1);
  return result;
}

static SEXP from_month_kobject(K object) { return from_int_kobject(object); }

static SEXP from_date_kobject(K x) {
  SEXP result;
  SEXP dateclass;
  int i, length= x->n;
  if(scalar(x)) {
    PROTECT(result= NEW_INTEGER(1));
    INTEGER_POINTER(result)[0]= x->i + 10957;
  } else {
    PROTECT(result= NEW_INTEGER(length));
    for(i= 0; i < length; i++)
      INTEGER_POINTER(result)[i]= kI(x)[i] + 10957;
  }
  dateclass= PROTECT(allocVector(STRSXP, 1));
  SET_STRING_ELT(dateclass, 0, mkChar("Date"));
  setAttrib(result, R_ClassSymbol, dateclass);
  UNPROTECT(2);
  return result;
}

static SEXP from_datetime_kobject(K x) {
  SEXP result;
  int i, length= x->n;
  if(scalar(x)) {
    PROTECT(result= NEW_NUMERIC(1));
    NUMERIC_POINTER(result)[0]= (x->f + 10957) * 86400;
  } else {
    PROTECT(result= NEW_NUMERIC(length));
    for(i= 0; i < length; i++)
      NUMERIC_POINTER(result)[i]= (kF(x)[i] + 10957) * 86400;
  }
  setdatetimeclass(result);
  return result;
}

static SEXP from_minute_kobject(K object) { return from_int_kobject(object); }

static SEXP from_second_kobject(K object) { return from_int_kobject(object); }

static SEXP from_time_kobject(K object) { return from_int_kobject(object); }

static SEXP from_timespan_kobject(K x) {
  SEXP result;
  int i, length= x->n;
  if(scalar(x)) {
    PROTECT(result= NEW_NUMERIC(1));
    NUMERIC_POINTER(result)[0]= x->j / 1e9;
  } else {
    PROTECT(result= NEW_NUMERIC(length));
    for(i= 0; i < length; i++)
      NUMERIC_POINTER(result)[i]= kJ(x)[i] / 1e9;
  }
  UNPROTECT(1);
  return result;
}

static SEXP from_timestamp_kobject(K x) {
  SEXP result;
  int i, length= x->n;
  if(scalar(x)) {
    PROTECT(result= NEW_NUMERIC(1));
    NUMERIC_POINTER(result)[0]= 946684800 + x->j / 1e9;
  } else {
    PROTECT(result= NEW_NUMERIC(length));
    for(i= 0; i < length; i++)
      NUMERIC_POINTER(result)[i]= 946684800 + kJ(x)[i] / 1e9;
  }
  setdatetimeclass(result);
  return result;
}

static SEXP from_dictionary_kobject(K x) {
  SEXP names, result;
  K table,k=kK(x)[0],v=kK(x)[1];

  /* if keyed, try to create a simple table */
  /* ktd will free its argument if successful */
  /* if fails, x is still valid */
  if(XT == k->t && XT == v->t) {
    r1(x);
    if((table= ktd(x))) {
      result= from_table_kobject(table);
      r0(table);
      return result;
    }
    r0(x);
  }

  PROTECT(names= from_any_kobject(k));
  PROTECT(result= from_any_kobject(v));
  SET_NAMES(result, names);
  UNPROTECT(2);
  return result;
}

static SEXP from_table_kobject(K x) {
  SEXP names, result;
  PROTECT(names= from_any_kobject(kK(x->k)[0]));
  PROTECT(result= from_columns_kobject(kK(x->k)[1]));
  SET_NAMES(result, names);
  UNPROTECT(2);
  make_data_frame(result);
  return result;
}

/*
 * convert R SEXP into K object.
 */
static K kintv(int len, int *val);
static K kinta(int len, int rank, int *shape, int *val);
static K kdoublev(int len, double *val);
static K kdoublea(int len, int rank, int *shape, double *val);

static K from_any_robject(SEXP);
static K error_broken_robject(SEXP);
static K from_null_robject(SEXP);
static K from_symbol_robject(SEXP);
static K from_pairlist_robject(SEXP);
static K from_closure_robject(SEXP);
static K from_environment_robject(SEXP);
static K from_promise_robject(SEXP);
static K from_language_robject(SEXP);
static K from_builtin_robject(SEXP);
static K from_char_robject(SEXP);
static K from_logical_robject(SEXP);
static K from_integer_robject(SEXP);
static K from_double_robject(SEXP);
static K from_complex_robject(SEXP);
static K from_character_robject(SEXP);
static K from_dot_robject(SEXP);
static K from_sxp_robject(SEXP);
static K from_vector_robject(SEXP);
static K from_numeric_robject(SEXP);
static K from_name_robject(SEXP);

static K from_any_robject(SEXP sxp) {
  K result = 0;
  int type = TYPEOF(sxp);
  switch (type) {
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
    return from_environment_robject(sxp);
    break; /* environments */
  case PROMSXP:
    return from_promise_robject(sxp);
    break; /* promises: [un]evaluated closure arguments */
  case LANGSXP:
    return from_language_robject(sxp);
    break; /* language constructs (special lists) */
  case SPECIALSXP:
    return error_broken_robject(sxp);
    break; /* special forms */
  case BUILTINSXP:
    return error_broken_robject(sxp);
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
    return from_complex_robject(sxp);
    break; /* complex variables */
  case STRSXP:
    return from_character_robject(sxp);
    break; /* string vectors */
  case DOTSXP:
    return from_dot_robject(sxp);
    break; /* dot-dot-dot object */
  case ANYSXP:
    return error_broken_robject(sxp);
    break; /* make "any" args work */
  case VECSXP:
    return from_vector_robject(sxp);
    break; /* generic vectors */
  case EXPRSXP:
    return from_sxp_robject(sxp);
    break; /* sxps vectors */
  case BCODESXP:
    return error_broken_robject(sxp);
    break; /* byte code */
  case EXTPTRSXP:
    return error_broken_robject(sxp);
    break; /* external pointer */
  case WEAKREFSXP:
    return error_broken_robject(sxp);
    break; /* weak reference */
  case RAWSXP:
    return error_broken_robject(sxp);
    break; /* raw bytes */
  case S4SXP:
    return error_broken_robject(sxp);
    break; /* S4 non-vector */
  case FUNSXP:
    return error_broken_robject(sxp);
    break; /* Closure or Builtin */
  }
  return result;
}

/* add attribute */
static K addattR(K x, SEXP att) { return knk(2, from_any_robject(att), x); }

/* add attribute if any */
static K attR(K x, SEXP sxp) {
  SEXP att = ATTRIB(sxp);
  if (isNull(att))
    return x;
  return addattR(x, att);
}

static K error_broken_robject(SEXP sxp) { return krr("Broken R object."); }

static K from_null_robject(SEXP sxp) { return attR(ki((int)0x80000000), sxp); }

static K from_symbol_robject(SEXP sxp) {
  const char *t = CHAR(CAR(sxp));
  char *s = malloc(1 + strlen(t));
  strcpy(s, t);
  K x = ks(s);
  free(s);
  return attR(x, sxp);
}

static K from_pairlist_robject(SEXP sxp) {
  K x = knk(0);
  SEXP s = sxp;
  while (0 < length(s)) {
    x = jk(&x, from_any_robject(CAR(s)));
    x = jk(&x, from_any_robject(TAG(s)));
    s = CDR(s);
  }
  return attR(x, sxp);
}

static K from_closure_robject(SEXP sxp) {
  K x = from_any_robject(FORMALS(sxp));
  K y = from_any_robject(BODY(sxp));
  return attR(knk(2, x, y), sxp);
}

static K from_environment_robject(SEXP sxp) {
  return attR(kp("environment"), sxp);
}

static K from_promise_robject(SEXP sxp) { return attR(kp("promise"), sxp); }

static K from_language_robject(SEXP sxp) {
  int i, len = length(sxp);
  K x = knk(0);
  SEXP s = sxp;
  while (0 < length(s)) {
    x = jk(&x, from_any_robject(CAR(s)));
    s = CDR(s);
  }
  return attR(x, sxp);
}

static K from_builtin_robject(SEXP sxp) { return attR(kp("builtin"), sxp); }

static K from_char_robject(SEXP sxp) {
  K x = kp((char *)CHAR(STRING_ELT(sxp, 0)));
  return attR(x, sxp);
}

static K from_logical_robject(SEXP sxp) {
  K x;
  int len = LENGTH(sxp);
  int *s = malloc(len * sizeof(int));
  DO(len, s[i] = LOGICAL_POINTER(sxp)[i]);
  SEXP dim = GET_DIM(sxp);
  if (isNull(dim)) {
    x = kintv(len, s);
    free(s);
    return attR(x, sxp);
  }
  x = kinta(len, length(dim), INTEGER(dim), s);
  free(s);
  SEXP dimnames = GET_DIMNAMES(sxp);
  if (!isNull(dimnames))
    return attR(x, sxp);
  SEXP e;
  PROTECT(e = duplicate(sxp));
  SET_DIM(e, R_NilValue);
  x = attR(x, e);
  UNPROTECT(1);
  return x;
}

static K from_integer_robject(SEXP sxp) {
  K x;
  int len = LENGTH(sxp);
  int *s = malloc(len * sizeof(int));
  DO(len, s[i] = INTEGER_POINTER(sxp)[i]);
  SEXP dim = GET_DIM(sxp);
  if (isNull(dim)) {
    x = kintv(len, s);
    free(s);
    return attR(x, sxp);
  }
  x = kinta(len, length(dim), INTEGER(dim), s);
  free(s);
  SEXP dimnames = GET_DIMNAMES(sxp);
  if (!isNull(dimnames))
    return attR(x, sxp);
  SEXP e;
  PROTECT(e = duplicate(sxp));
  SET_DIM(e, R_NilValue);
  x = attR(x, e);
  UNPROTECT(1);
  return x;
}

static K from_double_robject(SEXP sxp) {
  K x;
  int len = LENGTH(sxp);
  double *s = malloc(len * sizeof(double));
  DO(len, s[i] = REAL(sxp)[i]);
  SEXP dim = GET_DIM(sxp);
  if (isNull(dim)) {
    x = kdoublev(len, s);
    free(s);
    return attR(x, sxp);
  }
  x = kdoublea(len, length(dim), INTEGER(dim), s);
  free(s);
  SEXP dimnames = GET_DIMNAMES(sxp);
  if (!isNull(dimnames))
    return attR(x, sxp);
  SEXP e;
  PROTECT(e = duplicate(sxp));
  SET_DIM(e, R_NilValue);
  x = attR(x, e);
  UNPROTECT(1);
  return x;
}

static K from_complex_robject(SEXP sxp) { return attR(kp("complex"), sxp); }

static K from_character_robject(SEXP sxp) {
  K x;
  int i, length = LENGTH(sxp);
  if (length == 1)
    x = kp((char *)CHAR(STRING_ELT(sxp, 0)));
  else {
    x = ktn(0, length);
    for (i = 0; i < length; i++) {
      xK[i] = kp((char *)CHAR(STRING_ELT(sxp, i)));
    }
  }
  return attR(x, sxp);
}

static K from_dot_robject(SEXP sxp) { return attR(kp("pairlist"), sxp); }

static K from_sxp_robject(SEXP sxp) { return attR(kp("dot"), sxp); }

static K from_list_robject(SEXP sxp) { return attR(kp("list"), sxp); }

static K from_vector_robject(SEXP sxp) {
  int i, length = LENGTH(sxp);
  K x = ktn(0, length);
  for (i = 0; i < length; i++) {
    xK[i] = from_any_robject(VECTOR_ELT(sxp, i));
  }
  return attR(x, sxp);
}

static K from_numeric_robject(SEXP sxp) { return attR(kp("numeric"), sxp); }

static K from_name_robject(SEXP sxp) { return attR(kp("name"), sxp); }

/*
 * various utilities
 */

/* get k string or symbol name */
static char *getkstring(K x) {
  char *s;
  int len;
  switch (xt) {
  case -KC:
    s = calloc(2, 1);
    s[0] = xg;
    break;
  case KC:
    s = calloc(1 + xn, 1);
    memcpy(s, xG, xn);
    break;
  case -KS:
    len = 1 + strlen(xs);
    s = calloc(len, 1);
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

static K kintv(int len, int *val) {
  K x = ktn(KI, len);
  DO(len, kI(x)[i] = (val)[i]);
  return x;
}

static K kinta(int len, int rank, int *shape, int *val) {
  K x, y;
  int i, j, r, c, k;
  switch (rank) {
  case 1:
    x = kintv(len, val);
    break;
  case 2:
    r = shape[0];
    c = shape[1];
    x = knk(0);
    for (i = 0; i < r; i++) {
      y = ktn(KI, c);
      for (j = 0; j < c; j++)
        kI(y)[j] = val[i + r * j];
      x = jk(&x, y);
    };
    break;
  default:
    k = rank - 1;
    r = shape[k];
    c = len / r;
    x = knk(0);
    for (i = 0; i < r; i++)
      x = jk(&x, kinta(c, k, shape, val + c * i));
  }
  return x;
}

static K kdoublev(int len, double *val) {
  K x = ktn(KF, len);
  DO(len, kF(x)[i] = (val)[i]);
  return x;
}

static K kdoublea(int len, int rank, int *shape, double *val) {
  K x, y;
  int i, j, r, c, k;
  switch (rank) {
  case 1:
    x = kdoublev(len, val);
    break;
  case 2:
    r = shape[0];
    c = shape[1];
    x = knk(0);
    for (i = 0; i < r; i++) {
      y = ktn(KF, c);
      for (j = 0; j < c; j++)
        kF(y)[j] = val[i + r * j];
      x = jk(&x, y);
    };
    break;
  default:
    k = rank - 1;
    r = shape[k];
    c = len / r;
    x = knk(0);
    for (i = 0; i < r; i++)
      x = jk(&x, kdoublea(c, k, shape, val + c * i));
  }
  return x;
}
