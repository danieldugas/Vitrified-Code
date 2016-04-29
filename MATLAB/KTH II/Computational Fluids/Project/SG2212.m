% Navier-Stokes solver,
% adapted for course SG2212
% KTH Mechanics
% Modified by Daniel Dugas
%
% Depends on avg.m and DD.m
%
% Code version:
% 20140320

clear all; close all;

%% Enable/Disable Script Features                                   {{{2
off = false;
on  = true;

Display_Divergence  = off;
Display_Velocities  = on;
Display_Temperature = off;
Display_History     = off;

Store_Movies        = off;
Store_Workspace     = off;

% Post processing features can be run independently, after execution
% by clicking on the feature code and pressing <CTRL-Enter>
% or <right-click>Evaluate Current Section
%------------------------------------------
%% Set Parameters                                                   {{{2
% Parameters for test case I: Lid-driven cavity
% The Richardson number is zero, i.e. passive scalar.

Pr = 0.71;     % Prandtl number
Re = 10;     % Reynolds number
Pe = Re*Pr;    % Peclet number
Ri = 0.;       % Richardson number

dt = 0.001;    % time step
Tf = 20;       % final time
Lx = 1;        % width of box
Ly = 1;        % height of box
Nx = 30;       % number of cells in x
Ny = 30;       % number of cells in y

Nit = ceil(Tf/dt); % Number of iterations
frames = 24; % number of instants for which to save and plot data
playback_fps = 0; % set to 0 for real time
ig = Nit/(frames-1); % number of iterations between output

% Boundary and initial conditions:
Utop = 1.;
Tbottom = 1.; Ttop = 0.;
namp = 0.;

%------------------------------------------
%% Variable Initialisations                                         {{{2
% Spatial grid: Location of corners
x = linspace( 0,Lx,Nx+1 );
y = linspace( 0,Ly,Ny+1 );
X = avg(x,2);
Y = avg(y,2);
% Grid spacing
hx = x(2)-x(1); hx2 = hx^2;
hy = y(2)-y(1); hy2 = hy^2;
% Boundary conditions:
uN = x*0+Utop;    vN = X*0;   tN = X*0+Ttop;
uS = x*0;         vS = X*0;   tS = X*0+Tbottom;
uW = Y*0;         vW = y*0;
uE = Y*0;         vE = y*0;
% Initial conditions
U = zeros(Nx-1,Ny); V = zeros(Nx,Ny-1);
% linear profile for T with random noise
T = zeros(Nx,Ny) + namp*rand(Nx,Ny);
% Time series
tser = [];
Tser = [];
s = 0;

%-----------------------------------------

% Compute system matrices for pressure
% First set homogeneous Neumann condition all around
% Laplace operator on cell centres: Fxx + Fyy
Lp = kron(speye(Ny),DD(Nx,hx)) + kron(DD(Ny,hy),speye(Nx));
% Set one Dirichlet value to fix pressure in that point
Lp(1,:) = 0   ;
Lp(1,1) = 1   ;
% For more speed, you could pre-compute the LU decomposition
[LLp,ULp] = lu(Lp);

%-----------------------------------------
%% Main loop over iterations                                         {{{1

% Progress bar
abort = false;
W = waitbar(0,'Computing','CreateCancelBtn','abort = true; delete(W)');

for k = 1:Nit
    if abort == true
        break
    end
    %% Update Extended Matrices and Center/Corner Averages           {{{2
    % include all boundary points for u and v (linear extrapolation
    % for ghost cells) into extended array (Ue,Ve)
    % _short extended matrices contain only the west and east boundaries
    % _slim extended matrices contain only the north and south boundaries
    Ue_short = [ uW ;
               U  ;
               uE ];
    Ue = [2*uS'-Ue_short(:,1) Ue_short 2*uN'-Ue_short(:,end)];
    Ve_slim = [vS' V vN'];
    Ve = [ 2*vW-Ve_slim(1,:)   ;
           Ve_slim             ;
           2*vE-Ve_slim(end,:) ];
    Te_short = [ T(1,:)   ;   %West Neumann B.C T0 = T1
                 T        ;   
                 T(end,:)  ]; %East Neumann B.C TN+1 = TN
    Te_slim  = [ 2*tS'-T(:,1) T 2*tN'-T(:,end) ];   %South and North Dirichlet B.C
    
    % averaged (Ua,Va) of u and v on corners
    Ua = avg(Ue,2);
    Va = avg(Ve,1);
    
    % averaged (Up,Vp) of u and v on centers
    Up = avg(Ue,1);
    Vp = avg(Ve,2);
    
    
    %% Velocity and pressure calculation                             {{{2
    
    % construct individual parts of nonlinear terms
    UVx = 1/hx *diff(Ua.*Va,1,1) ; % Nx   by Ny+1
    UVy = 1/hy *diff(Ua.*Va,1,2) ; % Nx+1 by Ny
    U2x = 1/hx *diff(Up.^2,1,1)  ; % Nx-1 by Ny+2
    V2y = 1/hy *diff(Vp.^2,1,2)  ; % Nx+2 by Ny-1
    Uxx = 1/hx2 *diff(Ue,2,1)   ; % Nx-1 by Ny+2
    Uyy = 1/hy2 *diff(Ue,2,2)   ; % Nx+1 by Ny
    Vxx = 1/hx2 *diff(Ve,2,1)   ; % Nx   by Ny+1
    Vyy = 1/hy2 *diff(Ve,2,2)   ; % Nx+2 by Ny-1
    
    % treat advection explicitly
    advcu = -U2x(:,2:end-1) -UVy(2:end-1,:) ; % Nx-1 by Ny
    advcv = -UVx(:,2:end-1) -V2y(2:end-1,:) ; % Nx   by Ny-1
    
    % treat viscosity explicitly
    viscu = 1/Re *( Uxx(:,2:end-1) + Uyy(2:end-1,:) ); % Nx-1 by Ny
    viscv = 1/Re *( Vxx(:,2:end-1) + Vyy(2:end-1,:) );  % Nx   by Ny-1
    
    % buoyancy term
    % IF TEMPERATURE: fy = ...
    
    % compose final nonlinear term + explicit viscous terms
    U = U + dt*viscu + dt*advcu;
    V = V + dt*viscv + dt*advcv; % IF TEMPERATURE: + dt*fy;
    
    % pressure correction, Dirichlet P=0 at (1,1)
    rhs = (diff([uW;U;uE],1,1)/hx + diff([vS' V vN'],1,2)/hy)/dt;
    rhs = reshape(rhs,Nx*Ny,1);
    rhs(1) = 0 ;
    % Direct Calculation
    % P = Lp\rhs;
    % Alternatively, you can use the pre-computed LU decompositon
    P = ULp\(LLp\rhs);
    % Or as another alternative, reformulate HW6 as function
    % and call it here:
    % [PP,msoriter] = GS_SOR_solver(PP,rhs,Lp,Nx,Ny,hx,hy,1.95,1e-4);
    P = reshape(P,Nx,Ny);
    
    % apply pressure correction
    U = U - dt/hx *diff(P,1,1);
    V = V - dt/hy *diff(P,1,2);
    
    
    %% Temperature equation                                          {{{2
    
    UTx = diff( avg(Te_short,1).*Ue_short ,1,1)/hx;
    VTy = diff( avg(Te_slim,2).*Ve_slim   ,1,2)/hy;
    H = -UTx -VTy + 1/Pe *( diff(Te_short,2,1)/hx2 + diff(Te_slim,2,2)/hy2 );
    T = T + dt*H;
    
    
    %% Store solution if needed                                      {{{2
    
    if k==1 || ( ig*ceil((k-1)/ig)-(k-1) < 1 ) || k==Nit
        
        % storage index
        s = s+1;
        
        % divergence on cell centres
        div(:,:,s) = diff(Ue_short)/hx + diff(Ve_slim,1,2)/hy;
        
        % velocity on cell corners
        Len(:,:,s) = sqrt(Ua.^2+Va.^2);
        Us(:,:,s) = Ua./(Len(:,:,s)+eps);
        Vs(:,:,s) = Va./(Len(:,:,s)+eps);
     
        % compute temperature on cell corners
        Ta(:,:,s) = [Tbottom+0*x' avg(avg(Te_short,1),2) Ttop+0*x'];
        
        % Time history
        tser = [tser ((k==1)*(k-1)+(k>1)*k)*dt];
        Tser = [Tser Ue(ceil((Nx+1)/2),ceil((Ny+1)/2))];
   
    end
    % }}}2
    %% progress bar
    %if floor(51*k/Nit)>floor(51*(k-1)/Nit), fprintf('.'), end
    W = waitbar(k/Nit);
    
end
delete(findall(0,'Tag','TMWWaitbar'));


%% POST PROCESS                                                     {{{1
if abort == false
if Display_Divergence
%% Display DIV                                                      {{{2
figure(1); set(figure(1), 'Name', 'Divergence Plot');clf; 
extrema = max(abs(min(min(min(div)))),abs(max(max(max(div)))));
button2 = uicontrol('Position',[0 0 200 40],'String','Cancel',...
            'Callback','abort = true;');
for i = 1:s
    if abort == true
        abort = false;
        break
    end
    contourf(avg(x,2),avg(y,2),div(:,:,i)');colorbar
    caxis([-extrema extrema])
    axis equal; axis([0 Lx 0 Ly]);
    title(sprintf('divergence at t=%g',(i-1)*ig*dt))
    drawnow
end

end

if Display_Velocities
%% Display U,V                                                      {{{2                     
figure(2); set(figure(2), 'Name', 'Velocity Plot','Position',[10 80 1000 600]);clf; 
%contourf(avg(x,2),avg(y,2),P');colorbar
button2 = uicontrol('Position',[0 0 200 40],'String','Cancel',...
            'Callback','abort = true;');
for i = 1:s
    if abort == true
        abort = false;
        break
    end
    % i = input(''); % Uncomment to Navigate manually
    contourf(x,y,Len(:,:,i)',20,'k-');colorbar
    hold on;
    quiver(x,y,Us(:,:,i)',Vs(:,:,i)',.4,'k-')
    axis equal; axis([0 Lx 0 Ly]);
    title(sprintf('u at t=%g',(i-1)*ig*dt))
    xlabel('x');ylabel('y');
    drawnow
    Velocity(i) = getframe;
    hold off;
end
fps = (playback_fps == 0)/(ig*dt) + playback_fps;
button2 = uicontrol('Position',[0 0 200 40],'String','Play',...
            'Callback',' movie(Velocity, 1, fps);');
end   
  
if Display_Temperature
%% Display T                                                        {{{2
figure(3); clf; hold on;
button3 = uicontrol('Position',[0 0 200 40],'String','Cancel',...
            'Callback','abort = true;');
for i = 1:s
    if abort == true
        abort = false;
        break
    end
    % i = input(''); % Uncomment to Navigate manually
    contourf(x,y,Ta(:,:,i)',20,'k-');colorbar
    hold on;
    quiver(x,y,Us(:,:,i)',Vs(:,:,i)',.4,'k-')
    axis equal; axis([0 Lx 0 Ly]);
    title(sprintf('T at t=%g',(i-1)*ig*dt))
    xlabel('x');ylabel('y');
    drawnow
    Temperature(i) = getframe;
    hold off;
end
fps = (playback_fps == 0)/(ig*dt) + playback_fps;
button3 = uicontrol('Position',[0 0 200 40],'String','Play',...
    'Callback',' movie(Temperature, 1, playback_acceleration/(ig*dt));');
end

if Display_History
%% Display Time history                                             {{{2
figure(4); set(figure(4), 'Name', 'Probe Plot'); hold on;
title(sprintf('Probe signal at x=%g, y=%g',...
      x(ceil((Nx+1)/2)),y(ceil((Ny+1)/2))))
set(gca,'yscale','log')
xlabel('time t');ylabel('u(t)')
plot(tser,abs(Tser))

end

if Store_Movies
%% Store movies to .avi files                                       {{{2
if Display_Velocities
buffer = VideoWriter('Velocities');
buffer.Quality = 100;
buffer.FrameRate = fps;
open(buffer)
writeVideo(buffer,Velocity)
close(buffer)
fprintf('Movie saved as Velocities.avi')
end
if Display_Temperature
buffer = VideoWriter('Temperature');
buffer.Quality = 100;
buffer.FrameRate = fps;
open(buffer)
writeVideo(buffer,Temperature)
close(buffer)
fprintf('Movie saved as Temperature.avi')
end
end

if Store_Workspace
%% Store Workspace to .mat files                                    {{{2
Case = 0;
if Re==10 && dt == 0.001 && Nx == 30 && Ny == 30
    Case = 'A';
end
if Re==100 && dt == 0.001 && Nx == 50 && Ny == 50
    Case = 'B';
end
if Re==8000 && dt == 0.001 && Nx == 30 && Ny == 100
    Case = 'C';
end
if Case == 0;
    Case = 'D';
end
save([Case '.mat'])
fprintf(['Worspace saved as ' Case '.mat'])

end

end
%% Wrap Up                                                           {{{1
fprintf('\n')
abort = false;
% }}}1

% vim:foldmethod=marker