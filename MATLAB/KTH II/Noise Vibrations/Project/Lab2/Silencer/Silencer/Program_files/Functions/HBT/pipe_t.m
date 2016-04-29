if not(exist('Pn'))|not(exist('Tn'))
    element=0;
    CodeError='Variable Pn and/or Tn doesn´t exist!';
else
    [Rp,Cp]=size(Pn);
    [Rt,Ct,Dt]=size(Tn);
    if (Rp~=1)|(Cp~=1)
        element=0;
        CodeError='Variable Pn must be a scalar!';
    elseif (Rt~=2)|(Ct~=2)|(Dt~=length(f))
        element=0;
        CodeError='Variable Tn must be a 3-dimensional matrix of size: [Rows,Columns,Layers]=[2,2,length(f)]';
    else    
        element.type='Pipe';
        element.c=c;
        element.rho=rho;
        element.Q=Q;
        element.L=L;
        element.S=S;
        element.xi=xi;
        element.frequency=frequency;
        element.Pn=Pn;
        element.Tn=Tn;
        CodeError='No error';
    end %if
end %if

catch
    element=0;
    if (isempty(lasterr))&(not(isempty(lastwarn)))
        CodeError=lastwarn;
    elseif (isempty(lastwarn))&(not(isempty(lasterr)))
        CodeError=lasterr;
    elseif (not(isempty(lastwarn)))&(not(isempty(lasterr)))
        CodeError=[lasterr,' ',lastwarn];
    end    
end %try-catch