function [] = printvec(A, filename)
    fileID = fopen(filename, 'w');

    fprintf(fileID, [repmat('%f ', 1, size(A, 1)) '\n'], A);

    fclose(fileID);
end
