%% PART I - Direct
clear all;close all

Nx=60;Ny=60;                             % Number of points
Lx=1;Ly=1;                               % Length of the domain  
hx=Lx/Nx;hy=Ly/Ny;                       % Grid spacing
beta = hx/hy; b2 = beta^2; hx2=hx^2;     % Grid spacing parameter
x=linspace(0,Lx,Nx+1);                   % x-coordinates length N+1 at grid nodes
y=linspace(0,Ly,Ny+1);                   % y-coordinates length N+1 at grid nodes

% Assemble x and y - coordinate at cell centers into 1-D vector length N*N
X=(x(1:end-1)+x(2:end))/2;
Y=(y(1:end-1)+y(2:end))/2;

% Define the forcing function (NOTE: correct ordering!)
 xx = kron(ones(Ny,1),X');
 yy = kron(Y',ones(Nx,1));
 f = cos(pi*xx).*cos(pi*yy);

%-----------------------------------------
 % Seemingly Lp is A_ _
 % LP is the inverse of Lp
     
 Lp = kron(speye(Ny),DD(Nx,hx)) + kron(DD(Ny,hy),speye(Nx));

% Set one Dirichlet value to fix pressure in that point
 % first line: p00 + 0 + 0 + ... = 0
 Lp(1,:) = 0; Lp(1,1) = 1; f(1)=0; 

 LP = Lp^(-1);
 P = LP*f; 

figure(2)
surf(X,Y,reshape(P,Nx,Ny)')
zlabel('P_{dir}');xlabel('x');ylabel('y');title('Direct Solution')

PP = reshape(P,Nx,Ny);
ff    = reshape(f,Nx,Ny);
LL  = 4*del2(PP,hx,hy); % discrete Laplacian of the pressure field
error = mean(mean(LL - ff))
%  -----------------------------------------
%% PART II - GS
omega=1.0;                               % SOR parameter
 crnr = (omega/(1+b2));                   % corner parameter
 ledg = (omega/(2*(1+b2)-1));             % left and right edge parameter
 bedg = (omega/(2*(1+b2)-b2));            % bottom and top edge parameter
 innr = (omega/(2*(1+b2)));               % inner parameter

% Set up u for iterative method
p=zeros(Nx*Ny,1)    ;                         % Inital condition

% Choose a criterion for convergence and the maximum iteration steps
err=1e-12;
Mmax=100000;
m=0;
convergence=1;

while convergence>err &&  m<Mmax
    m=m+1;
    
    % B.C. P_[1,1]
     p(1)= 0;
    
    for i=2:(Nx-1)
        % B.C. P_[i,1]
        p(i)= (1-omega)*p(i)+ ...
           bedg*... % IS THAT SO?
           ( p(i+1) + p(i-1) + b2*p(i+Nx) - hx2*f(i) );
    end
    
    % B.C. P_[Nx,1]
    p(Nx)= (1-omega)*p(Nx)+ ...
           crnr*( p(Nx-1) + b2*p(2*Nx) - hx2*f(Nx) );
    
    for j=2:(Ny-1)
        % B.C. P_[1,j]
        k=1+Nx*(j-1);
        p(k)= (1-omega)*p(k)+ ...
           ledg*( p(k+1) + b2*( p(k+Nx) + p(k-Nx) ) - hx2*f(k) );

        % General Solution (March through all inner points)
        for i=2:(Nx-1)
            k=i+Nx*(j-1);
            p(k)=(1-omega)*p(k)+ ...
           innr*( p(k+1) + p(k-1) + b2*( p(k+Nx) + p(k-Nx) ) - hx2*f(k) );
        end
        % B.C. P_[Nx,j]
        k=Nx+Nx*(j-1);
        p(k)= (1-omega)*p(k)+ ...
           ledg*( p(k-1) + b2*( p(k+Nx) + p(k-Nx) ) - hx2*f(k) );
    end
    
    % B.C. P_[1,Ny]
    k=1+Nx*(Ny-1);
    p(k)= (1-omega)*p(k)+ ...
           crnr*( p(k+1) + b2*p(k-Nx) - hx2*f(k) );
    
    
    for i=2:(Nx-1)
        % B.C. P_[i,Ny]
        k=i+Nx*(Ny-1);
        p(k)= (1-omega)*p(k)+ ...
               bedg*... % IS THAT SO?
              ( p(k+1) + p(k-1) + b2*p(k-Nx) - hx2*f(k) );
    end
    
    % B.C. P_[Nx,Ny]
    k=Nx*Ny;
    p(k)= (1-omega)*p(k)+ ...
           crnr*( p(k-1) + b2*p(k-Nx) - hx2*f(k) );
    
    
    % Calculate residuals
    R(m,1)= norm( Lp*p - f );
    convergence=R(m,1);
    
    % Calculate difference to exact solution
    epsilon(m,1)= norm( p - P );
    
end

figure(3)
surf(X,Y,reshape(p,Nx,Ny)');
xlabel('x');ylabel('y');zlabel('P_{iter}');zlim([0 0.1]);
title(['Iterative solution after steps = ',num2str(m),' - omega = ', num2str(omega)])


%% PART III - SOR
 omega = 1.5;
 %omega = 1.665;
 crnr = (omega/(1+b2));                   % corner parameter
 ledg = (omega/(2*(1+b2)-1));             % left and right edge parameter
 bedg = (omega/(2*(1+b2)-b2));            % bottom and top edge parameter
 innr = (omega/(2*(1+b2)));               % inner parameter
 
% Set up u for iterative method
p=zeros(Nx*Ny,1)    ;                         % Inital condition

% Choose a criterion for convergence and the maximum iteration steps
err=1e-12;
Mmax=100000;
m=0;
convergence=1;

while convergence>err &&  m<Mmax
    m=m+1;
    
    % B.C. P_[1,1]
     p(1)= 0;
    
    for i=2:(Nx-1)
        % B.C. P_[i,1]
        p(i)= (1-omega)*p(i)+ ...
           bedg*...
           ( p(i+1) + p(i-1) + b2*p(i+Nx) - hx2*f(i) );
    end
    
    % B.C. P_[Nx,1]
    p(Nx)= (1-omega)*p(Nx)+ ...
           crnr*( p(Nx-1) + b2*p(2*Nx) - hx2*f(Nx) );
    
    for j=2:(Ny-1)
        % B.C. P_[1,j]
        k=1+Nx*(j-1);
        p(k)= (1-omega)*p(k)+ ...
           ledg*( p(k+1) + b2*( p(k+Nx) + p(k-Nx) ) - hx2*f(k) );

        % General Solution (March through all inner points)
        for i=2:(Nx-1)
            k=i+Nx*(j-1);
            p(k)=(1-omega)*p(k)+ ...
           innr*( p(k+1) + p(k-1) + b2*( p(k+Nx) + p(k-Nx) ) - hx2*f(k) );
        end
        % B.C. P_[Nx,j]
        k=Nx+Nx*(j-1);
        p(k)= (1-omega)*p(k)+ ...
           ledg*( p(k-1) + b2*( p(k+Nx) + p(k-Nx) ) - hx2*f(k) );
    end
    
    % B.C. P_[1,Ny]
    k=1+Nx*(Ny-1);
    p(k)= (1-omega)*p(k)+ ...
           crnr*( p(k+1) + b2*p(k-Nx) - hx2*f(k) );
    
    
    for i=2:(Nx-1)
        % B.C. P_[i,Ny]
        k=i+Nx*(Ny-1);
        p(k)= (1-omega)*p(k)+ ...
               bedg*... 
              ( p(k+1) + p(k-1) + b2*p(k-Nx) - hx2*f(k) );
    end
    
    % B.C. P_[Nx,Ny]
    k=Nx*Ny;
    p(k)= (1-omega)*p(k)+ ...
           crnr*( p(k-1) + b2*p(k-Nx) - hx2*f(k) );
    
    
    % Calculate residuals
    R(m,2)= norm( Lp*p - f );
    convergence=R(m,2);
    
    % Calculate difference to exact solution
    epsilon(m,2)= norm( p - P );
    
end

figure(3)
surf(X,Y,reshape(p,Nx,Ny)');
xlabel('x');ylabel('y');zlabel('P_{iter}');zlim([0 0.1]);
title(['Iterative solution after steps = ',num2str(m),' - omega = ', num2str(omega)])

%% PLOTS

figure(4);
semilogy(R);hold on
xlabel('Iterations')
ylabel('log_{10}(R)')
legend('Gauss Siedel','Successive Over Relaxation');

figure(5);
semilogy(epsilon);hold on
xlabel('Iterations')
ylabel('log_{10}(\epsilon)')
legend('Gauss Siedel','Successive Over Relaxation');

% GSslope =(((log10(R(9000,1))-(log10(R(12000,1)))))/(12000-9000))
% SORslope=(((log10(R(9000,2))-(log10(R(12000,2)))))/(12000-9000))
