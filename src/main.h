#include <limits.h>
#include <cmd/hex.h>
#include <cmd/run.h>
#include "xparameters.h"
#include "xfifo.h"
#include "math.h"

// Programmable Includes
#ifdef SCABOX_TDC
#include "xtdc.h"
#endif

#ifdef SCABOX_RO
#include "xro.h"
#endif

#ifdef SCABOX_AES
#include "xaes.h"
#include <tiny-AES-c/aes.h>
#include <openssl/aes.h>
#include <AES/aes.h>
#endif

#ifdef SCABOX_PRESENT
#include "xpresent.h"
#endif

#ifdef SCABOX_KLEIN
#include "xklein.h"
#endif

#ifdef SCABOX_CRYPTON
#include "xcrypton.h"
#endif

#define SCA_PROJECT_VERSION "1.2.0"

XFIFO fifo_inst[2];

#ifdef SCABOX_AES
	XAES aes_inst;
	uint32_t key_aes[XAES_WORDS_SIZE], block_aes[XAES_WORDS_SIZE];
	uint8_t key_tiny[16], in_tiny[16];
	unsigned char in_ssl[AES_BLOCK_SIZE], out_ssl[AES_BLOCK_SIZE], key_ssl[AES_BLOCK_SIZE];
	uint8_t in_dhuertas[16], out_dhuertas[16], key_dhuertas[16];
	AES_KEY key32_ssl;
	uint8_t* ctx_dhuertas;
	struct AES_ctx ctx_tiny;
#endif

#ifdef SCABOX_PRESENT
	XPRESENT present_inst;
	uint32_t key_present[XPRESENT_WORDS_SIZE], block_present[XPRESENT_WORDS_SIZE-2];
#endif

#ifdef SCABOX_KLEIN
	XKLEIN klein_inst;
	uint32_t key_klein[XKLEIN_WORDS_SIZE], block_klein[XKLEIN_WORDS_SIZE-2];
#endif

#ifdef SCABOX_CRYPTON
	XCRYPTON crypton_inst;
	uint32_t key_crypton[XCRYPTON_WORDS_SIZE], block_crypton[XCRYPTON_WORDS_SIZE-2];
#endif

#ifdef SCABOX_RO
	XRO ro_inst;
#endif

#ifdef SCABOX_TDC
	XTDC tdc_inst;
#endif

char buffer[512];
int t_start, t_end;
int seed = 1;

#ifdef SCABOX_AES

	static void AesHwHandler(void *CallBackRef)
	{
		XAES_Run(&aes_inst);
	}

	static void AesTinyDecryptHandler(void *CallBackRef)
	{
		AES_ECB_decrypt(&ctx_tiny,in_tiny);
	}

	static void AesTinyEncryptHandler(void *CallBackRef)
	{
		AES_ECB_encrypt(&ctx_tiny,in_tiny);
	}

	static void AesSslDecryptHandler(void *CallBackRef)
	{
		AES_decrypt(in_ssl, out_ssl, &key32_ssl);
	}

	static void AesSslEncryptHandler(void *CallBackRef)
	{
		AES_encrypt(in_ssl, out_ssl, &key32_ssl);
	}

	static void AesDhuertasEncryptHandler(void *CallBackRef)
	{
		asm volatile ("MRC p15, 0, %0, c9, c13, 0\t\n": "=r"(t_start));
		aes_cipher(in_dhuertas, out_dhuertas, ctx_dhuertas);
		asm volatile ("MRC p15, 0, %0, c9, c13, 0\t\n": "=r"(t_end));

	}

	static void AesDhuertasDecryptHandler(void *CallBackRef)
{
    aes_inv_cipher(in_dhuertas, out_dhuertas, ctx_dhuertas);
}

#endif

#ifdef SCABOX_PRESENT
	static void PresentHwHandler(void *CallBackRef)
	{
		XPRESENT_Run(&present_inst);
	}
#endif

#ifdef SCABOX_KLEIN
	static void KleinHwHandler(void *CallBackRef)
	{
		XKLEIN_Run(&klein_inst);
	}
#endif

#ifdef SCABOX_CRYPTON
	static void CryptonHwHandler(void *CallBackRef)
	{
		XCRYPTON_Run(&crypton_inst);
	}
