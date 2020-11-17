/* ========================================
 *
 * Copyright YOUR COMPANY, THE YEAR
 * All Rights Reserved
 * UNPUBLISHED, LICENSED SOFTWARE.
 *
 * CONFIDENTIAL AND PROPRIETARY INFORMATION
 * WHICH IS THE PROPERTY OF your company.
 *
 * ========================================
*/
#include "project.h"
#include "stdio.h"
#include <time.h>
#include <math.h>
//#include "cy_pdl.h"
//#include "cy_rtc.h"

#define TIMER_PERIOD_MSEC   1U
#define addrADXL (0x53)

uint16 counter = 0;
int8 rbuff[2]; // Read buffer
int8 wbuff[2]; // write buffer

static cy_stc_scb_i2c_master_xfer_config_t register_setting;

void waitForOperation()
{
    while(0 != (I2C_MasterGetStatus() &CY_SCB_I2C_MASTER_BUSY))
    {
        CyDelayUs(1); // venter på at status ikk er busy mere.
    }
    return;
    
}

void handleError(uint8 x)
{
    Cy_GPIO_Write(RED_PORT,RED_NUM,0);
    Cy_GPIO_Write(GREEN_PORT,GREEN_NUM,1);
    printf(" error %d \r\n ", x);
}
//for at skrive til et register vil vi gerne skrive to værdier. 
// En addresse og en indstilling i form af data.

void writeRegister(uint8 reg_addr, int8 data)
{
    wbuff[0] = reg_addr;    //tildel det første element til register addressen.
    wbuff[1] = data;        //tildel det andet element til den data værdi som vi gerne vil bruge til at indstille registeret.
    
    register_setting.slaveAddress = addrADXL;
    register_setting.buffer = wbuff;
    register_setting.bufferSize = 2;
    register_setting.xferPending = false;
    I2C_MasterWrite(&register_setting);
    waitForOperation();
}

// for at læse fra et register skal vi oplyse addressen på det register som vi gerne vil læse på. 

uint8 readRegister(uint8 reg_addr)
{
    wbuff[0] = reg_addr;
    
    register_setting.slaveAddress = addrADXL;
    register_setting.buffer = wbuff;
    register_setting.bufferSize = 1;
    register_setting.xferPending = true;
    
    I2C_MasterWrite(&register_setting);
    waitForOperation();
    
    register_setting.buffer = rbuff;
    register_setting.xferPending = false;
    
    I2C_MasterRead(&register_setting);
    waitForOperation();
    
    return rbuff[0];
}
void TimerInterruptHandler(void)
{
    /* Clear the terminal count interrupt */
    Cy_TCPWM_ClearInterrupt(Timer_HW, Timer_CNT_NUM, CY_TCPWM_INT_ON_TC);
    counter ++;
    Cy_GPIO_Inv(RED_PORT,RED_NUM);
}
int16 RPS(float xData, float zData)
{
    int16 tempcount, tempcount2, tempcount3, tempcount4 = 0,RPS,revs,revs2,revs3,revs4;
    int16 samlet = xData+zData;
    while(zData != 0  &&  xData != 1)
    {
        tempcount = counter;
        revs = tempcount4 - tempcount;
    }
    while(zData != -1  &&  xData != 0)
    {
        tempcount2 = counter;
        revs2 = tempcount - tempcount2;
    }
    while(zData != 0  &&  xData != -1)
    {
        tempcount3 = counter;
        revs3 = tempcount2 - tempcount3;
    }
    while(zData != 0  &&  xData != 1)
    {
        tempcount4 = counter;
        revs4 = tempcount3 - tempcount4;
    }
    
    RPS = (revs+revs2+revs3+revs4)/4;
    
    return RPS;
}

float deltaAxis2Deg(float xData1,float yData1, float zData1, 
                    float xData2, float yData2, float zData2)
{
    // Beregner vinklen i grader, mellem 2 accelerometer
    // målinger. Testet og virker i visual studio.
    
    // 3D pythagoras
    float ZXY1 = sqrt((pow(xData1,2)+pow(yData1,2)+pow(zData1,2)));
    float ZXY2 = sqrt((pow(xData2,2)+pow(yData2,2)+pow(zData2,2)));
   
    // "acos( ( X1*X2 + Y1*Y2 + Z1*Z2 ) /  ZXY1 * ZXY2)" ...
    // ... giver vinklen mellem de 2 punkter i radianer.
    
    // Det ganges med 180/pi for at omregne til deg.
	float vinkelRad = acos(((xData1 * xData2) + (yData1 * yData2) + (zData1 * zData2)) / (ZXY1 * ZXY2));
	float vinkelDeg = vinkelRad * (180.0 / M_PI);
	
    return vinkelDeg;
}

