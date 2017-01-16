/* All Include Files */

#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>
#include "inc/tm4c123gh6pm.h"
#include "init_modules.h"
#include "type.h"
#include "ff.h"
#include "diskio.h"
#include "integer.h"
#include "PLL.h"


#include "simplelink.h"
#include "sl_common.h"

/* All Definitions */

DIR Handle_dir;
FIL Handle,Handle2;
FRESULT Fresult;
FATFS sdVolume;
#define FILETESTSIZE 512

#define MMC_DATA_SIZE 512

#define APPLICATION_VERSION "1.0.0"

#define SL_STOP_TIMEOUT        0xFF

/* Use bit 32:
 *      1 in a 'status_variable', the device has completed the ping operation
 *      0 in a 'status_variable', the device has not completed the ping operation
 */
#define STATUS_BIT_PING_DONE  31

#define HOST_NAME       "www.ee.iitb.ac.in"    //need2change

/* IP addressed of server side socket. Should be in long format,
 * E.g: 0xc0a8010a == 192.168.1.10 */
//#define IP_ADDR         0xC0A80101    //need2change
#define IP_ADDR         0xC0A80104    //need2change
#define PORT_NUM        5001            /* Port number to be used */

#define BUF_SIZE        1400
#define NO_OF_PACKETS   1000

/*
 * Values for below macros shall be modified for setting the 'Ping' properties
 */
#define PING_INTERVAL   1000    /* In msecs */
#define PING_TIMEOUT    3000    /* In msecs */
#define PING_PKT_SIZE   20      /* In bytes */
#define NO_OF_ATTEMPTS  3

/* Application specific status/error codes */
typedef enum{
    LAN_CONNECTION_FAILED = -0x7D0,        /* Choosing this number to avoid overlap with host-driver's error codes */
    INTERNET_CONNECTION_FAILED = LAN_CONNECTION_FAILED - 1,
    DEVICE_NOT_IN_STATION_MODE = INTERNET_CONNECTION_FAILED - 1,

    TCP_SEND_ERROR = DEVICE_NOT_IN_STATION_MODE - 1,
    TCP_RECV_ERROR = TCP_SEND_ERROR -1,

	STATUS_CODE_MAX = -0xBB8

}e_AppStatusCodes;

#define IS_PING_DONE(status_variable)           GET_STATUS_BIT(status_variable, \
                                                               STATUS_BIT_PING_DONE)



/*
 * GLOBAL VARIABLES -- Start
 */
_u32  g_Status = 0;
_u32  g_PingPacketsRecv = 0;
_u32  g_GatewayIP = 0;

union
{
    _u8  BsdBuf[BUF_SIZE];
    _u32 demobuf[BUF_SIZE/4];
} uBuf;

/*
 * GLOBAL VARIABLES -- End
 */

/* SD_TEST_CODE*/

unsigned char buffer[512];
unsigned short block; 		//block is equivalent to sector
#define MAXBLOCKS 100
#define PF1  (*((volatile unsigned long *)0x40025008))
#define PF2  (*((volatile unsigned long *)0x40025010))
#define PF3  (*((volatile unsigned long *)0x40025020))

/* Definitions ends here */
/****************************************************************************************/
/* All functions */

void diskError(char* errtype, unsigned long n){
  PF2 = 0x00;      // turn LED off to indicate error
  while(1){};
}


/*unsigned char* TestDisk(void){  DSTATUS result;  unsigned short block;  int i; unsigned long n;
  // simple test of eDisk
  result = eDisk_Init(0);  // initialize disk
  if(result) diskError("eDisk_Init",result);
  n = 1;    // seed
  for(block = 0; block < MAXBLOCKS; block++){
    for(i=0;i<512;i++){
      n = (16807*n)%2147483647; // pseudo random sequence
      buffer[i] = 0xFF&n;
    }
    PF1 = 0x02;
    if(eDisk_WriteBlock(buffer,block))diskError("eDisk_WriteBlock",block); // save to disk
    PF1 = 0;
  }
  n = 1;  // reseed, start over to get the same sequence
  for(block = 0; block < MAXBLOCKS; block++){
    PF3 = 0x08;
    if(eDisk_ReadBlock(buffer,block))diskError("eDisk_ReadBlock",block); // read from disk
    PF3 = 0;
    for(i=0;i<512;i++){
      n = (16807*n)%2147483647; // pseudo random sequence
      if(buffer[i] != (0xFF&n)){
          PF2 = 0x00;   // turn LED off to indicate error
      }
    }
  }
  return buffer;
}
*/

