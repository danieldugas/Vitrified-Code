function pos_ecef = bancroft(x, y, z, t)

c = 0.299792458; %m/nsec
Rt = 6378000;

load('gps_data.mat')
rho = t_k * c ;

% finding B, a, and e
B = [x_k, y_k, z_k, -rho] ;
S = [x_k, y_k, z_k,  rho]' ; % si vectors as columns
a = 0.5 * lorenz_product(S,S)';
e = ones(size(a));

% finding the least squares coefficients
Bplus = (B'*B)^-1 * B' ;
alpha = lorenz_product(Bplus*e,Bplus*e) ;
beta  = 2* ( lorenz_product(Bplus*a,Bplus*e) - 1 ) ;
gamma = lorenz_product(Bplus*a,Bplus*a) ;

lambda(1) = ( -beta + sqrt(beta^2 - 4*alpha*gamma) )...
            / (2*alpha) ;
lambda(2) = ( -beta - sqrt(beta^2 - 4*alpha*gamma) )...
            / (2*alpha) ;

% finding the least squares solution
ustar(1,:) = Bplus*(a + lambda(1)*e) ;
ustar(2,:) = Bplus*(a + lambda(2)*e) ;

r(1) = norm(ustar(1,1:3)) ;
r(2) = norm(ustar(2,1:3)) ;
[~, i] = min(abs(r-Rt)) ;

pos_ecef = ustar(i,1:3) ;

end
