%% Homework 3


%% Task 1
close all;

dx = double(10.^[-20:0.1:0]);
x = double(2);

deriv = (x*(x+2))/(x+1)^2; % u'(x)
deriv3 = -6*(x+1)^-4; % u'''(x)
deriv4 = 24/(x+1)^5;  % u''''(x)

a = x+1./(1+x); % u(x)
b = (x-dx)+1./(1+(x-dx)); % u(x-dx)
c = (x+dx)+1./(1+(x+dx)); % u(x+dx)
d = (x+2*dx)+1./(1+(x+2*dx)); % u(x+2dx)
oldderivnum = (c-b)./(2.*dx); % 2-valued u'num(x) 
derivnum = (-3*a-2*b+6*c-d)./(6.*dx); % 4-valued u'num(x)

eps = abs(deriv - derivnum)./abs(deriv);
oldeps = abs(deriv - oldderivnum)./abs(deriv);

dx4 = dx.^3;
dx4(1:160) = NaN;
dx2 = dx.^2;
dx2(1:160) = NaN;

loglog(dx,[oldeps; eps ]);
hold all;
loglog(dx, dx2, 'r--');
loglog(dx, dx4, 'y--');
xlabel('dx');
ylabel('eps');
title('Error');
legend('2-Valued', '4-Valued','dx^2', 'dx^3');




%% Task 2
close all; 

x = linspace(-3,1);
y = linspace(-3,3);
[xx, yy] = meshgrid(x,y);
zz = xx + 1i * yy;

G =  abs( 1 + zz + (1/2)*zz.^2 + (1/6)*zz.^3 +(1/24)*zz.^4 );
surf(xx, yy, G);
zlabel('G(z)');
xlabel('x');
ylabel('iy');
title('G(z)');

figure();
contour(xx, yy,G,[1,1]);
zlabel('G(z)');
xlabel('x');
ylabel('iy');
title('G(z) = 1');

%% Task 3

N = 200;
dx = 2*pi/N;
x = 0:dx:2*pi; 
x = x(1:N); % remove the last value

I = eye(N);
Irs = [I(:,2:N) I(:,1) ];
D = (I - Irs) / dx; 

k=5; 

f = exp(1i*k*x);
derivf = 1i*k*exp(1i*k*x);
fnum = D*f';
fnum = fnum';

plot(x, real([f; fnum; derivf]) );
xlabel('x');
ylabel('Real part');
title( {['N = ' num2str(N)]} );
legend('f(x)', 'f''(x)','f''num(x)');

mu = fnum./f;
subplot(1,2,1);
plot(x, real(mu));
xlabel('x');
ylabel('Re(mu)');
title( {['N = ' num2str(N)]} );
subplot(1,2,2);
plot(x, imag(mu));
xlabel('x');
ylabel('Im(mu)');
title( {['N = ' num2str(N)]} );

ktilde = ( exp(1i*k*dx) - 1 )./...
         ( dx * 1i * exp(1i*k*dx) );
