close all; clear all;

%n = [5, 10, 20, 40];
%errors = 0;
%for i = 1:4

[err,t,f,x,p, fx]=intlag(10);
plot(x, fx,'-g','LineWidth',3);
hold all;
plot(t, f,'--ks','LineWidth',1, 'MarkerEdgeColor','k','MarkerFaceColor','w','MarkerSize',10);
plot(x, p,'-r','LineWidth',2);

%errors(i)=err;
%end

