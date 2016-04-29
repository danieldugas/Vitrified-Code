%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%% Solution to L7_Exercise Nr. 1
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%


%% tabula rasa
clear all
clc

%% Load parameters
parameters
FoM = 0.8;
Cap = 1200;
VBat = 12;

%% Thrust of one propeller
Tprop = 1/4*m*g

%% Ideal power from momentum theory
% propeller disc
Ap = R^2*pi;
% ideal power at hover
Pideal = Tprop^(3/2)/(sqrt(2*rho*Ap))

%% real power
% Power at one propeller
Preal = Pideal/FoM
% Power for quadrotor
Pt = 4*Preal

%% Flight time
% Battery energy
Ebat = VBat*Cap/1000
% flight time until battery is empty
time = Ebat/Pt
% time in minutes
timemin = time*60