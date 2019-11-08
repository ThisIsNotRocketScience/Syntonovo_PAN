
#include "midi.h"
#include "peripherals.h"
#include "midiparser.h"

#define WEAK __attribute__ ((weak))
extern "C" void DoMidiCommand(midicmd_t cmd);

#define FIFO_SIZE (256)
volatile static int input_fifo_readpos = 0;
volatile static int input_fifo_writepos = 0;
volatile static uint8_t input_fifo[FIFO_SIZE];

static int input_fifo_free() {
	return (input_fifo_readpos - input_fifo_writepos - 1) & (FIFO_SIZE - 1);
}

static int input_fifo_used() {
	return (input_fifo_writepos - input_fifo_readpos) & (FIFO_SIZE - 1);
}

static uint8_t input_fifo_read() {
	int readpos = input_fifo_readpos;
	uint8_t b = input_fifo[readpos];
	input_fifo_readpos = (readpos + 1) & (FIFO_SIZE - 1);
	return b;
}

static void input_fifo_write(uint8_t b) {
	int writepos = input_fifo_writepos;
	input_fifo[writepos] = b;
	input_fifo_writepos = (writepos + 1) & (FIFO_SIZE - 1);
}

midiin_state_t midi_state;

void midi_clear()
{
	midiin_reset(&midi_state);
}

void midi_init()
{
	input_fifo_readpos = 0;
	input_fifo_writepos = 0;
	midiin_reset(&midi_state);

	USART_MIDI_PERIPHERAL->FIFOCFG &= ~(USART_FIFOCFG_EMPTYTX_MASK | USART_FIFOCFG_ENABLETX_MASK);
	USART_MIDI_PERIPHERAL->FIFOCFG |= USART_FIFOCFG_EMPTYRX_MASK | USART_FIFOCFG_ENABLERX_MASK;
    /* setup trigger level */
    USART_MIDI_PERIPHERAL->FIFOTRIG &= ~(USART_FIFOTRIG_RXLVL_MASK | USART_FIFOTRIG_TXLVL_MASK);
    USART_MIDI_PERIPHERAL->FIFOTRIG |= USART_FIFOTRIG_RXLVL(0);
    USART_MIDI_PERIPHERAL->FIFOTRIG |= USART_FIFOTRIG_TXLVL(0);

    /* enable trigger interrupt */
    USART_MIDI_PERIPHERAL->FIFOTRIG |= USART_FIFOTRIG_RXLVLENA_MASK;

	USART_MIDI_PERIPHERAL->FIFOINTENSET |= /*USART_FIFOINTENSET_RXLVL_MASK |*/ USART_FIFOINTENSET_RXERR_MASK;

    NVIC_SetPriority(USART_MIDI_FLEXCOMM_IRQN, 2);
	EnableIRQ(USART_MIDI_FLEXCOMM_IRQN);
}

void midi_process()
{
    while (input_fifo_used()) {
    	midicmd_t cmd;
    	if (midiin_process(&cmd, input_fifo_read(), &midi_state)) {
    		cmd.header = 0x10;
    		__disable_irq();
        	DoMidiCommand(cmd);
        	__enable_irq();
    	}
    }
	USART_MIDI_PERIPHERAL->FIFOINTENSET |= USART_FIFOINTENSET_RXLVL_MASK;
}

/*WEAK*/ extern "C" void USART_MIDI_FLEXCOMM_IRQHANDLER(void)
{
    if (USART_MIDI_PERIPHERAL->FIFOSTAT & USART_FIFOSTAT_RXERR_MASK)
    {
        /* Clear rx error state. */
    	USART_MIDI_PERIPHERAL->FIFOSTAT |= USART_FIFOSTAT_RXERR_MASK;
        /* clear rxFIFO */
    	USART_MIDI_PERIPHERAL->FIFOCFG |= USART_FIFOCFG_EMPTYRX_MASK;
    	/* Clear tx interrupt */
    	//USART_1_PERIPHERAL->FIFOINTENCLR |= USART_FIFOINTENSET_TXLVL_MASK;
    	midi_clear();
    }

    int repeat = 1;
    while (repeat)
    {
    	repeat = 0;

        if (USART_MIDI_PERIPHERAL->FIFOSTAT & USART_FIFOSTAT_RXNOTEMPTY_MASK)
        {
        	repeat = 1;

            uint8_t b = USART_MIDI_PERIPHERAL->FIFORD & 0xFF;

			input_fifo_write(b);
            if (input_fifo_free() < 4) {
            	USART_MIDI_PERIPHERAL->FIFOINTENCLR |= USART_FIFOINTENSET_RXLVL_MASK;
            }
            if (b == 0xF8) {
            	SCTIMER_1_PERIPHERAL->CTRL |= 4; // halt
            	SCTIMER_1_PERIPHERAL->COUNT = 100;
            	SCTIMER_1_PERIPHERAL->CTRL &= ~4; // unhalt
            }
        }
/*
        if (sendEnabled)
		{
			if (fifo_tx_empty())
			{
				if (USART_DSP_PERIPHERAL->FIFOSTAT & USART_FIFOSTAT_TXEMPTY_MASK) {
				    USART_DSP_PERIPHERAL->FIFOINTENCLR = USART_FIFOINTENSET_TXLVL_MASK;
				}
			}
			else
			{
				if (USART_DSP_PERIPHERAL->FIFOSTAT & USART_FIFOSTAT_TXNOTFULL_MASK) {
					repeat = 1;

					volatile uint8_t b = fifo_tx_pop();
					USART_DSP_PERIPHERAL->FIFOWR = b;
				}
			}
        }
        */
    }
    /* Add for ARM errata 838869, affects Cortex-M4, Cortex-M4F Store immediate overlapping
      exception return operation might vector to incorrect interrupt */
#if defined __CORTEX_M && (__CORTEX_M == 4U)
    __DSB();
#endif
}

