// ADC with DMA
// http://www.atmel.com/Images/Atmel-42258-ASF-Manual-SAM-D21_AP-Note_AT07627.pdf pg 73

#include <Arduino.h>
#include "adc-dma.h"

typedef struct {
    uint16_t btctrl;
    uint16_t btcnt;
    uint32_t srcaddr;
    uint32_t dstaddr;
    uint32_t descaddr;
} dmacdescriptor;

volatile dmacdescriptor wrb[12] __attribute__ ((aligned (16)));
dmacdescriptor descriptor_section[12] __attribute__ ((aligned (16)));
dmacdescriptor descriptor __attribute__ ((aligned (16)));


static uint32_t chnl = 0;  // DMA channel
volatile uint32_t adc_done = 0;

fnPtr doneHandler = 0;

void setADCDoneHandler(fnPtr handler) {
  doneHandler = handler;
}


void DMAC_Handler() {
    // interrupts DMAC_CHINTENCLR_TERR DMAC_CHINTENCLR_TCMPL DMAC_CHINTENCLR_SUSP
    uint8_t active_channel;

    // disable irqs ?
    __disable_irq();
    active_channel = DMAC->INTPEND.reg & DMAC_INTPEND_ID_Msk; // get channel number
    DMAC->CHID.reg = DMAC_CHID_ID(active_channel);
    adc_done = DMAC->CHINTFLAG.reg;

    // Why setting this here?
    DMAC->CHINTFLAG.reg = DMAC_CHINTENCLR_TCMPL; // clear
    DMAC->CHINTFLAG.reg = DMAC_CHINTENCLR_TERR;
    DMAC->CHINTFLAG.reg = DMAC_CHINTENCLR_SUSP;
    __enable_irq();

    if (doneHandler) {
      (*doneHandler)();
    }
}


void dma_init() {
    // probably on by default
    PM->AHBMASK.reg |= PM_AHBMASK_DMAC ;
    PM->APBBMASK.reg |= PM_APBBMASK_DMAC ;
    NVIC_EnableIRQ( DMAC_IRQn ) ;

    DMAC->BASEADDR.reg = (uint32_t)descriptor_section;
    DMAC->WRBADDR.reg = (uint32_t)wrb;
    DMAC->CTRL.reg = DMAC_CTRL_DMAENABLE | DMAC_CTRL_LVLEN(0xf);
}

void adc_dma(void *rxdata, size_t dataLength) {

    DMAC->CHID.reg = DMAC_CHID_ID(chnl);
    DMAC->CHCTRLA.reg &= ~DMAC_CHCTRLA_ENABLE;
    DMAC->CHCTRLA.reg = DMAC_CHCTRLA_SWRST;
    DMAC->SWTRIGCTRL.reg &= (uint32_t)(~(1 << chnl));
    DMAC->CHCTRLB.reg = DMAC_CHCTRLB_LVL(0) | DMAC_CHCTRLB_TRIGSRC(ADC_DMAC_ID_RESRDY) | DMAC_CHCTRLB_TRIGACT_BEAT;
    DMAC->CHINTENSET.reg = DMAC_CHINTENSET_MASK ; // enable all 3 interrupts

    adc_done = 0;
    descriptor.descaddr = 0;
    descriptor.srcaddr = (uint32_t) &ADC->RESULT.reg;
    descriptor.btcnt =  dataLength;
    descriptor.dstaddr = (uint32_t)rxdata + dataLength;   // end address // Why end address?
    descriptor.btctrl =  DMAC_BTCTRL_BEATSIZE_BYTE | DMAC_BTCTRL_DSTINC | DMAC_BTCTRL_VALID;
    memcpy(&descriptor_section[chnl],&descriptor, sizeof(dmacdescriptor));

    // start channel
    DMAC->CHID.reg = DMAC_CHID_ID(chnl);
    DMAC->CHCTRLA.reg |= DMAC_CHCTRLA_ENABLE;
}

static __inline__ void ADCsync() __attribute__((always_inline, unused));
static void   ADCsync() {
  while (ADC->STATUS.bit.SYNCBUSY == 1); //Just wait till the ADC is free
}


void adc_init(const uint8_t ADC_PIN, const uint8_t samplingSpeed) {
  analogRead(ADC_PIN);  // do some pin init  pinPeripheral()

  // ADC is macro defined in hardware/tools/CMSIS/Device/ATMEL/samd21/include/samd21g18a.h
  // It points to data structure defined in hardware/tools/CMSIS/Device/ATMEL/samd21/include/component/adc.h:667

  ADC->CTRLA.bit.ENABLE = 0x00;             // Disable ADC
  ADCsync();
  //ADC->REFCTRL.bit.REFSEL = ADC_REFCTRL_REFSEL_INTVCC0_Val; //  2.2297 V Supply VDDANA
  //ADC->INPUTCTRL.bit.GAIN = ADC_INPUTCTRL_GAIN_1X_Val;      // Gain select as 1X
  ADC->INPUTCTRL.bit.GAIN = ADC_INPUTCTRL_GAIN_DIV2_Val;  // default
  ADC->REFCTRL.bit.REFSEL = ADC_REFCTRL_REFSEL_INTVCC1_Val;
  ADCsync();    //  ref 31.6.16
  ADC->INPUTCTRL.bit.MUXPOS = g_APinDescription[ADC_PIN].ulADCChannelNumber;
  ADC->INPUTCTRL.bit.INPUTSCAN = 0;
  ADC->INPUTCTRL.bit.INPUTOFFSET = 0;
  ADCsync();
  ADC->AVGCTRL.reg = 0x00;       //no averaging
  ADC->SAMPCTRL.reg = samplingSpeed; //sample length in 1/2 CLK_ADC cycles // Setting to zero does not work for 10bit resolution
  ADCsync();
  ADC->CTRLB.reg = ADC_CTRLB_PRESCALER_DIV16 | ADC_CTRLB_FREERUN | ADC_CTRLB_RESSEL_8BIT;

  // Result correction
  //ADC->CTRLB.bit.ADC_CTRLB_CORREN = 1;
  //ADC->OFFSETCORR.reg = 240;
  //ADC->GAINCORR.reg = 0b111111111111;

  ADCsync();
  ADC->CTRLA.bit.ENABLE = 0x01;
  ADCsync();
}
