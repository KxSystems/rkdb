context("basic")
library(nanotime)

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
  h <- skip_unless_has_test_db()
  
  bool <- testKdbToRType(h, '1b')
  expect_is(bool, "logical")
  expect_true(bool)

  guid <- testKdbToRType(h,'"G"$"f988c316-d1ec-6541-ee4c-44af6b5fc747"')
  expect_is(guid, "character")
  expect_equal(guid,'f988c316-d1ec-6541-ee4c-44af6b5fc747')
  
  byte <- testKdbToRType(h, '0x26')
  expect_is(byte, "raw")
  expect_equal(byte, as.raw(38L))
  
  short <- testKdbToRType(h, '42h')
  expect_is(short, "integer")
  expect_equal(short, 42L)
  
  int <- testKdbToRType(h, '1i')
  expect_is(int, "integer")
  expect_equal(int, 1L)
  
  long <- testKdbToRType(h, '42000j')
  expect_is(long, "integer64")
  expect_equal(long, as.integer64(42000L))
  
  real <- testKdbToRType(h, '4.2e')
  expect_is(real, "numeric")
  expect_equal(real,4.2,tolerance=1e-7)  #not equal due to truncation to 32 bit
  
  float <- testKdbToRType(h, '4.2')
  expect_is(float, "numeric")
  expect_equal(float, 4.2)
  
  char <- testKdbToRType(h, '"a"')
  expect_is(char, "character")
  expect_equal(char, 'a')
  
  symbol <- testKdbToRType(h, '`a')
  expect_is(symbol, "character")
  expect_equal(symbol, 'a')
  
  timestamp <- testKdbToRType(h, '2015.01.01D00:01:00.000000000')
  expect_is(timestamp, "nanotime")
  expect_equal(timestamp, nanotime("2015-01-01T00:01:00.000000000+00:00"))
  
  month <- testKdbToRType(h, '2015.01m')
  expect_is(month, "integer")
  expect_equal(month, 180L)
  
  date <- testKdbToRType(h, '2015.01.03')
  expect_is(date, "Date")
  expect_equal(date, as.Date('2015-01-03'))
  
  # we do not set the timezone attribute that is GMT on kdb unless explicitely set otherwise
  datetime <- testKdbToRType(h, '2006.07.21T09:13:39')
  expect_is(datetime, "POSIXt")
  rdatetime <- as.POSIXct('2006-07-21 09:13:39', tz='GMT')
  expect_equal(datetime, rdatetime)
  
  timespan <- testKdbToRType(h, '0D12')
  expect_is(timespan, "integer64")
  expect_equal(timespan, as.integer64(43200000000000))
  
  minute <- testKdbToRType(h, '12:00')
  expect_is(minute, "difftime")
  expect_equal(minute, as.difftime(12*60,units = 'mins'))
  
  second <- testKdbToRType(h, '12:00:00')
  expect_is(second, "difftime")
  expect_equal(second, as.difftime(12*60*60,units = 'secs'))

  time <- testKdbToRType(h, '12:00:00.000')
  expect_is(time, "integer")
  midnight <- as.POSIXct('00:00:00.000',format='%H:%M:%S',tz='UTC')
  midday   <- as.POSIXct('12:00:00.000',format='%H:%M:%S',tz='UTC')
  milli_12hr <- 1000*as.numeric(difftime(midday,midnight,units="sec"))
  expect_equal(time, milli_12hr)
  
  enumeration <-
    testKdbToRType(h, 'letters:`a`b`c`d; l:`a`b`a`b`d; `letters$l')
  expect_is(enumeration,"character")
  expect_equal(enumeration,c('a','b','a','b','d'))
  
  table <- testKdbToRType(h, '([] x:`a`b;y:2#1.;t:(1 2;3 4))')
  # use expect_equivalent to drop the attributes comparaison
  expect_is(table, "data.frame")
  expect_equivalent(table, data.frame(
    x = c('a', 'b'),
    y = c(1, 1),
    t = I(list(as.integer64(c(1, 2)), as.integer64(c(3, 4)))),
    stringsAsFactors = F
  ))
  keyed_table <- testKdbToRType(h, '([x:`a`b];y:2#1.;t:(1 2;3 4))')
  expect_is(keyed_table, "data.frame")
  expect_equivalent(keyed_table, data.frame(
    x = c('a', 'b'),
    y = c(1, 1),
    t = I(list(as.integer64(c(1, 2)), as.integer64(c(3, 4)))),
    stringsAsFactors = F
  ))
  
  dictionary <- testKdbToRType(h, '`a`b!10 12')
  expect_is(dictionary, "integer64")
  expect_equal(dictionary, c(a = as.integer64(10), b = as.integer64(12)))
  
  dictionary2 <- testKdbToRType(h, '`a`b!(10;`toto)')
  expect_is(dictionary2, "list")
  expect_equal(dictionary2, list(a = as.integer64(10), b = 'toto'))
  
  fonction <- testKdbToRType(h, '{[x] 2*x}')
  expect_is(fonction, "character")
  expect_equal(fonction, '{[x] 2*x}')

  list1 <- testKdbToRType(h, '1 2 3 4')
  expect_is(list1, "integer64")
  expect_equal(list1, as.integer64(c(1, 2, 3, 4)))
  
  list2 <- testKdbToRType(h, '(1;"toto";`tata)')
  expect_is(list2, "list")
  expect_equal(list2, list(as.integer64(1), 'toto', 'tata'))
  
  list3 <- testKdbToRType(h, '(1 2i;3 4i;4 5i)')
  expect_is(list3, "list")
  expect_equal(list3, list(c(1, 2), c(3, 4), c(4, 5)))

  listattr <-testKdbToRType(h,'(`s#1 2;`u#3 4;`p#4 4 5 5;`g#1 1 1 3 4 5i)')
  expect_is(listattr, "list")
  expect_equal(listattr,list(as.integer64(c(1,2)),
                             as.integer64(c(3,4)),
                             as.integer64(c(4,4,5,5)),
                             c(1,1,1,3,4,5)))
  
  nulllist <-testKdbToRType(h,'first each upper[.Q.t except \" bgxcjnps\"]$\\:()')
  expect_true(all(is.na(nulllist)))
})

