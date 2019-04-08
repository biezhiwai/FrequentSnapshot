bound = 2000000
page_size = 4096
count = 1024000
sink("rfg.txt")
x <- sample(1:bound*page_size, count, replace=T)
for( i in 1:count)
        cat(x[i],"\n")
sink()
