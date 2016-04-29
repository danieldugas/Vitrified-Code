function out=DCM(in)
phi=in(1);
theta=in(2);
psi=in(3);

%transformation matrix
R_EB=[cos(psi)*cos(theta) cos(psi)*sin(theta)*sin(phi)-sin(psi)*cos(phi) cos(psi)*sin(theta)*cos(phi)+sin(psi)*sin(phi);...
      sin(psi)*cos(theta) sin(psi)*sin(theta)*sin(phi)+cos(psi)*cos(phi) sin(psi)*sin(theta)*cos(phi)-cos(psi)*sin(phi);...
      -sin(theta) cos(theta)*sin(phi) cos(theta)*cos(phi)]; %Body to Earth
  
out =R_EB';