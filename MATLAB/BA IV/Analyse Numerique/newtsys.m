function[x] = newtsys(N)
%
% Methode de Newton : Etant donne x^n, trouver x^{n+1} 
% tel que DF(x^n)(x^n - x^{n+1}) = F(x^n)
% En pratique on construit A=DF(x^n), b=F(x^n)
% on resout Ay=b (decomposition LL^T de A) et on pose x^{n+1}=x^n-y
%
% parametres
%
%  N        : nombre d inconnues du systeme non lineaire
%  a        : N-vecteur, diagonale de A, puis diagonale de L telle que A=LL^T
%  c        : (N-1)-vecteur, sous-diagonale de A, puis sous-diagonale de L
%  b        : N-vecteur, second membre de Ay=b, puis solution de Ay=b
%  x        : N-vecteur, contient x^n puis x^{n+1}
%
for i=1:N
    x(i) = 1;
end
stop=1;
iter=0;
while stop>1e-10
    iter=iter+1;
    for i=1:N
	    a(i) = 2+exp(x(i));
    end
    for i=1:N-1
        c(i) = -1;
    end
    b(1) = 2*x(1)-x(2)+exp(x(1));
    for i=2:N-1
	    b(i) = 2*x(i)-x(i-1)-x(i+1)+exp(x(i));
    end
    b(N) = 2*x(N)-x(N-1)+exp(x(N));
    
    % Decomposition de Cholesky de la matrice A
    
    a(1) = sqrt(a(1));
    for i=1:N-1
        c(i) = c(i)/a(i);
        a(i+1) = sqrt(a(i+1)-c(i)*c(i));
    end
    
    % Resolution du systeme lineaire Ly = b, puis L^T x = y
    
    b(1)=b(1)/a(1);
    for i=1:N-1
	    b(i+1) = (b(i+1)-c(i)*b(i))/a(i+1);
    end
    
    % resolution du systeme lineaire L^T x = y
    
    b(N)=b(N)/a(N);
    for i=N-1:-1:1
      b(i) = (b(i)-c(i)*b(i+1))/a(i);
    end
    
    %x^n+1 = x^n - y
    
    for i=1:N
	    x(i) = x(i) - b(i);
    end
        
    % calcul de ||b||/||x||

    stop=norm(b)/norm(x);
    fprintf('iter=%i, stop = %e \n',iter,stop)
end
end