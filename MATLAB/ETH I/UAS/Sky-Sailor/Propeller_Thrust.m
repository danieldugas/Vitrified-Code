function out = Propeller_Thrust(in)

u           =   in(1);
speed_x     =   in(2);

param_motor;

Speed_propeller_equations;

if (u < 6.5)
            T = 0;
        else
                T = Const_prop1*speed_x^3+Const_prop2*speed_x^2*omega_p+Const_prop3*speed_x^2+...
                    Const_prop4*speed_x*omega_p^2+Const_prop5*speed_x*omega_p+Const_prop6*speed_x+...
                    Const_prop7*omega_p^3+Const_prop8*omega_p^2+Const_prop9*omega_p+Const_prop10;
        end
        if T < 0
            T = 0;
        else 
            T = real(T);
        end

out = T;