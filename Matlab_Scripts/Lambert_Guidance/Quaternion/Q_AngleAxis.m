function quat = Q_AngleAxis(angle, axis, angleIsInDEG)
    % The specified 'angle' must be in RADIANS
    % The specified 'axis' must be a column vector (1x3 array)
    if(angleIsInDEG)
        angle = angle*pi/180;
    end
    normVec = axis/norm(axis);
    qx = normVec(1) * sin(angle/2);
    qy = normVec(2) * sin(angle/2);
    qz = normVec(3) * sin(angle/2);
    qw = cos(angle/2);
    quat = quaternion(qw,qx,qy,qz);
    quat = normalize(quat);
end