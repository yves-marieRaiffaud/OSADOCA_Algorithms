function sphericalImpactP = PointToSpherePoint(Points, i, lAscN, omega, anglesInDEG)
    % From a 2D point in the XY plane, returns the 3D world point by
    % applying the orbital parameters for the orbit: inclination 'i',
    % longitude of the ascending node 'lAscN' and argument of the
    % perihelion 'omega'
    if(anglesInDEG)
        i = i*pi/180;
        lAscN = lAscN*pi/180;
        omega = omega*pi/180;
    end
    
    apogeeLineDir = [1 0 0];
    ascendingNodeLineDir = [0 1 0];
    iRotQuat = Q_AngleAxis(i,ascendingNodeLineDir, false);
    rotatedApogeeDir = Q_RotateVector(iRotQuat, apogeeLineDir);

    normalUp = cross(ascendingNodeLineDir, rotatedApogeeDir);
    perihelionArgRot = Q_AngleAxis(omega, normalUp, false);
    lAscNRot = Q_AngleAxis(lAscN, [0 0 1], false);
    rotQuat = Q_Multiply(lAscNRot, Q_Multiply(perihelionArgRot, iRotQuat));
    
    sphericalImpactP = Points;
    for idx=1:size(Points,1)
        sphericalImpactP(idx,1:3) = Q_RotateVector(rotQuat, Points(idx,1:3));
    end
end