// IOC  bluetooth connectivity
// ext0 speed
// ext1 rpm
// ext2 ignition
// adc  battery
// adc  fuel

/*

 * bluet_conn and igni_t
 * initially IOC and ext2 on rest all off
 * igni_t message
 * acknowledgement ign
 * bluet_conn check
 * all interrupts_on
 * at 1 s uart on data sent
 * counter

  */
// PIC24FJ16GA002 Configuration Bit Settings

#include <xc.h>

// CONFIG2
#pragma config POSCMOD = NONE           // Primary Oscillator Select (HS Oscillator mode selected)
#pragma config I2C1SEL = PRI            // I2C1 Pin Location Select (Use default SCL1/SDA1 pins)
#pragma config IOL1WAY = OFF            // IOLOCK Protection (IOLOCK may be changed via unlocking seq)
#pragma config OSCIOFNC = OFF           // Primary Oscillator Output Function (OSC2/CLKO/RC15 functions as CLKO (FOSC/2))
#pragma config FCKSM = CSDCMD           // Clock Switching and Monitor (Clock switching and Fail-Safe Clock Monitor are disabled)
#pragma config FNOSC = FRCDIV           // Oscillator Select (Fast RC Oscillator with Postscaler (FRCDIV))
#pragma config SOSCSEL = SOSC           // Sec Oscillator Select (Default Secondary Oscillator (SOSC))
#pragma config WUTSEL = LEG             // Wake-up timer Select (Legacy Wake-up Timer)
#pragma config IESO = OFF               // Internal External Switch Over Mode (IESO mode (Two-Speed Start-up) disabled)

// CONFIG1
#pragma config WDTPS = PS32768          // Watchdog Timer Postscaler (1:32,768)
#pragma config FWPSA = PR128            // WDT Prescaler (Prescaler ratio of 1:128)
#pragma config WINDIS = OFF             // Watchdog Timer Window (Windowed Watchdog Timer enabled; FWDTEN must be 1)
#pragma config FWDTEN = OFF             // Watchdog Timer Enable (Watchdog Timer is disabled)
#pragma config ICS = PGx1               // Comm Channel Select (Emulator EMUC1/EMUD1 pins are shared with PGC1/PGD1)
#pragma config GWRP = OFF               // General Code Segment Write Protect (Writes to program memory are allowed)
#pragma config GCP = OFF                // General Code Segment Code Protect (Code protection is disabled)
#pragma config JTAGEN = OFF

#define rad 1.43
#define PI 3.14

// Status flags
unsigned char ign=0;
unsigned char eng_on=0;
unsigned char status=0x00;
unsigned char flag_e=0;
unsigned char counter=0;
unsigned char sequence=0;
unsigned char e_1=0;
unsigned char flag_spd=0;
unsigned char status1=0x00;
unsigned int adc =0;
unsigned int bat_v=0;
unsigned int r=0;
unsigned int r_imp=0;
float rpm;
unsigned char f=0;
unsigned int rpm_i;
unsigned char bat_off=0;
unsigned char a4=0;
unsigned int speed=0;
float dist;
unsigned int adc_past=0;
unsigned char q=0;
unsigned int rpm_ilast=0;
unsigned int dist_i=0;
unsigned int fuel=0;
unsigned char ack=0;
unsigned char ack_status=0;
unsigned char ind=0;
unsigned char i1=0;
unsigned char type_id=0;
unsigned char final[17]={0x41,0x41,0,0,0,0X30,0,0,0,0,0,0,0,0,0,0x58,0x58};
unsigned char ready=0;
unsigned char dat_sent=0;
unsigned char rcv[8];
unsigned int present_data[8];
unsigned char success=0;
unsigned char failure=0;
unsigned char status_rcv=0x00;
short final_crc=0;




void adc_init()
{
    AD1CON1=0x00E4;
    AD1CON2=0x0014;
    AD1CON3=0x0901;
    AD1CHS=0x0000;
    AD1PCFG=0xF7FE;
}

void init_interrupt()
{
    INTCON1=0x8000;
    INTCON2=0x0000;
    IFS0bits.INT0IF=0;
    T1CON=0x0020;
    TRISBbits.TRISB7=1;
}
void flag_init()
{
     ign=0;
     eng_on=0;
     status=0x00;
     status1=0x01;
     flag_e=0;
     counter=0;
     sequence=0;
     e_1=0;
     flag_spd=0;
     bat_off=0;
}

