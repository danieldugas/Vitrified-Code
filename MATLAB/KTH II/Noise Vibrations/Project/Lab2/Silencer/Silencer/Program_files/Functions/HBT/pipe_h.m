function [element,CodeError]=ActivatedPipe(c,rho,Q,L,S,xi,frequency);

f=[frequency.minimum:frequency.resolution:frequency.maximum]';  % Frequency column vector
lasterr('');
lastwarn('');

try