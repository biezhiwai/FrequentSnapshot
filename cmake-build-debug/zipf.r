sink("rfg.txt")

library(methods)
library(stats4)
library(splines)
library(VGAM)

size <- 256000 * 60
max <- 250 * 1000
delta <- 0.1
x <- rzipf(size,max,delta)

for( i in 1:size)
        cat(x[i],"\n")
sink()
