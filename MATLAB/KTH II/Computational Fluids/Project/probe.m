n = 3; % set to 4 to include case D

load('A.mat')
sA = s;
tA = tser';
TA = abs(Tser)';
load('B.mat')
sB = s;
tB = tser';
TB = abs(Tser)';
load('C.mat')
sC = s;
tC = tser';
TC = abs(Tser)';

if n == 4
load('D.mat') 
sD = s;
tD = tser';
TD = abs(Tser)';
RD = Re;
else
sD = nan;
tD = nan;
TD = nan; 
RD = nan;
end


p = NaN(max([sA sB sC sD]),n);
t = NaN(max([sA sB sC sD]),n);

p(1:sA,1) = TA;
t(1:sA,1) = tA;

p(1:sB,2) = TB;
t(1:sB,2) = tB;

p(1:sC,3) = TC;
t(1:sC,3) = tC;

if n == 4
    p(1:sD,4) = TD;
    t(1:sD,4) = tD;
end

figure(4); set(figure(4), 'Name', 'Probe Plot');
plot(t, p)
title(sprintf('Probe signal at x=%g, y=%g',...
      x(ceil((Nx+1)/2)),y(ceil((Ny+1)/2))))
%set(gca,'yscale','log')
xlabel('time t [s]');ylabel('u(t)')
legend('A: Re=10','B: Re=100','C: Re=8000',['D: Re=' num2str(RD)])

 t99A = tA'*[0; diff(TA>0.99*TA(end))];
 t99B = tB'*[0; diff(TB>0.99*TB(end))];
 line([t99A t99A],ylim(),'Color','b')
 text(t99A-0.5,-0.02,{['t99 = ' num2str(t99A)]})
 line([t99B t99B],ylim(),'Color','g')
 text(t99B-0.5,-0.02,{['t99 = ' num2str(t99B)]})
 if n==4
 t99D = tD'*[0; diff(TD>0.99*TD(end))];
 line([t99D t99D],ylim(),'Color','c')
 text(t99D-0.5,-0.02,{['t99 = ' num2str(t99D)]})
 end
 