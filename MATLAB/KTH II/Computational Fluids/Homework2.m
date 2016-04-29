%% Homework 2

%% Task 1: Machine Accuracy

%numprec=double(1.0); % Define 1.0 with double precision
%umprec=single(1.0); % Define 1.0 with single precision
%while(0.1 < 0.1 + numprec)
%numprec=numprec*0.5;
%end
%numprec=numprec*2


%% Task 2: Round-o? Error


dx = single(10.^[-20:0.1:0]);
x = single(2);

deriv = (x*(x+2))/(x+1)^2; % f'(x)

a = (x-dx)+1./(1+(x-dx)); % f(x-dx)
b = (x+dx)+1./(1+(x+dx)); % f(x+dx)
derivnum = (b-a)./(2.*dx); % f'num(x)

eps(1,:) = abs(deriv - derivnum)./abs(deriv);

dx = double(10.^[-20:0.1:0]);
x = double(2);

deriv = (x*(x+2))/(x+1)^2; % f'(x)

a = (x-dx)+1./(1+(x-dx)); % f(x-dx)
b = (x+dx)+1./(1+(x+dx)); % f(x+dx)
derivnum = (b-a)./(2.*dx); % f'num(x)

eps(2,:) = abs(deriv - derivnum)./abs(deriv);

loglog(dx,eps);
xlabel('dx');
ylabel('eps');
title('Machine Error');
legend('Single Precision', 'Double Precision');

% the following is still in double precision

eps2 = eps(2,:);
fx = x+1/(1+x);
deriv3 = -6*(x+1)^-4;
epsr = eps2.*abs(fx)./(dx.*abs(deriv));
epsd = dx.^2*abs(deriv3) / abs(6*deriv);
epsg = epsr + epsd;

figure();
loglog(dx,[eps2; epsr; epsd; epsg]);
xlabel('dx');
ylabel('eps');
title('Error');
legend('Double Precision Eps', 'Epsr', 'Epsd', 'Epsg');

[~, i] = min(epsg);
dx(i)


%% Task 3: Integration of Ordinary Di?erential Equation
figure();

la = -0.2 + 0.1i;

u = [exp(la*16) exp(la*16) exp(la*16) exp(la*16) exp(la*16)];

T = 16;
j=0;
for N = [20 40 50 100 200]
j = j+1;
dt = T/N;
u1(j) = 1;
 for t = 0:dt:T
     u1(j) = (1 + dt*la)*u1(j); % Euler Explicit
 end
end

j=0;
for N = [20 40 50 100 200]
j = j+1;
dt = T/N;
u2(j) = 1;
 for t = 0:dt:T
     u2(j) = u2(j)/(1 - dt*la); % Euler Implicit
 end
end

j=0;
for N = [20 40 50 100 200]
j = j+1;
dt = T/N;
u3(j) = 1;
 for t = 0:dt:T
     u3(j) = (1+0.5*dt*la)*u3(j)/(1-0.5*dt*la); % Crank-Nicolson
 end
end

N = [20 40 50 100 200];
plot(N,[u; u1; u2; u3]);
xlabel('N');
ylabel('u(T)');
title('');
legend('Analytical', 'Euler Explicit', 'Euler Implicit', 'Crank-Nicolson');

figure();
loglog(N, [abs(u1-u); abs(u2-u); abs(u3-u)]);
xlabel('N');
ylabel('Error');
title('');
legend('Euler Explicit', 'Euler Implicit', 'Crank-Nicolson');
