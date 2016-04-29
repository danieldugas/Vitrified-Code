function V = pm_error(simulation_parameters)
% Executes the simulink model with the specified parameters, 
% outputs a scalar V representing the magnitude of the error
% between the reference data and simulated data.

Vm = simulation_parameters;
assignin('base', 'Vm', Vm);

[null,null2,YOLO] = sim('plant');
assignin('base', 'YOLO', YOLO);

%plot([t,t], [p,psim])
%legend('p','psim')
%title(['Vm = ', num2str(Vm), '   Vdot = ', num2str(Vdot_ideal)])

Vm

V = sum(abs(YOLO(:,1)-YOLO(:,2)));

end

