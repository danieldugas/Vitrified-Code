%% Monte-Carlo Simulation
%
%  By Daniel Dugas, Maxence Legrelle & Mario Nakhle

close all; %clear all;
load Modifiers.mat % imports MODY and MODX


%% Script Flags
DEBUG = 0;
WEATHER_POWER = 1;

%% Script ENUMs
% Bool
  YES = 1;
  NO  = 0;
% Weather
W_ = 4;
  WIND = 1;
  TEMP = 2;
  HUMD = 3;
  WAVE = 4;
% Faults
F_ = 2;
  NONE = 0;
  BRKN = 1;
  REPR = 2;
% Components
  ROTR = 1;
  GNTR = 2;
  GRBX = 3;
  SHFT = 4;
  BRKE = 5;
  YAWs = 6;
  PBRK = 7;
  PTCH = 8;
  CTRL = 9;
% Fleet
R_ = 3;
  SMLL = 1;
  LRGE = 2;
  HELI = 3;
  

%% Constants
% Script Parameters
iterations       = 1000;
  total_iterations = iterations; % useful when extra loops are added
% Time Parameters
dt       = 1; % [hour]
tm       = 24*365; % 1 year
  Time     = 0:dt:tm;
    instants  = size(Time,2); % time-Points
    TimeOfDay = mod(Time,24);
update_t = NaN;
% Model Parameters
%  Components          ROTR GNTR GRBX SHFT BRKE YAWs PBRK PTCH CTRL   
Critical_components = [YES  YES  YES  YES  NO   NO   YES  NO   YES  ];
  components = size(Critical_components,2);
Base_reliabilities  = [0.11 0.05 0.15 0.02 0.05 0.15 0.05 0.14 0.17 ]/ 24 / 365;
Repair_types        = [LRGE LRGE LRGE SMLL SMLL SMLL HELI SMLL SMLL ];
Repair_times        = [48   48   48   24   24   24   24   24   24   ];
WINDMODY = MODY(:,:,WIND);
TEMPMODY = MODY(:,:,TEMP);
HUMDMODY = MODY(:,:,HUMD);

%  Repair Fleet
Fleet               =   [SMLL   LRGE    HELI  ];
  fleet      = size(Fleet         ,2);
Travel_times(SMLL) = 10;
Travel_times(LRGE) = 10;
Travel_times(HELI) = 10;



%% Initialising Monte Carlo Loop Values and Tables
iterations_count = 0;
States       = zeros(iterations,instants  );
Unrel_states = zeros(iterations,instants  );
Faults       = zeros(1         ,components); % status of each component
Timers       = zeros(1         ,components); % timers to next fault/repair
Reliabilities= zeros(1         ,components);
Crews        = zeros(1         ,fleet     );
Weather      = zeros(W_        ,instants  );
Offenders    = zeros(1         ,components);


%% Interface
fig  = figure(1);
  set(fig,'Position',[48 161 560 420],'NumberTitle','off')
fig2 = figure(2);
  set(fig2,'Name','Yearly Weather')
  set(fig2,'Position',[635 161 560 420],'NumberTitle','off')
    w = waitbar(0, 'Work in Progress.');
      set(w,'Position', [347 26 270 56])
    withvisuals = 1;
    visuals_btn = uicontrol('Style', 'pushbutton',...
        'String', 'Toggle Dynamic Visuals (faster)',...
        'Position', [20 20 200 40],...
        'Callback', 'withvisuals = ~withvisuals;');
    cancelled = 0;
    cancel_btn = uicontrol('Style', 'pushbutton',...
        'String', 'Cancel',...
        'Position', [340 20 200 40],...
        'Callback', 'cancelled = 1;');
fig3 = figure(3);
  set(fig3,'Name','Component Failure')
  set(fig3,'Position',[1200 161 560 420],'NumberTitle','off')


%% Generate Yearly Weather
%
%  Weather is fitted to yearly statistics from Palma de Mallorca
%  TODO: implement daily variations

% Wind        [m/s]
%        mean   change                 peaktime
Mean   =    3 +    0.1 *cos(2*pi*(Time-  100*24 )/tm); 
StdDev =  1.8 +    1.5 *cos(2*pi*(Time-  180*24 )/tm);
Weather(WIND,:) = abs(random('norm',Mean,StdDev));
% Temperature [C]
Mean   =   18 +     12 *cos(2*pi*(Time-  210*24 )/tm);
StdDev =  2.5 +    0.1 *cos(2*pi*(Time-   30*24 )/tm);
Weather(TEMP,:) =     random('norm',Mean,StdDev);
% Humidity    [%]
Mean   =   75 +      5 *cos(2*pi*(Time-    0*24 )/tm);
StdDev = 12.5 +    2.5 *cos(2*pi*(Time-  180*24 )/tm);
Weather(HUMD,:) = abs(random('norm',Mean,StdDev))';
% Waves       [m]
Mean   =    4 +      2 *cos(2*pi*(Time-    0*24 )/tm);
StdDev =  0.5 +    0.5 *cos(2*pi*(Time-    0*24 )/tm);
Weather(WAVE,:) = abs(random('norm',Mean,StdDev));

