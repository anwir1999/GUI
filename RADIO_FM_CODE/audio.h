#ifndef __AUDIO_H
#define __AUDIO_H
//
#define NAU8822_ADDRESS		0x34
#define WM8804_ADDRESS		0x74
#define AW9523_ADDRESS		0xB0
//I2S PIN
#define I2S_LRCLK		3
#define I2S_BCLK	  2
#define I2S_DOUT    4
//AW9523 PIN
#define FM_RST      9
#define OSC_EN      12
#define PWR_LED     10 //led nguồn
#define PWR_OFF     4  //bật/tắt nguồn
#define OPT_SLT     8
//NAU8822 REG
#define NAU8822_REG_RESET			            0x00
#define NAU8822_REG_POWER_MANAGEMENT_1		0x01
#define NAU8822_REG_POWER_MANAGEMENT_2		0x02
#define NAU8822_REG_POWER_MANAGEMENT_3		0x03
#define NAU8822_REG_AUDIO_INTERFACE		    0x04
#define NAU8822_REG_COMPANDING_CONTROL		0x05
#define NAU8822_REG_CLOCK_CONTROL_1			  0x06
#define NAU8822_REG_CLOCK_CONTROL_2		    0x07
#define NAU8822_REG_GPIO_CONTROL		      0x08
#define NAU8822_REG_JACK_DETECT_CONTROL_1	0x09
#define NAU8822_REG_DAC_CONTROL			      0x0A
#define NAU8822_REG_LEFT_DAC_DIGITAL_VOLUME	  0x0B
#define NAU8822_REG_RIGHT_DAC_DIGITAL_VOLUME	0x0C
#define NAU8822_REG_JACK_DETECT_CONTROL_2	    0x0D
#define NAU8822_REG_ADC_CONTROL			          0x0E
#define NAU8822_REG_LEFT_ADC_DIGITAL_VOLUME	  0x0F
#define NAU8822_REG_RIGHT_ADC_DIGITAL_VOLUME	0x10
#define NAU8822_REG_EQ1				        0x12
#define NAU8822_REG_EQ2				        0x13
#define NAU8822_REG_EQ3				        0x14
#define NAU8822_REG_EQ4				        0x15
#define NAU8822_REG_EQ5				        0x16
#define NAU8822_REG_DAC_LIMITER_1		  0x18
#define NAU8822_REG_DAC_LIMITER_2		  0x19
#define NAU8822_REG_NOTCH_FILTER_1		0x1B
#define NAU8822_REG_NOTCH_FILTER_2		0x1C
#define NAU8822_REG_NOTCH_FILTER_3		0x1D
#define NAU8822_REG_NOTCH_FILTER_4		0x1E
#define NAU8822_REG_ALC_CONTROL_1		  0x20
#define NAU8822_REG_ALC_CONTROL_2		  0x21
#define NAU8822_REG_ALC_CONTROL_3		  0x22
#define NAU8822_REG_NOISE_GATE			  0x23
#define NAU8822_REG_PLL_N			        0x24
#define NAU8822_REG_PLL_K1			      0x25
#define NAU8822_REG_PLL_K2			      0x26
#define NAU8822_REG_PLL_K3			      0x27
#define NAU8822_REG_3D_CONTROL			  0x29
#define NAU8822_REG_RIGHT_SPEAKER_CONTROL	    0x2B
#define NAU8822_REG_INPUT_CONTROL		          0x2C
#define NAU8822_REG_LEFT_INP_PGA_CONTROL	    0x2D
#define NAU8822_REG_RIGHT_INP_PGA_CONTROL	    0x2E
#define NAU8822_REG_LEFT_ADC_BOOST_CONTROL	  0x2F
#define NAU8822_REG_RIGHT_ADC_BOOST_CONTROL	  0x30
#define NAU8822_REG_OUTPUT_CONTROL		        0x31
#define NAU8822_REG_LEFT_MIXER_CONTROL		    0x32
#define NAU8822_REG_RIGHT_MIXER_CONTROL		    0x33
#define NAU8822_REG_LHP_VOLUME			          0x34
#define NAU8822_REG_RHP_VOLUME			          0x35
#define NAU8822_REG_LSPKOUT_VOLUME		        0x36
#define NAU8822_REG_RSPKOUT_VOLUME		        0x37
#define NAU8822_REG_AUX2_MIXER			          0x38
#define NAU8822_REG_AUX1_MIXER			          0x39
#define NAU8822_REG_POWER_MANAGEMENT_4		    0x3A
#define NAU8822_REG_LEFT_TIME_SLOT		        0x3B
#define NAU8822_REG_MISC			                0x3C
#define NAU8822_REG_RIGHT_TIME_SLOT		        0x3D
#define NAU8822_REG_DEVICE_REVISION		        0x3E
#define NAU8822_REG_DEVICE_ID			            0x3F
#define NAU8822_REG_DAC_DITHER			          0x41
#define NAU8822_REG_ALC_ENHANCE_1		          0x46
#define NAU8822_REG_ALC_ENHANCE_2		          0x47
#define NAU8822_REG_192KHZ_SAMPLING		        0x48
#define NAU8822_REG_MISC_CONTROL		          0x49
#define NAU8822_REG_INPUT_TIEOFF		          0x4A
#define NAU8822_REG_POWER_REDUCTION		        0x4B
#define NAU8822_REG_AGC_PEAK2PEAK		          0x4C
#define NAU8822_REG_AGC_PEAK_DETECT		        0x4D
#define NAU8822_REG_AUTOMUTE_CONTROL		      0x4E
#define NAU8822_REG_OUTPUT_TIEOFF		          0x4F
//#define NAU8822_REG_MAX_REGISTER		NAU8822_REG_OUTPUT_TIEOFF
//WM8804 REG
#define WM8804_RST_DEVID1		0x00
#define WM8804_DEVID2			  0x01
#define WM8804_DEVREV			  0x02
#define WM8804_PLL1				  0x03
#define WM8804_PLL2				  0x04
#define WM8804_PLL3				  0x05
#define WM8804_PLL4				  0x06
#define WM8804_PLL5				  0x07
#define WM8804_PLL6				  0x08
#define WM8804_SPDMODE			0x09
#define WM8804_INTMASK			0x0A
#define WM8804_INTSTAT			0x0B
#define WM8804_SPDSTAT			0x0C
#define WM8804_RXCHAN1			0x0D
#define WM8804_RXCHAN2			0x0E
#define WM8804_RXCHAN3			0x0F
#define WM8804_RXCHAN4			0x10
#define WM8804_RXCHAN5			0x11
#define WM8804_SPDTX1			  0x12
#define WM8804_SPDTX2			  0x13
#define WM8804_SPDTX3			  0x14
#define WM8804_SPDTX4			  0x15
#define WM8804_SPDTX5			  0x16
#define WM8804_GPO0				  0x17
#define WM8804_GPO1				  0x18
#define WM8804_GPO2				  0x1A
#define WM8804_AIFTX			  0x1B
#define WM8804_AIFRX			  0x1C
#define WM8804_SPDRX1			  0x1D
#define WM8804_PWRDN			  0x1E
//
/* EQ Freq Select
EQ0:
	0 =  80Hz 
	1 = 105Hz (default) 
	2 = 135Hz 
	3 = 175Hz 
EQ1:
	0 = 230Hz 
	1 = 300Hz (default) 
	2 = 385Hz 
	3 = 500Hz 
EQ2:
	0 = 650Hz 
	1 = 850Hz (default) 
	2 = 1.1kHz 
	3 = 1.4kHz 
EQ3:
	0 = 1.8kHz 
	1 = 2.4kHz (default) 
	2 = 3.2kHz 
	3 = 4.1kHz 
EQ4:
	0 = 5.3kHz 
	1 = 6.9kHz (default) 
	2 = 9.0kHz 
	3 = 11.7kHz 
*/

