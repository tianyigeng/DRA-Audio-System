function [ ] = playaudio (x, sampling_rate)

    if nargin == 1
        sampling_rate = 8000
    end
    file = tmpnam ();
    file= [file, '.wav'];
    wavwrite(x, sampling_rate, file);
    ['play ' file ]
    system(['play ' file ]);
    system(['rm ' file]);
end