void data_flush()
{
    present_data[0]=0;
    present_data[1]=0;
    present_data[2]=0;
    present_data[3]=0;
    present_data[4]=0;
    present_data[5]=0;
    present_data[6]=0;
    present_data[7]=0;
    rpm_i=0;
    speed=0;
    bat_v=0;
    dist_i=0;
    fuel=0;
    r=0;
    adc=0;
    ack_status=0;
    dat_sent=0;
    ready=0;
    status_rcv=0x00;
}

void ext_int()
{
    INTCON1=0x8000;
    INTCON2=0x0000;
    IFS1bits.INT1IF=0;
    RPINR0=0x0800;
    T2CON=0x0030;
    T5CON=0x0020;
}
void acknow_data()
{

        if(rcv[0]==0x41)
        {
            if(rcv[1]==0x41)
            {

                            if(rcv[6]==0x58)
                            {
                                if(rcv[7]==0x58)
                                {
                                    if(rcv[5]==0x30&&ready==1)
                                    {
                                        status_rcv=0x84;
                                        ack_status=0;
                                        dat_sent=0;

                                        ack=0;
                                        success++;
                                     }
                                    if(ready==1&&rcv[5]==0x37)
                                    {
                                            status_rcv=0x84;
                                            ack_status=0;
                                            dat_sent=0;
                                            ack=0;
                                            failure++;

                                     }
                                    if(rcv[5]==0x32&&status_rcv==0x04)
                                    {
                                        status_rcv=0x40;
                                        ack_status=0;
                                        dat_sent=0;

                                    }
                                    if(ready==1&&rcv[5]==0x34)
                                        {
                                            status_rcv=0x84;

                                            ack_status=0;
                                            dat_sent=0;
                                        }
                                    if(ready==1&&rcv[5]==0x35)
                                        {
                                                status_rcv=0x85;
                                                ack_status=0;
                                                dat_sent=0;
                                        }
                                   }

                    }
                 }

        }
          else
        {
            if(status_rcv==0x04)
            {
                q=0;
            for(q=0;q<8;q++)
            {
                if(rcv[q]==0x32);
                {
                  status_rcv=0x40;
                   ack_status=0;
                   dat_sent=0;
                }
            }
            }
            else
            {
                if(ready==1)
                {
                    q=0;
                     for(q=0;q<8;q++)
                {
                      if(rcv[q]==0x34)
                                        {
                                            status_rcv=0x84;

                                            ack_status=0;
                                            dat_sent=0;
                                        }
                      else
                      {
                            if(rcv[q]==0x35)
                                        {
                                                status_rcv=0x85;
                                                ack_status=0;
                                                dat_sent=0;
                                        }
                            else
                            {
                                 if(rcv[q]==0x30)
                                    {
                                        status_rcv=0x84;
                                        ack_status=0;
                                        dat_sent=0;

                                        ack=0;
                                        success++;
                                     }
                                 else
                                 {
                                     if(rcv[q]==0x37)
                                    {
                                            status_rcv=0x84;
                                            ack_status=0;
                                            dat_sent=0;
                                            ack=0;
                                            failure++;

                                     }
                                 }
                            }


                      }

                     }
                }
            }
             /*   if(rcv[6]==0x58&&status_rcv==0x04)
                {
                    if(rcv[5]==0x32&&status_rcv==0x04)
                                    {
                                        status_rcv=0x40;
                                        ack_status=0;
                                        dat_sent=0;

                                    }
                }
                else
                {
                if(rcv[7]==0x58&&status_rcv==0x04)
                {
                    if(rcv[6]==0x32&&status_rcv==0x04)
                                    {
                                        status_rcv=0x40;
                                        ack_status=0;
                                        dat_sent=0;

                                    }
                }
                else{
                     if(rcv[0]==0x58&&status_rcv==0x04)
                {
                    if(rcv[7]==0x32&&status_rcv==0x04)
                                    {
                                        status_rcv=0x40;
                                        ack_status=0;
                                        dat_sent=0;

                                    }
                }
                }
                }*/
        }
}
void __attribute__((__interrupt__, __auto_psv)) _U2RXInterrupt(void)
{
    IFS1bits.U2RXIF=0;
    rcv[ind]=U2RXREG;
    if(ind==7)
    {
        asm("NOP");
        if(ign==1)
        {
            acknow_data();
        }
        ind=0;
    }
    else
    {
        ind++;
    }
}

void rpm_calc()
{
    if(r!=0)
    {
        r_imp=r*16;
        rpm=(6000.0/r_imp);
        rpm_i=(unsigned int)(rpm*10000);
        if(rpm_i>10000)
        {
             rpm_i=rpm_ilast;
        }
        else
        {
          rpm_ilast=rpm_i;
        }
        r=0;

    }
}

