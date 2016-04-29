function [ q_new ] = propagateQuat( w, q_old, dt )
%1st order integration of quaternion q_old with rot. vel. w for timespan dt;

if (norm(w)*dt/2 < 1E-7)
    q_new = q_old;
else
  Omega = omegaMat(w);
  
  Theta = eye(4) + 0.5*Omega*dt;
  
  q_new = Theta*q_old;
  q_new = q_new/norm(q_new);
end
end