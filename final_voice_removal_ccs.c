//iir_intr.c
//
// IIR filter implemented using second order sections
// integer coefficients read from file
//

#include "dsk6713.h"
#include "dsk6713_aic23.h"

#include "bsint.cof" // coefficient file for low pass and high pas filter

int w[NUM_SECTIONS][2] = {0}; // NUM_SECTIONS = 14 (FOR BOTH FILTER)
int y[HI_SECTIONS][2] = {0};
DSK6713_AIC23_Config config = { \
    0x0017,  /* 0 DSK6713_AIC23_LEFTINVOL  Left line input channel volume */ \
    0x0017,  /* 1 DSK6713_AIC23_RIGHTINVOL Right line input channel volume */\
    0x00d8,  /* 2 DSK6713_AIC23_LEFTHPVOL  Left channel headphone volume */  \
    0x00d8,  /* 3 DSK6713_AIC23_RIGHTHPVOL Right channel headphone volume */ \
    0x0011,  /* 4 DSK6713_AIC23_ANAPATH    Analog audio path control */      \
    0x0000,  /* 5 DSK6713_AIC23_DIGPATH    Digital audio path control */     \
    0x0000,  /* 6 DSK6713_AIC23_POWERDOWN  Power down control */             \
    0x0043,  /* 7 DSK6713_AIC23_DIGIF      Digital audio interface format */ \
    0x0081,  /* 8 DSK6713_AIC23_SAMPLERATE Sample rate control */            \
    0x0001   /* 9 DSK6713_AIC23_DIGACT     Digital interface activation */   \
};
static short in_buffer[100];

void main()
{
	DSK6713_AIC23_CodecHandle hCodec;
  int section;   // index for section number
  Uint32 input_L,input_R;     // input to each section
  Uint32 wn,yn_LP_R,yn_LP_L,yn_HP_R,yn_HP_L,out_left_HF,out_right_HF,out_left,out_right;
  /* Initialize the board support library, must be called first */
     DSK6713_init();

     /* Start the codec */
     hCodec = DSK6713_AIC23_openCodec(0, &config);

     DSK6713_AIC23_setFreq(hCodec, 6);
      while(1)
              {	/* Read a sample to the left channel */
      			while (!DSK6713_AIC23_read(hCodec, &input_L));

      			/* Read a sample to the right channel */
      			while (!DSK6713_AIC23_read(hCodec, &input_R));
      			// LOW PASS FILTER ON LEFT CHANNEL
      			  for (section=0 ; section< NUM_SECTIONS ; section++)
      			  {
      			    wn = input_L - ((a[section][1]*w[section][0])>>15) - ((a[section][2]*w[section][1])>>15);
      			    yn_LP_L = ((b[section][0]*wn)>>15) + ((b[section][1]*w[section][0])>>15) + ((b[section][2]*w[section][1])>>15);
      			    w[section][1] = w[section][0];
      			    w[section][0] = wn;
      			    input_L = yn_LP_L;
      			  }
      			// LOW PASS FILTER ON RIGHT CHANNEL
      			  for (section=0; section<NUM_SECTIONS ; section++)
      			  {
      			    wn = input_R - ((a[section][1]*w[section][0])>>15) - ((a[section][2]*w[section][1])>>15);
      			    yn_LP_R = ((b[section][0]*wn)>>15) + ((b[section][1]*w[section][0])>>15) + ((b[section][2]*w[section][1])>>15);
      			    w[section][1] = w[section][0];
      			    w[section][0] = wn;
      			    input_R = yn_LP_R;
      			  }

      			  // HIGH PASS FILTER ON LEFT CHANNEL
      			  for (section=0 ; section< HI_SECTIONS ; section++)
      			  {
      			    wn = input_L - ((a1[section][1]*y[section][0])>>15) - ((a1[section][2]*y[section][1])>>15);
      			    yn_HP_L = ((b1[section][0]*wn)>>15) + ((b1[section][1]*y[section][0])>>15) + ((b1[section][2]*y[section][1])>>15);
      			    y[section][1] = y[section][0];
      			    y[section][0] = wn;
      			    input_L = yn_HP_L;
      			  }
      			// HIGH PASS FILTER ON RIGHT CHANNEL
      			  for (section=0 ; section< HI_SECTIONS ; section++)
      			  {
      			    wn = input_R - ((a1[section][1]*y[section][0])>>15) - ((a1[section][2]*y[section][1])>>15);
      			    yn_HP_R = ((b1[section][0]*wn)>>15) + ((b1[section][1]*y[section][0])>>15) + ((b1[section][2]*y[section][1])>>15);
      			    y[section][1] = y[section][0];
      			    y[section][0] = wn;
      			    input_R = yn_HP_R;
      			  }

      			  out_left_HF = yn_HP_L - yn_HP_R ; //subtracting the common frequencies in left and right channel which majorly consist of vocal voice
      			  out_right_HF = yn_HP_R - yn_HP_L ; //subtracting the common frequencies in left and right channel which majorly consist of vocal voice

      			  out_left = (out_left_HF + yn_LP_L)*10; //adding back the bass frequency and amplifying
      			  out_right = (out_right_HF + yn_LP_R)*10; //adding back the bass frequency and amplifying



      			/* Send a sample to the left channel */
                  while (!DSK6713_AIC23_write(hCodec,out_left));

                  /* Send a sample to the right channel */
                  while (!DSK6713_AIC23_write(hCodec,out_right));
              }
      DSK6713_AIC23_closeCodec(hCodec);



}