/*SD_TEST_CODE_END*/



/*
 * STATIC FUNCTION DEFINITIONS -- Start
 */
static _i32 configureSimpleLinkToDefaultState();
static _i32 establishConnectionWithAP();
static _i32 checkLanConnection();
static _i32 checkInternetConnection();
static _i32 initializeAppVariables();
static _i32 BsdTcpClient(_u16 Port,unsigned char test);
static _i32 BsdTcpServer(_u16 Port);
static void displayBanner();
/*
 * STATIC FUNCTION DEFINITIONS -- End
 */

 /*
 * ASYNCHRONOUS EVENT HANDLERS -- Start
 */
/*!
    \brief This function handles WLAN events

    \param[in]      pWlanEvent is the event passed to the handler

    \return         None

    \note

    \warning
*/
void SimpleLinkWlanEventHandler(SlWlanEvent_t *pWlanEvent)
{
    if(pWlanEvent == NULL)
    {
        CLI_Write((_u8 *)" [WLAN EVENT] NULL Pointer Error \n\r");
        return;
    }
    
    switch(pWlanEvent->Event)
    {
        case SL_WLAN_CONNECT_EVENT:
        {
            SET_STATUS_BIT(g_Status, STATUS_BIT_CONNECTION);

            /*
             * Information about the connected AP (like name, MAC etc) will be
             * available in 'slWlanConnectAsyncResponse_t' - Applications
             * can use it if required
             *
             * slWlanConnectAsyncResponse_t *pEventData = NULL;
             * pEventData = &pWlanEvent->EventData.STAandP2PModeWlanConnected;
             *
             */
        }
        break;

        case SL_WLAN_DISCONNECT_EVENT:
        {
            slWlanConnectAsyncResponse_t*  pEventData = NULL;

            CLR_STATUS_BIT(g_Status, STATUS_BIT_CONNECTION);
            CLR_STATUS_BIT(g_Status, STATUS_BIT_IP_ACQUIRED);

            pEventData = &pWlanEvent->EventData.STAandP2PModeDisconnected;

            /* If the user has initiated 'Disconnect' request, 'reason_code' is
            SL_USER_INITIATED_DISCONNECTION */
            if(SL_WLAN_DISCONNECT_USER_INITIATED_DISCONNECTION == pEventData->reason_code)
            {
                CLI_Write((_u8 *)" Device disconnected from the AP on application's request \n\r");
            }
            else
            {
                CLI_Write((_u8 *)" Device disconnected from the AP on an ERROR..!! \n\r");
            }
        }
        break;

        default:
        {
            CLI_Write((_u8 *)" [WLAN EVENT] Unexpected event \n\r");
        }
        break;
    }
}

/*!
    \brief This function handles events for IP address acquisition via DHCP
           indication

    \param[in]      pNetAppEvent is the event passed to the handler

    \return         None

    \note

    \warning
*/
void SimpleLinkNetAppEventHandler(SlNetAppEvent_t *pNetAppEvent)
{
    if(pNetAppEvent == NULL)
    {
        CLI_Write((_u8 *)" [NETAPP EVENT] NULL Pointer Error \n\r");
        return;
    }
 
    switch(pNetAppEvent->Event)
    {
        case SL_NETAPP_IPV4_IPACQUIRED_EVENT:
        {
            SET_STATUS_BIT(g_Status, STATUS_BIT_IP_ACQUIRED);

            /*
             * Information about the connection (like IP, gateway address etc)
             * will be available in 'SlIpV4AcquiredAsync_t'
             * Applications can use it if required
             *
             * SlIpV4AcquiredAsync_t *pEventData = NULL;
             * pEventData = &pNetAppEvent->EventData.ipAcquiredV4;
             *
             */

			 SlIpV4AcquiredAsync_t *pEventData = NULL;
			 pEventData = &pNetAppEvent->EventData.ipAcquiredV4;
             g_GatewayIP = pEventData->gateway;
        }
        break;

        default:
        {
            CLI_Write((_u8 *)" [NETAPP EVENT] Unexpected event \n\r");
        }
        break;
    }
}


