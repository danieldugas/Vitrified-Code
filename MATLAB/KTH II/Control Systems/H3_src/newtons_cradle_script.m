%% PART I - Computation
clear all; close all;
x = [pi/8;
     0;
     0;
     0;
     0;
     0];
q =  1;
g =  9.81;
l =  10;
d =  1;

t = 0; i = 0; j = 0;
dt = 0.01;
while t <= 20
    t = t+dt
    i = i+1;
    %if mod(t,0.10) == 0
        j = j+1
        t_store(j) = t;
        q_store(j) = q;
        theta_store(1,j) = x(1);
        theta_store(2,j) = x(3);
        theta_store(3,j) = x(5);
    %end
    [x_prime, q] = f(x, q);
    x = x+x_prime*dt;
    [x, q] = edge_check(x, q);
end

%% PART II - Display Results
X = kron([d; 0; -d],ones(1,j)) + sin(theta_store);
Y = 1-cos(theta_store);
plot3(X,t_store,Y);
zlim([0 0.1]);
xlim([-1.5 1.5]);
xlabel('x [m]');
ylabel('t [s]');
zlabel('y [m]');
figure();
stairs(t_store,q_store);
xlim([0 20]);
xlabel('t [s]');
ylabel('q');