function [Q,SP,Shock_speed] = artificial_visc(dir,U)
global  C2 C0 Kentr GAMMA;
global X DX
[m,n] = size(U);
% Artificial dissipative flux
% DX (abs(u) + c)*(C2*sw  + C0) dx(u)
% sw density switch
% (abs(u) + c) velocity scaling
% decode
rho = U(:,1); u = U(:,2) ./ rho;
% density switch
sw      = [0;abs(diff(diff(rho)));0];
sw(1,1) = sw(2  ,1);
sw(m,1) = sw(m-1,1);
den     = rho;
den(2:m-1,:) = ((den(1:m-2) + 2*den(2:m-1,:) + den(3:m,:)));
sw           = sw./den;
[C,S]=max(sw);
SP=S*DX; %shock position
Shock_speed=(rho(S-1)*u(S-1)-rho(S+1)*u(S+1))/(rho(S-1)-rho(S+1));

% add the speed of sound
c = (GAMMA*Kentr*rho.^(GAMMA-1)).^0.5;
 
% add Vscal

Vscal = abs(max(max(U(:,2)./U(:,1)+c),max(U(:,2)./U(:,1)-c)));
Q = DX*Vscal.* (C2*sw*ones(1,2) + C0).* dx(dir,U);
end

 
 
