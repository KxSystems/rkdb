context("tables")

test_that("table shapes", {
  h <- skip_unless_has_test_db()
  col3K <- execute(h, "([]s:`a`b``1;e:``aa`bb`cc;qty:100 0N 0W -0Wi)")
  col3R <-
    data.frame(
      s = c('a', 'b', '', '1'),
      e = c('', 'aa', 'bb', 'cc'),
      qty = c(100L, NA, as.integer(2147483647), -as.integer(2147483647)),
      stringsAsFactors = FALSE
    )
  expect_equal(col3R, col3K)
  col3K<-execute(h,"::",col3R)
  expect_equal(col3R, col3K)
})