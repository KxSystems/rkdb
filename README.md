## R client for kdb+

Execute kdb+ queries from R for advanced high-performance analytics.

## See [Interfacing with R](http://code.kx.com/q/interfaces/with-r/) on Kx wiki.

## Installation

Using `devtools` package

```
devtools::install_github('kxsystems/rkdb')
```

To test, assuming a local q instance listening on port `5000`, try in R:
```
> library(rkdb)
> test.rkdb()
```
## API

`open_connection` - opens connection to kdb+
Note that open_connection takes 3 arguments with defaults of `host='localhost'`, `port=5000`, `user=NULL`.

`execute` - run a query using string on connection provided

`close_connection` - close previously opened connection


## Want to contribute?
- Prepare rkdb to be published to CRAN
- Pass R objects directly to execute
- Any open issues or documentation improvements
