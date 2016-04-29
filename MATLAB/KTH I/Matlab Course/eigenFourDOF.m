function [V, Z] = eigenFourDOF(m1, m2, m3, j, l, p, k1, k2, k3, c1, c2, c3, d, z)

K = [ k2+k3             -k2     -k3         (p*k2-(1-p)*k3)*l       ;
      -k2               k1+k2   0           -p*k2*l                 ;
      -k3               0       k1+k3       (1-p)*k3*l              ;
      (p*k2-(1-p)*k3)*l -p*k2*l (1-p)*k3*l  k2*(p*l)^2+k3*((1-p)*l)^2 ];

M = blkdiag(m1, m2, m3, j);

[Z, L] = eig(K, M);

%we divide each column by it's first value, the resulting columns are  
%eigenvectors containing the amplitudes of z2 z3 z4 in relation to z1
Z(:,1:4) = Z(:,1:4)./repmat(Z(1,1:4),4,1);
%the critical speeds in km/h
V = 3.6*d*(diag(L).^0.5)/(2*pi);

end