/*!
    \brief This function handles callback for the HTTP server events

    \param[in]      pHttpEvent - Contains the relevant event information
    \param[in]      pHttpResponse - Should be filled by the user with the
                    relevant response information

    \return         None

    \note

    \warning
*/
void SimpleLinkHttpServerCallback(SlHttpServerEvent_t *pHttpEvent,
                                  SlHttpServerResponse_t *pHttpResponse)
{
    /* Unused in this application */
    CLI_Write(" [HTTP EVENT] Unexpected event \n\r");
}

/*!
    \brief This function handles ping report events

    \param[in]      pPingReport holds the ping report statistics

    \return         None

    \note

    \warning
*/
static void SimpleLinkPingReport(SlPingReport_t *pPingReport)
{
    SET_STATUS_BIT(g_Status, STATUS_BIT_PING_DONE);

    if(pPingReport == NULL)
    {
        CLI_Write((_u8 *)" [PING REPORT] NULL Pointer Error\r\n");
        return;
    }

    g_PingPacketsRecv = pPingReport->PacketsReceived;
}

/*!
    \brief This function handles general error events indication

    \param[in]      pDevEvent is the event passed to the handler

    \return         None
*/
void SimpleLinkGeneralEventHandler(SlDeviceEvent_t *pDevEvent)
{
    /*
     * Most of the general errors are not FATAL are are to be handled
     * appropriately by the application
     */
    CLI_Write(" [GENERAL EVENT] \n\r");
}

/*!
    \brief This function handles socket events indication

    \param[in]      pSock is the event passed to the handler

    \return         None
*/
void SimpleLinkSockEventHandler(SlSockEvent_t *pSock)
{
    if(pSock == NULL)
    {
        CLI_Write(" [SOCK EVENT] NULL Pointer Error \n\r");
        return;
    }

    switch( pSock->Event )
    {
        case SL_SOCKET_TX_FAILED_EVENT:
            /*
             * TX Failed
             *
             * Information about the socket descriptor and status will be
             * available in 'SlSockEventData_t' - Applications can use it if
             * required
             *
            * SlSockEventData_u *pEventData = NULL;
            * pEventData = & pSock->socketAsyncEvent;
             */
            switch( pSock->socketAsyncEvent.SockTxFailData.status )
            {
                case SL_ECLOSE:
                    CLI_Write(" [SOCK EVENT] Close socket operation, failed to transmit all queued packets\n\r");
                    break;
                default:
                    CLI_Write(" [SOCK EVENT] Unexpected event \n\r");
                    break;
            }
            break;

        default:
            CLI_Write(" [SOCK EVENT] Unexpected event \n\r");
            break;
    }
}


/*
 * ASYNCHRONOUS EVENT HANDLERS -- End
 */


 /*
 * Application's entry point
 */

