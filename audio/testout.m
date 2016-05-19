cd audio;
au2 = dlmread('out.pcm');
playaudio(au2, 44100);
cd ..;
