context("timestamp")

test_that("timestamp conversions", {
  h <- skip_unless_has_test_db()
  Qtorigin <- ISOdatetime(2000,1,1,0,0,0,tz='UTC')
  x <- as.POSIXct(1534458275.511368513107,origin=ISOdatetime(1970,1,1,0,0,0,tz='UTC'))
  xs <- sprintf('12h$%.0fj',1e9*as.numeric(x-Qtorigin,units='secs'))
  y <- execute(h,xs)
  expect_equal(x,as.POSIXct(y))
})