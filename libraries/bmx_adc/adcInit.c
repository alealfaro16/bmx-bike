//Configures an ADC input
#include <stdint.h>
#include <stdbool.h>
#include <math.h>
#include "inc/hw_memmap.h"
#include "inc/hw_ints.h"
#include "driverlib/interrupt.h"
#include "driverlib/debug.h"
#include "driverlib/sysctl.h"
#include "driverlib/pin_map.h"
#include "driverlib/adc.h"
#include "driverlib/gpio.h"

uint32_t pui32ADC0Value[4];

struct sensors
{
    int POT;
    int LDR;
    int IR;
};

void ConfigureADC(void)
{
    // Two identical ADC modules, shares 12 input channel
    // Which means we get 12 ADC inputs
    // The module ADC0 peripheral must be enabled for use.
    SysCtlPeripheralEnable(SYSCTL_PERIPH_ADC0);
    while(!SysCtlPeripheralReady(SYSCTL_PERIPH_ADC0))
    {
    }

    // Use ADC 0, 1, and 2
    // Which correspond to PE3 PE2 PE1
    // ADC 3 correspond to PE0, if we want it in the future
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOE);
    while(!SysCtlPeripheralReady(SYSCTL_PERIPH_GPIOE))
    {
    }
    GPIOPinTypeADC(GPIO_PORTE_BASE, GPIO_PIN_3);
    GPIOPinTypeADC(GPIO_PORTE_BASE, GPIO_PIN_2);
    GPIOPinTypeADC(GPIO_PORTE_BASE, GPIO_PIN_1);

    // This is unique to Tiva
    // information collected from ADC are placed in a FIFO
    // the size of FIFO us determined by the "sequence"
    // Sequence 3 2 1 0
    // Sequence 3 has a FIFO size of 1, only 1 input can be collected
    // Sequence 2 and 1 has 4, and Sequence 0 has 8
    // Here I set the sequence to be 1 since we only have 3 inputs for now
    // The value will be placed into pui32ADC0Value
    // 0 means priority of the sequence, 0 is highest and 3 is the lowest
    ADCSequenceConfigure(ADC0_BASE, 1, ADC_TRIGGER_PROCESSOR, 0);

    // Configure which output use which FIFO slot
    // The last one must has ADC_CTL_IE and ADC_CTL_END
    // to set the interrupt flag when sampling is completed and to show the end of the conversion

    ADCSequenceStepConfigure(ADC0_BASE, 1, 0, ADC_CTL_CH0);
    ADCSequenceStepConfigure(ADC0_BASE, 1, 1, ADC_CTL_CH1);
    ADCSequenceStepConfigure(ADC0_BASE, 1, 2, ADC_CTL_CH2 | ADC_CTL_IE |
                             ADC_CTL_END);


    // Since sample sequence 1 is now configured, it must be enabled.
    //
    ADCSequenceEnable(ADC0_BASE, 1);
    // Clear the interrupt status flag.  This is done to make sure the
    // interrupt flag is cleared before we sample.
    //
    ADCIntClear(ADC0_BASE, 1);
}

struct sensors getADCValue(void)
{
    struct sensors s;
    // Trigger the ADC conversion.
    //
    ADCProcessorTrigger(ADC0_BASE, 1);
    // Wait for conversion to be completed.
    //
    while(!ADCIntStatus(ADC0_BASE, 1, false))
    {
    }
    // Clear the ADC interrupt flag.
    //
    ADCIntClear(ADC0_BASE, 1);
    // Read ADC Value.
    // 1 represents the sequence here
    ADCSequenceDataGet(ADC0_BASE, 1, pui32ADC0Value);
    s.POT = (pui32ADC0Value[0]*1.52587)+6250;
    s.LDR = pui32ADC0Value[1];
    s.IR = 27.726*pow(((double)pui32ADC0Value[2])/1241.21,-1.2045);

    return s;
}

















