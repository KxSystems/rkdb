#' Run simple series of tests for qserver
#' Note: Start kdb+ instance on localhost:5000 before running this function
#' @export
test.rkdb <- function() {
  h=open_connection()   # default is localhost:5000

  execute (h,"sp:([]s:10?`3;p:10?`1;qty:100*10?10)")
  s=execute(h,"select[5] from sp")
  execute (h,"a:.z.d")
  execute (h,"b:a+100 * til 5")
  execute (h,"c:.z.z")
  execute (h,"d:c+1.23 * til 5")

  print(s)
  print(execute(h,"b"))
  print(execute(h,"d"))
  print(execute(h, "raze", list(c(1,2,3), c(4,5,6))))
  
  close_connection(h)
}