int main(int argc, char** argv)
{

	char* path ;
	static FILINFO fno;
	 DSTATUS init_done;
	initializeAppVariables();
	/* start_sd_card */
	  int i = 0;
	  PLL_Init(Bus80MHz);    // bus clock at 80 MHz
	  PortF_Init();  // LaunchPad switches and LED
	  PF1 = 0x02;    // turn red LED on

	  commoninit();		//Initialise Peripherals

	  	init_done = f_mount(&sdVolume, "", 1);

	  	//init_done = disk_initialize(0); 	//Initialise MMC/SD Card and store status in init_done variable

	  	inituart();			//Initialise UART
	  	UARTprintf("UART_Service Started \n");  //Checking UART Working or Not

	  	if(init_done)		//Error in initialisation
	  		{
	  			UARTprintf("SDCARD NOT FOUND \n");
	  			diskError("eDisk_Init",init_done);
	  		}
	  	else
	  		{
	  			UARTprintf("SDCARD FOUND \n");
	  			PF1 = 0x00;
	  			PF2 = 0x04;    // turn blue LED on
	  		}

	  /*******************Initialisation Ends***************************/

	  /////////////////OBTAINING FAT PARAMETERS FROM SD CARD//////////////////

	  //		Get_FATparam();
	  				  char c, d;
	  				  uint32_t n;
	  				  c = 0;
	  				  n = 1;    // seed
	  				  UINT successfulreads, successfulwrites;

	  				Fresult = f_open(&Handle2, "testFile.txt", FA_CREATE_ALWAYS|FA_WRITE);
	  				  if(Fresult){
	  				    UARTprintf("testFile error");
	  				    while(1){
	  				    	};
	  				    }
	  				  else{
	  					  UARTprintf("testFile opened sucessfully \n");
	  					  for(i=0; i<FILETESTSIZE; i++){
	  				      n = (16807*n)%2147483647; // pseudo random sequence
	  				      c = ((n>>24)%10)+'0'; // random digit 0 to 9
	  				      Fresult = f_write(&Handle2, &c, 1, &successfulwrites);
	  				      if((successfulwrites != 1) || (Fresult != FR_OK)){
	  				        UARTprintf("f_write error");
	  				        while(1){};
	  				      }
	  				    }
	  				    Fresult = f_close(&Handle2);
	  				    if(Fresult){
	  				     UARTprintf("file2 f_close error");
	  				      while(1){};
	  				    }
	  				  }


//===============================================================================================
	  				Fresult = f_open(&Handle, "testfile.txt", FA_READ);

	  				UARTprintf("File size = %u \n",f_size(&Handle));

	  					  				  Fresult = f_read(&Handle, &buffer, f_size(&Handle), &successfulreads);

	  					  				  //UARTprintf("data => %c", *(&c+1));

	  					  				  Fresult = f_close(&Handle);


	  				Fresult = f_opendir(&Handle_dir, "/");

	  				if(Fresult != FR_OK) {
	  					UARTprintf("Failed to open the directory \n");
	  				}
	  				else
	  				{
	  					UARTprintf("Directory Opened Successfully. \n");

	  					int k=0;

	  					for (;;) {
	  						Fresult = f_readdir(&Handle_dir, &fno);                   /* Read a directory item */
	  					            if (Fresult != FR_OK || fno.fname[0] == 0) break;  /* Break on error or end of dir */
	  				//	                UARTprintf("%d/) %s/%s\n", k, path, fno.fname);
	  				                	UARTprintf("%d) %s\n", k,fno.fname);
	  					                k++;
	  					            }
	  					f_closedir(&Handle_dir);
	  				}

//=====================================================================================================


	/* sd_card_end */

	_i32 retVal = -1;

//    retVal = initializeAppVariables();
//    ASSERT_ON_ERROR(retVal);

    /* Stop WDT and initialize the system-clock of the MCU
       These functions needs to be implemented in PAL */
    stopWDT();
    initClk();

    /* Configure command line interface */
  //  CLI_Configure();

    displayBanner();

	/*
     * Following function configures the device to default state by cleaning
     * the persistent settings stored in NVMEM (viz. connection profiles &
     * policies, power policy etc)
     *
     * Applications may choose to skip this step if the developer is sure
     * that the device is in its default state at start of application
     *
     * Note that all profiles and persistent settings that were done on the
     * device will be lost
     */

	 retVal = configureSimpleLinkToDefaultState();
    if(retVal < 0)
    {
        if (DEVICE_NOT_IN_STATION_MODE == retVal)
            CLI_Write((_u8 *)" Failed to configure the device in its default state \n\r");

        LOOP_FOREVER();
    }

	CLI_Write((_u8 *)" Device is configured in default state \n\r");

	/*
     * Assumption is that the device is configured in station mode already
     * and it is in its default state
     */
    /* Initializing the CC3100 device */
    retVal = sl_Start(0, 0, 0);
    if ((retVal < 0) ||
        (ROLE_STA != retVal) )
    {
        CLI_Write(" Failed to start the device \n\r");
        LOOP_FOREVER();
    }

    CLI_Write((_u8 *)" Device started as STATION \n\r");

	/* Connecting to WLAN AP - Set with static parameters defined at the top
    After this call we will be connected and have IP address */
    retVal = establishConnectionWithAP();
    if(retVal < 0)
       {
        CLI_Write((_u8 *)" Failed to establish connection w/ an AP \n\r");
           LOOP_FOREVER();
       }

    CLI_Write((_u8 *)" Connection established w/ AP and IP is acquired \n\r");
	CLI_Write((_u8 *)" Pinging...! \n\r");

/*	retVal = checkLanConnection();
    if(retVal < 0)
    {
        CLI_Write((_u8 *)" Device couldn't connect to LAN \n\r");
        LOOP_FOREVER();
    }

    CLI_Write((_u8 *)" Device successfully connected to the LAN\r\n");

 */

	CLI_Write((_u8 *)" Establishing connection with TCP server \n\r");

	/*Before proceeding, please make sure to have a server waiting on PORT_NUM*/
    retVal = BsdTcpClient(PORT_NUM,buffer);
    if(retVal < 0)
        CLI_Write(" Failed to establishing connection with TCP server \n\r");
    else
        CLI_Write(" Connection with TCP server established successfully \n\r");


	/*After calling this function, you can start sending data to CC3100 IP
    * address on PORT_NUM
    retVal = BsdTcpServer(PORT_NUM);
    if(retVal < 0)
           CLI_Write(" Failed to start TCP server \n\r");
    else
           CLI_Write(" TCP client connected successfully \n\r");*/

    /* Stop the CC3100 device */
    retVal = sl_Stop(SL_STOP_TIMEOUT);
    if(retVal < 0)
        LOOP_FOREVER();

    return 0;

}

