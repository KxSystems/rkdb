## R client for kdb+

Execute kdb+ queries from R for advanced high-performance analytics.

See also [Kx wiki](http://code.kx.com/q/interfaces/with-r/).

## Installation

Using `devtools` package

```
devtools::install_github('kxsystems/qserver')
```

To test, assuming a local q instance listening on port `5000`, try in R:
```
> library(qserver)
> test.qserver()
```
## API

`open_connection` - opens connection to kdb+
Note that open_connection takes 3 arguments with defaults of `host='localhost'`, `port=5000`, `user=NULL`.

`execute` - run a query using string on connection provided

`close_connection` - close previously opened connection


## Want to contribute?
- Prepare qserver to be published to CRAN
- Pass R objects directly to execute
- Any open issues or documentation improvements
