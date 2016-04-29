    close all; clear all
% taille du conteneur
% taille du conteneur
N=30;    % taille horizontal du tas
M=50;
initHaueur=25;  % hauteur initial du tas
      % combien de particules sont ejectï¿½es
a=3;  

tasi=round(initHaueur*(N:-1:1)/N+a*rand([1 N]));
equil=false;
tas=tasi;

A = figure('Name','Profil Initial',...
           'NumberTitle','off',...
           'Position',[700 100 600 500]);
plot(tas,'o');
hold on;
B = figure('Name','Iterations',...
           'NumberTitle','off',...
           'Position',[50 100 600 500]);
plot(tas,'o');
pause;

roule = 0;
rouletot=0;
tombes=0;
tombestot=0;
oldtas=tas;


i= 0;
while i < M;
    indg=find(tas>[tas(2:N) 0]+2);
    mus=length(indg);
    roule = roule + 2*mus;
    rouletot=rouletot+2*mus;
    if mus==0
        
        i = i+1;
        set(B, 'Name', ['Equilibre ' num2str(i) '; ' num2str(roule) ' de roulées;  ' num2str(tombes) ' de tombées']);
    
        hold off;
        plot(tas,'o');
        hold on;
        tastemp = tas;
        tastemp(find(oldtas==tas)) = inf;
        plot( tastemp, 'ro');
        pluie = round((N-1)*rand()+1);
        tas(pluie)=tas(pluie)+1;
        plot(pluie, tas(pluie), 's');
        pause;
        hold off;
        
        tombes = 0;
        roule = 0;
        oldtas = tas; 
        
    elseif indg(mus)==N
         
        tombes=tombes+2;
        tombestot=tombestot+2;
        tas(indg)=tas(indg)-2;
        tas(indg(1:mus-1)+1)=tas(indg(1:mus-1)+1)+2;
        
    else
        
        tas(indg)=tas(indg)-2;
        tas(indg(1:mus)+1)=tas(indg(1:mus)+1)+2;
        
    end
   
end
figure(A);
set(A, 'Name', ['Profil Final : ' num2str(rouletot) ' de roulées;  ' num2str(tombestot) ' de tombées']);
plot(tas,'ro');


Niter=200;
tombHist=zeros([1 Niter+1]);
rouleHist=zeros([1 Niter+1]);
tombHist(1)=tombestot;
rouleHist(1)=rouletot;
newParti=round(1+(N-1)*rand(1,Niter)); % la position ou les nouvelles particules tomberont