//USE API FUNCTION
uint16_t NAU8822_GetDeviceID();
void NAU8822_Begin(); //Init power, adc, dac, mixer - System init call 3
void NAU8822_Sleep(); //Go to Sleep
void NAU8822_Wakeup(); //Wakeup
void NAU8822_Input(uint8_t input); //Input Select: 0-RADIO ADC MIC, 1-PCM BLUETOOTH/FM_ONLINE
void NAU8822_3D_Control(uint8_t value); //3D Stereo Enhancement effect depth control, +6.67% per binary bit value, 4 bit 0 -> 15 (0x03 = 20% effect)
void NAU8822_Equalizer(uint8_t eq, uint8_t freq, uint8_t gain); //0-4 EQ, 2 bit freq select 0 -> 3, 5 bit gain control 0 -> 24 (+12dB -> -12dB)
void NAU8822_Volume(uint8_t vol); //=0 Mute, volume total L/R 1 -> 24 (24 level)
//OPTICAL
uint16_t WM8804_GetDeviceID();
void WM8804_Begin(); //System init call 2
void WM8804_Sleep(); //sleep
void WM8804_Wakeup(); //Wakeup, run mode call
void WM8804_Mute(uint8_t mute); //mute = 1, run = 0
void WM8804_Input(uint8_t input); //Input Select: 0-RADIO PCM MIC, 1-PCM BLUETOOTH/FM_ONLINE		(I2S_DAT)
//GPIO
void AW9523_Begin(); //System init call 1
void AW9523_Data(uint8_t pin, uint8_t val); //pin = 0-15, val = 0-1, set data to reg (no update)
void AW9523_Update(); //Update reg to ic
void AW9523_DigitalWrite(uint8_t pin, uint8_t val); //pin = 0-15, val = 0-1 (update)
void AW9523_PortWrite(uint8_t port, uint8_t val); //port = 0-1, val = port data 8bit mask (update)
//I2S
//void I2S_End(); //WM8804_Input call
//void I2S_Begin(); //WM8804_Input call
void I2S_Sleep(); //stop
void I2S_Wakeup(); //start
void I2S_SampleRate(uint32_t rate, uint32_t channel);  //Mono = 1, Stereo = 2
uint32_t I2S_Write(uint32_t *data, uint32_t len); //Write to DMA, len 64Byte or max 64*16 (start), check return written < len => DMA Buffer Full
//len: max write 64*16 (16 buffer dma fifo)
void I2S_Load(uint32_t *data, uint32_t sample); //Load Audio PCM to I2S DMA, task call polling

#endif
