function A = D(n,h)
% D(n,h)
%
% One-dimensional finite-difference derivative matrix 
% of size n times n for first derivative:
% h^2 * f''(x_j) = f(x_j+1) - f(x_j-1)
%
% Homogeneous Neumann boundary conditions on the boundaries 
% are imposed, i.e.
% f'(t=0) = 0
%
% For n=5 and h=1 the following result is obtained:
%
% A =
%
%    -2     2     0     0     0
%    -1     0     1     0     0
%     0    -1     0     1     0
%     0     0    -1     0     1
%     0     0     0    -2     2
%

a = [-ones(1,n-1) 2]'; %left diagonal
b = [-2 zeros(1,n-2) 2]'; %center diagonal
c = [2 ones(1,n-1)]'; %right diagonal
A = spdiags([a b c], [-1 0 1], n, n)/h;

%full(A) % shows the full matrix. Used for debugging.

% "spdiags" generalises the function "diags" such that multiple
% vectors can be provided which are then put on the 
% respective diagonals. In addition, sparse storage is used.
% See "help spdiags" for more information.
