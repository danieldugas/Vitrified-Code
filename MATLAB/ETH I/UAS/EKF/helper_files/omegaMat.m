function [ Omega] = omegaMat(w)
%calculates Omega out of ang. vel. w according to 
%Simon Lynen's EKF-Framework

if (size(w,1) ~= 3)
    
    disp('input argument must be a 3x1 vector');
    
else

Omega = [0       w(3)    -w(2)     w(1);...
        -w(3)      0      w(1)     w(2);...
         w(2)   -w(1)       0      w(3);...
        -w(1)   -w(2)    -w(3)       0];
    
end

end