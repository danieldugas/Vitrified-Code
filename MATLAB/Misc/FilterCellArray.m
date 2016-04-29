
[nrows,ncols]= size(ArtistsFixed);

filename = 'Artists.dat';
fid = fopen(filename, 'w');

for col = 1:ncols
    fprintf(fid, '%s,\n', ArtistsFixed{1,col});
end

fclose(fid);