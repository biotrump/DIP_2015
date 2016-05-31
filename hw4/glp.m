% @brief Gaussian Low pass filter
% @param im is the fourier transform of the image
% @param D0 is the cutoff circle radius
% @return res is the filtered image
function res = glp(im,D0)
    [M,N]=size(im);
    d0=D0;

    d=zeros(M,N);
    h=zeros(M,N);

    for u=1:M
        for v=1:N
         d(u,v)=  sqrt( (u-(M/2))^2 + (v-(N/2))^2);
        end
    end

    for u=1:M
        for v=1:N
          h(u,v)= exp ( -( (d(u,v)^2)/(2*(d0^2)) ) );
        end
    end


    for u=1:M
        for v=1:N
        res(u,v)=(h(u,v))*im(u,v);
        end
    end
end
