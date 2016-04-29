function [ quat_out ] = invertquat( quat_in )
%INVERTQUAT Inverts the quaternion given in Hamilton notation.
quat_out = [quat_in(1); -quat_in(2:4)];
end

