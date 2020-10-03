function newAngle = ClampAngle(lowerBound,upperBound,angle)
    newAngle = angle;
    while(newAngle <= lowerBound)
        newAngle = newAngle + 2*pi;
    end
    while(newAngle > upperBound)
        newAngle = newAngle - 2*pi;
    end
end