#endif

char *weights_to_ascii(char *str, int *weights, size_t len)
{
	int min = 256;
	int overflow = 0;
	float avgWeights = 0.0;
	int avg = 0;

	// weigths sum
    for (size_t t = 0; t < len; t++){  avgWeights += weights[t];  }

    // weights mean
    avg = (int)(floor(avgWeights / len));

    // remove mean from each weight index and find the minimal value
    for (size_t t = 0; t < len; t++)
    {
    	if(weights[t] == 0)
    	{
    		// remove weights equal to 0 (bug?)
    	}
    	else
    	{
    		weights[t] -= avg;
    	}

    	if(weights[t] < min)
        {
        	min = weights[t];
        }
    }

    /*printf("weights = ");
    for(int i = 0 ; i < len ; i++)
	{
		printf("%d ",weights[i]);
	}
    printf("\n");*/

    // print ascii caracters in range 33-256
    for (size_t t = 0; t < len; t++)
    {
    	str[t] = weights[t]+abs(min)+33;

        if (str[t] > 255)
        {
        	overflow = 1;
            str[t] = 255;
        }

        //printf("%d,",str[t]);
		/*if (str[t] == '\0' || str[t] == '\n' || str[t] == '\r')
		{
			str[t]++;
		}*/
    }

    if(overflow)
    {
    	printf("ascii overflow was detected, the sensor value cannot be encoded using 8-bit representation\n\r");
    }

    str[len] = '\0';
    return str;
}

char *weights_to_string(char *str, int *weights, size_t len)
{
    if (len == 0)
    {
        return str;
    }

    char *ptr = str;
    for (size_t t = 0; t < len - 1; t++)
    {
        sprintf(ptr, "%d,", weights[t]);
        ptr += strlen(ptr);
    }
    sprintf(ptr, "%d", weights[len - 1]);
    return str;
}

void init_perfcounters(int do_reset, int enable_divider)
{
    // in general enable all counters (including cycle counter)
    int value = 1;

    // peform reset:
    if (do_reset)
    {
        value |= 2; // reset all counters to zero.
        value |= 4; // reset cycle counter to zero.
    }

    if (enable_divider)
        value |= 8; // enable "by 1" divider for CCNT.

    value |= 16;

    // program the performance-counter control-register:
    asm volatile("MCR p15, 0, %0, c9, c12, 0\t\n" ::"r"(value));

    // enable all counters:
    asm volatile("MCR p15, 0, %0, c9, c12, 1\t\n" ::"r"(0x8000000f));

    // clear overflows:
    asm volatile("MCR p15, 0, %0, c9, c12, 3\t\n" ::"r"(0x8000000f));

    // program the performance-counter control-register:
    asm volatile("MCR p15, 0, %0, c9, c12, 0\t\n" ::"r"(value));
}

