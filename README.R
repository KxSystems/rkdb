#' Check if a package is installed and install it if needs be
#' 
#' @name checkInstallAndLoad 
#' @usage install packages if needs be to render the README
checkInstallAndLoad <- function(packages){
  for( package in packages){
    if(! package %in% rownames(installed.packages())) install.packages(package)
    library(package, character.only = TRUE)
  }
}

# check and install packages necessary to render the README file
checkInstallAndLoad(c('knitr', 'rmarkdown'))

# render to html
# rmarkdown::render(input='./README.Rmd', output_format='html_document')
# render to md (default)
rmarkdown::render(input='./README.Rmd', output_file = './README.md')