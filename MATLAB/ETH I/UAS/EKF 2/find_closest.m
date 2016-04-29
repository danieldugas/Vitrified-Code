function idx = find_closest(vec, val)

if(~isvector(vec))
    error('only for vectors');
end

[~, idx] = min(abs(vec-val));