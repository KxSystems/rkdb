#' Connect to kdb+ instance.
#'
#' @param host Hostname. For Unix Domain sockets pass '0.0.0.0'
#' @param port Port number.
#' @param user Username and password as user:password string
#' @param timeout Number of milliseconds to wait for connection
#' @param tls Wether or not to use secure connection. Alway false(unsupported).
#' @return Handle to kdb+ instance for \code{execute} and \code{close_connection}.
#' @export
#' @examples
#' \dontrun{
#' h<-open_connection()
#' h<-open_connection(port=5000)
#' }
open_connection <- function(host="localhost", port=5000, user="", timeout = 0,  tls = FALSE) {
  parameters <- list(host, as.integer(port), user, as.integer(timeout), as.logical(tls))
  h <- .Call("kx_r_open_connection", parameters)
  h
}

#' Execute \code{query} using \code{connection} connection to kdb+.
#'
#' @param connection Connection handle. handle>0 will perform sync requst, handle<0 async.
#' @param query A string to send to kdb+.
#' @param ... Optional parameters to pass to function provided in query.
#' @return Result of execution.
#' @export
#' @examples
#' \dontrun{
#' execute(h,"til 10")
#' execute(h,"dev 1000?0")
#' execute(h, "+", 2, 5)
#' execute(-h, "upd",as.name("trade"),list(c(as.name("AAPL")),1.35,100L))
#' }

execute <- function(connection, query, ...) {
  .Call("kx_r_execute", as.integer(connection), query, list(...))
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