void __attribute__((__interrupt__,auto_psv)) _INT1Interrupt(void)
{
    if(flag_spd==0)
    {
        T2CONbits.TON=1;
        flag_spd=1;
        T5CONbits.TON=1;
    }

    else
    {
        speed=TMR2;
        T2CONbits.TON=0;
        TMR2=0x0000;
        flag_spd=0;

    }
    counter++;
    if(counter==11)
    {
       counter=0;
       dist+=2.0*PI*rad;
       dist_i=(unsigned int)dist;
    }
    IFS1bits.INT1IF=0;
}

void __attribute__((__interrupt__,auto_psv)) _INT0Interrupt(void)
{

    if(flag_e==0)
        {
            TMR1=0x0000;
            T1CONbits.TON=1;
            flag_e=1;
            if(status==0x02)
            {
            bat_off=1;
            }
        }
    else
        {
            T1CONbits.TON=0;
            r=TMR1;
            TMR1=0x0000;
            e_1=1;
            flag_e=0;
            eng_on=1;
         }
         IFS0bits.INT0IF=0;
}

void __attribute__((__interrupt__,auto_psv)) _ADC1Interrupt(void)
 {
       IFS0bits.AD1IF=0;
       adc=(ADC1BUF0+ADC1BUF1+ADC1BUF2+ADC1BUF3+ADC1BUF4);
       adc=adc/5;
       if(status==0x03)
       {
            bat_v=adc;
            status=0x04;
            IEC0bits.AD1IE=0;
            AD1CON1bits.ADON=0;
       }
       else
       {
           fuel=adc;
       }
       if(fuel>950||fuel<230)
       {
           fuel=adc_past;
       }
       adc_past=fuel;
       IEC0bits.AD1IE=0;
       AD1CON1bits.ADON=0;
 }


void __attribute__((__interrupt__,auto_psv)) _CNInterrupt(void)
{
   if(PORTBbits.RB5==1&&status==0x00&&status1==0x01)
    {
        ign=1;
        status=0x01;
        status1=0x00;
    }
   else
    {
        if(PORTBbits.RB5==0)
        {
            ign=0;
        }
    }
    IFS1bits.CNIF=0;
}

void __attribute__((__interrupt__, __auto_psv)) _T5Interrupt(void)
{
    //if(status==0x08){
    IFS1bits.T5IF=0;
    status=0x48;
    T5CONbits.TON=0;//}
}

void init_all()
{
      init_interrupt();
      ext_int();

}

void disable_int()
{
   // IEC1bits.CNIE=0;
    IEC0bits.AD1IE=0;                   // fuel off
    IEC1bits.T5IE=0;                    // timer off
    IEC0bits.INT0IE=0;                  // engine off
    IEC1bits.INT1IE=0;                  // speed off
}
void enable_int()
{
    IEC1bits.CNIE=1;
    IEC0bits.AD1IE=1;
    IEC1bits.T5IE=1;
    IEC0bits.INT0IE=1;

}
void interrupt_priot_datacollection()
{
    IPC4=0x3000;    // ignition and bluetooth
    IPC3=0x0050;    // fuel and battery
    IPC0=0x0007;    // engine
    IPC5=0x0006;    // speed
    IPC7=0x0204;    // data collection timer
}
void interrupt_priot_ign()
{
    IPC4=0x5000;    // ignition and bluetooth
    IPC3=0x0060;    // fuel and battery
    IPC0=0x0000;    // engine
    IPC5=0x0000;    // speed
    IPC7=0x0200;    // data collection timer
}

short CRC16(char  *pbuf,char   cnt)
{
    short  crc;
    char   data;
    char   i;
    char   j;

    crc=0;
    for (i = 0; i < cnt; i++) {
        data = pbuf[i];
        asm("NOP");
        for (j = 0; j < 8; j++) {
            if ((data & 0x80) ^ ((crc & 0x8000) >> 8)) {
                crc <<= 1;
                crc ^= 0x1021;
            } else {
                crc <<= 1;
            }
            data <<= 1;
        }
    }
    return(crc);
}

void disable_trans()
{
    IPC7=0x0200;
    U2STAbits.UTXEN=0;
}

