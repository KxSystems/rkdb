context("datetime")

test_that("datetime conversions", {
  h <- skip_unless_has_test_db()
  t1 <- execute(h, "`datetime$6803.5601388888890142")
  t2 <- execute(h, "`datetime$6803.5601388888917427")
  expect_equal(t2 - t1 > 0,TRUE)
})