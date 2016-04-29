function [unew, err]=transport(N,M,tau)
    %
    %  Schema d'Euler progressif pour le probleme de transport unidimensionnel
    %
    %  indications :
    %
    %  uold     : N-vecteur, uold(i) est une approximation de u(x_i,t_n)
    %  unew     : N-vecteur, unew(i) est une approximation de u(x_i,t_n+1)
    %
    %  definitions preliminaires
    %
    h=1/(N+1);       % pas d'espace
    c=0.5;           % vitesse de transport (positive)
    t=0.;            % temps initial
    %
    % condition initiale (fct w() definie ci-dessous)
    %
    for i=1:(N+1)
      uold(i)=???;
    end
    %
    % schema progressif decentre
    %
    for n=1:M
      t=t+tau;
      unew(1)=???;
      for i=2:(N+1)
        unew(i)=???;
      end
    %
      for i=1:(N+1)
        uold(i)=???;
      end
    end
    %
    % imprimer l'erreur maximum au temps final
    %
    err = 0;
    for i=1:(N+1)
		u_ex(i)=uex(i*h,t,c);
        erri = abs(unew(i)-uex(i*h,t,c));
        if (erri>err) 
            err = erri;
        end
    end
    fprintf(' erreur maximale au temps final  %e \n',err)
end
%
% definition de la condition initiale w(x)
%
function w = w(x)
	a=0.1;b=0.3;c=0.5;
	if (x>a & x<=b)
 		 w = ???;
	elseif (x>b & x<c)
 		 w = ???;
	else w = ???;
	end
end
%
% solution exacte (utilise pour le calcul de l'erreur)
%
function uex = uex(x,t,c)
uex =  ???;
end