void init_uart()
{

  U2BRG=25;
    U2MODEbits.BRGH=0;
    U2MODEbits.UARTEN=1;
    U2MODEbits.ABAUD=0;
    U2MODEbits.IREN=0;
    U2MODEbits.LPBACK=0;
    U2MODEbits.RXINV=0;
    U2MODEbits.PDSEL0=0;
    U2MODEbits.PDSEL1=0;
    U2MODEbits.UEN=0b00;
    RPOR7=0x0005;
    ODCBbits.ODB14=1;
   //  IFS1bits.U2TXIF=0;
   // IEC1bits.U2TXIE=1;
    IEC1bits.U2RXIE=1;
    IFS1bits.U2RXIF=0;
    U2STAbits.UTXEN=1;
    RPINR19=0x0009;

    IPC7=0x0700;
}

void send(unsigned char dat1)
{
    U2TXREG=dat1;
    while(U2STAbits.TRMT==0);
}
void send_data()
 {
     for(a4=0;a4<17;a4++)
     {
         send(final[a4]);          // For CRC please subtract 0x30 at the receiver's end // 7 bit CRC

     }

     dat_sent=1;
 }

void store_data(unsigned int dat2)
 {
       unsigned char a3=12;
       unsigned char data=0;
                while(a3>6)
                {
                data=dat2%10;
                final[a3]=(data+0x30);
                dat2=dat2/10;
                a3--;
                }
  }
  void crc()
  {
      final_crc=CRC16(&final[2],11);
      final[13]=(unsigned char)final_crc;
      final[14]=(unsigned char)(final_crc>>8);

  }
  void data_format(unsigned int v2,unsigned char type)
    {
               final[6]=(type+0x30);

                store_data(v2);
                if(i1==6)
                {

                    final[2]=0x30;
                    final[3]=0x30;
                    final[4]=0x30;
                }
                else
                {
                final[4]=((sequence%10)+0x30);
                final[3]=((sequence/10)+0x30);
                final[2]=((sequence/100)+0x30);
                }
                crc();
    }




