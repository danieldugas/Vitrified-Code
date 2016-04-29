function [F, dt] = visualisation(m1, m2, m3, j, l, p, k1, k2, k3, c1, c2, c3, d, z, v, Z)


DrawFigure = figure('Name',['Creating Movie... - 0% Done'],...
      'NumberTitle','off',...
      'Position',[50 50 1200 500]);
hold all;

td = 0.6;
rd = 0.25;
h = 1;
t = 0;
dt = 0.01;
tmax = 3.6*d/v;
o = 2*pi/tmax;

for n = 1:(tmax/dt)
    
    t = n*dt;
    road(d, z, v, t);
    hold all;
    tires(td, l, d, z, v, t);
    body(td, l, p, h, Z(1), Z(4), o, t);
    rims(td, rd, l, p, h, Z(2), Z(3), o, t);
    
    F(n) = getframe;
    set(DrawFigure, 'Name', ['Creating Movie... - ' num2str(100*t/tmax) '% Done']);
    hold off;
end

set(DrawFigure, 'Name', ['Movie - Ground Speed: ' num2str(v) ' km/h']);

end

function road(d, z, v, t)

ymin = 0;
ymax = 3;

xmin = 0;
xmax = 5;

x = xmin:0.1:xmax;

road = z*sin(2*pi*(x+v*t/3.6)/d)+ z;
plot(x, road);

ylim([ymin ymax]);
xlim([xmin xmax]);
set(gca,'DataAspectRatio',[1 1 1],'PlotBoxAspectRatio',[1 1 1]);

end

function tires(td, l, d, z, v, t)


ltire_xpos = 1;
rtire_xpos = 1 + l;
ltire_ypos = z*sin(2*pi*(ltire_xpos+td/2+v*t/3.6)/d)+z;
rtire_ypos = z*sin(2*pi*(rtire_xpos+td/2+v*t/3.6)/d)+z;
ltire_pos = [ltire_xpos, ltire_ypos];
rtire_pos = [rtire_xpos, rtire_ypos];
ltire = rectangle('Position',[ltire_pos,td,td],'Curvature',[1,1]);
rtire = rectangle('Position',[rtire_pos,td,td],'Curvature',[1,1]);

end

function body(td, l, p, h, z1, z4, o, t)


dy = z1*cos(o*t);
body_origin = [1+td/2, 4*td/3];
body_pos = body_origin + [0, dy];
body_CG = body_pos + [p*l, 0.5]; 

CG = plot(body_CG(1), body_CG(2), 'x');
body = rectangle('Position',[body_pos,l,h]);

dtheta = z4*cos(o*t);
dtd = dtheta*180/pi;

%% Rotated Body

pts = [body_pos(1) body_pos(1) body_pos(1)+l body_pos(1)+l body_pos(1);
       body_pos(2) body_pos(2)+h body_pos(2)+h body_pos(2) body_pos(2) ];

% transformation matrices
rot = [cosd(dtd) sind(dtd);
      -sind(dtd) cosd(dtd) ];
trans = [body_CG(1) body_CG(1) body_CG(1) body_CG(1) body_CG(1);
         body_CG(2) body_CG(2) body_CG(2) body_CG(2) body_CG(2) ];

% Transformation
pts = pts-trans ;
pts = rot*pts ; 
pts = pts+trans;

% Plot Rotated box
plot(pts(1,1:end),pts(2,1:end),'r*-');

end

function rims(td, rd, l, p, h, z2, z3, o, t)

lrim_dy = z2*cos(o*t);
lrim_origin = [1+(td-rd)/2, 0.8*rd];
lrim_pos = lrim_origin + [0, lrim_dy];
lrim = rectangle('Position',[lrim_pos,rd,rd],'Curvature',[1,1]);

rrim_dy = z3*cos(o*t);
rrim_origin = [1+l+(td-rd)/2, 0.8*rd];
rrim_pos = rrim_origin + [0, rrim_dy];
rrim = rectangle('Position',[rrim_pos,rd,rd],'Curvature',[1,1]);
end