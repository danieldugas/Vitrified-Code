function [element,CodeError]=ActivatedExpansion(c,rho,Q,L,S,Lc,Sc,xi,frequency);

f=[frequency.minimum:frequency.resolution:frequency.maximum]';  % Frequency column vector
lasterr('');
lastwarn('');

try