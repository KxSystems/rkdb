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
execute <- function(connection, query,arg1=NULL,arg2=NULL,arg3=NULL,arg4=NULL,arg5=NULL,arg6=NULL,arg7=NULL) {
  if(is.null(arg1)){ return(.Call("kx_r_execute0", as.integer(connection), query))}
  if(is.null(arg2)){ return(.Call("kx_r_execute1", as.integer(connection), query,arg1))}
  if(is.null(arg3)){ return(.Call("kx_r_execute2", as.integer(connection), query,arg1,arg2))}
  if(is.null(arg4)){ return(.Call("kx_r_execute3", as.integer(connection), query,arg1,arg2,arg3))}
  if(is.null(arg5)){ return(.Call("kx_r_execute4", as.integer(connection), query,arg1,arg2,arg3,arg4))}
  if(is.null(arg6)){ return(.Call("kx_r_execute5", as.integer(connection), query,arg1,arg2,arg3,arg4,arg5))}
  if(is.null(arg7)){ return(.Call("kx_r_execute6", as.integer(connection), query,arg1,arg2,arg3,arg4,arg5,arg6))}          
  return( .Call("kx_r_execute7", as.integer(connection), query,arg1,arg2,arg3,arg4,arg5,arg6,arg7))
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