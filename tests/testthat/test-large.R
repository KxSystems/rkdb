context("large")

test_that("allocating large objs", {
  skip_on_cran()
  skip("It is a long test for 64bit only")
  h <- skip_unless_has_test_db()
  longvec <- execute(h, "(1+0Wi)?0x00")
  expect_length(longvec,2*1024*1024*1024)
  rcount <- execute(h,"count",longvec)
  expect_equal(as.numeric(rcount),length(longvec))
})