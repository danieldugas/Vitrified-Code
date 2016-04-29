%==============================================================
%===  File containing the parameters of the motor, the gearbox
%==== and the propeller                                         
%==============================================================

%== Parameters of the motor

Ra = 2.32;              %[ohm] Resistance Joule
Km = 23.4e-3;           %[Nm]  Torque constant
I0 = 37e-3;             %[A]   No load current of the motor


%=== Parameters of the gearbox

r = 8;                  % Gearbox reduction ratio
eff = 0.98;             % Gearbox efficiency

%=== Interpolation constants

  
Const_prop1 =  0.0002;
Const_prop2 =  0.00093987978580;
Const_prop3 = -0.05697495226239;
Const_prop4 = -0.00012693501889;
Const_prop5 = -0.00694410349433;
Const_prop6 =  0.74958142514170;
Const_prop7 =  0.00000243941079;
Const_prop8 =  0.00117740419580; 
Const_prop9 = -0.04193228690631;
Const_prop10 = -1.41432125473983;
 
 
Const_prop1_pow =  0.01502300736875;
Const_prop2_pow =  0.00356939443628;
Const_prop3_pow = -0.54891474360182;
Const_prop4_pow = -0.00082625957766;
Const_prop5_pow = -0.00983028690283;
Const_prop6_pow =  5.42584377625499;
Const_prop7_pow =  0.00004104274706;
Const_prop8_pow =  0.00354557780353;
Const_prop9_pow =  -0.11948283989006;
Const_prop10_pow = -15.30140857439783;