#ifdef SCABOX_AES

	void tiny_aes(int inv, int verbose, int end, int id)
	{
		if (verbose)
		{
			printf("mode: tiny\n");
			printf("direction: %s\n", inv ? "dec" : "enc");
			printf("key: %s\n", HEX_bytes_to_string(buffer, key_tiny, 16));
			printf("%s: %s\n", inv ? "ciphers" : "plains", HEX_bytes_to_string(buffer, in_tiny, 16));
		}

		AES_init_ctx(&ctx_tiny, key_tiny);
		fifo_inst[id].Mode = XFIFO_MODE_SW;
		XFIFO_Reset(&fifo_inst[id]);
		XFIFO_Write(&fifo_inst[id], end, (XFIFO_WrAction)(inv ? AesTinyDecryptHandler : AesTinyEncryptHandler));

		printf("%s: %s;;\n", inv ? "plains" : "ciphers", HEX_bytes_to_string(buffer, in_tiny, 16));
	}

	void ssl_aes(int inv, int verbose, int end, int id)
	{
		if (verbose)
		{
			printf("mode: ssl\n");
			printf("direction: %s\n", inv ? "dec" : "enc");
			printf("key: %s\n", HEX_words_to_string(buffer, key_ssl, 4));
			printf("%s: %s\n", inv ? "ciphers" : "plains", HEX_bytes_to_string(buffer, in_ssl, AES_BLOCK_SIZE));
		}

		if (inv)
		{
			AES_set_decrypt_key(key_ssl, 128, &key32_ssl);
		}
		else
		{
			AES_set_encrypt_key(key_ssl, 128, &key32_ssl);
		}


		fifo_inst[id].Mode = XFIFO_MODE_SW;
		XFIFO_Reset(&fifo_inst[id]);
		XFIFO_Write(&fifo_inst[id], end, (XFIFO_WrAction)(inv ? AesSslDecryptHandler : AesSslEncryptHandler));

		printf("%s: %s;;\n", inv ? "plains" : "ciphers", HEX_bytes_to_string(buffer, out_ssl, AES_BLOCK_SIZE));
	}

	void dhuertas_aes(int inv, int verbose, int end, int id)
	{
		if (verbose)
		{
			printf("mode: ssl\n");
			printf("direction: %s\n", inv ? "dec" : "enc");
			printf("key: %s\n", HEX_bytes_to_string(buffer, key_dhuertas, 16));
			printf("%s: %s\n", inv ? "ciphers" : "plains", HEX_bytes_to_string(buffer, in_dhuertas, AES_BLOCK_SIZE));
		}

		free(ctx_dhuertas);
		ctx_dhuertas = aes_init(16);
		aes_key_expansion(key_dhuertas, ctx_dhuertas);
		fifo_inst[id].Mode = XFIFO_MODE_SW;
		XFIFO_Reset(&fifo_inst[id]);
		XFIFO_Write(&fifo_inst[id], end, (XFIFO_WrAction)(inv ? AesDhuertasDecryptHandler : AesDhuertasEncryptHandler));

		printf("%s: %s;;\n", inv ? "plains" : "ciphers", HEX_bytes_to_string(buffer, out_dhuertas, AES_BLOCK_SIZE));
	}

	void hw_aes(int inv, int verbose, int end, int id)
	{
		if (verbose)
		{
			printf("mode: hw\n");
			printf("direction: %s\n", inv ? "dec" : "enc");
			printf("keys: %s\n", HEX_words_to_string(buffer, key_aes, XAES_WORDS_SIZE));
			printf("%s: %s\n", inv ? "ciphers" : "plains", HEX_words_to_string(buffer, block_aes, XAES_WORDS_SIZE));
		}

		XAES_Reset(&aes_inst, inv ? XAES_DECRYPT : XAES_ENCRYPT);
		XAES_SetKey(&aes_inst, key_aes);
		XAES_SetInput(&aes_inst, block_aes);

		fifo_inst[id].Mode = XFIFO_MODE_HW;
		XFIFO_Reset(&fifo_inst[id]);
		XFIFO_Write(&fifo_inst[id], end, (XFIFO_WrAction)AesHwHandler);

		XAES_GetOutput(&aes_inst, block_aes);
		printf("%s: %s;;\n", inv ? "plains" : "ciphers", HEX_words_to_string(buffer, block_aes, XAES_WORDS_SIZE));
	}

	CMD_err_t *aes(const CMD_cmd_t *cmd)
{
    int data_idx = CMD_opt_find(cmd->options, 'd');
    int key_idx = CMD_opt_find(cmd->options, 'k');
    int mode_idx = CMD_opt_find(cmd->options, 'm');
    int inv = CMD_opt_find(cmd->options, 'i') != -1;
    int verbose = CMD_opt_find(cmd->options, 'v') != -1;
    int end = CMD_opt_find(cmd->options, 'e');
    int id = CMD_opt_find(cmd->options, 'c');
    char *mode = cmd->options[mode_idx].value.string;

    end = end != -1 ? cmd->options[end].value.integer : XFIFO_ConfigTable[0].Depth-1;
    id = id != -1 ? cmd->options[id].value.integer : 0;

    if (!strcmp(mode, "hw"))
    {
        HEX_bytes_to_words(key_aes, cmd->options[key_idx].value.bytes, XAES_BYTES_SIZE);
        HEX_bytes_to_words(block_aes, cmd->options[data_idx].value.bytes, XAES_BYTES_SIZE);
        hw_aes(inv, verbose, end, id);
    }
    else if (!strcmp(mode, "tiny"))
    {
        memcpy(key_tiny, cmd->options[key_idx].value.bytes, 16);
        memcpy(in_tiny, cmd->options[data_idx].value.bytes, 16);
        tiny_aes(inv, verbose, end, id);
    }
    else if (!strcmp(mode, "ssl"))
    {
        memcpy(key_ssl, cmd->options[key_idx].value.bytes, AES_BLOCK_SIZE);
        memcpy(in_ssl, cmd->options[data_idx].value.bytes, AES_BLOCK_SIZE);
        ssl_aes(inv, verbose, end, id);
    }
    else if (!strcmp(mode, "dhuertas"))
    {
        memcpy(key_dhuertas, cmd->options[key_idx].value.bytes, AES_BLOCK_SIZE);
        memcpy(in_dhuertas, cmd->options[data_idx].value.bytes, AES_BLOCK_SIZE);
        dhuertas_aes(inv, verbose, end, id);
    }
    else
    {
        printf("unrecognized encryption mode: %s", mode);
    }
    return NULL;
}

