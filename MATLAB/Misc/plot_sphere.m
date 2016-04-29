prc = 20 ;

[h,theta] = meshgrid(linspace(0,1,prc),linspace(0,2*pi,prc)) ;
rz = h ;
rx = sqrt(1-h.^2).*cos(theta) ;
ry = sqrt(1-h.^2).*sin(theta) ;
surf(rx,ry,rz) ;