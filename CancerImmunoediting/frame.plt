title(n)=sprintf("t = %d", n);
file(n)=sprintf("bin/%d-cell.txt", n);
# set yl textcolor lt 0;set zrange[0:50];
set title title(n);
set view map;
set cbrange[0:5];
splot file(n) w pm3d;
if(n<100) n=n+1; reread;
