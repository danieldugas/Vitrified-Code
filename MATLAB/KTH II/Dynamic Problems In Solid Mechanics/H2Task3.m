clear all; close all;

%% Maximums
syms omega k1 k2 m M c z;
A = [ -m*omega^2+k2+k1+1i*omega*c -k2-1i*omega*c;
      -k2-1i*omega*c     -M*omega^2+k2+1i*omega*c ] ;
A_ = A^-1;


% equation is the denominator of A_(1,2)
extremas = solve(M*m*z^4- M*k2*z^2- M*k1*z^2 - k2*m*z^2 + k1*k2, z)




%% Plot
Xlim = [0 5];
Ylim = [0 10^-3];

figure()
m = 2 ;
M = 600 ;
D0 = 0.1 ;
k1 = 2000 ;
k2 = 1000;
c = 100 ;

syms omega;
A = [ -m*omega^2+k2+k1+1i*omega*c -k2-1i*omega*c;
      -k2-1i*omega*c     -M*omega^2+k2+1i*omega*c ] ;

X = A^-1 * [1; 0]*D0;

ezplot(abs(X(1)), Xlim)
hold all
ezplot(abs(X(2)), Xlim)

w_crit = sqrt(k1/(m+M));

w_crit1 = real(subs(extremas(1)));
w_crit2 = real(subs(extremas(2)));
w_crit3 = real(subs(extremas(3)));
w_crit4 = real(subs(extremas(4)));

plot([w_crit w_crit], Ylim, 'r')
plot([w_crit1 w_crit1], Ylim, 'r')
plot([w_crit2 w_crit2], Ylim, 'r')
plot([w_crit3 w_crit3], Ylim, 'r')
plot([w_crit4 w_crit4], Ylim, 'r')
text(w_crit, Ylim(2)/2,'$\leftarrow \omega = \sqrt{\frac{k1}{m+M}}$','interpreter','Latex')

ylim(Ylim)
legend('Wheel','Piano')
title('Amplification')

%% MultiPlot
Xlim = [0 5];
Ylim = [0 10^-3];

figure()

C = [0 100 250 500 1000 2500 5000 10000 1000000];
for i = 1:length(C)
m = 2 ;
M = 600 ;
D0 = 0.1 ;
k1 = 2000 ;
k2 = 1000;
c = C(i) ;

syms omega;
A = [ -m*omega^2+k2+k1+1i*omega*c -k2-1i*omega*c;
      -k2-1i*omega*c     -M*omega^2+k2+1i*omega*c ] ;

X = A^-1 * [1; 0]*D0;

h = ezplot(abs(X(2)), Xlim);
set(h,'Color',[0 i/20 0]);
hold all;
end

w_crit = sqrt(k1/(m+M));

w_crit1 = real(subs(extremas(1)));
w_crit2 = real(subs(extremas(2)));
w_crit3 = real(subs(extremas(3)));
w_crit4 = real(subs(extremas(4)));

plot([w_crit w_crit], Ylim, 'r')
plot([w_crit1 w_crit1], Ylim, 'r')
plot([w_crit2 w_crit2], Ylim, 'r')
plot([w_crit3 w_crit3], Ylim, 'r')
plot([w_crit4 w_crit4], Ylim, 'r')

ylim(Ylim)
legend(num2str(C'))
title('Amplification - Various Damping Coefficients (Legend)')

%% Surf

Xlim = [0 5];
Ylim = [0 10^-3];

figure()


m = 2 ;
M = 600 ;
D0 = 0.1 ;
k1 = 2000 ;
k2 = 1000;


syms omega c;
A = [ -m*omega^2+k2+k1+1i*omega*c -k2-1i*omega*c;
      -k2-1i*omega*c     -M*omega^2+k2+1i*omega*c ] ;

X = A^-1 * [1; 0]*D0;

 ezsurf(abs(X(2)),[100 10000 0 3]);

hold all;


title('Amplification')
