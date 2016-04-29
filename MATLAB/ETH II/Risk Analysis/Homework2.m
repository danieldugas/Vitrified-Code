u = 1/15;
ub = 1/25;
lh  = 10^-4;
lf = 3*10^-4;
lc = 3*10^-5;
A = [-ub   0          ub;
     lf+lc -(lf+lc+u) u;
     lc    2*lh       -(2*lh+lc) ];
 
B = A;
B(:,2) = 1;

Pi = [0 1 0]*B^-1;

yr = 24*365.25;

T0 = 5*yr*Pi(1);
T1 = 5*yr*Pi(2);

reps2 = 2*T0/25 + T1/15;
reps  =   T0/25 + T1/15;
fails = T0/25;