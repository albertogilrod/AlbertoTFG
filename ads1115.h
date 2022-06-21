/*=========================================================================
    I2C ADDRESS/BITS
    -----------------------------------------------------------------------*/
    #define ADS1115_ADDRESS                 (0x48)    // 1001 000 (ADDR = GND)
/*=========================================================================*/


/*=========================================================================
    POINTER REGISTER
    -----------------------------------------------------------------------*/
    #define ADS1115_REG_POINTER_CONVERT     (0x00)
    #define ADS1115_REG_POINTER_CONFIG      (0x01)
    /*=========================================================================*/

/*=========================================================================
    CONFIG REGISTER
    -----------------------------------------------------------------------*/
    #define ADS1115_REG_CONFIG_OS_SINGLE    (0x8000)  // Write: Set to start a single-conversion
    #define ADS1115_REG_CONFIG_OS_BUSY      (0x0000)  // Read: Bit = 0 when conversion is in progress
    #define ADS1115_REG_CONFIG_MUX_SINGLE_0 (0x4000)  // Single-ended AIN0
    
    #define ADS1115_REG_CONFIG_PGA_6_144V   (0x0000)  // +/-6.144V range = Gain 2/3
    #define ADS1115_REG_CONFIG_PGA_4_096V   (0x0200)  // +/-4.096V range = Gain 1
    #define ADS1115_REG_CONFIG_PGA_2_048V   (0x0400)  // +/-2.048V range = Gain 2 (default)
    #define ADS1115_REG_CONFIG_PGA_1_024V   (0x0600)  // +/-1.024V range = Gain 4
    #define ADS1115_REG_CONFIG_PGA_0_512V   (0x0800)  // +/-0.512V range = Gain 8
    #define ADS1115_REG_CONFIG_PGA_0_256V   (0x0A00)  // +/-0.256V range = Gain 16

    #define ADS1115_REG_CONFIG_MODE_CONTIN  (0x0000)  // Continuous conversion mode
    #define ADS1115_REG_CONFIG_MODE_SINGLE  (0x0100)  // Power-down single-shot mode (default)

    #define ADS1115_REG_CONFIG_DR_860SPS     ( 0x00E0 )  // 860 samples per second

    #define ADS1115_REG_CONFIG_CMODE_TRAD   (0x0000)  // Traditional comparator with hysteresis (default)
    
    #define ADS1115_REG_CONFIG_CPOL_ACTVLOW (0x0000)  // ALERT/RDY pin is low when active (default)
    #define ADS1115_REG_CONFIG_CPOL_ACTVHI  (0x0008)  // ALERT/RDY pin is high when active

    #define ADS1115_REG_CONFIG_CLAT_NONLAT  (0x0000)  // Non-latching comparator (default)
    #define ADS1115_REG_CONFIG_CLAT_LATCH   (0x0004)  // Latching comparator

    #define ADS1115_REG_CONFIG_CQUE_NONE    (0x0003)  // Disable the comparator and put ALERT/RDY in high state (default)
/*=========================================================================*/

#ifdef __cplusplus
extern "C" {
#endif

//extern int ads1115Setup(int pinBase, int id);
//extern void ini_timer(void);
//extern int myAnalogRead (struct wiringPiNodeStruct *node, int pin) ;

#ifdef __cplusplus
}
#endif