#endif

#ifdef SCABOX_PRESENT

	void hw_present(int inv, int verbose, int end, int id)
	{
		if (verbose)
		{
			printf("mode: hw\n");
			printf("direction: %s\n", inv ? "dec" : "enc");
			printf("keys: %s\n", HEX_words_to_string(buffer, key_present, XPRESENT_WORDS_SIZE));
			printf("%s: %s\n", inv ? "ciphers" : "plains", HEX_words_to_string(buffer, block_present, XPRESENT_WORDS_SIZE-2));
		}

		XPRESENT_Reset(&present_inst, inv ? XPRESENT_ENCRYPT : XPRESENT_ENCRYPT);
		XPRESENT_SetKey(&present_inst, key_present);
		XPRESENT_SetInput(&present_inst, block_present);

		fifo_inst[id].Mode = XFIFO_MODE_HW;
		XFIFO_Reset(&fifo_inst[id]);
		XFIFO_Write(&fifo_inst[id], end, (XFIFO_WrAction)PresentHwHandler);

		XPRESENT_GetOutput(&present_inst, block_present);
		printf("%s: %s;;\n", inv ? "plains" : "ciphers", HEX_words_to_string(buffer, block_present, XPRESENT_WORDS_SIZE-2));
	}

	CMD_err_t *present(const CMD_cmd_t *cmd)
{
    int data_idx = CMD_opt_find(cmd->options, 'd');
    int key_idx = CMD_opt_find(cmd->options, 'k');
    int mode_idx = CMD_opt_find(cmd->options, 'm');
    int inv = CMD_opt_find(cmd->options, 'i') != -1;
    int verbose = CMD_opt_find(cmd->options, 'v') != -1;
    int end = CMD_opt_find(cmd->options, 'e');
    int id = CMD_opt_find(cmd->options, 'c');
    char *mode = cmd->options[mode_idx].value.string;

    end = end != -1 ? cmd->options[end].value.integer : XFIFO_ConfigTable[0].Depth-1;
    id = id != -1 ? cmd->options[id].value.integer : 0;

    if (!strcmp(mode, "hw"))
    {
        HEX_bytes_to_words(key_present, cmd->options[key_idx].value.bytes, XPRESENT_BYTES_SIZE);
        HEX_bytes_to_words(block_present, cmd->options[data_idx].value.bytes, XPRESENT_BYTES_SIZE);
        hw_present(inv, verbose, end, id);
    }
    else
    {
        printf("unrecognized encryption mode: %s", mode);
    }
    return NULL;
}

#endif

