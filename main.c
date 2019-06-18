#include "images.h"

// Glcd module connections.
char GLCD_DataPort at PORTD;

sbit GLCD_CS1 at RB0_bit;
sbit GLCD_CS2 at RB1_bit;
sbit GLCD_RS  at RB2_bit;
sbit GLCD_RW  at RB3_bit;
sbit GLCD_EN  at RB4_bit;
sbit GLCD_RST at RB5_bit;

sbit GLCD_CS1_Direction at TRISB0_bit;
sbit GLCD_CS2_Direction at TRISB1_bit;
sbit GLCD_RS_Direction  at TRISB2_bit;
sbit GLCD_RW_Direction  at TRISB3_bit;
sbit GLCD_EN_Direction  at TRISB4_bit;
sbit GLCD_RST_Direction at TRISB5_bit;
// End Glcd module connections.

// Global variables.
char *someText;
char temp_text[] = "00.0";

void get_temp_text(float val, char *temp_text){

   int int_part1 = val*10;
   int int_part2 = (val*10 - int_part1)*10;
   int dec_part1 = ((val*10 - int_part1)*10 - int_part2)*10;

   temp_text[0] = '0' + int_part1;
   temp_text[1] = '0' + int_part2;
   temp_text[2] = '.';
   temp_text[3] = '0' + dec_part1;
   return;
}

void LoadScreen(char CurrScreen[], char PrevScreen[]){
    if(PrevScreen != CurrScreen){
        Glcd_Fill(0x00);                // Clear GLCD.
        Glcd_Image(*CurrScreen);         // Draw image.
    }

    if(strcmp(CurrScreen, "Home") == 0){
        // Get data from RE1, multiply by resolution and convert to str.
        float Temp = ADC_Get_Sample(6)*0.00100098;      // Resolution = (1.024 - 0)/(2^10 - 1) = 0.00100098.
        get_temp_text(Temp, temp_text);
        Glcd_Write_Text(temp_text, 2, 1, 2);            // Write temperature string.
    }
    else if(strcmp(CurrScreen, "Lights") == 0){
    // Put the buttons filling when pressed here.
    }
    else if(strcmp(CurrScreen, "Lock") == 0){

    }
    else if(strcmp(CurrScreen, "Fan") == 0){

    }
}

void read_memory(){
    unsigned char s=0;
    s = eeprom_read(0);
    delay_ms(20);
    
    if(s>59){
        eeprom_write(0,30);
        delay_ms(10);
    }
}

char key_pressed(){ 

    lata = 0b00000001;
    if(porta.ra4 == 1){
        // Button 1. Lock Num. 1 / LED C0.
        return "1";
    }
    else if (porta.ra5 == 1){
        // Button 4. Lock Num. 4 / LED C1.
        return "4";
    }
    else if (porta.ra6 == 1){
        // Button 7. Lock Num. 7 / LED C2.
        return "7";
    }
    else if (porta.ra7 == 1){
        // Button *. Lights. LED C3.
        return "*";
    }

    lata = 0b00000010;
    if(porta.ra4 == 1){
        // Button 2. Lock Num. 2 / LED C4.
        return "2";
    }
    else if (porta.ra5 == 1){
        // Button 5. Lock Num. 5 / LED C5.
        return "5";
    }
    else if (porta.ra6 == 1){
        // Button 8. Lock Num. 8 / LED C6.
        return "8";
    }
    else if (porta.ra7 == 1){
        // Button 0. Music. Lock Num. 0 / LED C7.
        return "0";
    }

    lata = 0b00000100;
    if(porta.ra4 == 1){
        // Button 3. Lock Num. 3 / LED E4.
        return "3";
    }
    else if (porta.ra5 == 1){
        // Button 6. Lock Num. 6 / LED E5.
        return "6"; 
    }
    else if (porta.ra6 == 1){
        // Button 9. Lock Num. 7 / LED E6.
        return "9";
    }
    else if (porta.ra7 == 1){
        // Button #. Lock / LED E7.3
        return "#";
    }

    lata = 0b00001000;
    if(porta.ra4 == 1){
        // Button A. Ok.
        return "A";
    }
    else if (porta.ra5 == 1){
        // Button B. Back.
        return "B";
    }
    else if (porta.ra6 == 1){
        // Button C.
        return "C";
    }
    else if (porta.ra7 == 1){
        // Button D.
        return "D";
    }
    
    return "n"; // n for no key pressed.
}

