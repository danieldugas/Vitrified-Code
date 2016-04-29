function [ result ] = lorenz_product( u, v )
% if u and v are 4-element vectors, the result is a scalar
% of value u1*v1 + u2*v2 + u3*v3 - u4*v4
% if they are 4xn matrices, the result is a 1xn matrix
% of corresponding lorenz products
szu = size(u);
szv = size(v);
result = 0;
if ( szu(1) == 4 ) && ( szv(1) == 4 ) && ( szv(2) == szu(2) )
  result = u(1,:).*v(1,:) + u(2,:).*v(2,:) + u(3,:).*v(3,:) - u(4,:).*v(4,:) ;
else 
  error('lorenz_product requires inputs of identical size, with a first dimension of size 4')
end

end