#ifdef SCABOX_KLEIN

	void hw_klein(int inv, int verbose, int end, int id)
	{
		if (verbose)
		{
			printf("mode: hw\n");
			printf("direction: %s\n", inv ? "dec" : "enc");
			printf("keys: %s\n", HEX_words_to_string(buffer, key_klein, XKLEIN_WORDS_SIZE-2));
			printf("%s: %s\n", inv ? "ciphers" : "plains", HEX_words_to_string(buffer, block_klein, XKLEIN_WORDS_SIZE-2));
		}

		XKLEIN_Reset(&klein_inst, inv ? XKLEIN_ENCRYPT : XKLEIN_ENCRYPT);
		XKLEIN_SetKey(&klein_inst, key_klein);
		XKLEIN_SetInput(&klein_inst, block_klein);

		fifo_inst[id].Mode = XFIFO_MODE_HW;
		XFIFO_Reset(&fifo_inst[id]);
		XFIFO_Write(&fifo_inst[id], end, (XFIFO_WrAction)KleinHwHandler);

		XKLEIN_GetOutput(&klein_inst, block_klein);
		printf("%s: %s;;\n", inv ? "plains" : "ciphers", HEX_words_to_string(buffer, block_klein, XKLEIN_WORDS_SIZE-2));
	}

	CMD_err_t *klein(const CMD_cmd_t *cmd)
{
    int data_idx = CMD_opt_find(cmd->options, 'd');
    int key_idx = CMD_opt_find(cmd->options, 'k');
    int mode_idx = CMD_opt_find(cmd->options, 'm');
    int inv = CMD_opt_find(cmd->options, 'i') != -1;
    int verbose = CMD_opt_find(cmd->options, 'v') != -1;
    int end = CMD_opt_find(cmd->options, 'e');
    int id = CMD_opt_find(cmd->options, 'c');
    char *mode = cmd->options[mode_idx].value.string;

    end = end != -1 ? cmd->options[end].value.integer : XFIFO_ConfigTable[0].Depth-1;
    id = id != -1 ? cmd->options[id].value.integer : 0;

    if (!strcmp(mode, "hw"))
    {
        HEX_bytes_to_words(key_klein, cmd->options[key_idx].value.bytes, XKLEIN_BYTES_SIZE);
        HEX_bytes_to_words(block_klein, cmd->options[data_idx].value.bytes, XKLEIN_BYTES_SIZE);
        hw_klein(inv, verbose, end, id);
    }
    else
    {
        printf("unrecognized encryption mode: %s", mode);
    }
    return NULL;
}

#endif

#ifdef SCABOX_CRYPTON

	void hw_crypton(int inv, int verbose, int end, int id)
	{
		if (verbose)
		{
			printf("mode: hw\n");
			printf("direction: %s\n", inv ? "dec" : "enc");
			printf("keys: %s\n", HEX_words_to_string(buffer, key_crypton, XCRYPTON_WORDS_SIZE-2));
			printf("%s: %s\n", inv ? "ciphers" : "plains", HEX_words_to_string(buffer, block_crypton, XCRYPTON_WORDS_SIZE-2));
		}

		XCRYPTON_Reset(&crypton_inst, inv ? XCRYPTON_ENCRYPT : XCRYPTON_ENCRYPT);
		XCRYPTON_SetKey(&crypton_inst, key_crypton);
		XCRYPTON_SetInput(&crypton_inst, block_crypton);

		fifo_inst[id].Mode = XFIFO_MODE_HW;
		XFIFO_Reset(&fifo_inst[id]);
		XFIFO_Write(&fifo_inst[id], end, (XFIFO_WrAction)CryptonHwHandler);

		XCRYPTON_GetOutput(&crypton_inst, block_crypton);
		printf("%s: %s;;\n", inv ? "plains" : "ciphers", HEX_words_to_string(buffer, block_crypton, XCRYPTON_WORDS_SIZE));
	}

	CMD_err_t *crypton(const CMD_cmd_t *cmd)
{
    int data_idx = CMD_opt_find(cmd->options, 'd');
    int key_idx = CMD_opt_find(cmd->options, 'k');
    int mode_idx = CMD_opt_find(cmd->options, 'm');
    int inv = CMD_opt_find(cmd->options, 'i') != -1;
    int verbose = CMD_opt_find(cmd->options, 'v') != -1;
    int end = CMD_opt_find(cmd->options, 'e');
    int id = CMD_opt_find(cmd->options, 'c');
    char *mode = cmd->options[mode_idx].value.string;

    end = end != -1 ? cmd->options[end].value.integer : XFIFO_ConfigTable[0].Depth-1;
    id = id != -1 ? cmd->options[id].value.integer : 0;

    if (!strcmp(mode, "hw"))
    {
        HEX_bytes_to_words(key_crypton, cmd->options[key_idx].value.bytes, XCRYPTON_BYTES_SIZE);
        HEX_bytes_to_words(block_crypton, cmd->options[data_idx].value.bytes, XCRYPTON_BYTES_SIZE);
        hw_crypton(inv, verbose, end, id);
    }
    else
    {
        printf("unrecognized encryption mode: %s", mode);
    }
    return NULL;
}

