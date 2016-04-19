cd audio;

if nargin ~= 2
    printf("Invalid arguments!\n");
    exit(-1);
end

arg_list = argv();
file1 = arg_list{1};
file2 = arg_list{2};

au1 = dlmread(file1);
au2 = dlmread(file2);

len1 = size(au1, 1);
len2 = size(au2, 1);

% diff = au2(1:len1) - au1;

% plot(diff)

len = 2 ^ nextpow2(len1);

fft1 = fft(au1, len);
fft2 = fft(au2, len);

fs = 8000;
xax = (1:len) * fs / len;

figure;
plot(xax, abs(fft1), 'r');
hold on;
plot(xax, abs(fft2), 'b');
xlim([1 fs/2]);
xlabel('freq(Hz)');

pause;

figure;
diff = abs(fft1) - abs(fft2);
plot(xax, diff);
xlim([1 fs/2]);
xlabel('freq(Hz)');
ylim([0 1000]);

pause;


cd ..;
