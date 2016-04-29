function V = simulation_error(simulation_parameters, reference_data)
% Executes the simulink model with the specified parameters, 
% outputs a scalar V representing the magnitude of the error
% between the reference data and simulated data.

Vm = simulation_parameters(1);
Vdot_ideal = simulation_parameters(2);
assignin('base', 'Vm', Vm);
assignin('base', 'Vdot_ideal', Vdot_ideal);

t = reference_data.time;
mdotin = [t,reference_data.mdotin];
p   = reference_data.p;


[tsim,x,psim] = sim('manifold',t,[],mdotin);
%sim(model name, time vector, flags, input)

plot([t,t], [p,psim])
legend('p','psim')
title(['Vm = ', num2str(Vm), '   Vdot = ', num2str(Vdot_ideal)])


V = sum((psim-p).^2);



end

