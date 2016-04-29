function V = modelerror(par, meas)
% Executes the simulink model with the specified parameters, 
% outputs a scalar V representing the magnitude of the error
% between the reference data and simulated data.

InitPar     % Reload initial values since we are working in a different workspace

Vm = par;   % Setting the manifold volume for the current iteration of fminsearch

[time,x,y] = sim('plant',...
    [meas.time(1)' meas.time(end)'],...         % Enter timevector for certain measurement
    sim_opt,...                                 % Simulation options see IdleSpeedControl.m
    [meas.time' meas.p_m.signals.values]);      % inputs for the simulation, time and p_m

%% Calculate the error between measurement and simulation
V = sum((meas.p_m.signals.values/1e5 - y(:,1)/1e5).^2);
% Normalize the pressure so that the error value does not have a too large
% value. This can lead to problems sometimes

%% plot result of current iteration
get(0,'CurrentFigure'); % use current figure - do not set it on top in each update process
plot(meas.time', meas.p_m.signals.values, 'b')
hold on
grid on;
plot(time,y(:,1),'-r')
hold off
xlabel('Time [s]')
ylabel('Intake Manifold Pressure [Pa]')
legend('Measurements','Simulation','Location','NorthEast')

set(gca,'XLim',[meas.time(1)' meas.time(end)']);
set(gca,'YLim',[min(meas.p_m.signals.values)-1/10*mean(meas.p_m.signals.values) max(meas.p_m.signals.values)+1/10*mean(meas.p_m.signals.values)]);

drawnow

