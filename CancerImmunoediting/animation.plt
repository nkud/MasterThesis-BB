set terminal gif animate optimize size 500,500 delay 5;
set output "animation.gif";
set style line 1 lw 2;
set key below right;
set key textcolor lt 0;
n=10;
load "frame.plt";