void main(){
    char PASSWORD[] = "xxxx"; // 4 digits password.
    char CurrScreen[] = "Lock  "; // Lock is the start screen.
    char key;

    //****************************** Fan - PWM1 *************************************//
    trise.re3 = 0;                          // Setting re3 as output.
    ansele.re3 = 0;                         // Setting re3 as digital.

    //*********************** Temperature Sensor LM35 ******************************//
    // ADC converter for temperature.
    adcon1 = 0b00001000;            // Selects the special trigger from CCP5; Vref+ = Vdd internal; Vref- = Vss internal.
    ADC_Init_Advanced(_ADC_INTERNAL_VREFL | _ADC_INTERNAL_FVRH1);

    // ADC output to re1.
    trise.re1 = 1;                  // Setting re1 as input.
    ansele.re1 = 1;                 // Setting re1 as analog.
    
    PWM1_Init(5000);                // 5 KHz.
    PWM1_Start();                   // Start PWM1.
    PWM1_Set_Duty(0);
    
    //*********************** GLCD ******************************//
    ANSELb = 0;                     // Configure AN pins as digital.
    ANSELd = 0;
    C1ON_bit = 0;                   // Disable comparators.
    C2ON_bit = 0;

    //initialize screen with lock screen
    Glcd_Init();                    // Initialize GLCD.
    Glcd_Fill(0x00);                // Clear GLCD.
    Glcd_Set_Font(Font_Glcd_System3x5, 3, 5, 32);   // Change font.
    Glcd_Image(Lock);                               // Draw image.
    
    //*********************** Keypad ******************************//
    ansela = 0;            // Porta 'a' as digital.
    trisa = 0b11110000;    // Setting as input / output pin.

    //************************ Leds *******************************//
    anselc = 0;            // Porta 'b' as digital.
    trisc = 0;             // Setting as output.

    ansele.re0 = 0;         // Setting as digital.
    ansele.re2 = 0;         // Setting as digital.
    ansele.re3 = 0;         // Setting as digital.

    trise.re0 = 0;         // Setting as output pin.
    trise.re2 = 0;         // Setting as output pin.
    trise.re3 = 0;         // Setting as output pin.
    //*************************************************************//

    while(1){

        if(strcmp(CurrScreen, "Lights")){
            key = key_pressed();
            switch (key)
            {
                case 'B':
                    strcpy(CurrScreen, "Home");
                    LoadScreen(CurrScreen, "Lights");
                    break;
                case '1':
                    latc.rc0 =~ latc.rc0;
                    break;
                case '5':
                    latc.rc1 =~ latc.rc1;
                    break;
                case '7':
                    latc.rc2 =~ latc.rc2;
                    break;
                case '*':
                    latc.rc3 =~ latc.rc3;
                    break;
                case '2':
                    latc.rc4 =~ latc.rc4;
                    break;
                case '5':
                    latc.rc5 =~ latc.rc5;
                    break;
                case '8':
                    latc.rc6 =~ latc.rc6;
                    break;
                case '0':
                    latc.rc7 =~ latc.rc7;
                    break;
                case '7':
                    latc.rc2 =~ latc.rc2;
                    break;
                case '3':
                    late.re0 =~ late.re0;
                    break;
                case '6':
                    late.re2 =~ late.re2;
                    break;
                case '9':
                    late.re3 =~ late.re3;
                    break;
                default:
                    break;
            }
        }

        else if(strcmp(CurrScreen, "Lock")){
            key = key_pressed();

            if(key != 'n'){
                if(PASSWORD[0] == 'x')
                    PASSWORD[0] = key;
                else if(PASSWORD[1] == 'x')
                    PASSWORD[1] = key;
                else if(PASSWORD[2] == 'x')
                    PASSWORD[2] = key;
                else if(PASSWORD[3] == 'x')
                    PASSWORD[4] = key;
                else if(key == 'A'){
                    if (strcmp(PASSWORD, "5093") == 0){
                        strcpy(CurrScreen, "Home");
                        LoadScreen(CurrScreen, "Lock");
                    }
                    else
                        strcpy(PASSWORD, "xxxx");
                }
            }
        }

        else if(strcmp(CurrScreen, "Home")){
            key = key_pressed();
            switch (key)
            {
                case '*':
                    strcpy(CurrScreen, "Lights");
                    LoadScreen(CurrScreen, "Home");
                    break;
                case '0':
                    strcpy(CurrScreen, "Fan");
                    LoadScreen(CurrScreen, "Home");
                    break;
                case '#':
                    strcpy(CurrScreen, "Lock");
                    LoadScreen(CurrScreen, "Home");
                    break;
                default:
                    break;
            }
        }

        else if(strcmp(CurrScreen, "Fan")){
            key = key_pressed();
            switch (key)
            {
                case 'B':
                    strcpy(CurrScreen, "Home");
                    LoadScreen(CurrScreen, "Lights");
                    break;
                case '0':
                    PWM1_Set_Duty(0);
                    break;
                case '1':
                    PWM1_Set_Duty(64);
                    break;
                case '2':
                    PWM1_Set_Duty(127);
                    break;
                case '3':
                    PWM1_Set_Duty(191);
                    break;
                case '4':
                    PWM1_Set_Duty(255);
                    break;
                default:
                    break;
            }
        }
    }
}