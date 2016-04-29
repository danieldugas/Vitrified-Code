function out=dynamics(in);

param;

%inputs:
u=in(1);
v=in(2);
w=in(3);

p=in(4);
q=in(5);
r=in(6);

phi=in(7);
theta=in(8);
psi=in(9);

Fx=in(10);
Fy=in(11);
Fz=in(12);

Mx=in(13);
My=in(14);
Mz=in(15);

%transformation matrices
R_EB=[cos(psi)*cos(theta) cos(psi)*sin(theta)*sin(phi)-sin(psi)*cos(phi) cos(psi)*sin(theta)*cos(phi)+sin(psi)*sin(phi);...
      sin(psi)*cos(theta) sin(psi)*sin(theta)*sin(phi)+cos(psi)*cos(phi) sin(psi)*sin(theta)*cos(phi)-cos(psi)*sin(phi);...
      -sin(theta) cos(theta)*sin(phi) cos(theta)*cos(phi)]; %Body to Earth

R_r_inv=[1 tan(theta)*sin(phi) tan(theta)*cos(phi);...
         0 cos(phi) -sin(phi);...
         0 sin(phi)/cos(theta) cos(phi)/cos(theta)];%jacobian inverse: body rates to Tait-Bryan Angles derivatives

%equations of motion:
u_dot=r*v-q*w+1/M*Fx-g*sin(theta);
v_dot=p*w-r*u+1/M*Fy+g*sin(phi)*cos(theta);
w_dot=q*u-p*v+1/M*Fz+g*cos(phi)*cos(theta);
X_dot=R_EB*[u;v;w];
x_dot=X_dot(1);
y_dot=X_dot(2);
z_dot=X_dot(3);

p_dot=1/Ixx*(Mx-q*r*(Izz-Iyy));
q_dot=1/Iyy*(My-p*r*(Ixx-Izz));
r_dot=1/Izz*(Mz-p*q*(Iyy-Ixx));
%disp(p_dot);
%disp(q_dot);
%disp(r_dot);
%disp(['t=' clock])
Angles_dot=R_r_inv*[p;q;r];
phi_dot=Angles_dot(1);
theta_dot=Angles_dot(2);
psi_dot=Angles_dot(3);

% assign outputs:
out(1)=x_dot;
out(2)=y_dot;
out(3)=z_dot;

out(4)=phi_dot;
out(5)=theta_dot;
out(6)=psi_dot;

out(7)=u_dot;
out(8)=v_dot;
out(9)=w_dot;

out(10)=p_dot;
out(11)=q_dot;
out(12)=r_dot;
 
