function X = rk4Orbit(X,h,mu)
    % Performs one step of the Runge-Kutta 4th order propagator
    % X is the state matrix
    % h is the timestep
    
    k1 = GetKRKV(X,mu);        % k1
    k2 = GetKRKV(X+k1*h/2,mu); % k2
    k3 = GetKRKV(X+k2*h/2,mu); % k3
    k4 = GetKRKV(X+k3*h,mu);   % k2
    
    X = X + (h/6)*(k1 + 2*k2 + 2*k3 + k4);
end