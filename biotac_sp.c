/*=========================================================================
| (c) 2011-2012  SynTouch LLC
|--------------------------------------------------------------------------
| Project : BioTac C Library for Cheetah
| File    : biotac_sp.c
| Authors : Pedro Machado (pedro.baptistamachado@ntu.ac.uk)
| Adapted from the original Authors:
|            Gary Lin (gary.lin@syntouchllc.com)
|			Tomonori Yamamoto (tomonori.yamamoto@syntouchllc.com)
|			Jeremy Fishel (jeremy.fishel@syntouchllc.com)
|--------------------------------------------------------------------------
| Function: Main function to run an example program
|--------------------------------------------------------------------------
| Redistribution and use of this file in source and binary forms, with
| or without modification, are permitted.
|
| THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
| "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
| LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
| FOR A PARTICULAR PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL THE
| COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
| INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
| BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
| LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
| CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
| LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN
| ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
| POSSIBILITY OF SUCH DAMAGE.
 ========================================================================*/

//=========================================================================
// INCLUDES
//=========================================================================
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "cheetah.h"
#include "biotac.h"


//=========================================================================
// MAIN PROGRAM
//=========================================================================
int main(int argc,char **argv)
{
	/****************************/
	/* --- Define variables --- */
	/****************************/
	bt_info biotac;
    bt_property biotac_property[MAX_BIOTACS_PER_CHEETAH];
	bt_data *data;
	BioTac bt_err_code;
	Cheetah ch_handle;
    
    
	int i;
	int length_of_data_in_second;
	int number_of_samples;
	int number_of_loops;

    /**************************************************************************/
	/* --- Initialize BioTac settings (only default values are supported) --- */
    /**************************************************************************/
	biotac.spi_clock_speed = BT_SPI_BITRATE_KHZ_DEFAULT;
	biotac.number_of_biotacs = 0;
	biotac.sample_rate_Hz = BT_SAMPLE_RATE_HZ_DEFAULT;
	biotac.frame.frame_type = 0;
	biotac.batch.batch_frame_count = BT_FRAMES_IN_BATCH_DEFAULT;
	biotac.batch.batch_ms = BT_BATCH_MS_DEFAULT;

	number_of_samples = (int)(BT_SAMPLE_RATE_HZ_DEFAULT);

	// Check if any initial settings are wrong
	if (MAX_BIOTACS_PER_CHEETAH != 3 && MAX_BIOTACS_PER_CHEETAH != 5)
	{
		bt_err_code = BT_WRONG_MAX_BIOTAC_NUMBER;
		bt_display_errors(bt_err_code);
		exit(1);
	}

    /******************************************/
	/* --- Initialize the Cheetah devices --- */
    /******************************************/
	ch_handle = bt_cheetah_initialize(&biotac);

	/*********************************************************/
	/* --- Get and print out properties of the BioTac(s) --- */
	/*********************************************************/
	for (i = 0; i < MAX_BIOTACS_PER_CHEETAH; i++)
	{
		bt_err_code = bt_cheetah_get_properties(ch_handle, i+1, &(biotac_property[i]));

		if (biotac_property[i].bt_connected == YES)
		{
			(biotac.number_of_biotacs)++;
		}

		if (bt_err_code)
		{
			bt_display_errors(bt_err_code);
			exit(1);
		}
	}

	// Check if any BioTacs are detected
	if (biotac.number_of_biotacs == 0)
	{
		bt_err_code = BT_NO_BIOTAC_DETECTED;
		bt_display_errors(bt_err_code);
		return bt_err_code;
	}
	else
	{
		printf("\n%d BioTac(s) detected.\n\n", biotac.number_of_biotacs);
	}
    static int results[4][162]; 
    static int results_vec[162];
    static int ivec_results[163];
    for  (int n = 0; n < 100; ++n)
        {
        /*************************************/
        /* --- Configure the save buffer --- */
        /*************************************/
        printf("Number of samples: %d\n",number_of_samples);
        data = bt_configure_save_buffer(number_of_samples);

        /*******************************/
        /* --- Configure the batch --- */
        /*******************************/
        bt_err_code = bt_cheetah_configure_batch(ch_handle, &biotac, number_of_samples);
        if (bt_err_code < 0)
        {
            bt_display_errors(bt_err_code);
            exit(1);
        }
        else
        {
            printf("\nConfigured the batch\n");
        }

        /***************************************************************/
        /* --- Collect the batch and display the data (if desired) --- */
        /***************************************************************/
        //number_of_loops = (int)(number_of_samples / ((double)(biotac.frame.frame_size * biotac.batch.batch_frame_count)));
        number_of_loops=1;
        printf("Collecting 100 batches using the 3x BioTac SP sensors...\n");
        
        
                memset(results, 0, sizeof results);
                for (i = 0; i < number_of_loops; i++)
                {
                    bt_cheetah_collect_1_batch(ch_handle, &biotac, data, results);
                }
                
                printf("Results: \n");
                memset(results_vec, 0, sizeof results_vec);
                for(int k=0; k<4; ++k)
                {
                    printf("\n\n");
                    for (int l=0; l<162;++l)
                    {
                        
                        results_vec[l]+=results[k][l]/4.;
                        printf(" %d",results[k][l]);
                    }
                }
                memset(ivec_results, 0, sizeof ivec_results);
                printf("\n\nResults:\n[");
                ivec_results[0]=time(NULL);
                for (int l=0; l<163;++l)
                {
                    if (l==0){
                        ivec_results[l]=time(NULL);
                        printf("%d",ivec_results[l]);   
                    }
                    else{
                        ivec_results[l]=results_vec[l-1];
                        printf(", %d",ivec_results[l]);
                    }
                }
                printf("]\n");
            

            free(data);
        }
    bt_cheetah_close(ch_handle);
    

    return 0;
}
