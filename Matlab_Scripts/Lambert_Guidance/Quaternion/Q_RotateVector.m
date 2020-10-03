function rotVec = Q_RotateVector(quat,vector)
    % The specified 'vector' to rotate must be a column vector (1x3 array)
    point = quaternion(0,vector(1),vector(2),vector(3));
    q = normalize(quat);
    qStar = Q_Conjugate(q);
    quatMultiplied = Q_Multiply(Q_Multiply(q,point), qStar);
    [w,x,y,z] = parts(quatMultiplied);
    rotVec = [x y z];
end