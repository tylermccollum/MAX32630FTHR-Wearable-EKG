#include "mbed.h"
#include "max32630fthr.h"
#include "cmsis.h"          // register base address definitions
#include "pwrseq_regs.h"    // power sequence register definitions
#include "rtc_regs.h"       //RTC register definitions
#include "SDFileSystem.h"



MAX32630FTHR pegasus(MAX32630FTHR::VIO_3V3);

DigitalOut rLed(LED1);
DigitalOut gLed(LED2);
DigitalOut bLed(LED3);

DigitalOut CTS(P0_2);


Serial dap(P2_1, P2_0);
Serial hci(P0_0,P0_1); //BT Tx and Rx (For mcu)

SDFileSystem sd(P0_5, P0_6, P0_4, P0_7, "sd");  // mosi, miso, sclk, cs

int val;
int x = 0;
int y = 0;
int z = 0;


void reg_cfg(){  //Configure registers

    dap.printf("Start\r\n");
    
    if ((MXC_PWRSEQ->reg0 |= MXC_F_PWRSEQ_REG0_PWR_RTCEN_RUN) != MXC_PWRSEQ->reg0){ //If the register isn't set, set it
        MXC_PWRSEQ->reg0 |= MXC_F_PWRSEQ_REG0_PWR_RTCEN_RUN;       //Enable 32k RTC
        x = 1;
        }
    dap.printf("x = %d\r\n", x);
    //dap.printf("RTC_En = %d\r\n\r\n", RTC_En);
    Thread::wait(100);
    
    if ((MXC_PWRSEQ->reg4 |= MXC_F_PWRSEQ_REG4_PWR_PSEQ_32K_EN) != MXC_PWRSEQ->reg4){
        MXC_PWRSEQ->reg4 |= MXC_F_PWRSEQ_REG4_PWR_PSEQ_32K_EN;    //Output RTC to P1_7
        y = 1;
        }
    dap.printf("y = %d\r\n",y);
    //dap.printf("ClkOut_En = %d\r\n\r\n", ClkOut_En);
    Thread::wait(100);
    
    //if ((MXC_UART0->ctrl |= MXC_F_UART_CTRL_CTS_EN) != MXC_UART0->ctrl){
        MXC_UART0->ctrl |= MXC_F_UART_CTRL_CTS_EN; //Enable Hardware Output flow control
        MXC_UART0->ctrl |= MXC_F_UART_CTRL_CTS_POLARITY;
        MXC_UART0->ctrl ^= MXC_F_UART_CTRL_RTS_EN; //Enable Hardware Input flow control (not enabled)
        MXC_UART0->ctrl ^= MXC_F_UART_CTRL_RTS_POLARITY;
        MXC_IOMAN->uart0_req ^= MXC_F_IOMAN_UART0_REQ_IO_MAP; //Properly map RX & TX
        MXC_IOMAN->uart0_req |= MXC_F_IOMAN_UART0_REQ_CTS_MAP; //Properly map CTS
        MXC_IOMAN->uart0_req |= MXC_F_IOMAN_UART0_REQ_RTS_MAP; //Properly map RTS
        z = 1;
       // }
    
    if((x == 1) | (y == 1)){
        x = 2;
        y = 2;
        z = 2;
        dap.printf("Z = %d", z);
        NVIC_SystemReset();
        }
        
        dap.printf("Z = %d", z);
        dap.printf("Made it through cfg\r\n");  
               
}

 
void ServicePack(){

    dap.printf("In service pack function\r\n");

    unsigned char buff[4];
    unsigned char buff2[1];
    int num = 0;

    FILE *packet;
    packet = fopen("/sd/packet.txt", "r");

    while(num < 9176 ){     //55054
        for(int i = 0; i<3;i++){
            buff[i] = getc(packet);
            }
            buff2 = buff[0]+buff[1]+buff[2]+buff[3];
            dap.putc(buff2);
            num++;
        }
    fclose(packet);
gLed = 0;

} 




int main()
{
    dap.baud(115200);
    dap.printf("Literally just starting up.\r\n");
    
    reg_cfg();
    
    //Config Interrupts
    //RTS.rise(&flowControl);
    //
    
    rLed = 1;
    bLed = 1;
    gLed = 1;
    CTS = 1;
    
    dap.printf("Starting rest\r\n");
    
    //val = RTS.read();
    //dap.printf("RTS = %d\r\n\r\n", val);
    
    
    /*
    for(int i = 0; i < 10; i++)
    {
        rLed = 1;
        Thread::wait(500);
        rLed = 0;
        val = RTS.read();
        dap.printf("RTS = %i\r\n", val);
        Thread::wait(500); //Something to do/see
        }
        
        dap.printf("\r\n");
        rLed = 1;
    */ //Test to see RTS value. RTS is low on proper initialization.
        
    //HCI Serial setup
    hci.baud(115200);
    //hci.format(8,Serial::None,1);         //Explicitly define 8n1 serial
    //hci.set_flow_control(Serial::RTSCTS,P0_2,P0_3);   //Enable RTS,CTS flow control  <--- Flow control doesn't work: causes break
    //^^ I did it mysyelf. See reg_cfg. RTS control only.
    //End setup
    
    bLed = 0;
    Thread::wait(250);
    bLed = 1;
    
    //Test: Output Service Pack
    
    
    dap.printf("Sending Service Pack...\r\n");
    
    ServicePack();
    
    
    dap.printf("Service Pack uploaded\r\n");
    
    //val = RTS.read();
    //dap.printf("RTS = %d", val);
    
    
    /* Test: Transparent Mode
     * This will take data from the daplink and output
     * it to the BT module and vice versa. Should allow
     * use with TI HCI Tool.
     */
    
    dap.printf("Begin Transparent Mode\r\n");
    
    CTS = 0;
    
    while(1) {
        
        //If terminal data available send through hci
        if(dap.readable()) {
            hci.putc(dap.getc());
        }
        //If dap gets data send to terminal
        if(hci.readable()) {
            dap.putc(hci.getc());
        }
    }
    
    
    
    
    
    /* Not Yet. Need service pack.
    //Write
    hci.putc(0x01); //Send command
    hci.putc(0x09);//Command LSB, Read_BD_ADDR
    hci.putc(0x10);
    hci.putc(0x00);//Null parameter length
    
    
    
    //Read
    CTS = 0;
    for(int n = 0; n < 32; n++){ //if there's data
        buff[n]=hci.getc();
        }
    
    
        
    for(int i=0; i< sizeof(buff); i++){
        dap.printf("buff[%i] = %c", i, buff[i]);
        }
    */
} 
