% @brief idea low pass filter
% @param F(u,v) : M*N
% @param D(u,v) = sqrt((u-M/2)^2 +(v-N/2)^2)
% @return lp : the result image after idea low pass filter
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
