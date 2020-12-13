
modelname = "result.data"
data = read.table(modelname)
attach(data);

x = V3
f2 = V2

png("bénéfices.png", width = 1024, height=1024)
plot(x, f2, type = "l", xlab = "nombre de parties", ylab = "bénéfices", col = "red", main = "Gains de l'IA B")
legend("topleft", legend=c("gains de l'IA B"), col = c("red"), lty = 1:1, cex=0.8)
dev.off()
