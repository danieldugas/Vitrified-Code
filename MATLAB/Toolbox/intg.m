function B = intg(A, dim)
%INTG Trapezoidal integration of a vector
%   like trapz, but instead of returning
%   the integral for the entire domain,
%   returns the trapz value at each point.

%   default dim = 1 (along x)
if nargin<2, dim = 1; end

nm = size(A);
n  = nm(1);
m  = nm(2);

if (dim == 1)
    trapezoids = [0; avg(A)]; %*dt?
    L = tril(ones(n));
    % L*areas sums the trapezoids
    B = L*trapezoids;
end
end

