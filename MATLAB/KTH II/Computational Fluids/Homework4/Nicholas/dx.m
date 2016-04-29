function dudx = dx(dir,u)
global X DX
[m,n] = size(u);
% manages the system case where u contains several
% variables = columns
ODX  = 1/DX;
dudx = zeros(m,n);
if     dir == 1           % forward
   dudx(1:m-1,:) = diff(u)*ODX;
elseif dir == -1          % rearward
   dudx(2:m  ,:) = diff(u)*ODX;
else					  % central
   dudx(2:m-1,:) = 0.5*(u(3:m,:)-u(1:m-2,:))*ODX;
end;
   	  
