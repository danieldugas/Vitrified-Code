function [element,CodeError]=ActivatedHelmholtz(c,rho,Q,L,S,Ln,Sn,Lc,Sc,xi,r,Lr,frequency);

f=[frequency.minimum:frequency.resolution:frequency.maximum]';  % Frequency column vector
lasterr('');
lastwarn('');

try