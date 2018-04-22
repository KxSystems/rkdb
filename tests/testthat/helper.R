startq <- function(){
  library(subprocess)
  handle <- 0 #subprocess::spawn_process(file.path(Sys.getenv('QHOME'),"m64/q"), c("-p 5000"))
  print(handle)
  handle
}

skip_unless_has_test_db <- function(expr) {
  if (!identical(Sys.getenv("NOT_CRAN"), "true")) {
    return(skip("On CRAN"))
  }
  tryCatch({
 #   startq()
    rkdb::open_connection()
  }, error = function(e) {
    skip(paste0("Test database not available:\n'", conditionMessage(e), "'"))
  })
}
