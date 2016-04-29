%%% HOMEWORK 5      - Projection Method and 
%%% Staggered Grid for Incompressible Flows
%%%
%%%                           Matlab R2013b
clear all; close all;

%%% TIME/SPACE GRID SETUP %%%
%vv Modifiable Parameters vv%
K = 100; %set # of time grid points
dt = 0.005; %set time resolution
N = 5001; %set # of spatial grid points
%vv Calculated Parameters vv%
dx = 1/(N-1); %spatial resolution
dtdx = dt/dx;
dt2dx = dt/(2*dx);
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

%%% PLOT SETUP %%%
plot_snapshots = 7; %amount of plotted instants
snapshot_period = ceil(K/plot_snapshots); 
x = linspace(0,1,N); %x positions of u, co-located p points
xp = [-dx/2 x+dx/2]; %x positions of staggered p points
%%%%%%%%%%%%%%%%%%
 
%%% INITIALISATIONS %%%
pinlet = 1; %boundary condition
u0 = ones(1,N); %initial condition
u0(2:end-1) = 1+ (2*rand(1,N-2)-1)/100; %scramble the internal points
u = zeros(K,N);
u(1,:) = u0;
u_s = zeros(K,N);
u_s(1,:) = u0;
snapshot_counter = 0;
%%%%%%%%%%%%%%%%%%%%%%%

for k = 1:K
    
%%% Co-located %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
    %%% solving equation (5) for pressure vector
    unow = u(k,:);
    ft = dt2dx*( [unow(2:end) 0] - [0 unow(1:end-1)] );
    ft(1) = pinlet; % p boundary condition
    ft(end) = 0;  % boundary condition
    
    A = diag([1 -2*ones(1,N-2) -1])+diag([0 ones(1,N-2)],1)+diag(ones(1,N-1),-1);
    
    pnext = ( A^(-1) * ft' )';
 
    %%% equation (3)
    unext = unow - dt2dx*( [pnext(2:end) 0] - [0 pnext(1:end-1)] );
    unext(1) = unext(2);
    unext(end) = unext(end-1);

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%%% Staggered %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

%%% u(1) = u_0.5    u(2) = u_1.5 and so on, therefore:
%%% u_a = u(a+0.5)

    %%% solving equation (5) for pressure vector
    unow_s = u_s(k,:);
    ft_s = dtdx*( [0 unow_s(2:end) 0] - [0 unow_s(1:end-1) 0] );
    ft_s(1) = pinlet; % p boundary condition
    ft_s(end) = 0;  % boundary condition
    
    A_s = diag([0.5 -2*ones(1,N-1) -1])+diag([0.5 ones(1,N-1)],1)+diag(ones(1,N),-1);
    
    pnext_s = ( A_s^(-1) * ft_s' )';
    
    %%% equation (3)
    unext_s = unow_s - dtdx*( pnext_s(2:end) - pnext_s(1:end-1) );
    unext_s(1) = unext_s(2);
    unext_s(end) = unext_s(end-1);
    %Alternate Method (higher machine precision results):
    %B =  diag([1 0*ones(1,N-2) -1])+diag([-1 ones(1,N-2)],1)+diag([-1*ones(1,N-2) 1],-1);
    %unext = unow - ( dt2dx*B*pnext' )';
    
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%    
%%% Adding the results to the storage matrices 
    u(k+1,:) = unext;
    u_s(k+1,:) = unext_s;
    
%%% plotting results
    if mod(k-1,snapshot_period) == 0
        snapshot_counter = snapshot_counter + 1;
        plot_timestamps(snapshot_counter) = (k-1)*dt;
        
        subplot(2,2,1);
        plot(x,unext);
        title('Co-located Grid');
        a = ylim;
        ylabel('u(x)');
        xlabel('x');
        hold all;
        
        subplot(2,2,3);
        plot(x,pnext);
        b1 = ylim;
        ylabel('p(x)');
        xlabel('x');
        hold all;
        
        subplot(2,2,2);
        plot(x,unext_s);
        ylim(a);
        title('Staggered Grid');
        ylabel('u(x)');
        xlabel('x');
        hold all;
        
        subplot(2,2,4);
        plot(xp,pnext_s);
        b2 = ylim;
        b = [min(b1(1),b2(1)) max(b1(2),b2(2))];
        ylim(b);
        ylabel('p(x)');
        xlabel('x');
        xlim([0 1]);
        hold all;
        
        subplot(2,2,3);
        ylim(b);
    end

end
legend(num2str(plot_timestamps'));

