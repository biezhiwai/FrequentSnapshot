count = 1024000
bound = 4000000
x <- sample(0:(bound-1), count, replace=T)
for( i in 1:count)
        cat(x[i],"\n")
