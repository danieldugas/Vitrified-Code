function err=bdf2(x0,h)

% formule de differences finies BDF2
  diff = (3*f(x0)-4*f(x0-h)+f(x0-2*h))/(2*h);
err = abs(diff-df(x0));
fprintf(' x0 %e h %e erreur %e \n',x0,h,err)

% la fonction f
function funct=f(x)
  %funct = sin(x);
  funct = x^2;

% la derivee de f
function dfunct=df(x)
  %dfunct = cos(x);
  dfunct = 2*x;