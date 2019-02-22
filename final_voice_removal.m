%% Removal of lead voice signal
% Authors
% Akhil Nori                   2016A3PS0106G
% Siddharth Bhardwaj           2016A3PS0129G
% Nitya Joshi                  2016A3PS0164G

%% Input and definitions
[input, Fs] = audioread('Pokemon.mp3');

% Here, 'input' is a matrix with two columns (Stereo).
% We use these as the 'left' and 'right' channels in the processing.

%input = input*1.5;      % Rudimentary amplification
fc = 200;               % approximate Bass cutoff frequency

% A low pass filter is used to separate the common bass frequency 
% components from the audio.

%% Filter Specifications
% Low Pass Filter
% Bass passband edge 175 Hz
% Bass stopband edge 225 Hz

 ld = LPcoeff;
 channel_left = filter(ld,input(:,1));
 channel_right = filter(ld,input(:,2));

% A high pass filter is used to obtain the higher frequency components
% which may include non-voice components and background singing as well.
% Only the lead singer's voice is undesirable in the output.

% High Pass Filter
% Bass passband edge 175 Hz
% Bass stopband edge 125 Hz

hd = HPcoeff;
channel_h_left = filter(hd,input(:,1));
channel_h_right = filter(hd,input(:,2));

%% Implementation of the voice removal algorithm.
% The bass components are retained in either channel, and the higher
% frequencies are compared with the other channel, to retain only unique
% components. Since the lead's singer's voice is common above the cutoff
% frequency, it gets cancelled in the subtraction.

ch_high_left = channel_h_left - channel_h_right;
ch_high_right = channel_h_right - channel_h_left;

ch_left = ch_high_left + channel_left;
ch_right = ch_high_right + channel_right;

% The output matrix also has two columns (Stereo).
karaoke = horzcat(ch_right,ch_left);

% Writing the output
audiowrite('karaoke_out.mp4', karaoke, Fs);

%% Plots of Time Samples

figure(1)
subplot(2,2,1)
plot(input(:,1));
title('Time Domain Plot of Input (L)')

subplot(2,2,2)
plot(karaoke(:,2), 'r');
title('Time Domain Plot of Input (R)')

subplot(2,2,3)
plot(karaoke(:,1));
title('Time Domain Plot of Output (L)')

subplot(2,2,4)
plot(karaoke(:,2), 'r');
title('Time Domain Plot of Output (R)')

%% Plots of Frequency Sprectrum
Y1 = fftshift(fft(input(:,1)));
N = length(Y1);
mod_f = (-N/2:N/2-1)*Fs/N;

figure(2)

subplot(2,2,1)
plot(mod_f,abs(Y1)/N);
xlim([100 500])
title('Frequency Spectrum of Input (L)');

Y2 = fftshift(fft(input(:,2)));
N = length(Y2);
mod_f = (-N/2:N/2-1)*Fs/N;
figure(2)

subplot(2,2,2)
plot(mod_f,abs(Y2)/N, 'r');
xlim([100 500])
title('Frequency Spectrum of Input (R)');

Karaoke = fftshift(fft(karaoke));
N = length(Karaoke);
mod_f = (-N/2:N/2-1)*Fs/N;

subplot(2,2,3)
plot(mod_f,abs(Karaoke(:,1))/N);
xlim([100 500])
title('Frequency Spectrum of Output (L)');

subplot(2,2,4)
plot(mod_f,abs(Karaoke(:,2))/N, 'r');
xlim([100 500])
title('Frequency Spectrum of Output (R)');

%% Filter Plots
% Low Pass Filter Response
freqz(ld)
%xlim([0 0.1])
title('Low Pass Filter Response')

% High Pass Filter Response
freqz(hd)
xlim([0 0.1])
title('High Pass Filter Response')


%% Filter Design Functions
function Hd = LPcoeff
%LPCOEFF Returns a discrete-time filter object.

% MATLAB Code
% Generated by MATLAB(R) 9.3 and DSP System Toolbox 9.5.
% Generated on: 26-Nov-2018 00:15:38

% Butterworth Lowpass filter designed using FDESIGN.LOWPASS.

% All frequency values are in Hz.
Fs = 44100;  % Sampling Frequency

Fpass = 175;         % Passband Frequency
Fstop = 225;         % Stopband Frequency
Apass = 3;           % Passband Ripple (dB)
Astop = 60;          % Stopband Attenuation (dB)
match = 'passband';  % Band to match exactly

% Construct an FDESIGN object and call its BUTTER method.
h  = fdesign.lowpass(Fpass, Fstop, Apass, Astop, Fs);
Hd = design(h, 'butter', 'MatchExactly', match);
end



function Hd = HPcoeff
%HPCOEFF Returns a discrete-time filter object.

% MATLAB Code
% Generated by MATLAB(R) 9.3 and DSP System Toolbox 9.5.
% Generated on: 25-Nov-2018 23:49:34

% Butterworth Highpass filter designed using FDESIGN.HIGHPASS.

% All frequency values are in Hz.
Fs = 44100;  % Sampling Frequency

Fstop = 125;         % Stopband Frequency
Fpass = 175;         % Passband Frequency
Astop = 60;          % Stopband Attenuation (dB)
Apass = 3;           % Passband Ripple (dB)
match = 'passband';  % Band to match exactly

% Construct an FDESIGN object and call its BUTTER method.
h  = fdesign.highpass(Fstop, Fpass, Astop, Apass, Fs);
Hd = design(h, 'butter', 'MatchExactly', match);
end
