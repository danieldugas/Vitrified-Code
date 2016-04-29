close all; clear all;
dt = 0.01;
omega = pi;

r = 1;
L = 3;
theta = 0:omega*dt:4*pi;
alpha = asin(r/L*sin(theta));
B = [r*sin(theta); r*cos(theta)];
A = [0*theta; r*cos(theta)+L*cos(alpha)];
AB = B-A;

kappa = 0.5+ 0.25*(sin(10*theta));
P = A+[kappa;kappa].*AB;
DP = ( [P(:,2:end), P(:,1)]-P )/dt ;


for i = 1:length(theta)
    
    hold all
    axis([-2*r, 2*r, -2*r, 2*L])
    axis equal
    
    
    c = viscircles([0,0], [r]);
    oa = plot([0,A(1,i)], [0,A(2,i)]);
    ob = plot([0,B(1,i)], [0,B(2,i)]);
    ab = plot([A(1,i),B(1,i)], [A(2,i),B(2,i)]);
    linep = plot(P(1,1:i), P(2,1:i));
    p = scatter(P(1,i), P(2,i));
    dp = quiver(P(1,i), P(2,i), 0.2*DP(1,i), 0.2*DP(2,i));
    
    F(i) = getframe();
    
    clf
end
