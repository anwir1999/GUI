
uint32_t read32(uint8_t *b){
    uint32_t *r = (uint32_t*)&b[0];
    return *r;
}

void write32(uint8_t *b, uint32_t d){
    uint32_t *w = (uint32_t*)&b[0];
    *w = d;
}

uint16_t read16(uint8_t *b){
    uint16_t *r = (uint16_t*)&b[0];
    return *r;
}

void write16(uint8_t *b, uint16_t d){
    uint16_t *w = (uint16_t*)&b[0];
    *w = d;
}

int CharToInt(char d){
    int i = (int)d;
    if(i > 127)i -= (int)256;
    return i;
}

int CharToInt(uint8_t d){
    return CharToInt((char)d);
}


int strcpy_F(char *ret, const char *str){
    int i = 0;
    char c;
    while(1){
        c = (char)pgm_read_byte(&str[i]);
        if(!c)break;
        ret[i] = c;
        i++;
    }
    ret[i] = 0;
    return i;
}

int strcpy_R(char *ret, char *str){
    int i = 0;
    char c;
    while(1){
        c = str[i];
        if(!c)break;
        ret[i] = c;
        i++;
    }
    ret[i] = 0;
    return i;
}

int strcpy_len(char *dst, char *src, int len){
    int i = 0;
    while(src[i] && i < len){
        dst[i] = src[i];
        i++;
    }
    dst[i] = 0;
    return i;
}
