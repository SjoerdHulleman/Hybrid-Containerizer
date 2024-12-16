require(Rcpp)
sourceCpp("source.cpp")
system.time(
    rec <- est_bc_source(dat.bc.t)
)
choose(5000, 2)
image.plot(rec, col=rev(tim.colors()))