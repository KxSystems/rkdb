context("dict")

test_that("dict conversions", {
  h <- skip_unless_has_test_db()
  dictK <- execute(h, "`a`b!(1 2;1 2 3)")
  dictR <-list(a=c(1,2),b=c(1,2,3))
  expect_equal(dictR, dictK)
})