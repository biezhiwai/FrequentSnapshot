bound = 2000
page_size = 4096
sink("rfg.txt")
x <- sample(1:bound*page_size, 1024000, replace=T)
for( i in 1:1024000)
        cat(x[i],"\n")
sink()
