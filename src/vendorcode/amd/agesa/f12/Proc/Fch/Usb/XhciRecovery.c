/* $NoKeywords:$ */
/**
 * @file
 *
 * Config Fch Xhci controller
 *
 * Init Xhci Controller features (PEI phase).
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:     AGESA
 * @e sub-project: FCH
 * @e \$Revision: 44324 $   @e \$Date: 2010-12-22 01:16:51 -0800 (Wed, 22 Dec 2010) $
 *
 */
/*
*****************************************************************************
*
* Copyright (c) 2011, Advanced Micro Devices, Inc.
 * All rights reserved.
 * 
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *     * Redistributions of source code must retain the above copyright
 *       notice, this list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above copyright
 *       notice, this list of conditions and the following disclaimer in the
 *       documentation and/or other materials provided with the distribution.
 *     * Neither the name of Advanced Micro Devices, Inc. nor the names of 
 *       its contributors may be used to endorse or promote products derived 
 *       from this software without specific prior written permission.
 * 
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL ADVANCED MICRO DEVICES, INC. BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
****************************************************************************
*/
#include "FchPlatform.h"
#include "Filecode.h"
#define FILECODE PROC_FCH_USB_XHCIRECOVERY_FILECODE

/**
 * XhciIndirectRegInit - Config XHCI Indirect Registers
 *
 *
 *
 * @param[in] StdHeader AMD Standard Header
 *
 */
VOID
XhciIndirectRegInit (
  IN       AMD_CONFIG_PARAMS *StdHeader
  )
{
  //
  // SuperSpeed PHY Configuration (adaptation mode setting)
  //
  RwXhciIndReg ( FCH_XHCI_IND_REG94, 0xFFFFFC00, 0x00000021, StdHeader);
  RwXhciIndReg ( FCH_XHCI_IND_REGD4, 0xFFFFFC00, 0x00000021, StdHeader);
  //
  // SuperSpeed PHY Configuration (CR phase and frequency filter settings)
  //
  RwXhciIndReg ( FCH_XHCI_IND_REG98, 0xFFFFFFC0, 0x0000000A, StdHeader);
  RwXhciIndReg ( FCH_XHCI_IND_REGD8, 0xFFFFFFC0, 0x0000000A, StdHeader);
  //
  // BLM Meaasge
  //
  RwXhciIndReg ( FCH_XHCI_IND_REG00, 0xF8FFFFFF, 0x07000000, StdHeader);
  //
  // xHCI USB 2.0 PHY Settings
  // Step 1 is done by hardware default
  // Step 2
  RwXhciIndReg ( FCH_XHCI_IND60_REG00, ~ BIT12, BIT12, StdHeader);
  // Step 3
  RwXhciIndReg ( FCH_XHCI_IND60_REG0C, ~ ((UINT32) (0x0f << 8)), ((UINT32) (0x02 << 8)), StdHeader);
  RwXhciIndReg ( FCH_XHCI_IND60_REG08, ~ ((UINT32) (0xff << 8)), ((UINT32) (0x0f << 8)), StdHeader);
}

/**
 * XhciA12Patch - Config XHCI A12 Fix
 *
 *
 */
