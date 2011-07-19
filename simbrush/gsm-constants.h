/******************************************************************************
 *
 * NAME:          gsm-constants.h
 *
 * AUTHORS:       Fabio Casadei
 *		  fabio.casadei@mercurio-ws.com
 *
 *		  Antonio Savoldi
 *		  antonio.savoldi@ing.unibs.it
 *
 * FILE TYPE:     Header file
 *
 * DESCRIPTION:   this file contains the definition of constants defined in
 *                GSM standards 
 *
 *****************************************************************************/

// INSTRUCTION CLASS CODES DEFINITION

#define GSM_CLASS 0xA0


// INSTRUCTION CODES DEFINITION

#define SELECT            0xA4
#define STATUS            0xF2
#define READ_BINARY       0xB0
#define UPDATE_BINARY     0xD6
#define READ_RECORD       0xB2
#define UPDATE_RECORD     0xDC
#define SEEK              0xA2
#define INCREASE          0x32
#define VERIFY_CHV        0x20
#define CHANGE_CHV        0x24
#define DISABLE_CHV       0x26
#define ENABLE_CHV        0x28
#define UNBLOCK_CHV       0x2C
#define INVALIDATE        0x04
#define REHABILITATE      0x44
#define RUN_GSM_ALGORITHM 0x88
#define SLEEP             0xFA
#define GET_RESPONSE      0xC0
#define TERMINAL_PROFILE  0x10
#define ENVELOPE          0xC2
#define FETCH             0x12
#define TERMINAL_RESPONSE 0x14


// STATUS WORD 1 (SW1) CODES DEFINITION

#define SW1_CORRECT                         0x90 // commands correctly executed
#define SW1_CORRECT_WITH_PROACTIVE_COMMAND  0x91
#define SW1_CORRECT_SIM_DATA_DOWNLOAD_ERROR 0x9E
#define SW1_CORRECT_WITH_RESPONSE           0x9F

#define SW1_SAT_BUSY 0x93 // commands postponed

#define SW1_MEMORY_NOTE 0x92 // answer regarding memory

#define SW1_REFERENCES 0x94 // answer regarding references

#define SW1_SECURITY 0x98 // answer regarding security management

#define SW1_ERROR_P3                0x67 // application independent errors
                                         // signalling
#define SW1_ERROR_P1_P2             0x6B
#define SW1_ERROR_INSTRUCTION_CODE  0x6D
#define SW1_ERROR_INSTRUCTION_CLASS 0x6E
#define SW1_ERROR_TECHNICAL         0x6F


// STATUS WORD 2 (SW2) CODES DEFINITIONS
// Security related:
#define SW2_CHV_NOT_INITIALIZED            0x02
#define SW2_BAD_ACCCOND_VERIFY_MORE_TRIES  0x04
#define SW2_CONTRADICTION_CHV              0x08
#define SW2_CONTRADICTION_INVALIDATION     0x10
#define SW1_VERIFY_NO_TRIES_CHV_BLOCKED    0x40
#define SW1_INCREASE_NOT_ALLOWED           0x50


// MF, DF AND EF IDENTIFIERS DEFINITION        <--- DA TOGLIERE ?

#define MF          0x3FFF // Master File

#define DFgsm       0x7F20 // Dedicated Files (Directory)
#define DFtelecom   0x7F10

#define EFiccid     0x2FE2 // Elementary Files under MF

#define EFimsi      0x6F07 // Elementary Files under DFgsm

#define EFadn       0x6F3A // Elementary Files under DFtelecom


// EF ACCESS CONDITION CODES DEFINITION (DF and MF does not have such
// permissions in the present version of the GSM)

#define ACC_ALW  0x00 // always
#define ACC_CHV1 0x01 // after CHV1 verification
#define ACC_CHV2 0x02 // after CHV2 verification
#define ACC_RFU  0x03
#define ACC_ADM4 0x04 // administrative codes (ALW, CHV1, CHV2 and NEV can be
                      // used as administrative codes too): they have meaning
                      // to the administrative authority only
#define ACC_ADM5 0x05
#define ACC_ADM6 0x06
#define ACC_ADM7 0x07
#define ACC_ADM8 0x08
#define ACC_ADM9 0x09
#define ACC_ADMA 0x0A
#define ACC_ADMB 0x0B
#define ACC_ADMC 0x0C
#define ACC_ADMD 0x0D
#define ACC_ADME 0x0E
#define ACC_NEV  0x0F // never


// FILE TYPE CODES DEFINITION

#define FTYP_RFU 0x00
#define FTYP_MF  0x01
#define FTYP_DF  0x02
#define FTYP_EF  0x04


// FILE STRUCTURE CODES DEFINITION

#define FSTR_TRANSP 0x00
#define FSTR_LINFIX 0x01
#define FSTR_CYCLIC 0x03


// FILE STATUS CHECKING DEFINITIONS

#define FSTATE_MASK   0x05 // This mask anded with byte 12 of GET RESPONSE
                           // result gives:
#define FSTATE_DEATH  0x00 // --> invalidated and not readable nor updatable
#define FSTATE_ASLEEP 0x04 // --> invalidated but readable and updatable
#define FSTATE_ALIVE  0x01 // --> valid but not readable nor updatable if
                           //     invalidated
