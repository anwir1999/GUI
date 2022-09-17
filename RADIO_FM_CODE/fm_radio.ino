

void fm_radio_play(uint16_t channel)
{
  NAU8822_Input(0);
  WM8804_Input(0);
  SI4684_FM_TuneFrequency(channel);
}

void fm_radio_stop(void)
{
  SI4684_End();
}
