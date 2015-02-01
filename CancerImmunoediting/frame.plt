title(n)=sprintf("t = %d", n);
file(n)=sprintf("bin/%d-cell.txt", n);
set yl textcolor lt 0;set zrange[0:100];
set title title(n);
set view map;
set cbrange[0:10];
splot file(n) w pm3d;
if(n<-1) n=n+10; reread;