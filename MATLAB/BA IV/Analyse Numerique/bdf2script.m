close all; clear all;
x0 = 20;
min = 0;
max = 15;
d = 0.01;
h = 10.^-(min:d:max);
n = ((max-min)/d)+1;
for i = 1:n;
errors(i)=bdf2(x0,h(i));
end
plot(min:d:max, errors);
