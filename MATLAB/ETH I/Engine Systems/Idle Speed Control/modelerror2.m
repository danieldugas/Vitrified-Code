function V = modelerror2(par, meas)
% Executes the simulink model with the specified parameters, 
% outputs a scalar V representing the magnitude of the error
% between the reference data and simulated data.

InitPar     % Reload initial values since we are working in a different workspace

eta0 = par(1);   % Setting the manifold volume for the current iteration of fminsearch
eta1 = par(2);
Vm = par(3);

[time,x,y] = sim('plant',...
    [meas.time(1)' meas.time(end)'],...         % Enter timevector for certain measurement
    sim_opt,...                                 % Simulation options see IdleSpeedControl.m
    [meas.time' meas.omega_e.signals.values]);      % inputs for the simulation, time and p_m

%% Calculate the error between measurement and simulation
V = sum((meas.omega_e.signals.values/230 - y(:,2)/230).^2);
% Normalize the pressure so that the error value does not have a too large
% value. This can lead to problems sometimes

%% plot result of current iteration
get(0,'CurrentFigure'); % use current figure - do not set it on top in each update process
plot(meas.time', meas.omega_e.signals.values, 'b')
hold on
grid on;
plot(time,y(:,2),'-r')
hold off
xlabel('Time [s]')
ylabel('Engine Speed [Rad/s]')
legend('Measurements','Simulation','Location','NorthEast')

set(gca,'XLim',[meas.time(1)' meas.time(end)']);
set(gca,'YLim',[min(meas.omega_e.signals.values)-1/10*mean(meas.omega_e.signals.values) max(meas.omega_e.signals.values)+1/10*mean(meas.omega_e.signals.values)]);
drawnow

