function [ mat ] = skew_square( vec )
%SKEW_SQUARE Summary of this function goes here
%   Detailed explanation goes here

if size(vec,1) < 3
    vec = vec';
end

mat = vec*(vec') - (vec')*vec*eye(3);

end

