function B = avg(A,dim)
% AVG(A,dim)
%
% Averaging function to go from cell centres (pressure nodes)
% to cell corners (velocity nodes) and vice versa.
% avg acts on index idim; default is idim=1.
%
% This function belongs to SG2212.m

if nargin<2, dim = 1; end

if (dim==1)
  B = ( A(2:end,:)+A(1:end-1,:) )/2;  %x-direction average of A
elseif (dim==2)
  B = ( A(:,2:end)+A(:,1:end-1) )/2;  %y-direction average of A
else
  error('avg(A,idim): idim must be 1 or 2')
end