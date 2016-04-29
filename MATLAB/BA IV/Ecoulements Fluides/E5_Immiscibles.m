clear all
close all

set(0,'DefaultLineLineWidth',1);
set(0,'DefaultAxesFontSize',16);

% aribtrary values for K and eta1
K = 1;
eta1 = 1;           % interior fluid
eta2 = eta1/10;     % exterior fluid


%----- Question 1 ---------------------------------------------------
% Plot analytical flow rate Q1(h)
h = [0:0.01:1];
Q1b = K/eta1*2*pi*(h.^4/16) - K/eta2*2*pi*(h.^4/8-h.^2/8);
fig1=figure; plot(h,Q1b,'k'),  hold on
xlabel h, ylabel Q1, title('internal flow rate')

% Numerical integration for a few values of h
h0 = 0.2;
r = [0:0.01:h0];
u1 = -K/4/eta1*(r.^2-1) - K/4/eta2*(h0^2-1) + K/4/eta1*(h0^2-1);
Q0 = trapz(r,u1.*r*2*pi);
plot(h0,Q0,'ok')
 
h0 = 0.5;
r = [0:0.01:h0];
u1 = -K/4/eta1*(r.^2-1) - K/4/eta2*(h0^2-1) + K/4/eta1*(h0^2-1);
Q0 = trapz(r,u1.*r*2*pi);
plot(h0,Q0,'ok')

h0 = 0.8;
r = [0:0.01:h0];
u1 = -K/4/eta1*(r.^2-1) - K/4/eta2*(h0^2-1) + K/4/eta1*(h0^2-1);
Q0 = trapz(r,u1.*r*2*pi);
plot(h0,Q0,'ok')

saveas(gca,'fig1.pdf')
saveas(gca,'fig1.eps','psc2')

%----- Question 2 ---------------------------------------------------
[Q1max,ind] = max(Q1b);
disp(['max flow rate Q1max = ' num2str(Q1max) ' at h = ' num2str(h(ind))])

Q = pi*K/8/eta1 % flow rate for non-lubricated case (only one fluid) 
plot(h,Q*ones(size(h)),'k-.')
disp(['ratio Q1max/Q = ' num2str(Q1max/Q)])

saveas(gca,'fig2.pdf')
saveas(gca,'fig2.eps','psc2')

%----- Question 3 ---------------------------------------------------
eta2 = eta1./[1000 300 100 30 10 3 1]; % for this question only, eta2 can vary
Q1m = zeros(size(eta2));
for j=1:length(eta2)
    Q1m(j) = max(K/eta1*2*pi*(h.^4/16) - K/eta2(j)*2*pi*(h.^4/8-h.^2/8));
end
figure, loglog(eta2,Q1m,'ob-'), xlabel eta2, ylabel Q1max
title('Max. inner flow rate versus outer viscosity')

saveas(gca,'fig3.pdf')
saveas(gca,'fig3.eps','psc2')

%----- Question 4 ---------------------------------------------------
eta2 = eta1/10;

% Find h such that Q1 = ratio*Q
ratio = 2;
h1 = sqrt(roots([K/eta1*pi/8-2*K/eta2*pi/8; K/eta2*pi/4; -ratio*K/eta1*pi/8]))
Q11 = (K/eta1*pi/8-2*K/eta2*pi/8)*h1(1)^4 + K/eta2*pi/4*h1(1)^2;
Q12 = (K/eta1*pi/8-2*K/eta2*pi/8)*h1(2)^4 + K/eta2*pi/4*h1(2)^2;

figure(fig1);
plot(h,ratio*Q*ones(size(h)),'k--')         % value of Q1 we want to reach
plot([h1(1) h1(1)],[0 Q11],'b')             % first solution
plot([h1(2) h1(2)],[0 Q12],'r')             % second solution

saveas(gca,'fig4.pdf')
saveas(gca,'fig4.eps','psc2')

%----- Question 5 ---------------------------------------------------
figure, hold on
r = -1:0.001:1;
plot(r,  -K/4/eta1*(r.^2-1),'k-.') % velocity profile for non-lubricated case (only one fluid)  

% First solution
r11 = r( abs(r)<=h1(1) );                                        % extract the part of vector r corresponding to inner fluid
r12 = r( r>=h1(1) );                                             % extract the part of vector r corresponding to outer fluid
% Second solution
r21 = r( abs(r)<=h1(2) );
r22 = r( r>=h1(2) );

plot(r11,  -K/4/eta1*(r11.^2-h1(1).^2)-K/4/eta2*(h1(1).^2-1))        % plot inner profile, for solution 1
plot(r21,  -K/4/eta1*(r21.^2-h1(2).^2)-K/4/eta2*(h1(2).^2-1),'r--')  % plot inner profile, for solution 2
plot(r12,  -K/4/eta2*(r12.^2-1))                                     % plot outer profiles, for solution 1
plot(-r12, -K/4/eta2*(r12.^2-1))
plot(r22,  -K/4/eta2*(r22.^2-1),'r--')                               % plot outer profiles, for solution 2
plot(-r22, -K/4/eta2*(r22.^2-1),'r--')

xlabel r, ylabel u
legend('non-lubricated','lubricated, solution 1','lubricated, solution 2')
title('velocity profiles')

saveas(gca,'fig5.pdf')
saveas(gca,'fig5.eps','psc2')