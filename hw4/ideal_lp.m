% F(u,v) : M*N 
%H(u,v) 
%D(u,v) = sqrt((u-M/2)^2 +(v-N/2)^2)
function lp=ideal_lp(F, D0)
    [M N]=size(F);
    lp=F;
    for u=1:M,
        for v=1:N,
            D=sqrt((u-M/2)^2 +(v-N/2)^2);
            if(D > D0),%ideal low pass filter
                lp(u,v)=0;
            end
        end
    end
end