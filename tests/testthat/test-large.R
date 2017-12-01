context("large")

test_that("allocating large objs", {
  h <- skip_unless_has_test_db()
  dictK <- execute(h, "til 64*1024*1024")
  expect_length(dictK,64*1024*1024)
})