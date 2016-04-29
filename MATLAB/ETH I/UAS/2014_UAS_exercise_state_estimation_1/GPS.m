clear all; close all;

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

% display position
figure()

[X,Y,Z] = sphere(10);
GRs = 2e5; % Graphical radius of satellite
GRp = 5e5; % Graphical radius of points
%points data
x = pos_ecef(1);
y = pos_ecef(2);
z = pos_ecef(3);
x2 = ustar(end+1-i,1);
y2 = ustar(end+1-i,2);
z2 = ustar(end+1-i,3);
%satellite data
theta = [1:100]/100 * 2*pi;

for i = 1:length(t_k)
rs(i) = norm([x_k(i), y_k(i), z_k(i)]);
kappa(i) = atan2(y_k(i),x_k(i));
eps(i) = atan2(z_k(i),(x_k(i)/cos(kappa(i))));
xo(:,i) = rs(i)*cos(theta)*cos(eps(i))*cos(kappa(i))...
         -rs(i)*sin(theta)*            sin(kappa(i));
yo(:,i) = rs(i)*cos(theta)*cos(eps(i))*sin(kappa(i))...
         +rs(i)*sin(theta)*            cos(kappa(i));
zo(:,i) = rs(i)*cos(theta)*sin(eps(i));

end

hold all
surf(Rt*X,Rt*Y,Rt*Z,Z*0) % earth
%points
plot3(ustar(:,1),ustar(:,2), ustar(:,3)) % line
surf(GRp*X + x, GRp*Y + y, GRp*Z + z, Z*0+1) % actual position
surf(GRp*X + x2, GRp*Y + y2, GRp*Z + z2, Z*0+1) % possible position
% satellites
for i = 1:length(t_k)
surf(GRs*X + x_k(i), GRs*Y + y_k(i), GRs*Z + z_k(i), Z*0+1)
plot3(xo(:,i),yo(:,i),zo(:,i))
end

axis equal

