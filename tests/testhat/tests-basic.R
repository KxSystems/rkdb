# Run this test file with: 
# library(testthat)
# test_results <- test_dir("./tests/testhat/", reporter="summary")

# parameters
params <- list(server='localhost', port=1234)

library(rkdb)
# this opens a connection
h <- open_connection(params$server,params$port)

#' Helper to test kdb types are accurately converted to R types
#'
#' @param h connection object
#' @param x character command to be execute on the kdb server
#' @return result through \code{execute}.
testKdbToRType <- function(h, x){
  ans <- execute(h, x)
  return(ans)
}

# test kdb -> R
test_that("kdb types to R types", {
  
  bool <- testKdbToRType(h, '1b')
  byte <- testKdbToRType(h, '0x26')
  short <- testKdbToRType(h, '42h')
  int <- testKdbToRType(h, '1i')
  long <- testKdbToRType(h, '42000j')
  real <- testKdbToRType(h, '4.2e')
  float <- testKdbToRType(h, '4.2')
  char <- testKdbToRType(h, '"a"')
  symbol <- testKdbToRType(h, '`a')
  timestamp <- testKdbToRType(h, '2015.01.01T00:00:00.000000000')
  month <- testKdbToRType(h, '2015.01m')
  date <- testKdbToRType(h, '2015.01.01')
  datetime <- testKdbToRType(h, '2006.07.21T09:13:39')
  timespan <- testKdbToRType(h, '12:00:00.000000000')
  minute <- testKdbToRType(h, '12:00')
  second <- testKdbToRType(h, '12:00:00')
  time <- testKdbToRType(h, '12:00:00.000')
  enumeration <- testKdbToRType(h, 'letters:`a`b`c`d; l:`a`b`a`b`d; `letters$l')
  table <- testKdbToRType(h, '([] x:`a`b;y:2#1.;t:(1 2;3 4))')
  keyed_table <- testKdbToRType(h, '([x:`a`b];y:2#1.;t:(1 2;3 4))')
  dictionary <- testKdbToRType(h, '`a`b!(10 12)')
  dictionary2 <- testKdbToRType(h, '`a`b!(10;`toto)')
  fonction <- testKdbToRType(h, '{[x] 2*x}')
  
  list1 <- testKdbToRType(h, '1 2 3 4')
  list2 <- testKdbToRType(h, '(1;"toto";`tata)')
  list3 <- testKdbToRType(h, '(1 2;3 4;4 5)')
  
  expect_is( bool, "logical" ); expect_true( bool )
  expect_is( byte, "integer" ); expect_equal( byte, 38L )
  expect_is( short, "integer" ); expect_equal( short, 42L )
  expect_is( int, "integer" ); expect_equal( int, 1L )
  # kdb long is 8 bytes integer, R integer in 32 bytes integer
  expect_is( long, "integer" ); expect_equal( long, 42000L )
  expect_is( real, "numeric" ); expect_equal( real, 4.2 )
  expect_is( float, "numeric" ); expect_equal( float, 4.2 )
  expect_is( char, "character" ); expect_equal( char, 'a' )
  expect_is( symbol, "character" ); expect_equal( symbol, 'a' )
  expect_is( timestamp, "POSIXt" ); expect_equal( timestamp, as.POSIXct('2015-01-01T00:00:00.000000') )
  expect_is( month, "character" ); expect_equal( month, '2015.01' )
  expect_is( date, "Date" ); expect_equal( date, as.Date('2015-01-01') )
  expect_is( datetime, "POSIXt" ); expect_equal( datetime, as.POSIXct('2006-07-21T09:13:39') )
  expect_is( timespan, "character" ); expect_equal( timespan, '12:00:00.000000000' )
  expect_is( minute, "character" ); expect_equal( minute, '12:00' )
  expect_is( second, "character" ); expect_equal( second, '12:00:00' )
  expect_is( time, "character" ); expect_equal( time, '12:00:00.000' )
  # use expect_equivalent to drop the attributes comparaison
  expect_is( table, "data.frame" ); expect_equivalent( table, data.frame(x=c('a','b'),y=c(1,1),t=I(list(c(1,2),c(3,4))),stringsAsFactors=F) )
  expect_is( keyed_table, "data.frame" ); expect_equivalent( table, data.frame(x=c('a','b'),y=c(1,1),t=I(list(c(1,2),c(3,4))),stringsAsFactors=F) )
  expect_is( dictionary, "numeric" ); expect_equal( dictionary, c(a=10,b=12) )
  expect_is( dictionary2, "list" ); expect_equal( dictionary2, list(a=10,b='toto') )
  expect_is( fonction, "character" ); expect_equal( fonction, '{[x] 2*x}' )
  
  expect_is( list1, "numeric" ); expect_equal( list1, c(1,2,3,4) )
  expect_is( list2, "list" ); expect_equal( list2, list(1,'toto','tata') )
  expect_is( list3, "list" ); expect_equal( list3, list(c(1,2),c(3,4),c(4,5)) )
  
})

# test R -> kdb
test_that("R types to kdb types", {

int <- execute(h, '{[x] show("type is ",string[type[x]]); `tmp set x; :(`okType`okValue)!(type[x]~-6h;x~1i)}', 1L); expect_equal( int, c(okType=TRUE, okValue=TRUE) )
intV <- execute(h, '{[x] show("type is ",string[type[x]]); `tmp set x; :(`okType`okValue)!(type[x]~6h;x~(1 2))}', c(1L,2L)); expect_equal( intV, c(okType=TRUE, okValue=TRUE) )
dbl <- execute(h, '{[x] show("type is ",string[type[x]]); `tmp set x; :(`okType`okValue)!(type[x]~-6h;x~1.)}', 1.); expect_equal( dbl, c(okType=TRUE, okValue=TRUE) )
dblV <- execute(h, '{[x] show("type is ",string[type[x]]); `tmp set x; :(`okType`okValue)!(type[x]~9h;x~(1. 2.))}', c(1.,2.)); expect_equal( dblV, c(okType=TRUE, okValue=TRUE) )
unamedL <- execute(h, '{[x] show("type is ",string[type[x]]); `tmp set x; :(`okType`okValue)!(type[x]~9h;x~(1. 2.))}', list(1.,2.)); expect_equal( unamedL, c(okType=TRUE, okValue=TRUE) )
unamedL2 <- execute(h, '{[x] show("type is ",string[type[x]]); `tmp set x; :(`okType`okValue)!(type[x]~0h;x~(1.;"2"))}', list(1.,"2")); expect_equal( unamedL2, c(okType=TRUE, okValue=TRUE) )
namedV <- execute(h, '{[x] show("type is ",string[type[x]]); `tmp set x; :(`okType`okValue)!(type[x]~9h;x~((`a`b)!(1. 2.)))}', c(a=1.,b=2.)); expect_equal( namedV, c(okType=TRUE, okValue=TRUE) )
namedL <- execute(h, '{[x] show("type is ",string[type[x]]); `tmp set x; :(`okType`okValue)!(type[x]~9h;x~((`a`b)!(1. 2.)))}', list(a=1.,b=2.)); expect_equal( namedL, c(okType=TRUE, okValue=TRUE) )

})