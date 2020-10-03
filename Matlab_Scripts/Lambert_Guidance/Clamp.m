function out = Clamp(lowerBound, upperBound, value)
    if(value <= lowerBound)
        out = lowerBound;
    elseif(value >= upperBound)
        out = upperBound;
    else
        out = value;
    end
end