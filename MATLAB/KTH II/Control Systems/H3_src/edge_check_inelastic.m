function [xo , transitions] = edge_check_inelastic(xi)
xo = xi; %pre-assignment
transitions = 0; %pre-assignment


x_in_guard1 = ( xi(1) <= xi(3) );
if x_in_guard1
    
    %transitions(end+1) = 1
    dw = (xi(2)-xi(4))/2;
    xo(1) = xi(3);
    xo(2) = xi(2) - dw;
    xo(3) = xi(1);
    xo(4) = xi(4) + dw;
    xo(5) = xi(5);
    xo(6) = xi(6);
    %[xo, transitions] = edge_check_inelastic(xo); %recursion
else
    x_in_guard2 = ( xi(3) <= xi(5) );
    if x_in_guard2
        
        %transitions(end+1) = 2
        dw = (xi(4)-xi(6))/2
        xo(1) = xi(1);
        xo(2) = xi(2);
        xo(3) = xi(5);
        xo(4) = xi(4) - dw;
        xo(5) = xi(3);
        xo(6) = xi(6) + dw;
        %[xo, transitions] = edge_check_inelastic(xo); %recursion
        
        
    end
end





end