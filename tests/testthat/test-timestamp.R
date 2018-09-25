context("timestamp")

test_that("timestamp conversions", {
  h <- skip_unless_has_test_db()
  Qtorigin <- ISOdatetime(2000,1,1,0,0,0,tz='UTC')
  x <- as.POSIXct(1534458275.511368513107,origin=ISOdatetime(1970,1,1,0,0,0,tz='UTC'))
  xs <- sprintf('`timestamp$%.0fj',1e9*as.numeric(x-Qtorigin,units='secs'))
  y <- execute(h,xs)
  expect_equal(x,as.POSIXct(y))
  rnull_ts <-nanotime(as.integer64(NA))
  qnull_ts <- execute(h,"{show x;x}",rnull_ts)
  expect_equal(rnull_ts,qnull_ts)
})