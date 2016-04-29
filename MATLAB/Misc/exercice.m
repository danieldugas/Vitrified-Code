%MODULE 9
%exercice 1
clear A;
clear iterations;

iterations=10;
A(1:iterations)=0;
A(1:2:iterations)=1;
plot(A);
%----------

%exercice 2
plot( max( min( sin(linspace(0, 6*pi)), 0.8 ), -0.8 ));
%----------
%==========

%MODULE 10
%exercice 1
hold off;
eyeseparation=0.6;
eyeheight=0.3;
mouthheight=-0.6;
eyesize=1/6;
eyeratio=2.5;
pupilsize=1/10;
pupilheight=0.2;

v=linspace(0, 2*pi);
x=sin(v);
y=cos(v);

plot(x,y);
hold all;
xlefteye=(x*eyesize)-(eyeseparation/2);
ylefteye=(eyeratio*y*eyesize)+eyeheight;
plot(xlefteye,ylefteye);

xrighteye=(x*eyesize)+(eyeseparation/2);
yrighteye=(eyeratio*y*eyesize)+eyeheight;
plot(xrighteye,yrighteye);

xleftpupil=(x*pupilsize)-(eyeseparation/2);
yleftpupil=(y*pupilsize)+pupilheight;
plot(xleftpupil, yleftpupil);

xrightpupil=(x*pupilsize)+(eyeseparation/2);
yrightpupil=(y*pupilsize)+pupilheight;
plot(xrightpupil, yrightpupil);

xmouth=(x/2);
ymouth=((y/6)+mouthheight)+abs(xmouth.*xmouth);
plot(xmouth,ymouth);

for n=1:5
TIME=[5, 10, 8, 13, 9];
pause(TIME(n));
hold off;


%blink --------------------------------------------------------------------
plot(x,y);
hold all;

plot(xmouth,ymouth);

xblinklefteye=(x*eyesize)-(eyeseparation/2);
yblinklefteye=(y*0.0001)+pupilheight;
plot(xblinklefteye,yblinklefteye);

xblinkrighteye=(x*eyesize)+(eyeseparation/2);
yblinkrighteye=(y*0.0001)+pupilheight;
plot(xblinkrighteye,yblinkrighteye);


pause(0.2);
hold off;
%--------------------------------------------------------------------------

%back to regular face -----------------------------------------------------
plot(x,y);
hold all;
plot(xlefteye,ylefteye);
plot(xrighteye,yrighteye);
plot(xleftpupil, yleftpupil);
plot(xrightpupil, yrightpupil);
plot(xmouth,ymouth);
end