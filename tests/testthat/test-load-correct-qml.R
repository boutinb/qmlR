context("test-load-correct-qml")

test_that("loading correct qml works", {
  expect_equal(runQml("TestMeCorrect.qml"), c("Could load QML satisfactorily"))
})
