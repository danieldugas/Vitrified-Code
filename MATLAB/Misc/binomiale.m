[X,Y] = meshgrid( (0:3),linspace(0,1,100)) ;
P = linspace(0,1,100) ;
a = (1-P).^3 ;
b = 3.*(P.^2).*(1-P) ;
c = 3.*P.*(1-P).^2 ;
d = P.^3 ;
surf(X, Y, [a;b;c;d]' ) ;
