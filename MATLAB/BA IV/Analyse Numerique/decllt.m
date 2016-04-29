function L = fleche(N)
l(1)=sqrt(3); 
m(1)=-1./l(1); 
d(1)=-1./l(1); 
for i=2:N-2 
l(i)=sqrt(3-l(i)*l(i)); 
m(i)=-1/l(i); 
d(i)=-l(i)*d(i-1)/l(i); 
end 
l(N-1)=sqrt(3-???); 
m(N-1)=(-1-d(N-2)*m(N-2))/???; 
l(N)=3; 
for i=1:N-2 
l(N)=l(N)-???; 
end 
l(N)=sqrt(l(N)-m(N-1)*m(N-1));

%remplissage de L
L = zeros(N);
for i=1:N-2
    L(i,i) = l(i);
    L(i+1,i) = m(i);
    L(N,i) = d(i);
end
L(N-1,N-1) = l(N-1);
L(N,N) = l(N);
L(N,N-1) = m(N-1);
    