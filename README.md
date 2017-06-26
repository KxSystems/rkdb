Installation
============

``` r
# remove old package
if('qserver' %in% rownames(installed.packages())) remove.packages('qserver')
# install devtools
if(! 'devtools' %in% rownames(installed.packages())) install.packages('devtools')
library(devtools)
# install rkdb
devtools::install_github('kxsystems/rkdb', quiet=TRUE)
library(rkdb)
```

First steps
===========

Open a q server and connect to it
---------------------------------

Open a qserver to test the installation

``` r
q -p 1234
```

Open a connection to it

``` r
h <- open_connection('localhost',1234) #this open a connection
```

Hello kdb
---------

``` r
execute(h, '1+1')
```

    ## [1] 2

Assigning a variable in q workspace:

``` r
execute(h, 'x:1+1') #assign x hopefully to 2
```

    ## NULL

``` r
execute(h, 'x') # get back the value
```

    ## [1] 2

Checks
======

Atoms from kdb to R
-------------------

As per [Q for mortals](http://code.kx.com/q4m3/2_Basic_Data_Types_Atoms/) kdb uses the following basic types:

| Type        | Size | CharType | NumType | Notation                       | Null Value |
|:------------|:-----|:---------|:--------|:-------------------------------|:-----------|
| boolean     | 1    | b        | 1       | 1b                             | 0b         |
| byte        | 1    | X        | 4       | 0x26                           | 0x00       |
| short       | 2    | H        | 5       | 42h                            | 0Nh        |
| int         | 4    | I        | 6       | 42                             | 0N         |
| long        | 8    | J        | 7       | 42j                            | 0Nj        |
| real        | 4    | E        | 8       | 4.2e                           | 0Ne        |
| float       | 8    | F        | 9       | 4.2                            | 0n         |
| char        | 1    | C        | 10      | z                              | " "        |
| symbol      | \*   | S        | 11      | \`zaphod                       | \`         |
| timestamp   | 8    | P        | 12      | 2015.01.01T00:00:00.000000000  | 0Np        |
| month       | 4    | M        | 13      | 2006.07m                       | 0Nm        |
| date        | 4    | D        | 14      | 2006.07.21                     | 0Nd        |
| (datetime)  | 4    | Z        | 15      | 2006.07.21T09:13:39            | 0Nz        |
| timespan    | 8    | N        | 16      | 12:00:00.000000000             | 0Nn        |
| minute      | 4    | U        | 17      | 23:59                          | 0Nu        |
| second      | 4    | V        | 18      | 23:59:59                       | 0Nv        |
| time        | 4    | T        | 19      | 09:01:02:042                   | 0Nt        |
| enumeration |      |          | 20+     | `sym$`kx                       |            |
| table       |      |          | 98      | (\[\] c1:`a`b\`c; c2:10 20 30) |            |
| dictionary  |      |          | 99      | `a`b\`v!10 20 30               |            |
| function    |      |          | 100     | {x}                            |            |
| nil item    |      |          | 101     | ::                             |            |

The following function allows to check how kdb atomic objects are returned in R:

``` r
test <- function(h, type, x){
    cat(sprintf('From kdb %s type object %s comes back as :', type, x))
    str(execute(h, x))
}
test(h, 'boolean', '1b')
```

    ## From kdb boolean type object 1b comes back as : logi TRUE

``` r
test(h, 'byte', '0x26')
```

    ## From kdb byte type object 0x26 comes back as : int 38

``` r
test(h, 'short', '42h')
```

    ## From kdb short type object 42h comes back as : int 42

``` r
test(h, 'int', '1i')
```

    ## From kdb int type object 1i comes back as : int 1

``` r
test(h, 'long', '42j')
```

    ## From kdb long type object 42j comes back as : num 42

``` r
test(h, 'real', '4.2e')
```

    ## From kdb real type object 4.2e comes back as : num 4.2

``` r
test(h, 'float', '4.2')
```

    ## From kdb float type object 4.2 comes back as : num 4.2

``` r
test(h, 'char', '"a"')
```

    ## From kdb char type object "a" comes back as : chr "a"

``` r
test(h, 'symbol', '`a')
```

    ## From kdb symbol type object `a comes back as : chr "a"

``` r
test(h, 'timestamp', '2015.01.01T00:00:00.000000000')
```

    ## From kdb timestamp type object 2015.01.01T00:00:00.000000000 comes back as : POSIXt[1:1], format: "2015-01-01"

``` r
test(h, 'month', '2015.01m')
```

    ## From kdb month type object 2015.01m comes back as : int 180

``` r
test(h, 'date', '.z.d')
```

    ## From kdb date type object .z.d comes back as : Date[1:1], format: "2017-06-26"

``` r
test(h, 'datetime', '2006.07.21T09:13:39')
```

    ## From kdb datetime type object 2006.07.21T09:13:39 comes back as : POSIXt[1:1], format: "2006-07-21 10:13:39"

``` r
test(h, 'timespan', '12:00:00.000000000')
```

    ## From kdb timespan type object 12:00:00.000000000 comes back as : num 43200

``` r
test(h, 'minute', '12:00')
```

    ## From kdb minute type object 12:00 comes back as : int 720

``` r
test(h, 'second', '12:00:00')
```

    ## From kdb second type object 12:00:00 comes back as : int 43200

``` r
test(h, 'time', '12:00:00.000')
```

    ## From kdb time type object 12:00:00.000 comes back as : int 43200000

``` r
#test(h, 'enumeration', '`sym$`kx')
test(h, 'table', '([] x:`a`b;y:2?1.;z:2#2006.07.21T09:13:39;t:(1 2;3 4);u:("toto";"tata"))')
```

    ## From kdb table type object ([] x:`a`b;y:2?1.;z:2#2006.07.21T09:13:39;t:(1 2;3 4);u:("toto";"tata")) comes back as :'data.frame': 2 obs. of  5 variables:
    ##  $ x: chr  "a" "b"
    ##  $ y: num  0.163 0.688
    ##  $ z: POSIXt, format: "2006-07-21 10:13:39" "2006-07-21 10:13:39"
    ##  $ t:List of 2
    ##   ..$ : num  1 2
    ##   ..$ : num  3 4
    ##  $ u: chr  "toto" "tata"

``` r
test(h, '(keyed) table', '([x:`a`b`c]y:3?1.)')
```

    ## From kdb (keyed) table type object ([x:`a`b`c]y:3?1.) comes back as :'data.frame':   3 obs. of  2 variables:
    ##  $ x: chr  "a" "b" "c"
    ##  $ y: num  0.818 0.752 0.109

``` r
test(h, 'dictionary', '`a`b!(10 12)')
```

    ## From kdb dictionary type object `a`b!(10 12) comes back as : Named num [1:2] 10 12
    ##  - attr(*, "names")= chr [1:2] "a" "b"
