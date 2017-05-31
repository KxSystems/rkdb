
This library provides a Q server to R.

See also kx wiki https://code.kx.com/trac/wiki/Cookbook/IntegratingWithR.

## Installation

Using `devtools` package

```
devtools::install_github('kxsystems/qserver')
```

To test, assuming a q instance listening on port 5000 with a table t defined, try in R:
```
> library(qserver)
> test.qserver()
```
## API

`open_connection` - opens connection to kdb+
Note that open_connection actually takes 3 arguments with defaults of "localhost" for the host to connect to, 5000 for the port and none for the user/password credentials.

`execute` - run a query using string on connection provided

`close_connection` - close previously opened connection


## Want to contribute?
- Prepare qserver to be published to CRAN