#endif

#ifdef SCABOX_TDC

	CMD_err_t *tdc(const CMD_cmd_t *cmd)
{
    int calibrate_idx = CMD_opt_find(cmd->options, 'c');
    int state_idx = CMD_opt_find(cmd->options, 's');
    int delay_idx = CMD_opt_find(cmd->options, 'd');
    int verbose = CMD_opt_find(cmd->options, 'v') != -1;

    int calibration = calibrate_idx != -1;
    int delay = delay_idx != -1;
    int state = state_idx != -1;
    uint64_t current_delay;

    if (delay)
    {
        XTDC_WriteDelay(&tdc_inst, -1, cmd->options[delay_idx].value.words[0], cmd->options[delay_idx].value.words[1]);
    }

    if (calibration)
    {
        XTDC_Calibrate(&tdc_inst, cmd->options[calibrate_idx].value.integer, verbose);
    }

    if (verbose || calibration)
    {
        current_delay = XTDC_ReadDelay(&tdc_inst, -1);
        printf("delay: 0x%08x%08x\n", (unsigned int)(current_delay >> 32), (unsigned int)current_delay);
    }

    if (state)
    {
        int id = cmd->options[state_idx].value.integer;
        XTDC_SetId(tdc_inst.Config.BaseAddr, id);
        printf("TDC %d state :",id);
        for(int i = 0 ; i < 64 ; i++)
        {
        	if(i%8 == 0){printf("\n");}
        	printf("%08lx ", XTDC_ReadState(tdc_inst.Config.BaseAddr));
        }
        printf("\n");
        return NULL;
    }
    else
    {
        printf("data: %ld\n", XTDC_ReadData(tdc_inst.Config.BaseAddr));
    }

    return NULL;
}

#endif

#ifdef SCABOX_RO

	CMD_err_t *ro(const CMD_cmd_t *cmd)
{
    int raw_idx = CMD_opt_find(cmd->options, 'r');
    int verbose = CMD_opt_find(cmd->options, 'v') != -1;
    int raw = raw_idx != -1;

    if (raw)
    {
        int id = cmd->options[raw_idx].value.integer;
        XRO_SetId(ro_inst.Config.BaseAddr, id);
        printf("state %d: %08lx\n", id, XRO_ReadState(ro_inst.Config.BaseAddr));
        return NULL;
    }
    else
    {
        printf("value: %ld\n", XRO_ReadData(ro_inst.Config.BaseAddr));
    }

    return NULL;
}

#endif

void fifo_flush(int id)
{
    XFIFO_Reset(&fifo_inst[id]);
}

void fifo_read(int id, int verbose, int start, int end)
{
    uint32_t *weights = malloc(32 * (end - start));
    int len = XFIFO_Read(&fifo_inst[id], weights, (uint32_t)start, (uint32_t)end, 1);

    printf("samples: %d;;\n", len);
    if (len == 0)
    {
        return;
    }
    char *str = malloc((verbose ? 4 : 1) * (end - start) * sizeof(char) + 1);
    if (verbose)
    {
        printf("weights: %s;;\n", weights_to_string(str, (int *)weights, len));
        printf("code: %s;;\n", weights_to_ascii(str, (int *)weights, len));
    }
    else
    {
        printf("code: %s;;\n", weights_to_ascii(str, (int *)weights, len));
    }
    free(str);
    free(weights);
}

void fifo_acquire(int id, int end)
{
    fifo_inst[id].Mode = XFIFO_MODE_SW;
    XFIFO_Reset(&fifo_inst[id]);
    XFIFO_Write(&fifo_inst[id], end, NULL);
}

