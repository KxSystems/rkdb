context("tables")

test_that("multiplication works", {
  h<-skip_unless_has_test_db()
  expect_equal(2 * 2, 4)
})
