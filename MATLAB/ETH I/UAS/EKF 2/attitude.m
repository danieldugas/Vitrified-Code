%%% This exercise uses the Hamilton quaternion convention: 
%%% The real part is the first element.
close all
clear all

%% Load real data
load('realdata.mat');

idx_gyr = 2;	% Set the data entry point for inputs.
idx_cam = 1;	% Set the data entry point for updates.
t_start = t_q_CW_m(idx_cam);  % Read the start time.
t_end = t_q_CW_m(800);
STEP = 0.01;				  % Define the time step for each loop.

% Physical setup gyroscope(IMU)/camera.
%% TODO: Add the IMU to camera calibration values.
q_IC = [0.0020,-0.0330,-0.0050,0.9994]';	% Orientation of the camera w.r.t. the IMU frame of reference.

% Variables for 1st order quaternion integration:
w_m_old = omega_m(idx_gyr,:)';	% Estimated rot velocity at t = k: w = w_m - b_w

% Guess the initial state as good as we can
%% TODO: Initialize the orientation with the first measurement.
q_WI = invertquat(mulquat(q_IC,q_CW_m(1,:)'));	% Orientation of IMU frame of reference w.r.t. the world frame of reference.
%% TODO: Initialize the bias.
b_w = [0 0 0]';
X = [q_WI' b_w']';	% The initial state.

% error state covariance initialization:
%% TODO: Initialize the error state covariance matrix.
P = eye(6)*0.01;

% Debug variables for plotting.
debug = [];
idxdebug = 1;

groundtruth_start = find(t_q_WI_gt > t_start, 1);
groundtruth_end = find(t_q_WI_gt > t_end, 1);

%% Main Loop
for t = t_start:STEP:t_end
	if t > t_omega_m(idx_gyr)	% Do we have an input reading?
		w_m = omega_m(idx_gyr,:)';	% Read inputs.
		dt = t_omega_m(idx_gyr) - t_omega_m(idx_gyr - 1);	% Get time.
		
		% Noise definition: 
		n_w = (4.5 * pi / (180 * 60)) * ones(3, 1) / 1;	% Angular vel. as stated by the manufacturer.
		n_bw = n_w / 100;	% Bias noise assumed to be much smaller than the angular vel noise.
		
%%%% Propagation:
		% State propagation.
        %% TODO: Calculate the bias corrected angular velocity.
		w = w_m - b_w;
        %% TODO: Implement the zero order quat. integrator:
%         PHI = [0,     w(3) , -w(2), w(1);
%                -w(3), 0    , w(1) , w(2);
%                w(2) , -w(1), 0    , w(3);
%                -w(1), -w(2), -w(3), 0    ];
% 		q_WI = (eye(4)+dt/2*PHI)*q_WI;	
        q_WI = (expm(1 / 2 * quatskew([0; w_m_old + w] / 2) * dt) +... 
				1 / 48 * (quatskew([0; w]) * quatskew([0; w_m_old]) ...
                -quatskew([0; w_m_old]) * quatskew([0; w])) * (dt ^ 2)) * q_WI;	
			
		% Block parts of Fd and Qd:
		w_sk = skew(w);
		E = eye(3) - dt * w_sk + dt ^2 / 2 * w_sk ^ 2;
		F = -dt * eye(3) + dt ^ 2 / 2 * w_sk - dt ^ 3 / 6 * (w_sk ^ 2);

		% Discrete error state propagation Matrix Fd.
		Fd = [E F; zeros(3) eye(3)];

		% Discrete error state covariance matrix Qd:
		Qd = zeros(6);
		Qd(4:6, 4:6) = dt * diag(n_bw .^ 2);

		% Error state, state transition matrix:
        %% TODO: Predict the error state, state transition matrix:
		P = Fd*P*Fd'+Qd;
		
		w_m_old = w;
		idx_gyr = idx_gyr + 1;
	end

%%%% Update:
	if t > t_q_CW_m(idx_cam)	% Do we have a cam measurement?
        
		C_IC = quat2rot(q_IC);	% Convert to rotation matrix.
		
		% Measurements:
		z_q_CW = q_CW_m(idx_cam,:)';
        est_q_CW = invertquat(mulquat(q_WI,q_IC));
		
		% Measurement noise: Attitude measurement noise (0.01 rad = 0.5 degrees)
		n_q = [0.01 0.01 0.01]';	
		
		%% TODO: Implement the measurement noise matrix R:
		R = diag(n_q .^2);
		
		%% TODO: Implement the linearized measurement matrix H:
		H = [C_IC, zeros(3)];

		%% TODO: Implement the residual: estimated cam attitude x measured cam attitude^-1
		r_q = mulquat(est_q_CW,invertquat(z_q_CW));
		r = (r_q(2:4) / r_q(1)) * 2; % Because r_q = [1 1/2*dtheta]'

		% Update:
		%% TODO: Implement the residual covariance and Kalman gain calculation.
		S = H*P*H'+R;
		K = P*H'*S^-1;
        %% TODO: Implement the correction vector.
		dx = K*r;
	
		% Error state vector:
		dtheta = dx(1:3, 1);
		db_w =	dx(4:6, 1);

		if norm(r) < 0.1  % _very_ simple "check" if the attitude reading is ok.
		    %% TODO: Update the attitude.
            if(norm(dtheta / 2) < 1)
                q = [sqrt(1-norm(dtheta/2)^2);dtheta/2];
            else
                q = 1/sqrt(1+norm(dtheta/2)^2)*[1;dtheta/2];
            end
            q_WI = mulquat(q_WI, q);
            %% TODO: Update the bias.
			b_w = b_w + db_w;
	    	%% TODO: Implement the covariance update using Joseph form:
     		P = (eye(6)-K*H)*P*(eye(6)-K*H)' + K*R*K';
		
		    % Store some debug info for plotting.
		    debug(idxdebug, :) = [t_q_CW_m(idx_cam) z_q_CW' ...
                (invertquat(mulquat(q_WI, q_IC)))' r' b_w'];
		    idxdebug = idxdebug + 1;
        end
		idx_cam = idx_cam + 1;
	end
end


%% Plotting
plot_width = 1024;
plot_height = 768;
hfig =figure('position', [0, 0, plot_width, plot_height]); 

title('Attitude estimation')
subplot(3, 1, 1); 
plot(debug(:, 1), debug(:, 6:9),'LineWidth', 1); hold on;
plot(t_q_WI_gt(groundtruth_start:groundtruth_end), ...
    q_WI_gt(groundtruth_start:groundtruth_end, :), '--', 'LineWidth', 1); 
plot(debug(:, 1), debug(:, 2:5), '.', 'LineWidth', 2); hold on; 
hold off;
leg = legend('$\hat{q}_{c_w}$', '$\hat{q}_{c_x}$', '$\hat{q}_{c_y}$', ...
    '$\hat{q}_{c_z}$', '$q_w$', '$q_x$', ...
    '$q_y$', '$q_z$', '$z_w$', '$z_x$', '$z_y$', '$z_z$');
set(leg,'Interpreter','Latex');
rect = [0.88, 0.56, .1, .4];
set(leg, 'Position', rect)

xlabel('seconds')
ylabel('Orientation')
subplot(3, 1, 2); plot(debug(:, 1),debug(:, 10:12),'LineWidth', 1);
xlabel('seconds')
ylabel('Orientation Error [rad]')
leg = legend('roll', 'pitch', 'yaw');
rect = [0.88, 0.37, .1, .12];
set(leg, 'Position', rect)

set(leg,'Interpreter','Latex');
subplot(3, 1, 3); plot(debug(:, 1), debug(:, 13:15),'LineWidth', 1);
xlabel('seconds')
ylabel('Gyroscope bias [rad/sec]')
leg = legend('b_x', 'b_y', 'b_z');
rect = [0.88, 0.17, .1, .15];
set(leg, 'Position', rect)

set(subplot(3, 1, 1), 'Position', [0.05, 0.69, 0.8, 0.27])
set(subplot(3, 1, 2), 'Position', [0.05, 0.37, 0.8, 0.27])
set(subplot(3, 1, 3), 'Position', [0.05, 0.05, 0.8, 0.27])

display('finished');
