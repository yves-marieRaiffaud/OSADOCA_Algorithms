function outQuat = Q_Conjugate(quat)
    [w,x,y,z] = parts(quat);
    outQuat = quaternion(w,-x,-y,-z);
end