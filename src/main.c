#define SCABOX_TDC
//#define SCABOX_RO
#define SCABOX_AES
//#define SCABOX_PRESENT
//#define SCABOX_KLEIN
//#define SCABOX_CRYPTON

#include "main.h"

int main(int argc, char *argv[])
{

	printf("\n\n\n");
	printf("******************* Welcome in **********************\n");
	printf( "             ____________   ___          \n"
			"            / __/ ___/ _ | / _ )___ __ __\n"
			"           _\\ \\/ /__/ __ |/ _  / _ \\\\ \\ /\n"
			"          /___/\\___/_/ |_/____/\\___/_\\_\\"
	);
    printf("   v%s", SCA_PROJECT_VERSION);

	int init = -1;
    CMD_err_t *error;
    CMD_descr_tab_t tab = CMD_descr_tab();

	#ifdef SCABOX_AES
		CMD_descr_cmd_t aes_cmd = CMD_descr_cmd("aes", &aes, "AES encryptions.");
	#endif

	#ifdef SCABOX_PRESENT
		CMD_descr_cmd_t present_cmd = CMD_descr_cmd("present", &present, "Present encryptions.");
	#endif

	#ifdef SCABOX_KLEIN
		CMD_descr_cmd_t klein_cmd = CMD_descr_cmd("klein", &klein, "Klein encryptions.");
	#endif

	#ifdef SCABOX_TDC
		CMD_descr_cmd_t tdc_cmd = CMD_descr_cmd("tdc", &tdc, "TDC sensor setting and read.");
	#endif

	#ifdef SCABOX_RO
		CMD_descr_cmd_t ro_cmd = CMD_descr_cmd("ros", &ro, "RO sensor setting and read.");
	#endif

	#ifdef SCABOX_CRYPTON
		CMD_descr_cmd_t crypton_cmd = CMD_descr_cmd("crypton", &crypton, "Crypton encryptions.");
	#endif

    CMD_descr_cmd_t fifo_cmd = CMD_descr_cmd("fifo", &fifo, "FIFO setting, read and flush.");
    CMD_descr_cmd_t sca_cmd = CMD_descr_cmd("sca", &sca, "Side-channel FIFO acquisition.");

	#ifdef SCABOX_AES
		CMD_descr_push_opt(&aes_cmd, CMD_descr_opt('v', CMD_OPT_NONE, 1, "Verbose output."));
		CMD_descr_push_opt(&aes_cmd, CMD_descr_opt('c', CMD_OPT_INT, 1, "FIFO controller ID."));
		CMD_descr_push_opt(&aes_cmd, CMD_descr_opt('m', CMD_OPT_STRING, 0, "AES encryption mode."));
		CMD_descr_push_opt(&aes_cmd, CMD_descr_opt('i', CMD_OPT_NONE, 1, "Perform inverse AES encryption."));
		CMD_descr_push_opt(&aes_cmd, CMD_descr_opt('k', CMD_OPT_BYTES, 0, "AES key block."));
		CMD_descr_push_opt(&aes_cmd, CMD_descr_opt('d', CMD_OPT_BYTES, 0, "AES data block."));
		CMD_descr_push_opt(&aes_cmd, CMD_descr_opt('e', CMD_OPT_INT, 1, "Ending index to read the FIFO."));
	#endif

	#ifdef SCABOX_PRESENT
		CMD_descr_push_opt(&present_cmd, CMD_descr_opt('v', CMD_OPT_NONE, 1, "Verbose output."));
		CMD_descr_push_opt(&present_cmd, CMD_descr_opt('c', CMD_OPT_INT, 1, "FIFO controller ID."));
		CMD_descr_push_opt(&present_cmd, CMD_descr_opt('m', CMD_OPT_STRING, 0, "PRESENT encryption mode."));
		CMD_descr_push_opt(&present_cmd, CMD_descr_opt('k', CMD_OPT_BYTES, 0, "PRESENT key block."));
		CMD_descr_push_opt(&present_cmd, CMD_descr_opt('d', CMD_OPT_BYTES, 0, "PRESENT data block."));
		CMD_descr_push_opt(&present_cmd, CMD_descr_opt('e', CMD_OPT_INT, 1, "Ending index to read the FIFO."));
	#endif

	#ifdef SCABOX_KLEIN
		CMD_descr_push_opt(&klein_cmd, CMD_descr_opt('v', CMD_OPT_NONE, 1, "Verbose output."));
		CMD_descr_push_opt(&klein_cmd, CMD_descr_opt('c', CMD_OPT_INT, 1, "FIFO controller ID."));
		CMD_descr_push_opt(&klein_cmd, CMD_descr_opt('m', CMD_OPT_STRING, 0, "KLEIN encryption mode."));
		CMD_descr_push_opt(&klein_cmd, CMD_descr_opt('k', CMD_OPT_BYTES, 0, "KLEIN key block."));
		CMD_descr_push_opt(&klein_cmd, CMD_descr_opt('d', CMD_OPT_BYTES, 0, "KLEIN data block."));
		CMD_descr_push_opt(&klein_cmd, CMD_descr_opt('e', CMD_OPT_INT, 1, "Ending index to read the FIFO."));
	#endif

	#ifdef SCABOX_CRYPTON
		CMD_descr_push_opt(&crypton_cmd, CMD_descr_opt('v', CMD_OPT_NONE, 1, "Verbose output."));
		CMD_descr_push_opt(&crypton_cmd, CMD_descr_opt('c', CMD_OPT_INT, 1, "FIFO controller ID."));
		CMD_descr_push_opt(&crypton_cmd, CMD_descr_opt('m', CMD_OPT_STRING, 0, "CRYPTON encryption mode."));
		CMD_descr_push_opt(&crypton_cmd, CMD_descr_opt('k', CMD_OPT_BYTES, 0, "CRYPTON key block."));
		CMD_descr_push_opt(&crypton_cmd, CMD_descr_opt('d', CMD_OPT_BYTES, 0, "CRYPTON data block."));
		CMD_descr_push_opt(&crypton_cmd, CMD_descr_opt('e', CMD_OPT_INT, 1, "Ending index to read the FIFO."));
	#endif

	#ifdef SCABOX_RO
		CMD_descr_push_opt(&ro_cmd, CMD_descr_opt('v', CMD_OPT_NONE, 1, "Verbose output."));
		CMD_descr_push_opt(&ro_cmd, CMD_descr_opt('r', CMD_OPT_INT, 1, "Read raw RO sensors value."));
	#endif

	#ifdef SCABOX_TDC
		CMD_descr_push_opt(&tdc_cmd, CMD_descr_opt('v', CMD_OPT_NONE, 1, "Verbose output."));
		CMD_descr_push_opt(&tdc_cmd, CMD_descr_opt('s', CMD_OPT_INT, 1, "Read TDC sensors state."));
		CMD_descr_push_opt(&tdc_cmd, CMD_descr_opt('c', CMD_OPT_INT, 1, "Perform automatic calibration."));
		CMD_descr_push_opt(&tdc_cmd, CMD_descr_opt('d', CMD_OPT_INT, 1, "Perform manual calibration."));
	#endif

    CMD_descr_push_opt(&fifo_cmd, CMD_descr_opt('v', CMD_OPT_NONE, 1, "Verbose output."));
    CMD_descr_push_opt(&fifo_cmd, CMD_descr_opt('c', CMD_OPT_INT, 1, "FIFO controller ID."));
    CMD_descr_push_opt(&fifo_cmd, CMD_descr_opt('f', CMD_OPT_NONE, 1, "Flush the FIFO."));
    CMD_descr_push_opt(&fifo_cmd, CMD_descr_opt('s', CMD_OPT_INT, 1, "Starting index to read the FIFO."));
    CMD_descr_push_opt(&fifo_cmd, CMD_descr_opt('e', CMD_OPT_INT, 1, "Ending index to read the FIFO."));
    CMD_descr_push_opt(&fifo_cmd, CMD_descr_opt('a', CMD_OPT_NONE, 1, "Perform FIFO acquisition."));

    CMD_descr_push_opt(&sca_cmd, CMD_descr_opt('v', CMD_OPT_NONE, 1, "Verbose output."));
    CMD_descr_push_opt(&sca_cmd, CMD_descr_opt('c', CMD_OPT_INT, 1, "FIFO controller ID."));
    CMD_descr_push_opt(&sca_cmd, CMD_descr_opt('m', CMD_OPT_STRING, 0, "encryption mode."));
    CMD_descr_push_opt(&sca_cmd, CMD_descr_opt('i', CMD_OPT_NONE, 1, "Perform inverse encryption."));
    CMD_descr_push_opt(&sca_cmd, CMD_descr_opt('t', CMD_OPT_INT, 0, "Count of target iterations."));
    CMD_descr_push_opt(&sca_cmd, CMD_descr_opt('s', CMD_OPT_INT, 1, "Starting index to read the FIFO."));
    CMD_descr_push_opt(&sca_cmd, CMD_descr_opt('e', CMD_OPT_INT, 1, "Ending index to read the FIFO."));
    CMD_descr_push_opt(&sca_cmd, CMD_descr_opt('r', CMD_OPT_NONE, 1, "Perform raw FIFO acquisition before encryption."));


	#ifdef SCABOX_AES
		CMD_descr_push_cmd(&tab, aes_cmd);
	#endif

	#ifdef SCABOX_TDC
		CMD_descr_push_cmd(&tab, tdc_cmd);
	#endif

	#ifdef SCABOX_RO
		CMD_descr_push_cmd(&tab, ro_cmd);
	#endif

	#ifdef SCABOX_PRESENT
		CMD_descr_push_cmd(&tab, present_cmd);
	#endif

	#ifdef SCABOX_KLEIN
		CMD_descr_push_cmd(&tab, klein_cmd);
	#endif

	#ifdef SCABOX_CRYPTON
		CMD_descr_push_cmd(&tab, crypton_cmd);
	#endif

	CMD_descr_push_cmd(&tab, fifo_cmd);
	CMD_descr_push_cmd(&tab, sca_cmd);

    printf("\n\n******************** IP Cores ***********************\n\n");

	printf("FIFO Addr: 0x%08lx - Depth: %lu - Width: %lubit\n",XFIFO_ConfigTable[0].BaseAddr,XFIFO_ConfigTable[0].Depth,XFIFO_ConfigTable[0].Width);
    init = XFIFO_CfgInitialize(&fifo_inst[0], &XFIFO_ConfigTable[0], XFIFO_ConfigTable[0].BaseAddr);
	if(init != -1){printf("SCABox> Init OK\n");} else{printf("SCABox> Init Error\n");}

	#ifdef SCABOX_AES
		printf("AES Addr: 0x%08lx\n",XAES_ConfigTable[0].BaseAddr);
		init = XAES_CfgInitialize(&aes_inst, &XAES_ConfigTable[0]);
		if(init != -1){printf("SCABox> Init OK\n");} else{printf("SCABox> Init Error\n");}
	#endif

	#ifdef SCABOX_PRESENT
		printf("PRESENT Addr: %08lx\n",XPRESENT_ConfigTable[0].BaseAddr);
		init = XPRESENT_CfgInitialize(&present_inst, &XPRESENT_ConfigTable[0]);
		if(init != -1){printf("SCABox> Init OK\n");} else{printf("SCABox> Init Error\n");}
	#endif

	#ifdef SCABOX_KLEIN
		printf("KLEIN Addr: 0x%08lx\n",XKLEIN_ConfigTable[0].BaseAddr);
		init = XKLEIN_CfgInitialize(&klein_inst, &XKLEIN_ConfigTable[0]);
		if(init != -1){printf("SCABox> Init OK\n");} else{printf("SCABox> Init Error\n");}
	#endif

	#ifdef SCABOX_CRYPTON
		printf("CRYPTON Addr: 0x%08lx\n",XCRYPTON_ConfigTable[0].BaseAddr);
		init = XCRYPTON_CfgInitialize(&crypton_inst, &XCRYPTON_ConfigTable[0]);
		if(init != -1){printf("SCABox> Init OK\n");} else{printf("SCABox> Init Error\n");}
	#endif

	#ifdef SCABOX_TDC
		printf("TDC Addr: 0x%08lx - Quantization: %u levels - Number: %u\n",XTDC_ConfigTable[0].BaseAddr,XTDC_ConfigTable[0].Depth*4,XTDC_ConfigTable[0].Count);
		init = XTDC_CfgInitialize(&tdc_inst, &XTDC_ConfigTable[0]);
		XTDC_Calibrate(&tdc_inst, 0, 0);
		if(init != -1){printf("SCABox> Init OK\n");} else{printf("SCABox> Init Error\n");}

	#endif

	#ifdef SCABOX_RO
		printf("RO addr: 0x%08lx\n",XTDC_ConfigTable[0].BaseAddr);
		init = XRO_CfgInitialize(&ro_inst, &XRO_ConfigTable[0]);
		if(init != -1){printf("-> Init OK\n");} else{printf("-> Init Error\n");}
	#endif
	printf("\n*****************************************************\n\n");

	printf("Type man to display information about built-in commands\n\n");
    init_perfcounters(1,0);
    if ((error = CMD_run(&tab)) != NULL)
    {
        fprintf(stderr, "%s\n", error->message);
        free(error);
        return 1;
    }
    return 0;
}
