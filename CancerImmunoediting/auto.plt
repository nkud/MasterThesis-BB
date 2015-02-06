set terminal png size 500,500;
set output "cell-energy-average.png";
plot 'bin/cell-energy-average.txt' w l
