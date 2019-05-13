library(methods)
library(stats4)
library(splines)
library(VGAM)

args=commandArgs(T)
delta <- 0.1
bound <- 64000000
size <- 33000
x <- rzipf(size,bound,delta)
x <- sort(x)
for( i in 1:size)
        cat((x[i]-1),"\n")
