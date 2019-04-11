bound = 2000000 
page_size = 4096
x <- sample(0:(bound*page_size-10240), 1024000, replace=T)
for( i in 1:1024000)
        cat(x[i],"\n")
