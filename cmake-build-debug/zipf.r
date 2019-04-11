library(methods)
library(stats4)
library(splines)
library(VGAM)

args=commandArgs(T)
delta <- as.double(args[1])
bound <- 500000*4096
size <- 256000
x <- rzipf(size,bound,delta)

for( i in 1:size)
        cat(x[i],"\n")
