function [V, Z] = fourDOF(m1, m2, m3, j, l, p, k1, k2, k3, c1, c2, c3, d, z)

A = dlmread('bil0402.dat');

V = A(:,1);
Z = A(:,2:5);

end

