
clear all
close all

%--- Densites [kg/m^3]
rho_s = 1430;   % sirop
%rho_a = ...;   % acier: A CHERCHER

%--- Diametres [m]
d = 1e-3*[4.0 6.3 9.5 12.7];

%--- Donnees mesurees a partir des videos: temps t [s] et distance parcourue D [m]  
% A COMPLETER
t1 = [0:5:60];      t2 = [0:5:60];
t3 = [0:3:30];      t4 = [0:3:21];
D1 = 1e-2*[0  0.4  0.7  1.1  1.4  1.7  2.1  2.4  2.8  3.1  3.5  3.8  4.2];
D2 = 1e-2*[0  0.8  1.6  2.5  3.3  4.1  4.9  5.7  6.6  7.4  8.2  9.0  9.8];
D3 = 1e-2*[0  1    1.8  2.9  3.9  4.8  5.7  6.6  7.6  8.6  9.6];
D4 = 1e-2*[0  1.5  2.9  4.4  5.8  7.3  8.7  10.1];


%------ Question 2 ---------------------------------------------------
%--- On trace D(t)
fig_D_t = figure;  hold on,  box on,  grid on, xlabel('t [s]'),  ylabel('D [m]')
 plot(t1,D1,t2,D2,t3,D3,t4,D4);


%------ Question 4 --------------------------------------------------
method = 'a'    % On choisit ici la methode a ou b

switch method
    case 'a'
    %--- On calcule et trace la vitesse U [m/s] entre chaque paire de mesures (U=delta_D/delta_t)
    
     U1 = (D1(2:end) - D1(1:end-1)) / (t1(2)-t1(1));  % A COMPLETER
     U2 = (D2(2:end) - D2(1:end-1)) / (t2(2)-t2(1));
     U3 = (D3(2:end) - D3(1:end-1)) / (t3(2)-t3(1));
     U4 = (D4(2:end) - D4(1:end-1)) / (t4(2)-t4(1));
    
    fig_U_t = figure;  hold on,  box on,  grid on, xlabel('t [s]'),  ylabel('U [m/s]')
     plot(t1(1:end-1),U1,t2(1:end-1),U2,t3(1:end-1),U3,t4(1:end-1),U4)  % A COMPLETER
   
    %--- On calcule la vitesse moyenne Um [m/s] et l'ecart-type sigma [m/s]
%     U1m = ...    % A COMPLETER
%     U2m = ...
%     U3m = ...    
%     U4m = ...
%     sigma1 = ...   % A COMPLETER  
%     sigma2 = ...
%     sigma3 = ...   
%     sigma4 = ...
    
    case 'b'
    %--- Regression lineaire
    [c1,g1] = fit(t1',D1', fittype('poly1'));   
%     [c2,g2] = ...   % A COMPLETER
%     [c3,g3] = ...
%     [c4,g4] = ...
    
    %--- Pour simplifier on prend la pente comme vitesse moyenne
    U1m = c1.p1;
%     U2m = ...   % A COMPLETER
%     U3m = ...
%     U4m = ...
    %--- Erreur RMSE
    sigma1 = g1.rmse;   
%     sigma2 = ...   % A COMPLETER 
%     sigma3 = ...
%     sigma4 = ...
end

%--- On trace Um*t sur la figure de D(t)
figure(fig_D_t)
% plot(...)   % A COMPLETER

%------ Question 5 --------------------------------------------------
%--- On trace la vitesse en fonction du diametre 
fig_U_d = figure;  hold on,  box on,  grid on, xlabel('d [m]'),  ylabel('Um [m/s]')
% errorbar(...)   % A COMPLETER


%------ Question 6 --------------------------------------------------
%--- Meme chose en echelle log-log  
% set(...)   % A COMPLETER
% plot(...)
% plot(...)
legend('Um(d)','dependance lineaire','dependance quadratique')


%------ Questions 11-12 ---------------------------------------------
% mu = ...      % A COMPLETER
% muErr = ...

figure,  hold on,  box on,  grid on, xlabel('d [m]'), ylabel('mu [kg/m/s]')
% errorbar(...)   % A COMPLETER


%------ Question 13 -------------------------------------------------
% Re = ...     % A COMPLETER
