function [V, Z] = twoDOF(m1, m2, m3, j, l, p, k1, k2, k3, c1, c2, c3, d, z)

V = (10:0.5:120)' ; 

m = m1/2;
O = 2*pi*V/(3.6*d) ; %omega as a function of speed (V converted to m/s)

Z(:,1) = (    (k2+1i*c2*O) .* (k1+1i*c1*O) * z      )./ ...
 ( (k2+1i*c2*O-m*O.^2) .* (k2+k1+1i*(c1+c2)*O-m2*O.^2) - (k2+1i*c2*O).^2 );

Z(:,2) = (    (k2+1i*c2*O-m*O.^2) .* (k1+1i*c1*O) * z      )./ ...
 ( (k2+1i*c2*O-m*O.^2) .* (k2+k1+1i*(c1+c2)*O-m2*O.^2) - (k2+1i*c2*O).^2 );

Z = real(Z);

end

