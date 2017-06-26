
#' @name checkInstallAndLoad 
#' @usage install packages if needs be to render the README
checkInstallAndLoad <- function(packages){
  for( package in packages){
    if(! package %in% rownames(installed.packages())) install.packages(package)
    library(package, character.only = TRUE)
  }
}
checkInstallAndLoad(c('knitr', 'rmarkdown', 'data.table'))

# render to html
# rmarkdown::render(input='./R/README.Rmd', output_format='html_document')
# render to md (default)
rmarkdown::render(input='./R/README.Rmd', output_file = '../README.md')
