%% TASK 1

    global executionTime
    global schedulingPolicy

    %% Run only once
    setenv('TTKERNEL', 'C:\Users\Daniel\Files\Development\MATLAB\KTH II\Control Systems\HW2_SRC\truetime-2.0\kernel');
    make_truetime

    %% 1
    executionTime = 0.006;
    schedulingPolicy = 'prioDM';
    sim('inv_pend_three')

    %% 2
    theta6RMS = Theta.signals.values;

    executionTime = 0.010;
    sim('inv_pend_three')

    %% 3
    theta10RMS = Theta.signals.values;

    executionTime = 0.006;
    schedulingPolicy = 'prioEDF';
    sim('inv_pend_three')
    
    %% 4
    theta6EDF = Theta.signals.values;
    
    executionTime = 0.010;
    sim('inv_pend_three')
    
    %% 5
    theta10EDF = Theta.signals.values;
    
    %% Display
    
    subplot(3,2,1);
    plot(theta6RMS(:,1));
    hold all;
    plot(theta6EDF(:,1));
    title('Pendulum 1 - 6ms');
    legend('RMS' , 'EDF');
    ylabel('theta');
    xlabel('t [ms]');
    
    subplot(3,2,3);
    plot(theta6RMS(:,2));
    hold all;
    plot(theta6EDF(:,2));
    title('Pendulum 2 - 6ms');
    legend('RMS' , 'EDF');
    ylabel('theta');
    xlabel('t [ms]');
    
    subplot(3,2,5);
    plot(theta6RMS(:,3));
    hold all;
    plot(theta6EDF(:,3));
    title('Pendulum 3 - 6ms');
    legend('RMS' , 'EDF');
    ylabel('theta');
    xlabel('t [ms]');
    
    subplot(3,2,2);
    plot(theta10RMS(:,1));
    hold all;
    plot(theta10EDF(:,1));
    title('Pendulum 1 - 10ms');
    legend('RMS' , 'EDF');
    ylabel('theta');
    xlabel('t [ms]');
    
    subplot(3,2,4);
    plot(theta10RMS(:,2));
    hold all;
    plot(theta10EDF(:,2));
    title('Pendulum 2 - 10ms');
    legend('RMS' , 'EDF');
    ylabel('theta');
    xlabel('t [ms]');
    
    subplot(3,2,6);
    plot(theta10RMS(:,3));
    hold all;
    plot(theta10EDF(:,3));
    title('Pendulum 3 - 10ms');
    legend('RMS' , 'EDF');
    ylabel('theta');
    xlabel('t [ms]');
    
    %% Annex
    %title( {[schedulingPolicy ' - ' num2str(executionTime) 'ms']} );
    %stairs(Schedule.signals.values(1:100,:));

%% TASK 2
    

    %% Delay
    
    Kh = linspace(0,4);
    plot(Kh, [max( 0.5-1./Kh , 0 ); min( 1./Kh , 1 )]);
    ylabel('tau/h');
    xlabel('Kh');
    legend('lower bound', 'higher bound');
    
    
    %% Packet loss
    A = [1 1;
        0 -1];
    B = [0;
         0.1];
    C = [1 0];
    D = 0;
    K = [20 9];
    sys = ss(A, B, C, D);
    tf(sys)
    pole(sys)
    
    h = 0.3;
    e = exp(h);
    em = exp(-h);
    I = eye(2);
    
    %phi = [e 0.5*(e-em);
    %       0 em];
    %gamma = [(e+em-2)/20;
    %         0.1*(1-em)];
    %sysh = ss(phi,gamma,C,D,h);
    
    sysh = c2d(sys,h);
    [phi, gamma, ~, ~] = ssdata(sysh);
  
    tf(sysh)
    pole(sysh)
    
    g1 = 2*log(max(eig(phi - gamma*K)));
    g2 = 2*log(max(eig(phi)));
    rmin = 1/(1 - g1/g2)
   
    