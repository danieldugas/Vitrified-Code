load('F_Signal.txt');
plot(F_Signal,'LineWidth',1);figure(gcf);
hold all;
precision=100;
plot ((1:length(F_Signal)/precision:length(F_Signal)), F_Signal(1:precision),'LineWidth',2);