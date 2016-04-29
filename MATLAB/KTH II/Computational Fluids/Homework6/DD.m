function A = DD(n,h)
% DD(n,h)
%
% One-dimensional finite-difference derivative matrix 
% of size n times n for second derivative:
% h^2 * f''(x_j) = -f(x_j-1) + 2*f(x_j) - f(x_j+1)
%
% Homogeneous Neumann boundary conditions on the boundaries 
% are imposed, i.e.
% f(x_0) = f(x_1) 
% if the wall lies between x_0 and x_1. This gives then
% h^2 * f''(x_j) = + f(x_0) - 2*f(x_1) + f(x_2)
%                = + f(x_1) - 2*f(x_1) + f(x_2)
%                =              f(x_1) + f(x_2)
%
% For n=5 and h=1 the following result is obtained:
%
% A =
%
%    -1     1     0     0     0
%     1    -2     1     0     0
%     0     1    -2     1     0
%     0     0     1    -2     1
%     0     0     0     1    -1
%
% This function belongs to SG2212.m

a = ones(1,n)'; %left diagonal
b = [-1 -2*ones(1,n-2) -1]'; %center diagonal
c = a; %right diagonal
A = spdiags([a b c], [-1 0 1], n, n)/h^2;

%full(A) % shows the full matrix. Used for debugging.

% "spdiags" generalises the function "diags" such that multiple
% vectors can be provided which are then put on the 
% respective diagonals. In addition, sparse storage is used.
% See "help spdiags" for more information.