CMD_err_t *fifo(const CMD_cmd_t *cmd)
{
    int flush = CMD_opt_find(cmd->options, 'f') != -1;
    int verbose = CMD_opt_find(cmd->options, 'v') != -1;
    int acquire = CMD_opt_find(cmd->options, 'a') != -1;
    int start = CMD_opt_find(cmd->options, 's');
    int end = CMD_opt_find(cmd->options, 'e');
    int id = CMD_opt_find(cmd->options, 'c');

    start = start != -1 ? cmd->options[start].value.integer : 0;
    end = end != -1 ? cmd->options[end].value.integer : XFIFO_ConfigTable[0].Depth-1;
    id = id != -1 ? cmd->options[id].value.integer : 0;


    printf("end %d", end);
    printf("depth %d", XFIFO_ConfigTable[0].Depth-1);
    printf("fifo empty? %d\n\r", XFIFO_IsEmpty(XPAR_FIFO_AND_CTRL_0_S_AXI_BASEADDR));
    printf("getcount: %d\n\r",XFIFO_GetCount(XPAR_FIFO_AND_CTRL_0_S_AXI_BASEADDR));

    if (flush)
    {
        //fifo_flush(id);
    }

    if (acquire)
    {
        fifo_acquire(id, end);
    }
    printf("end %d", end);
    printf("depth %d", XFIFO_ConfigTable[0].Depth-1);
    printf("fifo empty? %d\n\r", XFIFO_IsEmpty(XPAR_FIFO_AND_CTRL_0_S_AXI_BASEADDR));
    printf("getcount: %d\n\r",XFIFO_GetCount(XPAR_FIFO_AND_CTRL_0_S_AXI_BASEADDR));
    fifo_read(id, verbose, start, end);
    return NULL;
}

