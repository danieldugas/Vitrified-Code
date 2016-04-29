for w = 0:0.1:1
    for x = 0:0.1:1
        for y = 0:0.1:1
            for z = 0:0.1:1
                q = [w x y z]';
                if (norm(q) == 0)
                   continue; 
                end
                q = q / norm(q);
                rot = quat2rot(q);
                q_from_rot = rot2quat(rot);
                if norm(q - q_from_rot) > 1e-15
                   error('Recovered quaternion not close enough to initial quaternion: [%.5f, %.5f, %.5f, %.5f] where initial: [%.5f, %.5f, %.5f, %.5f]', ...
                       q(1), q(2), q(3), q(4), q_from_rot(1), q_from_rot(2), q_from_rot(3), q_from_rot(4)); 
                end
            end
        end
    end
end