context("dict")

test_that("dict conversions", {
  h <- skip_unless_has_test_db()
  dictK <- execute(h, "`a`b!(1 2;1 2 3)")
  dictR <-list(a=as.integer64(c(1,2)),b=as.integer64(c(1,2,3)))
  expect_equal(dictR, dictK)
  dictK <- execute(h,"::",dictR)
  expect_equal(dictR,dictK)
  dictK <- execute(h,"::",as.pairlist(dictR))
  expect_equal(dictR,dictK)
})