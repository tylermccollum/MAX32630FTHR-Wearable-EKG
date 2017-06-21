#include "mbed.h"
#include "max32630fthr.h"
#include "SDFileSystem.h"
#include "SPI.h"

MAX32630FTHR pegasus;



DigitalOut rLED(P2_4);
DigitalOut gLED(P2_5);

DigitalOut rst(P3_5);
DigitalOut clkSel(P3_4);
DigitalOut cs(P5_3);
DigitalOut Start(P5_4);


SDFileSystem sd(P0_5, P0_6, P0_4, P0_7, "sd");  // mosi, miso, sclk, cs

Serial dap(P2_1,P2_0);
Serial bt(P3_1,P3_0);

SPI ecg(P5_1, P5_2, P5_0); // mosi, miso, sclk, cs

unsigned long buff_1[3];
unsigned int status;
unsigned long buff_2[3];
signed int data;
unsigned long buff_3[3];
unsigned int ch2;
double value;
double Vref = 2.42;



//Test sub for time
Timer timer;
int elapsed;
//


int main()
{
    pegasus.init(MAX32630FTHR::VIO_3V3);
    pegasus.vddioh(P3_0,MAX32630FTHR::VIO_3V3); //HC-05 uses 3v3 io
    pegasus.vddioh(P3_1,MAX32630FTHR::VIO_3V3);
    
    rLED = 0;       //ON on LOW
    gLED = 1;
    dap.baud(115200);
    bt.baud(9600);
    
    dap.printf("\r\n\r\n");
    
    //Startup sequence, always needed
    Start = 0;
    cs = 0;
    rst = 0;
    Thread::wait(1);    //Let power supplies level off
    clkSel = 1;         //ADS1291 Internal 512KHz clock
    Thread::wait(1);    //Oscillator Startup
    rst = 1;
    cs = 1;
    Thread::wait(1000); //tpor and change
    rst = 0;            //Reset
    Thread::wait(1);
    rst = 1;
    Thread::wait(1); 
    // Startup finished
    
    // <SPI Init>
    ecg.format(8,1);        //8-bit, spi mode 1
    ecg.frequency(1000000); //1MHz SPI speed
    // </SPI Init>
    
    //Begin Serial Comms
    cs = 0;
    ecg.write(0x11);    //Stop continuous read opcode (SDATAC)
    cs = 1;  

    cs = 0;
    ecg.write(0x42);    //Write beginning at 0x02 (Config2)
    ecg.write(0x00);    //1 register (0x00 = n -1)
    ecg.write(0xA8);    //Write 0xA8    (Internal buff_1, clock will output)
    cs = 1;
    
    cs = 0;
    ecg.write(0x41);    //WREG Address 0x01 (Config1)
    ecg.write(0x00);    
    ecg.write(0x02);    //Set buffer rate to 500SPS (default)
    cs = 1;
    
    //cs = 0;
    //ecg.write(0x44);    //WREG CH1SET (channel 1 settings)
    //ecg.write(0x00);  
    //ecg.write(0x01);    //lead shorted (for test/Startup I guess?
    //cs = 1;
    
    Start = 1;          //Begin conversion
    
    
        
    //Test register access
    cs = 0;
    ecg.write(0x20);           //RReg ID
    ecg.write(0x00);           //Read 1 register
    int ID = ecg.write(0x00);  //Write dummy byte, get id binary as int
    ID <<= 1;
    cs = 1;
    
    dap.printf("ID as int = %d\r\n",ID);
    
    
    //ID Write
    FILE *fq = fopen("/sd/info.txt", "w");        //Open and write to SD card
    fprintf(fq, "Reg 1: %i", ID);
    fclose(fq);
    rLED = 1;
    gLED = 0;
    //End Test 
    
    
    
    cs = 0;
    ecg.write(0x42);    //WREG Config2
    ecg.write(0x00); 
    //ecg.write(0xA3);     //Test Mode: Internal buffer On, test signal, square wave test
    ecg.write(0xA0);    //Internal buffer On
    cs = 1;
    
    cs = 0;
    ecg.write(0x44);    //WREG CH1SET
    ecg.write(0x00);    //
    //ecg.write(0x13);    //Measure power supply
    //ecg.write(0x15);    //Test Mode: Set CH1_MUX to test input, Gain = 1
    ecg.write(0x60);    //Normal ECG, Gain = 6
    cs = 1;
    
    cs = 0;
    Thread::wait(1);
    ecg.write(0x45);    //WREG CH2SET
    ecg.write(0x00);    //
    ecg.write(0x91);    //Shorted, off
    //ecg.write(0x13);    //Measure power supply
    cs = 1;
    
    
    FILE *fp = fopen("/sd/log.txt", "w");  //Start from blank slate
    fclose(fp);
    
    
    
    timer.start();      //Part of timer test form
    
    rLED = 1;    
    for(int x = 0; x < 100; x++){
    //while(1){ 
        
        cs = 0;
        ecg.write(0x00); //Dummy bit to clear.
        Thread::wait(1);
        ecg.write(0x12); //Initiate Read
        //status = ecg.write(0x000000);
        //data = ecg.write(0x000000);
        
        for(int i = 0; i <3; i++){
            buff_1[i] = ecg.write(0x00);  //Store buff_1 bits
            }
            
        for(int i = 0; i <3; i++){    
            buff_2[i] = ecg.write(0x00);    //Store buff_2 bits
            }
        
        //for(int i = 0; i <3; i++){    
        //    buff_3[i] = ecg.write(0x00);    //Store buff_3 bits
        //    }
        cs = 1;
        
        buff_1[0] <<= 17;
        buff_1[1] <<= 9;
        buff_1[2] <<= 1;
        
        status = buff_1[0] | buff_1[1] | buff_1[2];
        
        buff_2[0] <<= 17;
        buff_2[1] <<= 9;
        buff_2[2] <<= 1;
        
        data = buff_2[0] | buff_2[1] | buff_2[2];
        
        
        //buff_3[0] <<= 16;
        //buff_3[1] <<= 8;
        //buff_3[2] <<= 0;
        
        //ch2 = buff_3[0] | buff_3[1] | buff_3[2];
        
        if(data < 0x7FFFFF){
            
            value = data * (Vref /(0x800000 - 1));
            
            }
        
        if(data == 0x7FFFFF){
            
            value = 222;
            
            }
        
        
        if(data > 0x800000){
            
            value = -Vref * (0xFFFFFF - data)/(0x800000 - 1);
            
            }
            
        if(data == 0x800000){
            
            value = 111;
            
            }        
        
        elapsed = timer.read_ms(); //Part of timer test unit
        
        //dap.printf("Status = %li\r\n",status);
        //dap.printf("buff[0] = %li\r\n",buff_1[0]);
        //dap.printf("buff[1] = %li\r\n",buff_1[1]);
        //dap.printf("buff[2] = %li\r\n",buff_1[2]);
        dap.printf("\r\n");
        dap.printf("Data = %li\r\n",data);
        dap.printf("buff[0] = %li\r\n",buff_2[0]);
        dap.printf("buff[1] = %li\r\n",buff_2[1]);
        dap.printf("buff[2] = %li\r\n",buff_2[2]);
        dap.printf("Value = %f\r\n",value);
        //dap.printf("Ch2 reading = %d\r\n",ch2);
        dap.printf("\r\n\r\n");
        
        //bt.printf("Status = %li\r\n",status);
        //bt.printf("Data = %li\r\n",data);
        bt.printf("E%f\n",value);
          
            
        FILE *fp = fopen("/sd/log.txt", "a");        //Open and append to SD card
        fprintf(fp, "%d,%f\r", elapsed, value);      //Log time and value in 2D csv text file
        fclose(fp);                                  //Close file
                     
        gLED = 0;
        Thread::wait(40);
        gLED = 1;
        
        
    }
    
    
    
    rLED = 1;
    gLED = 0;
    

}