VOID
XhciA12Patch (
  )
{
  //
  // PLUG/UNPLUG of USB 2.0 devices make the XHCI USB 2.0 ports unfunctional - fix enable
  // ACPI_USB3.0_REG 0x20[12:11] = 2'b11
  RwMem (ACPI_MMIO_BASE + XHCI_BASE + XHCI_ACPI_MMIO_AMD_REG20, AccessWidth32, ~((UINT32) (0x3 << 11)), (UINT32) (0x3 << 11));
  //
  // XHC 2 USB2 ports interactional issue - fix enable
  // ACPI_USB3.0_REG 0x20[16] = 1'b1
  RwMem (ACPI_MMIO_BASE + XHCI_BASE + XHCI_ACPI_MMIO_AMD_REG20, AccessWidth32, ~((UINT32) (0x1 << 16)), (UINT32) (0x1 << 16));
  //
  // XHC USB2.0 Ports suspend Enhancement
  // ACPI_USB3.0_REG 0x20[15] = 1'b1
  //
  RwMem (ACPI_MMIO_BASE + XHCI_BASE + XHCI_ACPI_MMIO_AMD_REG20, AccessWidth32, ~((UINT32) (0x1 << 15)), (UINT32) (0x1 << 15));
  //
  // XHC HS/FS IN Data Buffer Underflow issue - fix enable
  // ACPI_USB3.0_REG 0x20[19:18] = 2'b11
  //
  RwMem (ACPI_MMIO_BASE + XHCI_BASE + XHCI_ACPI_MMIO_AMD_REG20, AccessWidth32, ~((UINT32) (0x3 << 18)), (UINT32) (0x3 << 18));
  //
  // XHC stuck in U3 after system resuming from S3 -fix enable
  // ACPI_USB3.0_REG 0x98[19] = 1'b1
  //
  RwMem (ACPI_MMIO_BASE + XHCI_BASE + XHCI_ACPI_MMIO_AMD_REG98, AccessWidth32, ~((UINT32) (0x1 << 19)), (UINT32) (0x1 << 19));
  //
  // Change XHC1 ( Dev 16 function 1) Interrupt Pin register to INTB# - Fix enable
  // ACPI_PMIO_F0[18] =1
  //
  RwMem (ACPI_MMIO_BASE + PMIO_BASE + FCH_PMIOA_REGF0, AccessWidth32, ~((UINT32) (0x1 << 18)), (UINT32) (0x1 << 18));
  //
  // EHCI3/OHCI3 blocks Blink Global Clock Gating when EHCI/OHCI Dev 22 fn 0/2 are disabled
  // ACPI_PMIO_F0[13] =1
  //
  RwMem (ACPI_MMIO_BASE + PMIO_BASE + FCH_PMIOA_REGF0, AccessWidth32, ~((UINT32) (0x1 << 13)), (UINT32) (0x1 << 13));
  //
  // Access register through JTAG fail when switch from XHCI to EHCI/OHCI - Fix enable
  // ACPI_PMIO_F0[17] =1
  //
  RwMem (ACPI_MMIO_BASE + PMIO_BASE + FCH_PMIOA_REGF0, AccessWidth32, ~((UINT32) (0x1 << 17)), (UINT32) (0x1 << 17));
  //
  // USB leakage current on differential lines when ports are switched to XHCI - Fix enable
  // ACPI_PMIO_F0[14] =1
  //
  RwMem (ACPI_MMIO_BASE + PMIO_BASE + FCH_PMIOA_REGF0, AccessWidth32, ~((UINT32) (0x1 << 14)), (UINT32) (0x1 << 14));
}

/**
 * IsItLpcRom - Is LPC Rom?
 *
 *
 * @retval  TRUE or FALSE
 *
 */
BOOLEAN
IsItLpcRom (
  OUT VOID
  )
{
  return ( (BOOLEAN) ((ACPIMMIO32 (ACPI_MMIO_BASE + MISC_BASE + FCH_MISC_REG80) & BIT1) == 0) );
}

/**
 * Is UMI in x1-Lane and GEN1 Mode?
 *
 *
 * @retval  TRUE or FALSE
 *
 */
BOOLEAN
IsUmiX1LaneGen1Mode (
  IN       AMD_CONFIG_PARAMS *StdHeader
  )
{
  UINT32   AbValue;

  AbValue = ReadAlink ((UINT32) (FCH_AX_CFG_REG68), StdHeader);
  AbValue >>= 16;
  if (((AbValue & 0x0f) == 1) && ((AbValue & 0x03f0) == 0x0010)) {
    return (TRUE);
  } else {
    return (FALSE);
  }
}

/**
 * FchXhciEarlyInit - Config XHCI controller in recovery mode
 *
 *
 */
