#include <stdlib.h>
#include <stdio.h>
#include <string.h>


void printusage (void) {

printf ("#############################\n");
printf ("dp tunning utility usage:\n");
printf ("1. index (0-3)\n");
printf ("2. tx_margin_nom [7:0]\n");
printf ("3. deepmph_gen1_mon [15:8]\n");
printf ("4. deepmph_gen2_mon [23:16]\n");
printf ("5. deemph60_gen2_nom [31:24]\n");
printf ("ex: dp_tunning 1 0 2a 2a 40\n");
printf ("#############################\n");


}


int get_parameter (int *parameter) {

    int val = 0;
    
    val = parameter[0] | parameter[1] << 8 | parameter[2] << 16 | parameter[3] << 24;

    return val;
}

int main (int argC, char *argV[]) {

    int mm_address;
    int i,len,val=0;
    int para[4];
    char index;
    char buffer[256];
    char cmd[256] = "iotools mmio_write32 0x";
    char cmd_val[256] = " 0x";
    char cmd_offset[256];

    if (argC != 6) {
        printusage();
        return -1; 
    }
   
    printf("the GPU BAR address is ...0x");
    scanf ("%x",&mm_address);
    
    index = *argV[1];

    switch (index) {
        case '0':
            mm_address += 0x177ac;
            break;
        case '1':
            mm_address += 0x17b0c;
            break;
        case '2':
            mm_address += 0x17e6c;
            break;
        case '3':
            mm_address += 0x181cc;
            break;
        default:
            puts("wrong index, please give 0-4");
            printusage();
            return 0;

    }
    sprintf (cmd_offset,"%x",mm_address);
    strcat (cmd,cmd_offset);

    for (i = 0; i < 4 ; i++) 
        para[i] = (int) strtol(argV[i+2],(char**)NULL, 16);

    val = get_parameter(para);
    sprintf(buffer,"%x",val);

    strcat(cmd_val,buffer);
    strcat(cmd,cmd_val);
    printf("%s\n",cmd);
    system(cmd);

    return 0;

}

