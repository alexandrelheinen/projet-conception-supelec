function y = imageProcess(x, n, r)
% this function process the image x by scaling it by a factor n and by
% filtering by its median with a radius r
    y = kron(double(x),ones(n));
    y = medfilt2(y, [r, r]);
end

