function out=rates(in)
%compute the rates to be commanded in order to keep zero side-slip

%10/2009 Stefan Leutenegger


param;

phi_dot_d=in(1);
theta_dot_d=in(2);
u=in(3);
w=in(4);
phi=in(5);
theta=in(6);

% calculate psi_dot_d keeping zero sideslip
psi_dot_d=((w*phi_dot_d+g*sin(phi)*cos(theta))/u+theta_dot_d*sin(phi))...
    /(cos(phi)*cos(theta)+w/u*sin(theta));

Rr=[1 0 -sin(theta);...
    0 cos(phi) sin(phi)*cos(theta);...
    0 -sin(phi) cos(phi)*cos(theta)];

%R_r_inv=[1 tan(theta)*sin(phi) tan(theta)*cos(phi);...
%         0 cos(phi) -sin(phi);...
%         0 sin(phi)/cos(theta) cos(phi)/cos(theta)];%jacobian inverse: body rates to Tait-Bryan Angles derivatives

out1=Rr*[phi_dot_d; theta_dot_d; psi_dot_d];
out(1)=out1(1);
out(2)=out1(2);
out(3)=out1(3);