function [ra,rp,e,p,a,b,c,i,lAscN,omega] = rv2Orbit(r,v,mu)
    % From the 3D position vector 'r' of the spacecraft in meters, its
    % velocity vector in m/s 'v' and the standard gravitational constant of
    % the orbited planet, returns the orbital parameters of the
    % spacecraft's orbit.

    vNorm = norm(v);
    rNorm = norm(r);
    a = rNorm*mu / (2*mu - rNorm*vNorm*vNorm);
    h = cross(r,v);
    eVec = cross(v,h)/mu - r/rNorm;
    e = norm(eVec);
    p = a*(1-e*e);
    b = sqrt(p*a);
    c = a*e;
    ra = a+c;
    rp = 2*a - ra;
    
    ascNVec = cross([1;0;0], h);
    i = (pi/2 - acos(h(2)/norm(ascNVec))) * 180/pi;
    lAscN = acos(ascNVec(1)/norm(ascNVec));
    if(ascNVec(3) >= 0)
        lAscN = ClampAngle(0,pi,lAscN);
    else
        lAscN = ClampAngle(pi,2*pi,lAscN);
    end
    lAscN = (pi/2-lAscN) * 180/pi;
    
    omega = 0;
    if(norm(ascNVec)*norm(eVec) ~= 0)
        omega = acos(dot(ascNVec,eVec)/norm(ascNVec)*norm(eVec)) * 180/pi;
    end
    %OrbitalState = [ra,rp,e,p,a,b,c,i,lAscN,omega];
end