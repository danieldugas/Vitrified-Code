function turbulences(in)
b=3.2; % wingspan hardcoded

lw=in(5);

%inputs:
V=(norm(in(1:3)));
if V<0.1
    V=0.1;
end
  
% approximation of the wind at 6m height (20ft)
w20=in(4)/1.5; 


sigma_w=0.1*w20;
sigma_u=sigma_w/(0.177+0.000823*lw)^0.4;
sigma_v=sigma_u;

lu=lw/(0.177+0.000823*lw)^1.2;
lv=lu;

% gains:
set_param('Sky_Sailor_Simulation/Wind and Turbulences/Van Karman Filters/Gain',...
    'Gain',num2str(sigma_u*sqrt(2*lu/(pi*V))));
set_param('Sky_Sailor_Simulation/Wind and Turbulences/Van Karman Filters/Gain1',...
    'Gain',num2str(sigma_v*sqrt(lv/(pi*V))));
set_param('Sky_Sailor_Simulation/Wind and Turbulences/Van Karman Filters/Gain2',...
    'Gain',num2str(sigma_w*sqrt(lw/(pi*V))));
set_param('Sky_Sailor_Simulation/Wind and Turbulences/Van Karman Filters/Gain3',...
    'Gain',num2str(sigma_w*sqrt(0.8/V)*(pi/4/b)^(1/6)/(2*lw)^(1/3)));
set_param('Sky_Sailor_Simulation/Wind and Turbulences/Van Karman Filters/Gain4',...
    'Gain',num2str(sigma_v*sqrt(lv/(pi*V))));
set_param('Sky_Sailor_Simulation/Wind and Turbulences/Van Karman Filters/Gain5',...
    'Gain',num2str(sigma_w*sqrt(lw/(pi*V))));

% tfs:
set_param('Sky_Sailor_Simulation/Wind and Turbulences/Van Karman Filters/Transfer Fcn',...
    'Numerator',['[' num2str(0.25*lu/V) ' 1]']);
set_param('Sky_Sailor_Simulation/Wind and Turbulences/Van Karman Filters/Transfer Fcn',...
    'Denominator',['[' num2str(0.1987*(lu/V)^2) ' ' num2str(1.357*(lu/V)) ' 1]']);

set_param('Sky_Sailor_Simulation/Wind and Turbulences/Van Karman Filters/Transfer Fcn1',...
    'Numerator',['[' num2str(0.3398*(lv/V)^2) ' ' num2str(2.7478*lv/V) ' 1]']);
set_param('Sky_Sailor_Simulation/Wind and Turbulences/Van Karman Filters/Transfer Fcn1',...
    'Denominator',['[' num2str(0.1539*(lv/V)^3) ' ' num2str(1.9754*(lv/V)^2) ' ' num2str(2.9958*(lv/V)) ' 1]']);

set_param('Sky_Sailor_Simulation/Wind and Turbulences/Van Karman Filters/Transfer Fcn2',...
    'Numerator',['[' num2str(0.3398*(lw/V)^2) ' ' num2str(2.7478*lw/V) ' 1]']);
set_param('Sky_Sailor_Simulation/Wind and Turbulences/Van Karman Filters/Transfer Fcn2',...
    'Denominator',['[' num2str(0.1539*(lw/V)^3) ' ' num2str(1.9754*(lw/V)^2) ' ' num2str(2.9958*(lw/V)) ' 1]']);

set_param('Sky_Sailor_Simulation/Wind and Turbulences/Van Karman Filters/Transfer Fcn3',...
    'Numerator',['[' num2str(1/V) ' 0]']);
set_param('Sky_Sailor_Simulation/Wind and Turbulences/Van Karman Filters/Transfer Fcn3',...
    'Denominator',['[' num2str((4*b)/(pi*V)) ' 1]']);
set_param('Sky_Sailor_Simulation/Wind and Turbulences/Van Karman Filters/Transfer Fcn5',...
    'Numerator',['[' num2str(1/V) ' 0]']);
set_param('Sky_Sailor_Simulation/Wind and Turbulences/Van Karman Filters/Transfer Fcn5',...
    'Denominator',['[' num2str((3*b)/(pi*V)) ' 1]']);
set_param('Sky_Sailor_Simulation/Wind and Turbulences/Van Karman Filters/Transfer Fcn7',...
    'Numerator',['[' num2str(1/V) ' 0]']);
set_param('Sky_Sailor_Simulation/Wind and Turbulences/Van Karman Filters/Transfer Fcn7',...
    'Denominator',['[' num2str((3*b)/(pi*V)) ' 1]']);

set_param('Sky_Sailor_Simulation/Wind and Turbulences/Van Karman Filters/Transfer Fcn6',...
    'Numerator',['[' num2str(0.3398*(lv/V)^2) ' ' num2str(2.7478*lv/V) ' 1]']);
set_param('Sky_Sailor_Simulation/Wind and Turbulences/Van Karman Filters/Transfer Fcn6',...
    'Denominator',['[' num2str(0.1539*(lv/V)^3) ' ' num2str(1.9754*(lv/V)^2) ' ' num2str(2.9958*(lv/V)) ' 1]']);

set_param('Sky_Sailor_Simulation/Wind and Turbulences/Van Karman Filters/Transfer Fcn4',...
    'Numerator',['[' num2str(0.3398*(lw/V)^2) ' ' num2str(2.7478*lw/V) ' 1]']);
set_param('Sky_Sailor_Simulation/Wind and Turbulences/Van Karman Filters/Transfer Fcn4',...
    'Denominator',['[' num2str(0.1539*(lw/V)^3) ' ' num2str(1.9754*(lw/V)^2) ' ' num2str(2.9958*(lw/V)) ' 1]']);