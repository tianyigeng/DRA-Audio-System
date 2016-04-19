cd audio;

if nargin ~= 2
    printf("Invalid arguments!\n");
    exit(-1);
end

arg_list = argv();
wav_name = arg_list{1};
pcm_name = arg_list{2};

au = wavread(wav_name);
printvec(au, pcm_name);

cd ..;
