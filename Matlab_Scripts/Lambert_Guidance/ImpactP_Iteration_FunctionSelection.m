clc
clear variables
close all
%=================================================
%=================================================
% Make functions in sub-folders available
currentFolderContents = dir(pwd);
currentFolderContents (~[currentFolderContents.isdir]) = [];
for i = 3:length(currentFolderContents)
   addpath(['./' currentFolderContents(i).name]);
end
%=================================================
%=================================================
% Earth µ
muEarth = 39.86004418*10^13; % m^3.s^-2
% Earth radius in m, assuming spherical Earth
rEarth = 6378e3;
x0 = [0 6578e3 0];
target = [6378e3 0 0];
v0 = [10 0 0];
%=================================================
%=================================================
[X,Y,Z] = sphere(100);
figure()
hSurface = surf(X*rEarth,Y*rEarth,Z*rEarth);
set(hSurface,'FaceColor',[0 0.4 0.8],'FaceAlpha',0.5,'EdgeColor','none');
hold on
plot3(x0(1),x0(2),x0(3),'og','MarkerFaceColor','y');
plot3(0,0,0,'og','MarkerFaceColor','b');
plot3(target(1),target(2),target(3),'og','MarkerFaceColor','r');
hitError = [inf inf inf];
counter = 0;
lastValidVel = v0;
startTheta = atan2(x0(1),x0(2))*180/pi
requiredTheta = atan2(target(1),target(2))*180/pi
while(norm(hitError) > 100 && counter < 150)
    fprintf('Iteration %d -- Velocity: [%.5f;%.5f;%.5f] m/s\n', counter,v0(1),v0(2),v0(3));
    [ra,rp,e,p,a,b,c,i,lAscN,omega] = rv2Orbit(x0,v0,muEarth);
    if(rp <= rEarth)
        lastValidVel = v0;
        disp('Will crash on Earth''s surface');
        impactPoints = CircleEllipseFociSharingIntersect(a,b,c,rEarth);
        sphericalImpactP = PointToSpherePoint(impactPoints,i,lAscN,omega,true);
        hitError = (impactPoints(1,:) - target) - sign(impactPoints(1,:) - target)*rEarth
        theta = 90-atan2(impactPoints(1,1), impactPoints(1,2))*180/pi;
        
        circleHitError = sqrt(6378^2 - (abs(hitError(2)/1000)-6378)^2)
        % Remap in the range (lb;up)
        lb = -200; %m/s
        ub = 200; %m/s
        xMin = -6378;
        xMax = 6378;
        velocityIncr = (ub-lb)/(xMax-xMin)*(circleHitError-xMax)+ub       
        
        if(theta >= requiredTheta)
            velocityIncr = -velocityIncr;
        end
        v0 = v0 + velocityIncr*[cos(startTheta) sin(startTheta) 0];
    else
        disp('Will not crash on the Earth''s surface');
        fprintf('Velocity = [%.5f;%.5f;%.5f]\n', v0(1),v0(2),v0(3));
        v0 = v0 - 1000*[cos(startTheta) sin(startTheta) 0];
    end
    t = linspace(0,2*pi);
    x = a*cos(t)+c;
    y = b*sin(t);
    orbitSphere = [transpose(x) transpose(y) zeros(size(x,2),1)];
    orbitSphere = PointToSpherePoint(orbitSphere,i,lAscN,omega, true);
    plot3(transpose(orbitSphere(:,1)),transpose(orbitSphere(:,2)),transpose(orbitSphere(:,3)), '-r');
    if(exist('sphericalImpactP','var') == 1)
        if(~isempty(sphericalImpactP))
            for idx=1:size(sphericalImpactP,1)-1
                plot3(sphericalImpactP(idx,1),sphericalImpactP(idx,2),sphericalImpactP(idx,3),'og','MarkerFaceColor','g');
            end
        end
        title(sprintf('Iteration %d - Velocity = [%.5f; %.5f; %.5f] m/s\nHitError = [%.5f; %.5f; %.5f] m\nTrue Anomaly = %.5f°', counter, v0(1),v0(2),v0(3), hitError(1), hitError(2), hitError(3), theta))
    end
    axis equal;
    xlabel('x (km)')
    ylabel('y (km)')
    zlabel('z (km)')
    pause(0.0000000005);
    %pause;
    counter = counter + 1;
end
hold off
axis equal
xlabel('x (km)')
ylabel('y (km)')
zlabel('z (km)')
title(sprintf('Stopped at Iteration %d - Velocity = [%.5f; %.5f; %.5f] m/s\nHitError = [%.5f; %.5f; %.5f] m\nTrue Anomaly = %.5f°', counter, v0(1),v0(2),v0(3), hitError(1), hitError(2), hitError(3), theta))
pause(0.0000000005);