/*!
    \brief This function configure the SimpleLink device in its default state. It:
           - Sets the mode to STATION
           - Configures connection policy to Auto and AutoSmartConfig
           - Deletes all the stored profiles
           - Enables DHCP
           - Disables Scan policy
           - Sets Tx power to maximum
           - Sets power policy to normal
           - Unregisters mDNS services
           - Remove all filters

    \param[in]      none

    \return         On success, zero is returned. On error, negative is returned
*/
static _i32 configureSimpleLinkToDefaultState()
{
    SlVersionFull   ver = {0};
    _WlanRxFilterOperationCommandBuff_t  RxFilterIdMask = {0};

    _u8           val = 1;
    _u8           configOpt = 0;
    _u8           configLen = 0;
    _u8           power = 0;

    _i32          retVal = -1;
    _i32          mode = -1;

    mode = sl_Start(0, 0, 0);
    ASSERT_ON_ERROR(mode);

    /* If the device is not in station-mode, try configuring it in station-mode */
    if (ROLE_STA != mode)
    {
        if (ROLE_AP == mode)
        {
            /* If the device is in AP mode, we need to wait for this event before doing anything */
            while(!IS_IP_ACQUIRED(g_Status)) { _SlNonOsMainLoopTask(); }
        }

        /* Switch to STA role and restart */
        retVal = sl_WlanSetMode(ROLE_STA);
        ASSERT_ON_ERROR(retVal);

        retVal = sl_Stop(SL_STOP_TIMEOUT);
        ASSERT_ON_ERROR(retVal);

        retVal = sl_Start(0, 0, 0);
        ASSERT_ON_ERROR(retVal);

        /* Check if the device is in station again */
        if (ROLE_STA != retVal)
        {
            /* We don't want to proceed if the device is not coming up in station-mode */
            ASSERT_ON_ERROR(DEVICE_NOT_IN_STATION_MODE);
        }
    }

    /* Get the device's version-information */
    configOpt = SL_DEVICE_GENERAL_VERSION;
    configLen = sizeof(ver);
    retVal = sl_DevGet(SL_DEVICE_GENERAL_CONFIGURATION, &configOpt, &configLen, (_u8 *)(&ver));
    ASSERT_ON_ERROR(retVal);

    /* Set connection policy to Auto + SmartConfig (Device's default connection policy) */
    retVal = sl_WlanPolicySet(SL_POLICY_CONNECTION, SL_CONNECTION_POLICY(1, 0, 0, 0, 1), NULL, 0);
    ASSERT_ON_ERROR(retVal);

    /* Remove all profiles */
    retVal = sl_WlanProfileDel(0xFF);
    ASSERT_ON_ERROR(retVal);

    /*
     * Device in station-mode. Disconnect previous connection if any
     * The function returns 0 if 'Disconnected done', negative number if already disconnected
     * Wait for 'disconnection' event if 0 is returned, Ignore other return-codes
     */
    retVal = sl_WlanDisconnect();
    if(0 == retVal)
    {
        /* Wait */
        while(IS_CONNECTED(g_Status)) { _SlNonOsMainLoopTask(); }
    }

    /* Enable DHCP client*/
    retVal = sl_NetCfgSet(SL_IPV4_STA_P2P_CL_DHCP_ENABLE,1,1,&val);
    ASSERT_ON_ERROR(retVal);

    /* Disable scan */
    configOpt = SL_SCAN_POLICY(0);
    retVal = sl_WlanPolicySet(SL_POLICY_SCAN , configOpt, NULL, 0);
    ASSERT_ON_ERROR(retVal);

    /* Set Tx power level for station mode
       Number between 0-15, as dB offset from max power - 0 will set maximum power */
    power = 0;
    retVal = sl_WlanSet(SL_WLAN_CFG_GENERAL_PARAM_ID, WLAN_GENERAL_PARAM_OPT_STA_TX_POWER, 1, (_u8 *)&power);
    ASSERT_ON_ERROR(retVal);

    /* Set PM policy to normal */
    retVal = sl_WlanPolicySet(SL_POLICY_PM , SL_NORMAL_POLICY, NULL, 0);
    ASSERT_ON_ERROR(retVal);

    /* Unregister mDNS services */
    retVal = sl_NetAppMDNSUnRegisterService(0, 0);
    ASSERT_ON_ERROR(retVal);

    /* Remove  all 64 filters (8*8) */
    pal_Memset(RxFilterIdMask.FilterIdMask, 0xFF, 8);
    retVal = sl_WlanRxFilterSet(SL_REMOVE_RX_FILTER, (_u8 *)&RxFilterIdMask,
                       sizeof(_WlanRxFilterOperationCommandBuff_t));
    ASSERT_ON_ERROR(retVal);

    retVal = sl_Stop(SL_STOP_TIMEOUT);
    ASSERT_ON_ERROR(retVal);

    retVal = initializeAppVariables();
    ASSERT_ON_ERROR(retVal);

    return retVal; /* Success */
}