void main()
{
    OSCCON=0x7700;
    CLKDIV=0x0000;
    T4CON=0x0030;
    T3CON=0x0030;
    for(f=0;f<3;f++)               // booting delay for bluetooth module
    {
    TMR3=0x0000;
    T3CONbits.TON=1;
    while(IFS0bits.T3IF==0);
    T3CONbits.TON=0;
    IFS0bits.T3IF=0;
    }
    data_flush();
    flag_init();
    adc_init();
   // interrupt_priot_ign();


    CNEN2bits.CN27IE=1;
    IEC1bits.CNIE=1;
    TRISBbits.TRISB5=1;
      IPC4=0x5000;
    if(PORTBbits.RB5==1&&status==0x00)
    {
        ign=1;
        status=0x01;
        status1=0x00;
    }
    while(ign==0);


    while(1)
    {
        while(ign)
        {
            if(e_1==1)
            {
                e_1=0;
                rpm_calc();
            }

            if(status==0x01&&ign==1)
            {
                 init_all();
                     adc_init();
                 IEC0bits.INT0IE=1;
                 IPC0=0x0004;    // engine
                 if(status==0x01&&bat_off==0&&eng_on==0)
            {
                IFS0bits.AD1IF=0;
                IEC0bits.AD1IE=1;
                AD1CHS=0x0000;
                IPC3=0x0070;    // fuel and battery
                AD1CON1bits.ADON=1;
                TRISAbits.TRISA0=1;
                status=0x03;
            }
            else
            {
                if(status==0x01&&bat_off==1)
                {
                   // enable_int();
                   // interrupt_priot_datacollection();
                    status=0x08;
                }
            }
                 IFS1bits.T4IF=0;
                  TMR4=0x3CAF;
                    T4CONbits.TON=1;
                    while(IFS1bits.T4IF==0);
                    T4CONbits.TON=0;
                    IFS1bits.T4IF=0;
                    IPC3=0x0000;
                disable_int();
                init_uart();                // receiving interrupt
                if(dat_sent==0)
                {
                    i1=6;
                    data_format(1,7);
                    send_data();
                }
                ack_status=1;
                status_rcv=0x04;
                while(ack_status==1 && status_rcv==0x04 && ign==1)
                {
                   // ack++;
                    TMR4=0x3CAF;
                    T4CONbits.TON=1;
                    while(IFS1bits.T4IF==0);
                    T4CONbits.TON=0;
                    if((ack_status==1)&&(IFS1bits.T4IF==1))
                    {
                        IFS1bits.T4IF=0;
                        if(dat_sent==1)
                        {
                        i1=6;
                        data_format(1,7);
                        send_data();
                        }
                    }
                }
                if(ack_status==0&&status_rcv==0x40)
                {
                if(dat_sent==0)
                {
                i1=6;
                data_format(3,7);
                send_data();
                ack_status=1;
                ready=1;
                status_rcv=0x80;
              //  status_rcv=0x84;
             //   dat_sent=0;
             //   ack_status=0;
                }


          /*     while(ack_status==1&&status_rcv==0x80&&ign==1)
                {
                   // ack++;
                    TMR4=0x3CAF;
                    T4CONbits.TON=1;
                    while(IFS1bits.T4IF==0);
                    T4CONbits.TON=0;
                    if((ack_status==1)&&(IFS1bits.T4IF==1))
                    {
                        IFS1bits.T4IF=0;
                        if(dat_sent==1)
                        {
                        i1=6;
                        data_format(3,7);
                        send_data();
                        }
                    }
                }
*/
                }
                if(status==0x04)
                {
                disable_trans();
                interrupt_priot_ign();
                }
                else
                {
                    if(status==0x08)
                    {
                        enable_int();
                        interrupt_priot_datacollection();
                    }
                }


            }


            if(status==0x04&&eng_on==0)
            {
                enable_int();
                interrupt_priot_datacollection();
                status=0x08;
            }
            else
            {
                if(status==0x04&&eng_on==1)
                {
                    status=0x08;
                    enable_int();
                    interrupt_priot_datacollection();

                }
            }
            if(status==0x08&&eng_on==1)    // removed eng_on==1
            {
                IEC1bits.INT1IE=1;
                AD1CHS=0x000B;
                IEC0bits.AD1IE=1;
                IFS0bits.AD1IF=0;
                TRISBbits.TRISB13=1;
                AD1CON1bits.ADON=1;
            }
            if(status==0x48&&eng_on==1)
            {
                disable_int();

                IPC4=0x6000;    // ignition
                present_data[0]=fuel;
                present_data[1]=dist_i;
                present_data[2]=rpm_i;
                present_data[3]=speed;
                present_data[4]=bat_v;
                present_data[5]=500;
                present_data[6]=70;
                present_data[7]=80;

                init_uart();                // uart int

                for(i1=0;i1<8;i1++)
                {
                  if(i1==0)
                          {
                              type_id=1;

                          }
                          if(i1==1)
                          {
                              type_id=2;
                          }
                           if(i1==2)
                          {
                              type_id=3;
                          }
                           if(i1==3)
                          {
                              type_id=4;
                          }
                           if(i1==4)
                          {
                              type_id=5;
                          } if(i1==5)
                          {
                              type_id=6;
                          }
                           if(i1==6)
                          {
                              type_id=8;
                          }
                           if(i1==7)
                          {
                              type_id=9;
                          }
                if(dat_sent==0&&ack_status==0&&status_rcv==0x84&&ready==1)
                {
                    data_format(present_data[i1],type_id);
                    send_data();
                   ack_status=0;
                    dat_sent=0;
                    ack=1;
                  while(ack_status==1&&status_rcv==0x84&&ack<3)
                    {

                    TMR4=0x0000;
                    T4CONbits.TON=1;
                    while(IFS1bits.T4IF==0);
                    T4CONbits.TON=0;
                    if((ack_status==1)&&(IFS1bits.T4IF==1))
                    {
                        IFS1bits.T4IF=0;
                        if(dat_sent==1&&status_rcv==0x84)
                        {
                            data_format(present_data[i1],type_id);
                             send_data();
                            ack_status=1;
                            ack++;

                        }
                         else
                          {
                           while(status_rcv!=0x85);
                          }

                    }
                  }
                    if(ack==3&&ack_status==1&&dat_sent==1)
                    {
                        dat_sent=0;
                        ack_status=0;
                        ack=0;
                    }
                }
                 else
                 {
                           while(status_rcv!=0x85);
                 }
                }
                sequence++;
                if(sequence==100)
                {
                    sequence=0;
                }
                disable_trans();
                eng_on=0;
                TMR5=0x0000;
                status=0x08;
                enable_int();
                interrupt_priot_datacollection();
            }


        }
        if(ign==0&&status!=0x00&&status1==0x00)
        {
            flag_init();
            disable_int();
            init_uart();
            i1=6;
            data_format(0,7);
            send_data();
            dist=0;
            dist_i=0;
            data_flush();
            disable_trans();
            IEC1bits.U2RXIE=0;
         }

    if(PORTBbits.RB5==1&&status==0x00&&status1==0x01)
    {
        ign=1;
        status=0x01;
        status1=0x00;
    }
    }
}


