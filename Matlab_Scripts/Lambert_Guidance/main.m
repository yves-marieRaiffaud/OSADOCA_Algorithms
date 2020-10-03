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
v0 = [7000 0 0];
%=================================================
%=================================================
[X,Y,Z] = sphere(100);
figure()
hSurface = surf(X*rEarth,Y*rEarth,Z*rEarth);
set(hSurface,'FaceColor',[0 0.4 0.8],'FaceAlpha',0.5,'EdgeColor','none');
hold on
plot3(x0(1),-x0(2),x0(3),'og','MarkerFaceColor','y');
plot3(0,0,0,'og','MarkerFaceColor','b');
plot3(target(1),target(2),target(3),'og','MarkerFaceColor','r');
hitError = [inf inf inf];
counter = 0;
lastValidVel = v0;
startTheta = atan2(x0(1),x0(2))*180/pi;
requiredTheta = atan2(target(1),target(2))*180/pi;
while(norm(hitError) > 5000 && counter < 120)
    fprintf('Iteration %d -- Velocity: [%.5f;%.5f;%.5f] m/s\n', counter,v0(1),v0(2),v0(3));
    [ra,rp,e,p,a,b,c,i,lAscN,omega] = rv2Orbit(x0,v0,muEarth);
    if(rp <= rEarth)
        lastValidVel = v0;
        disp('Will crash on Earth''s surface');
        impactPoints = CircleEllipseFociSharingIntersect(a,b,c,rEarth);
        sphericalImpactP = PointToSpherePoint(impactPoints,i,lAscN,omega,true);
        hitError = (impactPoints(1,:) - target) - sign(impactPoints(1,:) - target)*rEarth;
        theta = 90-atan2(impactPoints(1,1), impactPoints(1,2))*180/pi;
        velocityIncr = Clamp(-20, 20, -0.001*hitError(2));
        if(theta >= requiredTheta)
            velocityIncr = -velocityIncr;
        end
        v0 = v0 + velocityIncr*[cos(startTheta) sin(startTheta) 0];
    else
        disp('Will not crash on the Earth''s surface');
        fprintf('Velocity = [%.5f;%.5f;%.5f]\n', v0(1),v0(2),v0(3));
        v0 = v0 - 1*[cos(startTheta) sin(startTheta) 0];
    end
    t = linspace(0,2*pi);
    x = a*cos(t)+c;
    y = b*sin(t);
    orbitSphere = [transpose(x) transpose(y) zeros(size(x,2),1)];
    orbitSphere = PointToSpherePoint(orbitSphere,i,lAscN,omega, true);
    plot3(transpose(orbitSphere(:,1)),transpose(orbitSphere(:,2)),transpose(orbitSphere(:,3)), '-r');
    if(~isempty(sphericalImpactP))
        for idx=1:size(sphericalImpactP,1)-1
            plot3(sphericalImpactP(idx,1),sphericalImpactP(idx,2),sphericalImpactP(idx,3),'og','MarkerFaceColor','g');
        end
    end
    title(sprintf('Iteration %d - Velocity = [%.5f; %.5f; %.5f] m/s\nHitError = [%.5f; %.5f; %.5f] m\nTrue Anomaly = %.5f°', counter, v0(1),v0(2),v0(3), hitError(1), hitError(2), hitError(3), theta))
    axis equal;
    xlabel('x (km)')
    ylabel('y (km)')
    zlabel('z (km)')
    pause(0.0000000005);
    counter = counter + 1;
end
hold off
axis equal
xlabel('x (km)')
ylabel('y (km)')
zlabel('z (km)')
title(sprintf('Stopped at Iteration %d - Velocity = [%.5f; %.5f; %.5f] m/s\nHitError = [%.5f; %.5f; %.5f] m\nTrue Anomaly = %.5f°', counter, v0(1),v0(2),v0(3), hitError(1), hitError(2), hitError(3), theta))
pause(0.0000000005);
%=================================================
%=================================================
%=================================================
%=================================================
nbSteps = 300;
h = 40;
disp('Initial conditions : ');
X = [x0;v0]
[Xe,Ye,Ze] = sphere(100);
figure()
hSurface = surf(Xe*rEarth,Ye*rEarth,Ze*rEarth);
set(hSurface,'FaceColor',[0 0.4 0.8],'FaceAlpha',0.5,'EdgeColor','none');
axis equal
hold on
plot3(0,0,0,'og','MarkerFaceColor','b');
for t=1:nbSteps
    X = rk4Orbit(X,h,muEarth);
    XX(t) = X(1);
    YY(t) = X(2);
    ZZ(t) = X(3);
    plot3(XX,YY,ZZ,'-r');
    pause(0.0000000005);
end
hold off
xlabel('x')
ylabel('y')
zlabel('z')
title('Runge-Kutta 4th order');
view([0 0])
%=================================================
%=================================================
%=================================================
%=================================================
t = linspace(0,2*pi);
x = a*cos(t)+c;
y = b*sin(t);

figure()
plot(x,y,'r')
hold on
xCircle = rEarth*cos(t);
yCircle = rEarth*sin(t);
plot(xCircle,yCircle, '--b');
plot(0,0,'ob');
axis equal;
legend('Orbit','Earth surface');
xlabel('x (km)')
ylabel('y (km)')
title('In orbit''s plane representation of orbit''s impact points on spherical Planet');

impactPoints = CircleEllipseFociSharingIntersect(a,b,c,rEarth)
sphericalImpactP = PointToSpherePoint(impactPoints,i,lAscN,omega,true)

if(~isempty(impactPoints))
    for idx=1:size(impactPoints,1)
        plot(impactPoints(idx,1),impactPoints(idx,2),'og','MarkerFaceColor','g');
    end
end
hold off

orbitSphere = [transpose(x) transpose(y) zeros(size(x,2),1)];
orbitSphere = PointToSpherePoint(orbitSphere,i,lAscN,omega, true);

[X,Y,Z] = sphere(100);
figure()
hSurface = surf(X*rEarth,Y*rEarth,Z*rEarth);
set(hSurface,'FaceColor',[0 0.4 0.8],'FaceAlpha',0.5,'EdgeColor','none');
hold on
plot3(target(1),target(2),target(3),'og','MarkerFaceColor','r');
plot3(0,0,0,'og','MarkerFaceColor','b');
plot3(transpose(orbitSphere(:,1)),transpose(orbitSphere(:,2)),transpose(orbitSphere(:,3)), '-r');
if(~isempty(sphericalImpactP))
    for idx=1:size(sphericalImpactP,1)-1
        plot3(sphericalImpactP(idx,1),sphericalImpactP(idx,2),sphericalImpactP(idx,3),'og','MarkerFaceColor','g');
    end
end
hold off
axis equal
legend('','Target','Origin','Orbit','Impact point');
xlabel('x (km)')
ylabel('y (km)')
zlabel('z (km)')
title(sprintf('Orbit''s impact points - Spherical Planet\nInclination of i = %.3f° (along x-axis), omega = %.3f° and lAscN = %.3f° (along z-axis)',i,omega,lAscN))
view([0 -90])
%=================================================
%=================================================
%=================================================
%=================================================