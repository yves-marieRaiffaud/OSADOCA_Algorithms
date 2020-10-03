function quat = Q_Multiply(quatA,quatB)
    [d,a,b,c] = parts(quatA);
    [h,e,f,g] = parts(quatB);
    
    w = d*h-a*e-b*f-c*g;
    x = d*e+a*h+b*g-c*f;
    y = d*f-a*g+b*h+c*e;
    z = d*g+a*f-b*e+c*h;
    quat = quaternion(w,x,y,z);
end