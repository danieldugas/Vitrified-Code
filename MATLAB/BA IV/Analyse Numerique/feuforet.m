%imh = image(cat(3,cells,z,z));
%set(imh, 'erasemode', 'none')
%axis equal
%axis tight
  
clear all;
%Matrixes and images can be converted to one-another, 
%so initial conditions may be computed by matrix or graphics commands. 
%The following code generates an array of zeros, initializes the cell state to zero, 
%then makes a cross of cells in state=1 in the center of the array. 
%One of the examples below (percolation cluster) uses graphics 
%commands to initialize the CA array.

n=100;
z = zeros(n,n);
p=0.6;
%ninter=100;
%fo=zeros(ninter,1);
%for in=1:ninter

vegm1=(rand(n,n)<p)*2;
veg=vegm1;
veg(ceil(rand(1)*n),ceil(rand(1)*n))=1;
figure(1);
while norm(veg-vegm1,1)>0
vegm1=veg;
som = (veg(:,[n 1:n-1])==1) + (veg(:,[2:n 1])==1) + ...
           (veg([n 1:n-1], :)==1) + (veg([2:n 1],:)==1) ;
veg=2*(veg==2) - ((veg==2) & (som> 0));
foret=z;
feu=z;
foret(veg==2)=1;
feu(veg==1)=1;
hold off;
image(cat(3,z,foret,z)+cat(3,feu,z,z));
pause
end; 
%fo(in)=sum(foret(:));
%end;






