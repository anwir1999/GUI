//BUTTON GPIO
#define BUTTON_ADC 35
#define ENCODER_A 37
#define ENCODER_B 38

//BUTTON VALUE
#define BUTADC_POWER   0
#define BUTADC_MENU    288
#define BUTADC_LEFT    715
#define BUTADC_RIGHT   1285
#define BUTADC_OK      2095
#define BUTADC_OFFSET  60
/********************************************* PWM BACKLIGHT ****************************************************/
static uint32_t PWM_Run = 0;

void PWM_Begin(){
    #if(DEBUG_SYSTEM==1)
    Serial.end(); //End TX
    #endif
    ledcSetup(0, 1000, 8);
    ledcAttachPin(1, 0);
    PWM_Run = 1;
}

void PWM_SetDuty(uint8_t duty){
    if(!PWM_Run)PWM_Begin();
    ledcWrite(0, duty);
}
/********************************************* BUTTON ****************************************************/
static uint32_t ENC_Save, ENC_Cnt, ENC_Last;

void ENC_Begin(){ //Encoder
    pinMode(BUTTON_ADC, INPUT); //BUT ADC
    pinMode(ENCODER_A, INPUT_PULLUP); //ENC A
    pinMode(ENCODER_B, INPUT_PULLUP); //ENC B
    ENC_Save = digitalRead(ENCODER_A);
    ENC_Cnt = 0;
    ENC_Last = 0;
}

void ENC_RotaryTask(){ //Rotary -> UP/DOWN
    uint32_t a, b;
    a = digitalRead(ENCODER_A);
    if(a != ENC_Save){
        b = digitalRead(ENCODER_B);
        if(b == a)ENC_Cnt++;
        else ENC_Cnt--;
        ENC_Save = a;
    }
}

uint32_t Get_Rotary(){
    uint32_t key = 0;
    if(ENC_Cnt < ENC_Last)key = KEY_DOWN;
    else if(ENC_Cnt > ENC_Last)key = KEY_UP;
    ENC_Last = ENC_Cnt;
    return key;
}

void KeyPress_Wait(uint32_t but){
    while(but){
        but = Get_KeyPress();
        delay(100);
    }
}

uint32_t Get_KeyPress(){ //200ms get sample
    int but;// = Get_Rotary();
        but = analogRead(BUTTON_ADC);
        if(but < (BUTADC_POWER + BUTADC_OFFSET))return KEY_POWER;
        else if(but > (BUTADC_MENU - BUTADC_OFFSET) && but < (BUTADC_MENU + BUTADC_OFFSET))return KEY_MENU;
        else if(but > (BUTADC_LEFT - BUTADC_OFFSET) && but < (BUTADC_LEFT + BUTADC_OFFSET))return KEY_LEFT;
        else if(but > (BUTADC_RIGHT - BUTADC_OFFSET) && but < (BUTADC_RIGHT + BUTADC_OFFSET))return KEY_RIGHT;
        else if(but > (BUTADC_OK - BUTADC_OFFSET) && but < (BUTADC_OK + BUTADC_OFFSET))return KEY_OK; //Rotary SW
        return 0; //No Key
        //return Get_Rotary();
}
