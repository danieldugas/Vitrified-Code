function [xcog,ycog] = center_of_gravity(xv,yv)

xmin = min(xv);                             % Minimum x coordinate
xmax = max(xv);                             % Maximum x coordinate
ymin = min(yv);                             % Minimum y coordinate
ymax = max(yv);                             % Maximum y coordinate

n = 200;                                    % Nbr of points in
i=0;                                        % Array index is initialized

y = [];                                                 % Vector containing y coordinates is initialized
x = repmat((xmin:(xmax-xmin)/n:xmax),1,n+1);            % Vector containing x coordinates
for i=0:1:n
    y = horzcat(y,ones(1,n+1)*(ymin+i/n*(ymax-ymin)));  % Filling the vector containing y coordinates
end

in = inpolygon(x,y,xv,yv);                              % Contains 1/0 if point (xv,yv) is included in (x,y)
nbpoints = sum(in);                                     % Total number of points

xcog =  (in * x')/nbpoints                              % X coordinate of center of gravity
ycog =  (in * y')/nbpoints                              % Y coordinate of center of gravity

if(1)
    plot(xv,yv,x(in),y(in),'r+',x(~in),y(~in),'b.')         %
    hold on;
    plot(xcog,ycog,'ko');
    axis([xmin xmax ymin ymax]*1.3)
end
end