# test R -> kdb
test_that("R types to kdb types", {
  h <- skip_unless_has_test_db()
  remoteCheckFunc <- '`cc set {show"type is ",string[type x], " : ",-3!x;`tmp set x;`okType`okValue!(type[x]~y;x~z)}'
  execute(h, remoteCheckFunc)
  int <- execute(h, 'cc[;6h;(),1i]', 1L)   # R doesn't have scalars
  expect_equal(int, c(okType = TRUE, okValue = TRUE))
  intV <- execute(h, 'cc[;6h;1 2i]', c(1L, 2L))
  int64 <- execute(h, 'cc[;7h;(),1]', as.integer64(1))
  expect_equal(int64, c(okType = TRUE, okValue = TRUE))
  int64V <- execute(h, 'cc[;7h;1 2]', as.integer64(1:2))
  expect_equal(int64V, c(okType = TRUE, okValue = TRUE))
  dbl <- execute(h, 'cc[;9h;(),1.]', 1.)
  expect_equal(dbl, c(okType = TRUE, okValue = TRUE))
  dblV <- execute(h, 'cc[;9h;(1. 2.)]', c(1., 2.))
  expect_equal(dblV, c(okType = TRUE, okValue = TRUE))
  timestamp <- execute(h, 'cc[;12h; (), 2020.04.10D06:09:16.158302011]', c(nanotime("2020-04-10T06:09:16.158302011+00:00")))
  expect_equal(timestamp, c(okType = TRUE, okValue = TRUE))
  timestampV <- execute(h, 'cc[;12h; (2020.04.10D06:09:16.158302011; 2020.04.10D06:19:06.267740198)]', c(nanotime("2020-04-10T06:09:16.158302011+00:00"), nanotime("2020-04-10T06:19:06.267740198+00:00")))
  expect_equal(timestampV, c(okType = TRUE, okValue = TRUE)) 
  date <- execute(h, 'cc[;14h;(),2012.02.02]',as.Date(c("2012-02-02")))
  expect_equal(date, c(okType = TRUE, okValue = TRUE))
  dateV <- execute(h, 'cc[;14h;(1995.08.09;1759.01.01)]',as.Date(c("1995-08-09","1759-01-01")))
  expect_equal(dateV, c(okType = TRUE, okValue = TRUE))
  datetimect <- execute(h, 'cc[;15h;(),2018.02.18T04:00:01.000z]', c(as.POSIXct("2018-02-18 04:00:01", format="%Y-%m-%d %H:%M:%S", tz='UTC')))
  expect_equal(datetimect, c(okType = TRUE, okValue = TRUE))
  datetimelt <- execute(h, 'cc[;15h;(),2018.02.18T04:00:01.000z]', c(as.POSIXlt("2018-02-18 04:00:01", format="%Y-%m-%d %H:%M:%S", tz='UTC')))
  expect_equal(datetimelt, c(okType = TRUE, okValue = TRUE))
  datetimectV <- execute(h, 'cc[;15h;(2015.03.16T17:30:00.000z; 1978.06.01T12:30:59.000z)]', c(as.POSIXct("2015-03-16 17:30:00", format="%Y-%m-%d %H:%M:%S", tz='UTC'), as.POSIXct("1978-06-01 12:30:59", format="%Y-%m-%d %H:%M:%S", tz='UTC')))
  expect_equal(datetimectV, c(okType = TRUE, okValue = TRUE))
  datetimeltV <- execute(h, 'cc[;15h;(2015.03.16T17:30:00.000z; 1978.06.01T12:30:59.000z)]', c(as.POSIXlt("2015-03-16 17:30:00", format="%Y-%m-%d %H:%M:%S", tz='UTC'), as.POSIXlt("1978-06-01 12:30:59", format="%Y-%m-%d %H:%M:%S", tz='UTC')))
  expect_equal(datetimeltV, c(okType = TRUE, okValue = TRUE))
  unamedL <- execute(h, 'cc[;0h;((),1.;(),2.)]', list(1., 2.))
  expect_equal(unamedL, c(okType = TRUE, okValue = TRUE))
  unamedL2 <- execute(h, 'cc[;0h;((),1.;(),"2")]', list(1., "2"))
  expect_equal(unamedL2, c(okType = TRUE, okValue = TRUE))
  # TODO: should this be the case for named vectors as for lists
  #namedVector <- execute(h, 'cc[;99h;`a`b!1 2f]', c(a = 1., b = 2.))
  #expect_equal(namedVector, c(okType = TRUE, okValue = TRUE))
  namedList <- execute(h, 'cc[;99h;`a`b!((),1.;(),2.)]', list(a = 1., b = 2.))
  expect_equal(namedList, c(okType = TRUE, okValue = TRUE))
  floatMatrix<- execute(h,'cc[;0h;2 2#1f]',matrix(1.0,nrow=2,ncol=2))
  expect_equal(floatMatrix,c(okType=TRUE,okValue=TRUE))
  int64Matrix<-as.integer64(1:4)
  dim(int64Matrix)<-c(2,2)
  int64Matrix<- execute(h,'cc[;0h;flip 2 2#1 2 3 4]',int64Matrix)
  expect_equal(int64Matrix,c(okType=TRUE,okValue=TRUE))
  #intMatrix<-as.integer(0:11)
  #dim(intMatrix)<-c(2,2,3)
  #intMatrix<- execute(h,'cc[;0h;2 2 3#`int$til 12]',intMatrix)
  #expect_equal(intMatrix,c(okType=TRUE,okValue=TRUE))
})
