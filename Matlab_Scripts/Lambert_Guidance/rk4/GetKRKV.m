function krkv = GetKRKV(X, mu)
    % Compute the kr kv coefficients to perform one step of the Runge-Kutta
    % 4th order propagator
    r = X(1,:);
    v = X(2,:);
    
    kr = (-r*mu/norm(r)^3);
    krkv = [v;kr];
end