/*!
    \brief Connecting to a WLAN Access point

    This function connects to the required AP (SSID_NAME).
    The function will return once we are connected and have acquired IP address

    \param[in]  None

    \return     0 on success, negative error-code on error

    \note

    \warning    If the WLAN connection fails or we don't acquire an IP address,
                We will be stuck in this function forever.
*/
static _i32 establishConnectionWithAP()
{
    SlSecParams_t secParams = {0};
    _i32 retVal = 0;

    secParams.Key = PASSKEY;
    secParams.KeyLen = pal_Strlen(PASSKEY);
    secParams.Type = SEC_TYPE;

    retVal = sl_WlanConnect(SSID_NAME, pal_Strlen(SSID_NAME), 0, &secParams, 0);
    ASSERT_ON_ERROR(retVal);

    /* Wait */
    while((!IS_CONNECTED(g_Status)) || (!IS_IP_ACQUIRED(g_Status))) { _SlNonOsMainLoopTask(); }

    return SUCCESS;
}

/*!
    \brief This function checks the LAN connection by pinging the AP's gateway

    \param[in]  None

    \return     0 on success, negative error-code on error
*/
static _i32 checkLanConnection()
{
    SlPingStartCommand_t pingParams = {0};
    SlPingReport_t pingReport = {0};

    _i32 retVal = -1;

    CLR_STATUS_BIT(g_Status, STATUS_BIT_PING_DONE);
    g_PingPacketsRecv = 0;

    /* Set the ping parameters */
    pingParams.PingIntervalTime = PING_INTERVAL;
    pingParams.PingSize = PING_PKT_SIZE;
    pingParams.PingRequestTimeout = PING_TIMEOUT;
    pingParams.TotalNumberOfAttempts = NO_OF_ATTEMPTS;
    pingParams.Flags = 0;
    pingParams.Ip = g_GatewayIP;

    /* Check for LAN connection */
    retVal = sl_NetAppPingStart( (SlPingStartCommand_t*)&pingParams, SL_AF_INET,
                                 (SlPingReport_t*)&pingReport, SimpleLinkPingReport);
    ASSERT_ON_ERROR(retVal);

    /* Wait */
    while(!IS_PING_DONE(g_Status)) { _SlNonOsMainLoopTask(); }

    if(0 == g_PingPacketsRecv)
    {
        /* Problem with LAN connection */
        ASSERT_ON_ERROR(LAN_CONNECTION_FAILED);
    }

    /* LAN connection is successful */
    return SUCCESS;
}

