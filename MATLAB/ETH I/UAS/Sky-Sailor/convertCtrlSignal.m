function out ...
    = convertCtrlSignal(in)
% normalized control signals to motor voltage / control surface angle
% Stefan Leutenegger, 10/2009

delta_thr=in(1);
delta_ail=in(2);
delta_elev=in(3);
delta_rudd=in(4);

%limit
if delta_thr>1 
    delta_thr=1;
end
if delta_thr<0 
    delta_thr=0;
end
if delta_elev>1 
    delta_elev=1;
end
if delta_elev<-1 
    delta_elev=-1;
end
if delta_ail>1 
    delta_ail=1;
end
if delta_ail<-1 
    delta_ail=-1;
end
if delta_rudd>1 
    delta_rudd=1;
end
if delta_rudd<-1 
    delta_rudd=-1;
end

%set motor voltage
motor_volt = delta_thr*30;           % [V]

%set ailerons
if(delta_ail>0)
    right_ail = delta_ail/(1.5)*(15/180*pi); % [rad]
    left_ail = -2.5*right_ail;
else
    left_ail = delta_ail/(-1.5)*(15/180*pi); % [rad]
    right_ail = -2.5*left_ail;
end

%set elevator
left_v     = delta_elev*(10/180*pi)*0.3; % [rad]
right_v    = delta_elev*(10/180*pi)*0.3; % [rad]

%set rudder
left_v     = left_v+delta_rudd*(10/180*pi)*0.7; % [rad]
right_v    = right_v-delta_rudd*(10/180*pi)*0.7; % [rad]

%remark: mixing specific... factor of two may be adapted

out=[left_ail,right_ail,left_v,right_v,motor_volt];