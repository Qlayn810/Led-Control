#INCLUDE <16F887.H>
#DEVICE ADC=10
#FUSES NOWDT, PUT, HS, NOPROTECT, NOLVP
#USE DELAY (CLOCK=20M)
#BIT TMR1IF = 0x0c.0
#include <TV_LCD.c>

#DEFINE MODE PIN_B0
#DEFINE UP PIN_B1
#DEFINE DW PIN_B2
#DEFINE POWER PIN_B3
#DEFINE SKIP PIN_B4
#DEFINE OPEN PIN_B5

// U LA BIEN DUNG DE PHAT HIEN CO NGUOI DI QUA
// N LA BIEN DUNG DE TRANH LAP LAI ADDLINE CUA LCD
// CB LA BIEN DUNG DE BAT CAM BIEN SIEU AM
// C LA BIEN DE CHUYEN DOI DO PHAN GIAI CHO ADC
UNSIGNED INT8   CHEDO, I, SOLANDO, OFF, N, PAGE, U, CB;
UNSIGNED INT16 T, DISTANCE;
UNSIGNED INT32 CAPDO, DUTY, ADC, PT, C, M;
const unsigned char HANG1[16]={"STT:            "};
const unsigned char HANG2[16]={"NHOM 8  -LV:    "};
const unsigned char XOA[16]={"                "};
const unsigned char HANG3[16]={"DISTANCE:       "};
const unsigned char HANG4[16]={"CO NGUOI DI QUA "};

#INT_AD
void interrupt_ADC()
{
   SOLANDO++; 
   ADC=ADC+READ_ADC(ADC_READ_ONLY);
}

VOID PHIM_UP ()
{
   IF (!INPUT(UP)) 
   { 
      DELAY_MS (20);
      IF(!INPUT(UP)) 
      {
         IF (CAPDO<10)
         {
            CAPDO++; 
            SET_PWM1_DUTY(CAPDO*10); 
            DO {} WHILE (!INPUT(UP));
         }
      }
   }
}

VOID PHIM_DW ()
{
   IF (!INPUT(DW)) 
   { 
      DELAY_MS (20);
      IF(!INPUT(DW)) 
      { 
         IF (CAPDO>0)
         {
            CAPDO--; 
            SET_PWM1_DUTY(CAPDO*10); 
            DO {} WHILE (!INPUT(DW));
         }
      }
   }
}

VOID PHIM_MODE ()
{
   IF(!INPUT(MODE))
   {
      DELAY_MS(20);
      IF((CHEDO==0) && (!INPUT(MODE)))
      {
         CHEDO=1;             // 1 LA MANUAL
         DO{}WHILE (!INPUT(MODE));
         BREAK;
      }
      IF((CHEDO==1) && (!INPUT(MODE)))
      {
         CHEDO=0;             // 0 LA AUTO
         DO{}WHILE (!INPUT(MODE));
      }
   }
}

VOID PHIM_POWER()
{
   IF(!INPUT(POWER))
   {
      DELAY_MS(20);
      IF((OFF==1) && (!INPUT(POWER))) 
      {
         OFF=0;
         SETUP_CCP1(CCP_PWM); //  BAT PWM CHI CAN SET 1 LAN NEN DE TREN DAY
         DO{}WHILE (!INPUT(POWER));
         BREAK;
      }
      IF((OFF==0) && (!INPUT(POWER))) 
      {
         OFF=1;               //  TAT ADC THI CON HEN GIO NEN DE O DUOI 
         DO{}WHILE (!INPUT(POWER));
      }
   }
}

VOID PHIM_SKIP()
{
   IF(!INPUT(SKIP))
   {
      DELAY_MS(20);
      IF((PAGE==2) && (!INPUT(SKIP))) 
      {
         PAGE=1;
         N=1;
         DO{}WHILE (!INPUT(SKIP));
         BREAK;
      }
      IF((PAGE==1) && (!INPUT(SKIP))) 
      {
         PAGE=2;               
         N=1;
         DO{}WHILE (!INPUT(SKIP));
      }
   }
}

VOID PHIM_OPEN()
{
   IF(!INPUT(OPEN))
   {
      DELAY_MS(20);
      IF((CB==0) && (!INPUT(OPEN)))
      {
         CB=1;             // 1 LA BAT CAM BIEN
         DO{}WHILE (!INPUT(OPEN));
         BREAK;
      }
      IF((CB==1) && (!INPUT(OPEN)))
      {
         CB=0;             // 0 LA TAT CAM BIEN
         DO{}WHILE (!INPUT(OPEN));
      }
   }
}

