#include <termio.h>
#include <mc9s12c32.h>     /* derivative information */
#include <s12sci.h>

char TERMIO_GetChar(void) {
  /* receives character from the terminal channel */
    while (!(SCISR1 & _S12_RDRF))
    {}; /* wait for input */
    return SCIDRL;
}

void TERMIO_PutChar(char ch) {
  /* sends a character to the terminal channel */
    while (!(SCISR1 & _S12_TDRE))
    {};  /* wait for output buffer empty */
    SCIDRL = ch;
}

void TERMIO_Init(void) {
  /* initializes the communication channel */
/* set baud rate to 115.2 kbaud and turn on Rx and Tx */

        SCIBD = 13;
        SCICR2 = (_S12_TE | _S12_RE);
}

int kbhit(void) {
  /* checks for a character from the terminal channel */
    if (SCISR1 & _S12_RDRF)
            return 1;
    else
            return 0;

}


