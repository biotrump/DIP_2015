% Wr : bounding width ratio (w)/(w+h)
% Hr : bounding height ratio (h)/(w+h)
% Euler number : E = H-C
function [A0, Aa, P, Pa, E4, E8, La, Wa, Wr, Hr]=get_feature(roi,method)
    roi=logical(roi);
    [r,w]=size(roi);
    if (strcmp(method,'GRAY'))
        [A0 P E4 E8]=BidQuad_Gary(roi);
    else
        [A0 P E4 E8]=BidQuad_Duda(roi);
    end
    %Circularity (thinness ratio)
    C0 = 4 * pi * A0 / (P  * P);
    %Bounding box ratio
    Wr=(r)/(r+w);
    Hr=(w)/(r+w);

    %Average perimeter:An image with many components but fewer holes
    %E=C-H
    if E4 ~=0
        %Average area
        Aa4 = A0/E4;
        Pa4 = P/E4;
    else
        Aa4=0;
        Pa4=0;
    end
        
    if E8 ~=0,
        Aa8 = A0/E8;
        Pa8 = P/E8;
    else
        Aa8=0;
        Pa8 = 0;
    end
    Aa=[Aa4 Aa8];
    Pa=[Pa4 Pa8];
    %Thin objects (typewritten or script characters)
    %Average length
    La=Pa/2;
    %Average width
    Wa=2*Aa./Pa;
end

%Bit Quad match
function n=nQ(I,Q)
    n=0;
    for r=1:size(I,1)-1
        for c=1:size(I,2)-1
            m=0;
            for i=1:size(Q,1)
               for j=1:size(Q,2)
                   if Q(i,j)==I(r+i-1,c+j-1),
                       m=m+1;
                   end
               end
            end
            if m == size(Q,1)*size(Q,2),
                n=n+1;
            end
        end
    end
end

function n=nQ1(I)
    Q11=[1 0;0 0];
    Q12=[0 1;0 0];
    Q13=[0 0;0 1];
    Q14=[0 0;1 0];
    n=0;
    n=nQ(I,Q11);
    n=n+nQ(I,Q12);
    n=n+nQ(I,Q13);
    n=n+nQ(I,Q14);
end
function n=nQ2(I)
    n=0;
    Q21=[1 1; 0 0];
    Q22=[0 1; 0 1];
    Q23=[0 0;1 1];
    Q24=[1 0;1 0];
    n=nQ(I,Q21);
    n=n+nQ(I,Q22);
    n=n+nQ(I,Q23);
    n=n+nQ(I,Q24);
end
function n=nQ3(I)
    n=0;
    Q31=[1 1;0 1];
    Q32=[0 1;1 1];
    Q33=[1 0;1 1];
    Q34=[1 1;1 0];
    n=nQ(I,Q31);
    n=n+nQ(I,Q32);
    n=n+nQ(I,Q33);
    n=n+nQ(I,Q34);
end
function n=nQ4(I)
    n=0;
   Q4=[1 1;1 1];
   n=nQ(I,Q4);
end
function n=nQd(I)
    n=0;
    Qd1=[1 0;0 1];
    Qd2=[0 1;1 0];
    n=nQ(I,Qd1);
    n=n+nQ(I,Qd2);
end

function [A P E4 E8]=BidQuad_Gary(image)
%    Q0=[0 0; 0 0];
%    Q11=[1 0;0 0];
%    Q12=[0 1;0 0];
%    Q13=[0 0;0 1];
%    Q14=[0 0;1 0];
%    Q21=[1 1; 0 0];
%    Q22=[0 1; 0 1];
%    Q23=[0 0;1 1];
%    Q24=[1 0;1 0];
%    Q31=[1 1;0 1];
%    Q32=[0 1;1 1];
%    Q33=[1 0;1 1];
%    Q34=[1 1;1 0];
%    Q4=[1 1;1 1];
%    Qd1=[1 0;0 1];
%    Qd2=[0 1;1 0];
    image=logical(image);
    %A=(n{Q1}+2n{Q2}+3n{Q3}+4n{Q4}+2n{Qd})/4;
    A=( nQ1(image) + 2*nQ2(image) + 3 * nQ3(image) + 4 * nQ4(image) + 2* nQd(image) )/4;
    %P=(n{Q1}+n{Q2}+n{Q3}+2n{Qd});
    P=nQ1(image) + nQ2(image) + nQ3(image) + 2 * nQd(image);

    %Euler number, four connected 
    n1=nQ1(image);
    n3=nQ3(image);
    nd=nQd(image);
    E4= (nQ1(image) - nQ3(image) + 2 * nQd(image))/4;
    %Euler number, eight connected
    E8= (nQ1(image) - nQ3(image) - 2 * nQd(image))/4;
end
function [A P E4 E8]=BidQuad_Duda(image)
    image=logical(image);
    %A=n{Q1}/4+n{Q2}/2+7n{Q3}/8+n{Q4}+3n{Qd}/4;
    A=nQ1(image)/4 + nQ2(image)/2 + 7 * nQ3(image)/8 + 3 * nQd(image)/4;
    %P=n{Q2}+(n{Q1}+n{Q3}+2n{Qd})/sqrt(2);
    P=nQ2(image) + (nQ1(image) + nQ3(image) + 2 * nQd(image))/sqrt(2);
    
    %actuall this is GRAY
    %Euler number, four connected 
    E4= (nQ1(image) - nQ3(image) + 2 * nQd(image))/4;
    %Euler number, eight connected
    E8= (nQ1(image) - nQ3(image) - 2 * nQd(image))/4;
end