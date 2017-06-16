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
#' @param ... Optional parameters to pass to function provided in query.
#' @return Result of execution.
#' @export
#' @examples
#' \dontrun{
#' execute(h,"til 10")
#' execute(h,"dev 1000?0")
#' execute(h, "+", 2, 5)
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
