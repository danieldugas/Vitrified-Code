function[err,u] = pbmlim(N)
%
% Resolution de l'equation -u''(x)+exp(u(x))=f(x)
% par une methode de differences finies
% et une methode de Newton
% Etant donne u^n, trouver u^{n+1} 
% tel que DF(u^n)(u^n - u^{n+1}) = F(u^n)
% En pratique on construit A=DF(u^n), b=F(u^n)
% on resout Ay=b (decomposition LL^T de A) et on pose u^{n+1}=u^n-y
%
% parametres
%
%  N        : nombre d inconnues du systeme non lineaire
%  a        : N-vecteur, diagonale de A, puis diagonale de L telle que A=LL^T
%  c        : (N-1)-vecteur, sous-diagonale de A, puis sous-diagonale de L
%  b        : N-vecteur, second membre de Ay=b, puis solution de Ay=b
%  u        : N-vecteur, contient u^n puis u^{n+1}
%
for i=1:N
    u(i) = 1;
end
h=1/(N+1);
coeff=(N+1)*(N+1);
stop=1;
iter=0;
while stop>1e-10
    iter=iter+1;
    for i=1:N
	    a(i) = 2*coeff+exp(u(i));
    end
    for i=1:N-1
        c(i) = -1*coeff;
    end
    b(1) = coeff*(2*u(1)-u(2))+exp(u(1))-f(1*h);
    for i=2:N-1
	    b(i) = coeff*(2*u(i)-u(i-1)-u(i+1))+exp(u(i))-f(i*h);
    end
    b(N) = coeff*(2*u(N)-u(N-1))+exp(u(N))-f(N*h);
    
    % Decomposition de Cholesky de la matrice A
    
    a(1) = sqrt(a(1));
    for i=1:N-1
        c(i) = c(i)/a(i);
        a(i+1) = sqrt(a(i+1)-c(i)*c(i));
    end
    
    % Resolution du systeme lineaire Ly = b
    
    b(1)=b(1)/a(1);
    for i=1:N-1
      b(i+1) = (b(i+1)-c(i)*b(i))/a(i+1);
    end
    
    % Resolution du systeme lineaire L^T u^{n+1} = y
    
    b(N)=b(N)/a(N);
    for i=N-1:-1:1
      b(i) = (b(i)-c(i)*b(i+1))/a(i);
      
    end
    
    % u^{n+1} = u^n - y
    
    for i=1:N
	    u(i) = u(i) - b(i);
        
    end
       
    % Calcul de ||b||/||x||

    stop=norm(b)/norm(u);
    fprintf('iter=%i, stop = %e \n',iter,stop)
end
err = 0;
for i=1:N
   err=max(err,abs(u(i)-uex(i*h)));
end
end
% second membre de l equation -u''(x)+exp(u(x))=f(x)
function f=f(x)
   %f=2+exp(x*(1-x));
   f=4*pi*pi*sin(2*pi*x)+exp(sin(2*pi*x));
end
% solution de l equation -u''(x)+exp(u(x))=f(x)
function uex=uex(x)
   %uex=x*(1-x);
   uex=sin(2*pi*x);
end
