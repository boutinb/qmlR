context("test-load-broken-qml")


test_that("loading broken qml responds with proper errors", {
	expect_equal(
		runQml("TestMeBroken.qml"), 
		c(	"Error in 'TestMeBroken' at 7,5: Expected token `:'",
			"Loading QML failed miserably")
	)
})