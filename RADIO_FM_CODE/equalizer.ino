
void equalizer_set(void)
{
    NAU8822_3D_Control(3); //20% effect 3D, 3*6.67%
    NAU8822_Equalizer(0, 0, 12); //80Hz, 0db
    NAU8822_Equalizer(1, 1, 12); //300Hz, 0db
    NAU8822_Equalizer(2, 1, 11); //850kHz, +1db
    NAU8822_Equalizer(3, 1, 10); //2.4kHz, +2db
    NAU8822_Equalizer(4, 1, 12); //6.9kHz, 0db
}