VOID LCD_HIENTHI()
{
   // HIEN THI TRANG 1
   IF(PAGE==1)
   {
      LCD_COMMAND(0x84); DELAY_US(10);
      IF(OFF==0)
      {
         LCD_DATA(0X20);
         LCD_DATA(0X4F);
         LCD_DATA(0X4E);
      }
      ELSE
      {
         LCD_DATA(0X4F);
         LCD_DATA(0X46);
         LCD_DATA(0X46);
      }
   
      // HIEN THI TUNG CHE DO
      IF(CHEDO==0 && OFF==0)
      {
         LCD_COMMAND(0x88); DELAY_US(10);
         LCD_DATA(0X2D);
         LCD_DATA(0X20);
         LCD_DATA(0X20);
         LCD_DATA(0X20);
         LCD_DATA(0X41);
         LCD_DATA(0X55);
         LCD_DATA(0X54);
         LCD_DATA(0X4F);
         LCD_COMMAND(0xCC); DELAY_US(10);
         IF(PT/100!=0)
         LCD_DATA((PT/100)+0X30);
         ELSE
         LCD_DATA(0X20);
         IF(PT/10%10!=0 || PT/100!=0)
         LCD_DATA((PT/10%10)+0X30);
         ELSE
         LCD_DATA(0X20);
         LCD_DATA(PT%10+0X30);
         LCD_DATA(0X25);
      }
      ELSE IF(OFF==0)
      {
         LCD_COMMAND(0x88); DELAY_US(10);
         LCD_DATA(0X2D);
         LCD_DATA(0X20);
         LCD_DATA(0X4D);
         LCD_DATA(0X41);
         LCD_DATA(0X4E);
         LCD_DATA(0X55);
         LCD_DATA(0X41);
         LCD_DATA(0X4C);
         LCD_COMMAND(0xCC); DELAY_US(10);
         IF(PT<=5 || (PT>=95 && U==1))
         {
            IF(PT/100!=0)
            LCD_DATA((PT/100)+0X30);
            ELSE
            LCD_DATA(0X20);
            IF(PT/10%10!=0 || PT/100!=0)
            LCD_DATA((PT/10%10)+0X30);
            ELSE
            LCD_DATA(0X20);
            LCD_DATA(PT%10+0X30);
            LCD_DATA(0X25);
         }
         ELSE
         {
            IF(CAPDO/10!=0)
            LCD_DATA((CAPDO/10)+0X30);
            ELSE
            LCD_DATA(0X20);
            IF(CAPDO%10!=0 || CAPDO/10!=0)
            LCD_DATA((CAPDO%10)+0X30);
            ELSE
            LCD_DATA(0X20);
            LCD_DATA(0X30);
            LCD_DATA(0X25);
         }
      }
   }
   // HIEN THI TRANG 2
   ELSE 
   {
         LCD_COMMAND(0X8B);   DELAY_US(10);
         IF(DISTANCE/100!=0)
         LCD_DATA(DISTANCE/100+0x30);
         ELSE  
         LCD_DATA(0X20);
         IF(DISTANCE/10%10!=0 || DISTANCE/100!=0)
         LCD_DATA(DISTANCE%100/10+0x30);
         ELSE 
         LCD_DATA(0X20);
         LCD_DATA(DISTANCE%10+0x30);
         LCD_DATA(0X63);
         LCD_DATA(0X6D);
   }
}
VOID MAIN() 
{ 
   // SET CAU HINH CHO PORT
   SET_TRIS_A(0xFF); 
   SET_TRIS_B(0XFF);
   SET_TRIS_D(0X00);
   SET_TRIS_C(0X00); 
   OUTPUT_C(0X00);
   
   // SET UP VA NGAT ADC
   SETUP_ADC(ADC_CLOCK_DIV_8);
   SETUP_ADC_PORTS(no_analogs | SAN0); 
   SET_ADC_CHANNEL(0); 
   ENABLE_INTERRUPTS(GLOBAL); ENABLE_INTERRUPTS(INT_AD);
   
   // SET UP LCD
   LCD_SETUP();
   LCD_COMMAND(ADDR_LINE1); DELAY_US(10);
   FOR (I=0;I<16;I++) { LCD_DATA(HANG1[I]);}
  
   // SET UP PWM
   SETUP_TIMER_2(T2_DIV_BY_16,249,1);   // 0.8ms/((1/20Mhz)*16)=1000
   
   // THIET LAP CAC GIA TRI BAN DAU
   CHEDO=0; DUTY=0;  SOLANDO=0;  U=0;  CB=0;    M=0;
   OFF=1;   CAPDO=0; ADC=0;   PAGE=1;  C=0;
   
   WHILE(TRUE)
   {
      PHIM_POWER();
      IF(OFF==0)
      {
         
         PHIM_SKIP();
         PHIM_MODE();
         
         // CAM BIEN SIEU AM
         PHIM_OPEN();
         IF(CB==1)
         {
            OUTPUT_HIGH(PIN_A1);   
            DELAY_US(10);
            OUTPUT_LOW(PIN_A1);
            WHILE(!INPUT(PIN_B7));  
            SETUP_TIMER_1(T1_INTERNAL | T1_DIV_BY_4);     
            SET_TIMER1(0);  
            WHILE(INPUT(PIN_B7) && !TMR1IF);
            SETUP_TIMER_1 (T1_DISABLED);
            T=GET_TIMER1();
            T=T*0.8;               // 1/(4/5)=0.8 us
            DISTANCE = T*0.0344/2;
         }
         ELSE DISTANCE=0;
         
         // KHOANG CACH CO NGUOI DI QUA
         IF((DISTANCE>=300) && (DISTANCE<=310)) U=1;
         ELSE  U=0;
      
         // DOC CAM BIEN ANH SANG DUA VAO BIEN TRO
         IF (SOLANDO<100)
         {
            READ_ADC(ADC_START_ONLY);
         }
         ELSE
         {
            SOLANDO=0;
            M=ADC;   
            // DOI RA LEVEL CUA BIEN TRO 
            ADC=(ADC/10.231)/100;
            PT=ADC;
            C=M/1.0231/100;
         }
         
         // HIEN THI LINE 1 LAN LEN LCD VOI BIEN N
         IF(PAGE==1)
         {
            IF(N==1)
            {
               LCD_COMMAND(ADDR_LINE1); DELAY_US(10);
               FOR (I=0;I<16;I++) { LCD_DATA(HANG1[I]);}
               LCD_COMMAND(ADDR_LINE2); DELAY_US(10);
               FOR (I=0;I<16;I++) { LCD_DATA(HANG2[I]);}
            }
               N=0;
         }
         ELSE
         {
            IF(N==1)
            {
               LCD_COMMAND(ADDR_LINE1); DELAY_US(10);
               FOR (I=0;I<16;I++) { LCD_DATA(HANG3[I]);}
            }
            N=0;
            // NGOAI TRU CAI NAY VI LA XOA VA MA TRAN (KHONG CO DIEN) 
            // NEN NHIEU LAN VAN DUOC
            IF(U==0)
            {
               LCD_COMMAND(ADDR_LINE2); DELAY_US(10);
               FOR (I=0;I<16;I++) { LCD_DATA(XOA[I]);}
            }
            ELSE 
            {
               LCD_COMMAND(ADDR_LINE2); DELAY_US(10);
               FOR (I=0;I<16;I++) { LCD_DATA(HANG4[I]);}
            }
         }
         // CHE DO MANUAL
         IF(CHEDO==1)
         {
            PHIM_UP();
            PHIM_DW();
          
            // TAT DEN DAN KHI TROI SANG TUC LA BIEN TRO DUOI 5%
            IF(PT<=5)
            {
               SET_PWM1_DUTY(C);
               IF(PT==0)   SET_PWM1_DUTY(0); // TAT HAN KHI TROI SANG
            }
            
            // BAT DEN KHI CO NGUOI DI QUA VAO BUOI TOI TUC LA BIEN TRO
            // TREN 95% NGUOI CAO TOI THIEU 300cm DAO DONG 10cm
            ELSE IF((PT>=95) && U==1 && (CAPDO!=10) && (CAPDO!=0))
            {
               SET_PWM1_DUTY(1000);      
            }
            ELSE  SET_PWM1_DUTY(CAPDO*100);
         }
     
         // CHE DO AUTO
         IF(CHEDO==0)
         { 
             //SET_PWM1_DUTY(PT*100);  
             SET_PWM1_DUTY(C);
         }
      }
      ELSE 
      {
         IF(N==0)
         {
            LCD_COMMAND(ADDR_LINE1); DELAY_US(10);
            FOR (I=0;I<16;I++) { LCD_DATA(HANG1[I]);}
            LCD_COMMAND(ADDR_LINE2); DELAY_US(10);
            FOR (I=0;I<16;I++) { LCD_DATA(XOA[I]);}
            N+=1;
         }
         PAGE=1;
         CB=0;
         SETUP_CCP1(CCP_OFF);
      }

      LCD_HIENTHI();
      DELAY_MS(10);
   }
}
