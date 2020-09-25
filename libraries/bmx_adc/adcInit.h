#ifndef ADCINIT_H
#define ADCINIT_H


//Configures an ADC
extern uint32_t pui32ADC0Value[4];
struct sensors
{
    int POT;
    int LDR;
    int IR;
};
void ConfigureADC(void);
struct sensors getADCValue(void);
#endif /* ADCINIT_H */
