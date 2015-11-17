#include "mbed.h"

Serial pc(p28,p27);// tx,rx

SPI  lpc(p5,p6,p7); // mosi, miso, clk

DigitalOut cs(p8); //pin 8 is my GPIO pin and will be using it to select and deselect my at45
                   //remeber that the chip select of at45 is active low, so when cs is low, it will select the chip and when cs is high, you cant use the at45

int main()
{
    int option;
      
    int act_size; //this will be used to write data unto buffer and to figure out how many characters I have in my array(including the NULL character!!!)
    
    
    int individual_character; //using this to store the binaray representation of a character so you can store that value in the buffer/memory.
    
    
    char my_info[100]; //size of array is 100( 0 - 99 )
    
    
    
    cs = 1; //you have the at45 chip deselected
    
    
    
    cs = 0;
    lpc.format(8,0); //you are configuring the setting of the mbed. This mean you will be sending 8 bits every write and you will be in mode 3(dont know what the mode means though)
    lpc.frequency(1000000); //setting the frequency of the clock pin on the mbed to be 1MHZ
    cs = 1;
    wait(1);
    
    pc.baud(9600);
    wait(1);
    pc.printf("If you have written to the main memory of your at 45, please press 1, other wise press 0\n\r");
    pc.scanf("%i", &option);
    
    if( option == 1 ) //reading from the main memory after you wrote into it to buffer 2
    {
                  //below will be transferring the data of a specific page in the main memory to buffer 2
            cs = 0;

                lpc.write(0x55);    //op code to write from the main memory to buffer 2
                
                lpc.write(0x00);    ////////
                lpc.write(0x00);          ///// These are the 3 adress bytes, the upper 3 bits are the dont care bits, the the next 12 bits are the
                lpc.write(0x00);    ////////    adress bits that specify which page in the main memory that you are going to be reading from (each
                                            //  combination of the 12 adress bits represent a specific page(each page is 256 bytes long) in the main memory )
                                            // and the next 9 bits are dont core bits
                
            cs = 1;
            wait (1); //done tranferring data from  a specific page in the main meory to buffer 2
            
            
            //below will be reading from buffer 2 and displaying it on the screen
            cs = 0;
            
                lpc.write(0xD6); //opcode to read form buffer 2
                
                lpc.write(0x00); ///////
                lpc.write(0x00);        /// these are the 3 bytes. the upper 15 bits are dont care bits, and the next are the 9 adress bits to specificy
                lpc.write(0x00); ///////    the specific location of buffer 2 to start reading from (remeber that the first character you wrote in the
                                          //  buffer starts at adress 0, the second character starts at adress 1, and so on)
                                          
                lpc.write(0x00);  // this is just initialize reading from the buffer 2
                
                pc.printf("Data in your at45 is: ");        ///////////////
                for( int i = 0; i<256; i++)                              // //Their are 256 bytes in buffer 2 so thats why I have i<26
                {                                                        //
                    individual_character = lpc.write(0x00);              //
                    if( individual_character == '\0')                    ////  I am reading from buffer 2 and displaying it on the screen. The if statement
                    {                                                    //    is because I also wrote the NULL character in the main memroy which was 
                        break;                                           //    written in the buffer 2 so when I read the NULL character(from buffer 2), I stop reading
                    }                                                    //    from buffer 2 so I wont have to read garbage values and display it on my terminal
                    else                                                 //
                    {                                                    //
                        pc.printf("%c", individual_character);           //
                    }                                                    //
                }                                          ////////////////
            cs = 1;
            wait(1); //done reading from buffer 2 and displaying it on the screen
               
    }
        else //writing to the main memory
        {
            
            pc.printf("Please enter First/Last Name, Current Class, ad Last four of your SID(USE an underscore as a space)\n\r");
            pc.scanf("%99s",my_info); //you are scanning max 99 characters cause the last character must be the NULL character
            
            
            
            
            
            //below, you will be writing to buffer 1.
            cs = 0;
            
                lpc.write(0x84); //this is the 1 byte opcode to write to buffer 1 of the at 45
                
                
                lpc.write(0x00);////////
                lpc.write(0x00);       ////This is the 3 adress bytes, upper 15 bits are dont care and the last 9 are the adress bits you going to strat writing to buffer 1
                lpc.write(0x00);////////
                
                
                act_size = 0;                               //////////////////////
                individual_character = my_info[act_size];                       // you are saving the binary equivalent of each character to individual_character
                                                                                // so you can save that binary equivalnce in the buffer
                while( my_info[act_size] != '\0' )                              //
                {                                                               //
                    lpc.write(individual_character);                            //
                                                                                //////  Writing data to buffer 1. act_size will give you the actual size
                    act_size = act_size + 1;                                    //      of your array. so if your array has 3 character including the null 
                    individual_character = my_info[act_size];                   //      character, act_size will have the value of 2 after this while loop
                }                                                               //        
                                                                                // NOTICE THAT ACT_SIZE WILL HAVE THE INDEX VALUE THAT WILL INDEX
                                                                                // VALUE OF THE NULL CHARACTER
                                                                                //
                individual_character = my_info[act_size];                       // You are writing the null character to your buffer so when you read
                lpc.write(individual_character);                                // from buffer when you turn on device, you can stop reading at the
                                                                                // null bit so you wont read garbage values from the buffer
            cs = 1;  // done writing to buffer 1                                // 
            wait(1);                                      ////////////////////////
            
            ///////////*************WORKS up to this point *************//////////
            
            
            //below will be writing data from the buffer 1 to a specific page in the main memory
            cs = 0; 
                
                lpc.write(0x83);    //op code to write from buffer one to the main memory
                
                                
                lpc.write(0x00);      ///////
                lpc.write(0x00);           //// These are the 3 adress bytes, the upper 3 bits are dont care bits, the the next 12 bits are the
                lpc.write(0x00);      ///////   adress bits that specify which page in the main memory that you are going to be writing to (each
                                                // combination of the 12 adress bits represent a specific page(each page is 256 bytes long) in the main memory )
            cs = 1;
            wait(1); //done writing data from buffer 1 to a specific page in the main memory
            
        }//this ends the else statement
        
}// ends your main
