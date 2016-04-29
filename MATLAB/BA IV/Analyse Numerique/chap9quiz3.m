function[u,err] = chap9quiz3(h,N) 
% 
% entrees : h : pas de temps 
% N : nombre de pas de temps 
% 
% sorties : u : approximation de u au temps final (h*N) 
% err : erreur entre u et son approximation au temps final 
t=0; 
u=1; 
for n = 1:N 
u=u+h*f(u,t); 
t=t+h; 
end 
err=abs(u-uexact(t)); 
end 
function f=f(x,t) 
f=-x; 
end 
function uexact = uexact(t) 
uexact=exp(-t); 
end