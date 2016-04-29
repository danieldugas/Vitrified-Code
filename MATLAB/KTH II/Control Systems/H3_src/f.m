function [dx, qo] = f(x, qi)

%global g;
%global l;
g = 9.81;
l = 10;

dx = zeros(size(x));            %pre-allocation

switch qi
    case 1
        dx = [x(2)             ; %theta1'
              -(g/l)*sin(x(1)) ; %omega1'
              x(4)             ; %theta2'
              -(g/l)*sin(x(3)) ; %omega2'
              x(6)             ; %theta3'
              -(g/l)*sin(x(5)) ];%omega3'
    case 2
        dx = [x(2)             ; %theta1'
              -(g/l)*sin(x(1)) ; %omega1'
              x(4)             ; %theta2'
              -(g/l)*sin(x(3)) ; %omega2'
              x(6)             ; %theta3'
              -(g/l)*sin(x(5)) ];%omega3'
    case 3
        dx = [x(2)             ; %theta1'
              -(g/l)*sin(x(1)) ; %omega1'
              x(4)             ; %theta2'
              -(g/l)*sin(x(3)) ; %omega2'
              x(6)             ; %theta3'
              -(g/l)*sin(x(5)) ];%omega3'
    otherwise
        error = 'qi out of bounds in f function'
end

qo = qi;
end