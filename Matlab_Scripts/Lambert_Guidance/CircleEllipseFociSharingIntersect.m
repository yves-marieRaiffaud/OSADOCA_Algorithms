function impactPoints = CircleEllipseFociSharingIntersect(a, b, ellipseOriginX, r)
    % Returns the intersection points in the XY plane between the circle of
    % radius r and the ellipse of semi-major axis 'a' and semi-minor axis
    % 'b' with its origin at 'ellipseOriginX'. 
    % The center of the circle and one of the foci of the ellipse are
    % coincident
    % ellipseOriginX is the x-coordinate of the center of the ellipse
    % which is not the same coordinate as the circle's origin
    c = ellipseOriginX;
    x = (-c*b^2+sqrt(-a^2*(a^2*b^2-a^2*r^2-b^4-b^2*c^2+b^2*r^2)))/(a^2-b^2);
    y = sqrt(r^2-x^2);
    % Ignoring other solution [x -y 0]
    impactPoints = [x y 0;x -y 0];
end