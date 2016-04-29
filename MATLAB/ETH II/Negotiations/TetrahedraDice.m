% Can we make three non-transitive tetrahedric dice not sharing any
% numbers?

pool = 1:12;

rep  = [1 1 2 2 3 3 3 3 2 2 1 1];

dice = @(n,f) pool(rep==n)*((1:4)'==f);

%iterate across possible repartitions
for i = 1:10
%     x = round(rand*10+1);
%     rep(x:x+1) = rep(x+1:-1:x); % disturbance
    
    while 1
    xdice = round(rand*2+1);
    xface = round(rand*3+1);
    xnum  = dice(xdice,xface);
    if rep(xnum) ~= rep(xnum+1)
        break
    end
    end
     
    rep([xnum xnum+1]) = ...
        rep([xnum+1 xnum]);
    
    while 1
    x2f   = round(rand*3+1);
    x2n   = dice(xdice,xface);
    if x2f ~= xface
       if rep(x2n) ~= rep(x2n-1)
        break
       end
    end
    end
    
    rep([x2n x2n-1]) = ...
        rep([x2n-1 x2n]);
    
    rep
    
end