function [element,CodeError]=ActivatedQuarterwave(c,rho,Q,L,S,Ls,Ss,xi,eeta,frequency);

f=[frequency.minimum:frequency.resolution:frequency.maximum]';  % Frequency column vector
lasterr('');
lastwarn('');

try