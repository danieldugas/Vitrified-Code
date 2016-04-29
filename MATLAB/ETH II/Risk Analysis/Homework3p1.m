%% Indirect Monte-Carlo Simulation
%
%  First case scenario of third Homework
%  By Daniel Dugas

close all; clear all;

Tm = 24*365; % 1 year
L = [ 0    1/2000 1/1000;
    0    0      1/1000;
    1/10 0      0      ];
l0 = sum(L(1,:));
l1 = sum(L(2,:));
l2 = sum(L(3,:));

%% Interface
fig  = figure(1);
set(fig,'Position',[48 161 560 420],'NumberTitle','off')
fig2 = figure(2);
set(fig2,'Name','Histograms for Average # of Failures')
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

%% Constants
N = 10:10:100;
Iterations = 100; % Number of iterations per MC loop
iterations_count = 0;
total_iterations = sum(N*Iterations);
for k=1:10
    %% Initialising Fn Loop Values and Tables
    Simulations = N(k);
    
    %% Average Fn Histogram Loop
    for n=1:Simulations
        
        %% Initialising Monte Carlo Loop Values and Tables
        dt = 1;
        time = 0:dt:Tm;
        P = size(time,2); % time-Points
        states = zeros(Iterations,P);
        unrel_states = zeros(Iterations,P);
        
        %% Monte Carlo Loop
        for i=1:Iterations
            state = 0;
            t = 0;
            
            %% Run one iteration
            while(t<Tm)
                switch state
                    case 0
                        next_t = t+exprnd(1/l0);
                        next_state = 2 - (rand() < (L(1,2)/l0));
                    case 1
                        next_t = t+exprnd(1/l1);
                        next_state  = 2;
                    case 2
                        next_t = t+exprnd(1/l2);
                        next_state  = 0;
                    otherwise
                        error('Illegal State Reached');
                end
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
                figure(fig)
                set(fig, 'Name',['Monte Carlo Simulation #' num2str(n)...
                    ' - Iteration #' num2str(i)])
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
        
        Fn(n) = sum(F)/Iterations;
        if cancelled
            break
        end
    end
    
    vF(k) = var(Fn);
    
    figure(fig2)
    subplot(4,4,k)
    hist(Fn,Simulations)
    title(['N = ' num2str(N(k)) ' - Var = ' num2str(vF(k),2)])
    if cancelled
        break
    end
end
close(w)

if ~cancelled
    figure(fig2)
    subplot(4,4,k+1)
    plot(N,[vF; 1./N])
    title('N to Var')
    legend('Var','1/N')
    set(legend,'Position',[0.75 0.35 0.14 0.10])
end