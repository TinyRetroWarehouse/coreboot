#define SMBUS_IO_BASE 0x5000

#define SMBHSTSTAT 0x0
#define SMBSLVSTAT 0x1
#define SMBHSTCTL  0x2
#define SMBHSTCMD  0x3
#define SMBXMITADD 0x4
#define SMBHSTDAT0 0x5
#define SMBHSTDAT1 0x6
#define SMBBLKDAT  0x7
#define SMBSLVCTL  0x8
#define SMBTRNSADD 0x9
#define SMBSLVDATA 0xa
#define SMLINK_PIN_CTL 0xe
#define SMBUS_PIN_CTL  0xf 

/* Define register settings */
#define HOST_RESET 0xff
#define DIMM_BASE 0xa0        // 1010000 is base for DIMM in SMBus
#define READ_CMD  0x01        // 1 in the 0 bit of SMBHSTADD states to READ


#define SMBUS_TIMEOUT (100*1000*10)

static void enable_smbus(void)
{
  device_t dev;
  unsigned char c;
  /* Power management controller */
  dev = pci_locate_device(PCI_ID(0x1106,0x8235), 0);

  if (dev == PCI_DEV_INVALID) {
    die("SMBUS controller not found\r\n");
  }

  // set IO base address to SMBUS_IO_BASE
  pci_write_config32(dev, 0x90, SMBUS_IO_BASE|1);

  // Enable SMBus 
  c = pci_read_config8(dev, 0xd2);
  c |= 5;
  pci_write_config8(dev, 0xd2, c);

  c = pci_read_config8(dev, 0x54);
  c &= ~0x80;
  pci_write_config8(dev, 0xd2, c);


  print_debug("SMBus controller enabled\r\n");
}


static inline void smbus_delay(void)
{
  outb(0x80, 0x80);
}

static int smbus_wait_until_ready(void)
{
	unsigned char c;
  unsigned long loops;
  loops = SMBUS_TIMEOUT;
  do {
    unsigned char val;
    smbus_delay();
    c = inb(SMBUS_IO_BASE + SMBHSTSTAT);
    while((c & 1) == 1) {
      print_err("c is ");
      print_err_hex8(c);
      print_err("\n");
      c = inb(SMBUS_IO_BASE + SMBHSTSTAT);
      /* nop */ 
    }

  } while(--loops);
  return loops?0:-1;
}

void smbus_reset(void)
{
  outb(HOST_RESET, SMBUS_IO_BASE + SMBHSTSTAT);
  outb(HOST_RESET, SMBUS_IO_BASE + SMBHSTSTAT);
  outb(HOST_RESET, SMBUS_IO_BASE + SMBHSTSTAT);
  outb(HOST_RESET, SMBUS_IO_BASE + SMBHSTSTAT);

  smbus_wait_until_ready();
  print_err("After reset status ");
  print_err_hex8( inb(SMBUS_IO_BASE + SMBHSTSTAT));
  print_err("\n");
}
  


static int smbus_wait_until_done(void)
{
  unsigned long loops;
  unsigned char byte;
  loops = SMBUS_TIMEOUT;
  do {
    unsigned char val;
    smbus_delay();
		
    byte = inb(SMBUS_IO_BASE + SMBHSTSTAT);
    if (byte & 1)
      break;

  } while(--loops);
  return loops?0:-1;
}

static void smbus_print_error(unsigned char host_status_register)
{

  print_err("smbus_error: ");
  print_err_hex8(host_status_register);
  print_err("\n");
  if (host_status_register & (1 << 4)) {
    print_err("Interrup/SMI# was Failed Bus Transaction\n");
  }
  if (host_status_register & (1 << 3)) {
    print_err("Bus Error\n");
  }
  if (host_status_register & (1 << 2)) {
    print_err("Device Error\n");
  }
  if (host_status_register & (1 << 1)) {
    print_err("Interrupt/SMI# was Successful Completion\n");
  }
  if (host_status_register & (1 << 0)) {
    print_err("Host Busy\n");
  }
}


/* SMBus routines borrowed from VIA's Trident Driver */
/* this works, so I am not going to touch it for now -- rgm */
static unsigned char smbus_read_byte(unsigned char devAdr, 
				unsigned char bIndex) 
{
  unsigned short i;
  unsigned char  bData;
  unsigned char  sts;
		        
  /* clear host status */
  outb(0xff, SMBUS_IO_BASE);
			    
  /* check SMBUS ready */
  for ( i = 0; i < 0xFFFF; i++ )
    if ( (inb(SMBUS_IO_BASE) & 0x01) == 0 )
      break;

  /* set host command */
  outb(bIndex, SMBUS_IO_BASE+3);

  /* set slave address */
  outb(devAdr | 0x01, SMBUS_IO_BASE+4);

  /* start */
  outb(0x48, SMBUS_IO_BASE+2);

  /* SMBUS Wait Ready */
  for ( i = 0; i < 0xFFFF; i++ )
    if ( ((sts = inb(SMBUS_IO_BASE)) & 0x01) == 0 )
      break;
  if ((sts & ~3) != 0) {
    smbus_print_error(sts);
    return 0;
  }
  bData=inb(SMBUS_IO_BASE+5);

  return bData;

}

/* for reference, here is the fancier version which we will use at some 
 * point
 */
# if 0
int smbus_read_byte(unsigned device, unsigned address, unsigned char *result)
{
  unsigned char host_status_register;
  unsigned char byte;

  reset();

  smbus_wait_until_ready();

  /* setup transaction */
  /* disable interrupts */
  outb(inb(SMBUS_IO_BASE + SMBHSTCTL) & (~1), SMBUS_IO_BASE + SMBHSTCTL);
  /* set the device I'm talking too */
  outb(((device & 0x7f) << 1) | 1, SMBUS_IO_BASE + SMBXMITADD);
  /* set the command/address... */
  outb(address & 0xFF, SMBUS_IO_BASE + SMBHSTCMD);
  /* set up for a byte data read */
  outb((inb(SMBUS_IO_BASE + SMBHSTCTL) & 0xE3) | (0x2 << 2),
	  SMBUS_IO_BASE + SMBHSTCTL);

  /* clear any lingering errors, so the transaction will run */
  outb(inb(SMBUS_IO_BASE + SMBHSTSTAT), SMBUS_IO_BASE + SMBHSTSTAT);

  /* clear the data byte...*/
  outb(0, SMBUS_IO_BASE + SMBHSTDAT0);

  /* start the command */
  outb((inb(SMBUS_IO_BASE + SMBHSTCTL) | 0x40),
	  SMBUS_IO_BASE + SMBHSTCTL);

  /* poll for transaction completion */
  smbus_wait_until_done();

  host_status_register = inb(SMBUS_IO_BASE + SMBHSTSTAT);

  /* Ignore the In Use Status... */
  host_status_register &= ~(1 << 6);

  /* read results of transaction */
  byte = inb(SMBUS_IO_BASE + SMBHSTDAT0);
  smbus_print_error(byte);

  *result = byte;
  return host_status_register != 0x02;
}


#endif

