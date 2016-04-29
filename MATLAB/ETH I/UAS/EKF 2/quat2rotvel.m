function w = quat2rotvel(q,dt)

i = find(q(1,:)<0);
q(:,i) = -q(:,i);
halfang = real(acos(q(1,:))); 
halfang(find(~halfang)) = 1e-8;
w = 2*q(2:4,:).*repmat(halfang./sin(halfang),3,1);
w=w/dt;
