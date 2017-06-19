#' Connect to kdb+ instance.
#'
#' @param host Hostname.
#' @param port Port number.
#' @param user Username and password as user:password string
#' @return Handle to kdb+ instance for \code{execute} and \code{close_connection}.
#' @export
#' @examples
#' \dontrun{
#' h<-open_connection()
#' h<-open_connection(port=5000)
#' }
open_connection <- function(host="localhost", port=5000, user=NULL) {
  parameters <- list(host, as.integer(port), user)
  h <- .Call("kx_r_open_connection", parameters)
  h
}

#' Execute \code{query} using \code{con} connection to kdb+.
#'
#' @param con Connection handle.
#' @param query A string to send to kdb+.
#' @return Result of execution.
#' @export
#' @examples
#' \dontrun{
#' execute(h,"til 10")
#' execute(h,"dev 1000?0")
#' }
execute <- function(connection, query) {
  return(.Call("kx_r_execute", as.integer(connection), query))
}

#' Close connection to kdb+ instance.
#'
#' @param con Connection handle.
#' @return 0 on closed connection.
#' @export
#' @examples
#' \dontrun{
#' close_connection(h)
#' }
close_connection <- function(con) {
	.Call("kx_r_close_connection", as.integer(con))
}

# library(rkdb)
# hdl=open_connection(port=4537)
# tmp <- data.frame(a=c(1,2,3),b=c("a","b","b"))
# class(tmp$b)
# dict()
# class(tmp)
# execute(hdl,'{`tmp set x}',data.frame(a=c(1,2,3),b=c("a","b","b")))