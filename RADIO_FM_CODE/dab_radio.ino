

void dab_radio_play(uint8_t channel)
{
  NAU8822_Input(0);
  WM8804_Input(0);
  SI4684_DAB_TuneChannel(channel);
}

void dab_radio_stop(void)
{
  SI4684_End();
}
