function [xo , qo] = edge_check(xi, qi)

xo = xi; %pre-assignment
qo = qi; %pre-assignment

switch qi
    case 1
        x_in_guard12 = ( xi(1) <= 0 ) && ( xi(2) < 0 );
        
        if x_in_guard12
            qo = 2;
            xo(1) = 0;
            xo(2) = 0;
            xo(3) = 0;
            xo(4) = xi(2);
            xo(5) = 0;
            xo(6) = 0;
            [xo, qo] = edge_check(xo, qo); %recursion
        else
            
        end
    case 2
        x_in_guard21 = ( xi(4) > 0 ) ;
        x_in_guard23 = ( xi(4) < 0 ) ;
        
        if x_in_guard21
            qo = 1;
            xo(1) = 0;
            xo(2) = xi(4);
            xo(3) = 0;
            xo(4) = 0;
            xo(5) = 0;
            xo(6) = 0;
            [xo, qo] = edge_check(xo, qo); %recursion
        end
        if x_in_guard23
            qo = 3;
            xo(1) = 0;
            xo(2) = 0;
            xo(3) = 0;
            xo(4) = 0;
            xo(5) = 0;
            xo(6) = xi(4);
            [xo, qo] = edge_check(xo, qo); %recursion
        end
        
    case 3
        x_in_guard32 = ( xi(5) >= 0 ) && ( xi(6) > 0 ) ;
        
        if x_in_guard32
            qo = 2;
            xo(1) = 0;
            xo(2) = 0;
            xo(3) = 0;
            xo(4) = xi(6);
            xo(5) = 0;
            xo(6) = 0;
            [xo, qo] = edge_check(xo, qo); %recursion
        end
        
    otherwise
        error = 'qi out of bounds in edge_check function'
end

end