int main(void)
{
    UART_Start();
    I2C_Start();
    
    int8 xAxis[2], yAxis[2], zAxis[2];
    float xAxis2, yAxis2, zAxis2;
    
    
    cy_en_scb_i2c_status_t initStatus;
    cy_en_sysint_status_t sysStatus;
    
    initStatus = Cy_SCB_I2C_Init(I2C_HW, &I2C_config, &I2C_context);
    if(initStatus != CY_SCB_I2C_SUCCESS)
    {
        handleError(1);
        
    }
    sysStatus = Cy_SysInt_Init(&I2C_SCB_IRQ_cfg, &I2C_Interrupt);
    if(sysStatus != CY_SYSINT_SUCCESS)
    {
        handleError(2);
    }
    
    I2C_MasterSendStart(I2C_HW, CY_SCB_I2C_WRITE_XFER, 1);
    waitForOperation();
    
    Cy_SysInt_Init(&isrTimer_cfg, TimerInterruptHandler);
    NVIC_EnableIRQ(isrTimer_cfg.intrSrc); /* Enable the core interrupt */
    __enable_irq(); /* Enable global interrupts. */
    
    Cy_GPIO_Write(RED_PORT,RED_NUM,1);      // nulstiller RØD LED til debuging og advarsler.
    Cy_GPIO_Write(GREEN_PORT,GREEN_NUM,1);  // nulstiller GRØN LED til debuging og godkendelser.
    setvbuf ( stdin, NULL, _IONBF, 0 );
    setvbuf ( stdout, NULL, _IONBF, 0);
    
    (void)Cy_TCPWM_Counter_Init(Timer_HW, Timer_CNT_NUM, &Timer_config); 
    Cy_TCPWM_Enable_Multiple(Timer_HW, Timer_CNT_MASK); /* Enable the counter instance */
    
    Cy_TCPWM_Counter_SetPeriod(Timer_HW, Timer_CNT_NUM, TIMER_PERIOD_MSEC - 1);
    Cy_TCPWM_TriggerStart(Timer_HW, Timer_CNT_MASK); 
    
    // initialisering
    //Register 0x2C er BW-Rata som bestemmer båndbredden og data output rate. 
    writeRegister(0x2C,6);
    waitForOperation();
    //Register 0x2D er Power control som bestemmer hvilket mode den er i f.eks (sleep mode eller measure mode). 
    writeRegister(0x2D, 8);
    waitForOperation();
    //Register 0x31 er Data format som bestemmer hvilken resolution vi bruger. 
    writeRegister(0x31,9);
    waitForOperation();
    //Regiseter 0x27 er Activate/inactivatete control. Her aktiveres de akser vi vil læse fra. 
    writeRegister(0x27,112);
    waitForOperation();
    
    // Calibration
    // X-Axis
    writeRegister(0x1E,2);
    // Y-Axis 
    writeRegister(0x1F,1);
    // Z-Axis
    writeRegister(0x20,13);
    
    
    // stop transmission
    I2C_MasterSendStop(1);
    CyDelay(10);
    
    // test variable
    uint16 grader = 0;
    uint16 totalGrader =0;
    uint16 antalomgange = 0;
    
    // Brugt til beregning af graders drejning
    // Giver x,y,zAxis[] start værdier
    float xAxis_p1p2[2]={0,0},yAxis_p1p2[2]={0,0},zAxis_p1p2[2]={1,1};
    

    int16 rps;
    for (;;)
    {

        // Led sættes så de viser hvis vi oplever fejl
        Cy_GPIO_Write(GREEN_PORT,GREEN_NUM,0);
        //Cy_GPIO_Write(RED_PORT,RED_NUM,1);
        
        I2C_MasterSendReStart(I2C_HW, CY_SCB_I2C_READ_XFER, 1);
        waitForOperation();
        
        
        // Her udregnes Xaxis
        xAxis[0] = readRegister(0x32);
        xAxis[1] = readRegister(0x33);
        xAxis2 = (xAxis[0] | xAxis[1] << 8);
        //xAxis2 = xAxis2/256;
        xAxis_p1p2[0]=xAxis2;
        
        // Her udregnes Zaxis
        zAxis[0] = readRegister(0x36);
        zAxis[1] = readRegister(0x37);
        zAxis2 = (zAxis[0] | zAxis[1] << 8);
        //zAxis2 = zAxis2/256;
        zAxis_p1p2[0]=zAxis2;
        
        // Her udregnes Yaxis
        yAxis[0] = readRegister(0x34);
        yAxis[1] = readRegister(0x35);
        yAxis2 = (yAxis[0] | yAxis[1] << 8);
        //yAxis2 = yAxis2/256;
        yAxis_p1p2[0]=yAxis2;

    //  rps = RPS(xAxis2,zAxis2);
        
        CyDelay(10);
        // Grader drejet = antal grader allerede drejet + nyt antal grader.
        grader = deltaAxis2Deg(xAxis_p1p2[0],yAxis_p1p2[0],zAxis_p1p2[0],
                        xAxis_p1p2[1],yAxis_p1p2[1],zAxis_p1p2[1]);
        
        totalGrader = grader;
        
        // for at tælle antal omgange
        if(grader>=360)
        {
            
            antalomgange++;
            grader = grader-360;
        }
        
        // Rykker data en plads i arrayet
        // så der kan samles et nyt punkt. P1 = P0, P0 = ny værdi næste omgang i loop.
        xAxis_p1p2[1]=xAxis_p1p2[0];
        yAxis_p1p2[1]=yAxis_p1p2[0];
        zAxis_p1p2[1]=zAxis_p1p2[0];
            

        
        I2C_MasterSendStop(1);
        // Her udskrives de tre Akser
        printf("Antal grader drejet:\t%d", totalGrader);
        printf("Antal omgange:\t%d \r\n", antalomgange);
    }
    return 0;
}


/* [] END OF FILE */
