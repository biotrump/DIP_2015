%derivative of guass 1D
function dG = dgauss(sig, ms)
if nargin < 2,
    ms=2;
end
x = floor(-ms*sig):ceil(ms*sig);
G = exp(-0.5*x.^2/sig^2);
G = G/sum(G);
dG = -x.*G/sig^2;

end
