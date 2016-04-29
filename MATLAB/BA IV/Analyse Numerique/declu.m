function [err,b]=declu(N)
%
% Algorithme de decomposition A=LU
% pour resoudre le systeme lineaire Ax=b, 
% ou A est la matrice tridiagonale symetrique 
% de diagonale a, sur-diagonale c et sous-diagonale d.
% 
% parametres
%
%  N        : nombre d inconnues du systeme lineaire
%  a        : N-vecteur, diagonale de A puis diagonale de L
%  c        : (N-1)-vecteur, sur-diagonale de A en entree,
%             puis sur-diagonale de U
%  d        : (N-1)-vecteur, sous-diagonale de A,
%             puis sous-diagonale de L
%  b        : N-vecteur, second membre du systeme lineaire en entree,
%             solution du systeme lineaire Ax=b en sortie
%
% remplissage de la matrice A et du second membre b
%
for i=1:N
  a(i)=3.;
  b(i)=0.;
end
b(1)=2. ; b(N) = 1.;
for i=1:(N-1)
  c(i)=-1.;
  d(i)=-2.;
end
%
% algorithme de decomposition A=LU
%
c(1) = c(1)/a(1);
for i=2:(N-1)
	a(i)=a(i)-c(i-1)*d(i-1);
	c(i)=c(i)/a(i);
end
a(N)=a(N)-c(N-1)*d(N-1); 
%
%
%
% resolution du systeme lineaire Ly = b 
%
b(1)=b(1)/a(1);
for i=1:N-1
	b(i+1)=(b(i+1)-d(i)*b(i))/a(i+1) ;
end
%
% resolution du systeme lineaire U x = y 
%
for i=N-1:-1:1
        b(i)=b(i)-c(i)*b(i+1);
end
%
% calcul de l'erreur
%
err=0.;
for i=1:N
  err=err+(b(i)-1)^2;
end
err=sqrt(err);
end