/*!
    \brief Opening a client side socket and sending data

    This function opens a TCP socket and tries to connect to a Server IP_ADDR
    waiting on port PORT_NUM. If the socket connection is successful then the
    function will send 1000 TCP packets to the server.

    \param[in]      port number on which the server will be listening on

    \return         0 on success, -1 on Error.

    \note

    \warning        A server must be waiting with an open TCP socket on the
                    right port number before calling this function.
                    Otherwise the socket creation will fail.
*/
static _i32 BsdTcpClient(_u16 Port,unsigned char test )
{
    SlSockAddrIn_t  Addr;

    _u16          idx = 0;
    _u16          AddrSize = 0;
    _i16          SockID = 0;
    _i16          Status = 0;
    _u16          LoopCount = 0;

    for (idx=0 ; idx<512 ; idx++)
        {
            uBuf.BsdBuf[idx] = (_u8)(buffer[idx]);
        //    uBuf.BsdBuf[idx] = buffer[idx];
        }

    for (idx=511 ; idx<BUF_SIZE ; idx++)
    {
        uBuf.BsdBuf[idx] = (_u8)(idx % 10);
    //    uBuf.BsdBuf[idx] = buffer[idx];
    }

    Addr.sin_family = SL_AF_INET;
    Addr.sin_port = sl_Htons((_u16)Port);
    Addr.sin_addr.s_addr = sl_Htonl((_u32)IP_ADDR);
    AddrSize = sizeof(SlSockAddrIn_t);

    SockID = sl_Socket(SL_AF_INET,SL_SOCK_STREAM, 0);
    if( SockID < 0 )
    {
        CLI_Write(" [TCP Client] Create socket Error \n\r");
        ASSERT_ON_ERROR(SockID);
    }

    Status = sl_Connect(SockID, ( SlSockAddr_t *)&Addr, AddrSize);
    if( Status < 0 )
    {
        sl_Close(SockID);
        CLI_Write(" [TCP Client]  TCP connection Error \n\r");
        ASSERT_ON_ERROR(Status);
    }

    while (LoopCount < NO_OF_PACKETS)
    {
        Status = sl_Send(SockID, uBuf.BsdBuf, BUF_SIZE, 0 );
 //   	Status = sl_Send(SockID, buffer, BUF_SIZE, 0 );
        if( Status <= 0 )
        {
            CLI_Write(" [TCP Client] Data send Error \n\r");
            Status = sl_Close(SockID);
            ASSERT_ON_ERROR(TCP_SEND_ERROR);
        }

        LoopCount++;
    }

    Status = sl_Close(SockID);
    ASSERT_ON_ERROR(Status);

    return SUCCESS;
}


/*!
    \brief This function initializes the application variables

    \param[in]  None

    \return     0 on success, negative error-code on error
*/
static _i32 initializeAppVariables()
{
    g_Status = 0;
    g_PingPacketsRecv = 0;
    g_GatewayIP = 0;
	pal_Memset(uBuf.BsdBuf, 0, sizeof(uBuf));

    return SUCCESS;
}

/*!
    \brief This function displays the application's banner

    \param      None

    \return     None
*/
static void displayBanner()
{
    CLI_Write((_u8 *)"\n\r\n\r");
    CLI_Write((_u8 *)" Getting started with station application - Version ");
    CLI_Write((_u8 *) APPLICATION_VERSION);
    CLI_Write((_u8 *)"\n\r*******************************************************************************\n\r");
}