VOID
FchXhciEarlyInit (
  IN       AMD_CONFIG_PARAMS       *StdHeader
  )
{
  UINT16                BcdAddress;
  UINT16                BcdSize;
  UINT16                AcdAddress;
  UINT16                AcdSize;
  UINT16                FwAddress;
  UINT16                FwSize;
  UINTN                 XhciFwStarting;
  UINT32                SpiValidBase;
  UINT32                RegData;
  UINT16                Index;

  RwMem (ACPI_MMIO_BASE + XHCI_BASE + XHCI_ACPI_MMIO_AMD_REG00, AccessWidth32, 0x00000000, 0x00400700);
  FchStall (20, StdHeader);

  //
  // Get ROM SIG starting address for USB firmware starting address (offset 0x0C to SIG address)
  //
  GetRomSigPtr (&XhciFwStarting, StdHeader);

  if (XhciFwStarting == 0) {
    return;
  }
  XhciFwStarting = ACPIMMIO32 (XhciFwStarting + FW_TO_SIGADDR_OFFSET);
  if (IsItLpcRom ()) {
  //
  //XHCI firmware re-load
  //
    RwPci ((LPC_BUS_DEV_FUN << 16) + FCH_LPC_REGCC, AccessWidth32, ~BIT2, (BIT2 + BIT1 + BIT0), StdHeader);
    RwPci ((LPC_BUS_DEV_FUN << 16) + FCH_LPC_REGCC, AccessWidth32, 0x00000FFF, (UINT32) (XhciFwStarting), StdHeader);
  }
  //
  // Enable SuperSpeed receive special error case logic. 0x20 bit8
  // Enable USB2.0 RX_Valid Synchronization. 0x20 bit9
  // Enable USB2.0 DIN/SE0 Synchronization. 0x20 bit10
  //
  RwMem (ACPI_MMIO_BASE + XHCI_BASE + XHCI_ACPI_MMIO_AMD_REG20, AccessWidth32, 0xFFFFF8FF, 0x00000700);
  //
  // SuperSpeed PHY Configuration (adaptation timer setting)
  //
  RwMem (ACPI_MMIO_BASE + XHCI_BASE + XHCI_ACPI_MMIO_AMD_REG90, AccessWidth32, 0xFFF00000, 0x000AAAAA);
  //RwMem (ACPI_MMIO_BASE + XHCI_BASE + XHCI_ACPI_MMIO_AMD_REG90 + 0x40, AccessWidth32, 0xFFF00000, 0x000AAAAA);

  //
  // Step 1. to enable Xhci IO and Firmware load mode
  //
  RwMem (ACPI_MMIO_BASE + XHCI_BASE + XHCI_ACPI_MMIO_AMD_REG00, AccessWidth32, 0xF0FFFFFC, 0x00000003);
  RwMem (ACPI_MMIO_BASE + XHCI_BASE + XHCI_ACPI_MMIO_AMD_REG00, AccessWidth32, 0xEFFFFFFF, 0x10000000);

  //
  // Step 2. to read a portion of the USB3_APPLICATION_CODE from BIOS ROM area and program certain registers.
  //
  RwMem (ACPI_MMIO_BASE + XHCI_BASE + XHCI_ACPI_MMIO_AMD_REGA0, AccessWidth32, 0x00000000, (SPI_HEAD_LENGTH << 16));

  BcdAddress = ACPIMMIO16 (XhciFwStarting + BCD_ADDR_OFFSET);
  BcdSize = ACPIMMIO16 (XhciFwStarting + BCD_SIZE_OFFSET);
  RwMem (ACPI_MMIO_BASE + XHCI_BASE + XHCI_ACPI_MMIO_AMD_REGA4, AccessWidth16, 0x0000, BcdAddress);
  RwMem (ACPI_MMIO_BASE + XHCI_BASE + XHCI_ACPI_MMIO_AMD_REGA4 + 2, AccessWidth16, 0x0000, BcdSize);

  AcdAddress = ACPIMMIO16 (XhciFwStarting + ACD_ADDR_OFFSET);
  AcdSize = ACPIMMIO16 (XhciFwStarting + ACD_SIZE_OFFSET);
  RwMem (ACPI_MMIO_BASE + XHCI_BASE + XHCI_ACPI_MMIO_AMD_REGA8, AccessWidth16, 0x0000, AcdAddress);
  RwMem (ACPI_MMIO_BASE + XHCI_BASE + XHCI_ACPI_MMIO_AMD_REGA8 + 2, AccessWidth16, 0x0000, AcdSize);

  SpiValidBase = SPI_BASE2 (XhciFwStarting + 4) | SPI_BAR0_VLD | SPI_BASE0 | SPI_BAR1_VLD | SPI_BASE1 | SPI_BAR2_VLD;
  RwMem (ACPI_MMIO_BASE + XHCI_BASE + XHCI_ACPI_MMIO_AMD_REGB0, AccessWidth32, 0x00000000, SpiValidBase);
  //
  //   Copy Type0/1/2 data block from ROM image to MMIO starting from 0xC0
  //
  for (Index = 0; Index < SPI_HEAD_LENGTH; Index++) {
    RwMem (ACPI_MMIO_BASE + XHCI_BASE + XHCI_ACPI_MMIO_AMD_REGC0 + Index, AccessWidth8, 0, ACPIMMIO8 (XhciFwStarting + Index));
  }

  for (Index = 0; Index < BcdSize; Index++) {
    RwMem (ACPI_MMIO_BASE + XHCI_BASE + XHCI_ACPI_MMIO_AMD_REGC0 + SPI_HEAD_LENGTH + Index, AccessWidth8, 0, ACPIMMIO8 (XhciFwStarting + BcdAddress + Index));
  }

  for (Index = 0; Index < AcdSize; Index++) {
    RwMem (ACPI_MMIO_BASE + XHCI_BASE + XHCI_ACPI_MMIO_AMD_REGC0 + SPI_HEAD_LENGTH + BcdSize + Index, AccessWidth8, 0, ACPIMMIO8 (XhciFwStarting + AcdAddress + Index));
  }

  //
  // Step 3. to enable the instruction RAM preload functionality.
  //
  FwAddress = ACPIMMIO16 (XhciFwStarting + FW_ADDR_OFFSET);
  RwMem (ACPI_MMIO_BASE + XHCI_BASE + XHCI_ACPI_MMIO_AMD_REGB4, AccessWidth16, 0x0000, ACPIMMIO16 (XhciFwStarting + FwAddress));
  FwAddress += 2;
  RwMem (ACPI_MMIO_BASE + XHCI_BASE + XHCI_ACPI_MMIO_AMD_REG04, AccessWidth16, 0x0000, FwAddress);

  FwSize = ACPIMMIO16 (XhciFwStarting + FW_SIZE_OFFSET);
  RwMem (ACPI_MMIO_BASE + XHCI_BASE + XHCI_ACPI_MMIO_AMD_REG04 + 2, AccessWidth16, 0x0000, FwSize);
  //
  //   Set the starting address offset for Instruction RAM preload.
  //
  RwMem (ACPI_MMIO_BASE + XHCI_BASE + XHCI_ACPI_MMIO_AMD_REG08, AccessWidth16, 0x0000, 0);

  RwMem (ACPI_MMIO_BASE + XHCI_BASE + XHCI_ACPI_MMIO_AMD_REG00, AccessWidth32, ~BIT29, BIT29);

  for (;;) {
    ReadMem (ACPI_MMIO_BASE + XHCI_BASE + XHCI_ACPI_MMIO_AMD_REG00 , AccessWidth32, &RegData);
    if (RegData & BIT30) break;
  }
  RwMem (ACPI_MMIO_BASE + XHCI_BASE + XHCI_ACPI_MMIO_AMD_REG00, AccessWidth32, ~BIT29, 0);

  //
  // Step 4. to release resets in XHCI_ACPI_MMIO_AMD_REG00. wait for USPLL to lock by polling USPLL lock.
  //

  RwMem (ACPI_MMIO_BASE + XHCI_BASE + XHCI_ACPI_MMIO_AMD_REG00, AccessWidth32, ~U3PLL_RESET, 0);       ///Release U3PLLreset
  for (;;) {
    ReadMem (ACPI_MMIO_BASE + XHCI_BASE + XHCI_ACPI_MMIO_AMD_REG00 , AccessWidth32, &RegData);
    if (RegData & U3PLL_LOCK) break;
  }

  RwMem (ACPI_MMIO_BASE + XHCI_BASE + XHCI_ACPI_MMIO_AMD_REG00, AccessWidth32, ~U3PHY_RESET, 0);       ///Release U3PHY
  RwMem (ACPI_MMIO_BASE + XHCI_BASE + XHCI_ACPI_MMIO_AMD_REG00, AccessWidth32, ~U3CORE_RESET, 0);      ///Release core reset

  //
  // SuperSpeed PHY Configuration
  //
  //RwMem (ACPI_MMIO_BASE + XHCI_BASE + XHCI_ACPI_MMIO_AMD_REG90, AccessWidth32, 0xFFF00000, 0x000AAAAA);
  //RwMem (ACPI_MMIO_BASE + XHCI_BASE + XHCI_ACPI_MMIO_AMD_REGD0, AccessWidth32, 0xFFF00000, 0x000AAAAA);

  XhciIndirectRegInit (StdHeader);

  RwMem (ACPI_MMIO_BASE + PMIO_BASE + FCH_PMIOA_REGEF, AccessWidth8, ~(BIT4 + BIT5), 0);                /// Disable Device 22
  RwMem (ACPI_MMIO_BASE + PMIO_BASE + FCH_PMIOA_REGEF, AccessWidth8, ~(BIT7), BIT7);                    /// Enable 2.0 devices
  RwMem (ACPI_MMIO_BASE + XHCI_BASE + XHCI_ACPI_MMIO_AMD_REG00, AccessWidth32, ~(BIT21), BIT21);
  //
  // Step 5.
  //
  RwMem (ACPI_MMIO_BASE + XHCI_BASE + XHCI_ACPI_MMIO_AMD_REG00, AccessWidth32, ~(BIT17 + BIT18 + BIT19), BIT17 + BIT18);

  XhciA12Patch ();

  //
  // UMI Lane Configuration Information for XHCI Firmware to Calculate the Bandwidth for USB 3.0 ISOC Devices
  //
  if (!(IsUmiX1LaneGen1Mode (StdHeader))) {
    RwMem (ACPI_MMIO_BASE + XHCI_BASE + XHCI_ACPI_MMIO_AMD_REG20, AccessWidth32, ~(BIT25 + BIT24), BIT24);
  }
  // RPR 8.23 FS/LS devices not functional after resume from S4 fix enable (SB02699)
  RwMem (ACPI_MMIO_BASE + XHCI_BASE + XHCI_ACPI_MMIO_AMD_REG20, AccessWidth32, ~(BIT22), BIT22);
  // RPR 8.24 XHC USB2.0 Hub disable issue fix enable (SB02702)
  RwMem (ACPI_MMIO_BASE + XHCI_BASE + XHCI_ACPI_MMIO_AMD_REGB4, AccessWidth32, ~(BIT20), BIT20);
}