CMD_err_t *sca(const CMD_cmd_t *cmd)
{
    int iterations = cmd->options[CMD_opt_find(cmd->options, 't')].value.integer;
    int inv = CMD_opt_find(cmd->options, 'i') != -1;
    int verbose = CMD_opt_find(cmd->options, 'v') != -1;
    int raw = CMD_opt_find(cmd->options, 'r') != -1;
    int mode_idx = CMD_opt_find(cmd->options, 'm');
    int start = CMD_opt_find(cmd->options, 's');
    int end = CMD_opt_find(cmd->options, 'e');
    int id = CMD_opt_find(cmd->options, 'c');
    char *mode = cmd->options[mode_idx].value.string;

	#ifdef SCABOX_PRESENT
    	HEX_random_words(key_present, INT_MAX, XPRESENT_WORDS_SIZE);
	#endif

	#ifdef SCABOX_KLEIN
    	HEX_random_words(key_klein, INT_MAX, XKLEIN_WORDS_SIZE);
	#endif

	#ifdef SCABOX_CRYPTON
		HEX_random_words(key_crypton, INT_MAX, XCRYPTON_WORDS_SIZE);
	#endif

    #ifdef SCABOX_AES
		HEX_random_words(key_aes, INT_MAX, XAES_WORDS_SIZE);
		HEX_words_to_bytes(key_tiny, key_aes, 16);
		memcpy(key_ssl, key_aes, AES_BLOCK_SIZE);
		memcpy(key_dhuertas, key_aes, AES_BLOCK_SIZE);
	#endif

    start = start != -1 ? cmd->options[start].value.integer : 0;
    end = end != -1 ? cmd->options[end].value.integer : XFIFO_ConfigTable[0].Depth-1;
    id = id != -1 ? cmd->options[id].value.integer : 0;

	#ifdef SCABOX_RO
		printf("sensors: %d;;\n", XRO_ConfigTable[0].Count);
	#endif
	#ifdef SCABOX_TDC
		printf("sensors: %d;;\n", XTDC_ConfigTable[0].Count);
	#endif

    printf("target: %d;;\n", 0);
    printf("mode: %s;;\n", mode);
    printf("direction: %s;;\n", inv ? "dec" : "enc");

    if(!strcmp(mode, "present"))
    {
    	#ifdef SCABOX_PRESENT
    	printf("keys: %s;;\n", HEX_words_to_string(buffer, key_present, XPRESENT_WORDS_SIZE));
		#endif
    }
    else if(!strcmp(mode, "klein"))
    {
		#ifdef SCABOX_KLEIN
    	printf("keys: %s;;\n", HEX_words_to_string(buffer, key_klein, XKLEIN_WORDS_SIZE-2));
		#endif
    }
    else if(!strcmp(mode, "crypton"))
    {
		#ifdef SCABOX_CRYPTON
    	printf("keys: %s;;\n", HEX_words_to_string(buffer, key_crypton, XCRYPTON_WORDS_SIZE));
		#endif
    }
    else
    {
		#ifdef SCABOX_AES
    	printf("keys: %s;;\n", HEX_words_to_string(buffer, key_aes, XAES_WORDS_SIZE));
		#endif
    }

    for (int d = 0; d < iterations; d++)
    {
        if (raw)
        {
            printf("\xfd\xfd\xfd\xfd;;\n");
            fifo_acquire(id, end);
            fifo_read(id, verbose, start, end);
        }
        printf("\xfe\xfe\xfe\xfe;;\n");


        if(!strcmp(mode, "present"))
        {
        	#ifdef SCABOX_PRESENT
        	HEX_random_words(block_present, seed, XPRESENT_WORDS_SIZE-2);
        	printf("%s: %s;;\n", inv ? "ciphers" : "plains", HEX_words_to_string(buffer, block_present, XPRESENT_WORDS_SIZE-2));
			#endif
        }
        else if(!strcmp(mode, "klein"))
        {
			#ifdef SCABOX_KLEIN
        	HEX_random_words(block_klein, seed, XKLEIN_WORDS_SIZE-2);
        	printf("%s: %s;;\n", inv ? "ciphers" : "plains", HEX_words_to_string(buffer, block_klein, XKLEIN_WORDS_SIZE-2));
			#endif
        }
        else if(!strcmp(mode, "crypton"))
        {
			#ifdef SCABOX_CRYPTON
        	HEX_random_words(block_crypton, seed, XCRYPTON_WORDS_SIZE);
        	printf("%s: %s;;\n", inv ? "ciphers" : "plains", HEX_words_to_string(buffer, block_crypton, XCRYPTON_WORDS_SIZE));
			#endif
        }
        else
        {
			#ifdef SCABOX_AES
            HEX_random_words(block_aes, seed, XAES_WORDS_SIZE);
            printf("%s: %s;;\n", inv ? "ciphers" : "plains", HEX_words_to_string(buffer, block_aes, XAES_WORDS_SIZE));
			#endif
        }


        if (!strcmp(mode, "hw"))
        {
			#ifdef SCABOX_AES
            hw_aes(inv, verbose, end, id);
			#endif
        }
        else if (!strcmp(mode, "tiny"))
        {
			#ifdef SCABOX_AES
            HEX_words_to_bytes(in_tiny, block_aes, 16);
            tiny_aes(inv, verbose, end, id);
			#endif
        }
        else if (!strcmp(mode, "ssl"))
        {
			#ifdef SCABOX_AES
            memcpy(in_ssl, block_aes, AES_BLOCK_SIZE);
            ssl_aes(inv, verbose, end, id);
			#endif
        }
        else if (!strcmp(mode, "dhuertas"))
        {
			#ifdef SCABOX_AES
            memcpy(in_dhuertas, block_aes, AES_BLOCK_SIZE);
            dhuertas_aes(inv, verbose, end, id);
			#endif
        }
        else if (!strcmp(mode, "present"))
        {
			#ifdef SCABOX_PRESENT
        	hw_present(inv, verbose, end, id);
			#endif
        }
        else if (!strcmp(mode, "klein"))
        {
			#ifdef SCABOX_KLEIN
        	hw_klein(inv, verbose, end, id);
			#endif
        }
        else if (!strcmp(mode, "crypton"))
        {
			#ifdef SCABOX_CRYPTON
        	hw_crypton(inv, verbose, end, id);
			#endif
        }
        else
        {
            printf("unrecognized encryption mode: %s", mode);
        }
        fifo_read(id, verbose, start, end);
        seed++;
    }
    fifo_flush(id);
    printf("\xff\xff\xff\xff;;\n");
    return NULL;
}
