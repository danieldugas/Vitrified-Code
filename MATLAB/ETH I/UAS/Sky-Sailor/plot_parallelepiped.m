%===============================================================
%=== Plot Parallelepiped Version 1.00
%===
%=== This function plot, using patches, a 3D representation of a
%=== parallelepiped  with its axis aligned with x,y,z axis 
%=== (sufficient for the first application, could be extended...)
%=== 
%=== Input:         cog     center of gravity of cylinder
%===                lx      length of the cylinder
%===                ly      width of the cylinder
%===                lz      height of the cylinder
%===                color   color array (ex: [1,0,0] for red)
%===                
%=== Output:        none
%=== Graphical Out: 3D representation of the parallelepiped in 
%===                the current active figure window
%===
%=== More info:     Search "patch" and "3-D" in Matlab-Help
%===
%=== Author: André Noth
%===         andre.noth@epfl.ch
%===         http://aero.epfl.ch
%===
%===============================================================

function out = plot_parallelepiped(cog,lx,ly,lz,color)

vertex_list = [cog(1)-lx/2, cog(2)-ly/2, cog(3)-lz/2; ...   % List of point coordinates
               cog(1)+lx/2, cog(2)-ly/2, cog(3)-lz/2; ...
               cog(1)+lx/2, cog(2)+ly/2, cog(3)-lz/2; ...
               cog(1)-lx/2, cog(2)+ly/2, cog(3)-lz/2; ...
               cog(1)-lx/2, cog(2)-ly/2, cog(3)+lz/2; ...
               cog(1)+lx/2, cog(2)-ly/2, cog(3)+lz/2; ...
               cog(1)+lx/2, cog(2)+ly/2, cog(3)+lz/2; ...
               cog(1)-lx/2, cog(2)+ly/2, cog(3)+lz/2];
          

vertex_connection = [1,2,6,5; ...                           % List of interconnection between points
                     2,3,7,6; ...
                     3,4,8,7; ...
                     4,1,5,8; ...
                     1,2,3,4; ...
                     5,6,7,8];                 
                 
out = patch('Vertices',vertex_list, ...                           % List of point coordinates
      'Faces',vertex_connection, ...                        % List of interconnection between points
      'FaceColor',color, ...                                % Color of face
      'FaceAlpha',1);                                      % Transparency set to 0.2 (high)

end
               