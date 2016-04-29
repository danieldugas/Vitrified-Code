%% Direct Monte-Carlo Simulation
%
%  Second case scenario of third Homework
%  By Daniel Dugas

close all; clear all;

Tm = 24*365; % 1 year
dt = 1; Iterations=100;
time = 0:dt:Tm;
% Exponential Distribution Parameters
L = [ 0    1/2000 1/1000;
      0    0      1/1000;
      1/10 0      0      ];
l0 = sum(L(1,:));
l1 = sum(L(2,:));
l2 = sum(L(3,:));
% Weibull Distribution Parameters
alpha = 3;
beta = 10^(-11);
b = alpha;
a = 1/beta^(1/alpha);

%% Interface
fig  = figure(1);
set(fig,'Position',[48 161 560 420],'NumberTitle','off')
fig2 = figure(2);
set(fig2,'Name','Monte Carlo Simulation')
set(fig2,'Position',[635 161 560 420],'NumberTitle','off')
w = waitbar(0, 'Work in Progress.');
set(w,'Position', [347 26 270 56])
withvisuals = 1;
visuals_btn = uicontrol('Style', 'pushbutton',...
    'String', 'Toggle Visuals (faster without)',...
    'Position', [20 20 200 40],...
    'Callback', 'withvisuals = ~withvisuals;');
cancelled = 0;
cancel_btn = uicontrol('Style', 'pushbutton',...
    'String', 'Cancel',...
    'Position', [340 20 200 40],...
    'Callback', 'cancelled = 1;');

%% Plot the Hazard Rates
figure(fig)
weibull_hazard_rate = alpha*beta*time.^(alpha-1);
unrev_hazard_rate = time*0+ L(1,2);
rev_hazard_rate   = time*0+ L(1,3);
subplot(2,1,1)
plot(time,[weibull_hazard_rate;unrev_hazard_rate;rev_hazard_rate])
legend('Weibull Distribution Hazard Rate',...
    'Unrevealed Hazard Rate',...
    'Revealed Hazard Rate')

%% Plot the CDF
weibull_cdf = 1-exp(-beta*time.^alpha);
unrev_cdf   = 1-exp(-L(1,2)*time);
rev_cdf     = 1-exp(-L(1,3)*time);
subplot(2,1,2)
plot(time,[weibull_cdf;unrev_cdf;rev_cdf])
legend('Weibull CDF',...
    'Unrevealed CDF',...
    'Revealed CDF')


%% Initialising Monte Carlo Loop Values and Tables
P = size(time,2); % time-Points
states = zeros(Iterations,P);
unrel_states = zeros(Iterations,P);
iterations_count = 0;
total_iterations = Iterations;
        
%% Direct Monte Carlo Loop
for i=1:Iterations
    state = 0;
    t = 0;
    
    %% Run one iteration
    while t<Tm
        % sample times
        tj(1) = t+exprnd(1/l2);             %time to state 0
        tj(2) = t+wblrnd(a,b);              %time to state 1
        tj(3) = t+wblrnd(a,b);              %time to state 2
        if state > 0, tj(state) = NaN; end  %can't go from i to i-1
        tj(state+1) = NaN;                  %ignore t from i to i
        
        [next_t,j] = min(tj);
        next_state = j-1;
        
        states(i,((t < time) & (time < next_t))) = state;
        
        t = next_t;
        state = next_state;
    end
    states(i,t < time) = state; % fixes the last states
    
    %% Unreliability and Failures Calculation
    % first occurence of a non-zero state
    first_failure_index = find(states(i,:)>0,1,'first');
    unrel_states(i,first_failure_index:end) = 1;
    % Number of Failures
    F(i) = sum( diff( states(i,:)>0 )>0 );
    
    %% Plotting
    if withvisuals
        figure(fig2)
        set(fig2, 'Name',['Monte Carlo Simulation - Iteration #' num2str(i)])
        subplot(3,1,1)
        plot(time,states(1:i,:)')
        title('States')
        subplot(3,1,2)
        plot(time,sum(states(1:i,:)>0,1),'k--')
        title('Sum of failure states')
        subplot(3,1,3)
        plot(time,sum(states(1:i,:)>0,1)/i,'k-')
        title('Unavailability, Unreliability')
        hold on
        plot(time,sum(unrel_states(1:i,:),1)/i,'b-')
        hold off
        axis([0 Tm 0 1])
        pause(0.01);
    end
    iterations_count = iterations_count + 1;
    progress = iterations_count/total_iterations;
    waitbar(progress,w,...
        ['Work in Progress - ', num2str(100*progress), '% done.']);
    if cancelled
        break
    end
end

close(w)