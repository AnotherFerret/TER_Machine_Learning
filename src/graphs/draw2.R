modelname = "victory.data"
data = read.table(modelname)
attach(data);

x = V3
f2 = V2
f1 = V1

png("victoires.png", width = 1024, height=1024)
plot(x, f2, type = "l", xlab = "nombre de parties", ylab = "% de victoires", col = "red", main = "Nombre de victoires de l'IA B")
legend("topleft", col = c("red"), lty = 1:1, cex=0.8)
dev.off()
