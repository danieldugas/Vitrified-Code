alpha = (pi/20)-pi/4:pi/20:pi/4-pi/20;
GY = 0: 170/10 : 170 - 170/5;
[X,Y] = meshgrid(alpha,GY);
h = (Y - 126).*tan(X) + 9;
surf(X,Y,h);