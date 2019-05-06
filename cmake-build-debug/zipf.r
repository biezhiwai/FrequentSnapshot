library(methods)
library(stats4)
library(splines)
library(VGAM)

args=commandArgs(T)
delta <- 2
bound <- 16000000
size <- 256000
x <- rzipf(size,bound,delta)

for( i in 1:size)
        cat((x[i]-1),"\n")
