bound = 16000000 
x <- sample(0:(bound-1), 1024000, replace=T)
for( i in 1:1024000)
        cat(x[i],"\n")