#define FSTATE_AWAKEN 0x05 // --> valid and readable and updatable when
                           //     invalidated

// GSM RELATED MESSAGES AND WARNINGS

#define WRN_NO_CHV_INITIALIZED "No CHV has been initialized."
#define WRN_ACCCOND_NOT_FULLFILLED "Access condition non fullfilled."
#define WRN_BAD_CHV_VERIFY_MORE_TRIES "Unsuccessful CHV verification, at least one attempt left."
#define WRN_BAD_UNBLOCK_VERIFY_MORE_TRIES "Unsuccessful UNBLOCK CHV verification, at least one attempt left."
#define WRN_AUTH_FAILED "Authentication failed."
#define WRN_BROKEN_CHV_CONSTRAINT "In contradiction with CHV status."
#define WRN_BROKEN_INVALIDATION_CONSTRAINT "In contradiction with invalidation status."
#define WRN_BAD_CHV_VERIFY_NO_TRIES "Unsuccessful CHV verification, no attempt left."
#define WRN_BAD_UNBLOCK_VERIFY_NO_TRIES "Unsuccessful UNBLOCK CHV verification, no attempt left."
#define WRN_CHV_BLOCKED "CHV blocked."
#define WRN_UNBLOCK_CHV_BLOCKED "UNBLOCK CHV blocked."
#define WRN_INCREASE_NOT_ALLOWED "Increase cannot be performed, Max value reached."


// GSM STANDARD FILE
  static long int EF_list[] = {
			
			//Standard Ddicated Fle (DF)
			
			0x7F20, // GSM 			:: MF -> DF_GSM			
	  		0x7F10, // TELECOM 		:: MF -> DF_TELECOM 			
			0x7F22, // IS-41 		:: MF -> DF_IS-41
			0x7F23, // FP-CTS 		:: MF -> DF_FP-CTS
			
			0x5F50, // GRAPHICS 		:: MF -> DF_TELECOM -> DF_GRAPHICS
			
			0x5F30, // IRIDIUM
  			0x5F31, // GLOBST
  			0x5F32, // ICO
  			0x5F33, // ACeS 		:: MF -> DF_GSM 
			
  			0x5F40, // EIA/TIA-553
  			0x5F60, // CTS
  			0x5F70, // SoLSA 		:: MF -> DF_GSM
			
  			0x5F3C, // MExE 		:: MF -> DF_GSM -> EF_MExE
			
			
			//Standard Elementary File (EF)
			
			// MF(0x3F00) -> EF_MF
	  		0x2FE2, // ICCID
  			0x2F05, // ELP
			
			// MF -> DF_TELECOM(0x7F10) -> EF_TELECOM
			0x6F3A, // ADN
  			0x6F3B, // FDN
  			0x6F3C, // SMS
  			0x6F3D, // CCP
  			0x6F40, // MSISDN
  			
			0x6F42, // SMSP
  			0x6F43, // SMSS
  			0x6F44, // LND
  			0x6F47, // SMSR
  			0x6F49, // SDN
  			
			0x6F4A, // EXT1
  			0x6F4B, // EXT2
  			0x6F4C, // EXT3
  			0x6F4D, // BDN
  			0x6F4E, // EXT4
			
			// MF(0x3F00) -> DF_TELECOM(0x7F10) -> DF_GRAPHICS(0x5F50) -> EF_IMG
			0x4F20,
			
			// MF -> DF_GSM(0x7F20) -> DF_SoSLA(0x5F70) -> EF_SoSLA
  			0x4F30, // SAI
			0x4F31, // SLL
			
			// MF -> DF_GSM(0x7F20) -> DF_MExE(0x5F3C) -> EF_MExE
  			0x4F40, // MExE-ST
			0x4F41, // ORPK
			0x4F42, // ARPK
			0x4F43, // TPRPK
			
			// MF -> DF_GSM(0x7F20) -> EF_GSM
			0x6F05, // LP
			0x6F07, // IMSI
			0x6F20, // Kc
			0x6F30, // PLMNsel
			0x6F31, // HPPLMN
			0x6F37, // ACMmax

			0x6F38, // SST
			0x6F39, // ACM
			0x6F3E, // GID1
			0x6F3F, // GID2
			0x6F41, // PUCT
			0x6F45, // CBMI

			0x6F46, // SPN
			0x6F48, // CBMID
			0x6F74, // BCCH
			0x6F78, // ACC
			0x6F7B, // FPLMN
			0x6F7E, // LOCI

			0x6FAD, // AD
			0x6FAE, // PHASE
			0x6FB1, // VGCS
			0x6FB2, // VGCSS
			0x6FB3, // VBS
			0x6FB4, // VBSS

			0x6FB5, // eMLPP
			0x6FB6, // AAeM
			0x6FB7, // ECC
			0x6F50, // CBMIR
			0x6F51, // NIA
			0x6F52, // KcGPRS

			0x6F53, // LOCIGPRS
			0x6F54, // SUME
			0x6F60, // PLMNwAcT
			0x6F61, // OPLMNwAcT
			0x6F62, // HPLMNAcT
			0x6F63, // CPBCCH

			0x6F64 // INVSCAN
						
  };  	       	       