figure(fig2);
plot(Weather')
legend('Wind','Temperature','Humidity','Waves');




%% Monte Carlo Loop TODO: parfor
for i = 1:iterations
    state = 0;
    t = 0;
    Faults = 0;
Faults       = zeros(1         ,components); % status of each component
Timers       = zeros(1         ,components); % timers to next fault/repair
Crews        = zeros(1         ,fleet     );

    %% Run one iteration - Simulate a year of operation
    while(t<tm)
        % Reliability depending on Weather
        % TODO:
        discrete_time = find(diff([0 Time>=t 1]));
        wind = Weather(WIND,discrete_time);
        Multipliers(WIND,:) = WINDMODY(find(diff([zeros(1,components); ...
                                              MODX(2:end,:,WIND)>=wind; ...
                                              ones(1,components)]  )))' ;
        temp = Weather(TEMP,discrete_time);
        Multipliers(TEMP,:) = TEMPMODY(find(diff([zeros(1,components); ...
                                              MODX(2:end,:,TEMP)>=temp; ...
                                              ones(1,components)]  )))' ;
        humd = Weather(HUMD,discrete_time);
        Multipliers(HUMD,:) = HUMDMODY(find(diff([zeros(1,components); ...
                                              MODX(2:end,:,HUMD)>=humd; ...
                                              ones(1,components)]  )))' ;

        Reliabilities =  ...
                         (                     ... 
                              Multipliers(WIND,:) ...
                           .* Multipliers(HUMD,:) ...
                           .* Multipliers(TEMP,:) ...
                         ).^WEATHER_POWER      ...
                        .* Base_reliabilities ;
        
        % Randomly find the next brake-down/repair timers
        % next Brake-downs
        To_select = Faults == NONE;
        Timers(To_select) = t+ exprnd(1./Reliabilities(To_select)) ;
        % next Boat arrivals
        To_select = isnan(Timers) & Faults==BRKN;
        Timers(To_select) = t+ 24 ; % time until good weather + travel
%       TODO: update boat info
        % next Finished repairs
        To_select = isnan(Timers) & Faults==REPR;
        Timers(To_select) = t+ Repair_times(To_select) ;% repairtime depending on component
        
        if ( DEBUG )
            t
            Faults
            Timers
        end

        % Cycle component statuses and reset timers
        next_t = min([Timers t+update_t]);
        To_cycle = Timers == min(Timers);
        Faults(To_cycle) = Faults(To_cycle) + 1;
        Offenders(To_cycle) = Offenders(To_cycle) + (Faults(To_cycle) == BRKN) ;
        Timers(To_cycle) = NaN;
        Faults(Faults > F_) = NONE;

        % Determine next state
        % TODO:
        n_faults      = sum(Faults > 0);
        n_crit_faults = (Faults > 0)*Critical_components';
        next_state    = ( n_faults > 0 ) + ( n_crit_faults > 0 );

        % Record states
        Time_interval = find( (t < Time) & (Time < next_t) );
        States(i,Time_interval) = state;

        % Set-up next loop
        t = next_t;
        state = next_state;

    end % while loop

    %% Post iteration Processing:

    States(i,t < Time) = state; % fixes the last states

    %% Unreliability and Failures Calculation
    % first occurence of a non-zero state
    first_failure_index = find(States(i,:)>0,1,'first');
    Unrel_states(i,first_failure_index:end) = 1;
    % Number of Failures
    F(i) = sum( diff( States(i,:)>0 )>0 );

    %% Plotting
    if withvisuals
        figure(fig)
        set(fig, 'Name',['Monte Carlo Simulation'...
            ' - Iteration #' num2str(i)])
        subplot(3,1,1)
        plot(Time,States(1:i,:)')
        title('States')
        subplot(3,1,2)
        plot(Time,sum(States(1:i,:)>0,1),'k--')
        title('Sum of failure states')
        subplot(3,1,3)
        plot(Time,sum(States(1:i,:)>0,1)/i,'k-')
        title('Unavailability, Unreliability')
        hold on
        plot(Time,sum(Unrel_states(1:i,:),1)/i,'b-')
        hold off
        axis([0 tm 0 1])
        pause(0.01);
    end

    %% Progress Bar
    iterations_count = iterations_count + 1;
    progress = iterations_count/total_iterations;
    waitbar(progress,w, ...
        ['Work in Progress - ', num2str(100*progress), '% done.']);
    if cancelled
        break
    end

end % Iterations loop



set( cancel_btn, 'String', 'Close', 'Callback', 'close all;' );
close(w)

if ~cancelled
    %% Add post-processing and display here
    figure(fig)
    set(fig, 'Name',['Monte Carlo Simulation'...
        ' - Iteration #' num2str(i)])
    subplot(3,1,1)
    plot(Time,States(1:i,:)')
    title('States')
    subplot(3,1,2)
    plot(Time,sum(States(1:i,:)>0,1),'k--')
    title('Sum of failure states')
    subplot(3,1,3)
    plot(Time,sum(States(1:i,:)>0,1)/i,'k-')
    title('Unavailability, Unreliability')
    hold on
    plot(Time,sum(Unrel_states(1:i,:),1)/i,'b-')
    hold off
    axis([0 tm 0 1])
    pause(0.01);
    
    figure(fig3)
    pie(Offenders/sum(Offenders));
    legend('Rotor', ...
           'Generator', ...
           'Gearbox', ...
           'Shaft', ...
           'Brake', ...
           'Yaw Systems', ...
           'Parking Brake', ...
           'Pitch', ...
           'Control' );            
end
