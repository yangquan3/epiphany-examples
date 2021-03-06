/*
e-dump-regs.c

Copyright (C) 2013 Adapteva, Inc.
Contributed by Andreas Olofsson <support@adapteva.com>

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program, see the file COPYING. If not, see
<http://www.gnu.org/licenses/>.
*/

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include <e-hal.h>
#include <e-loader.h>

#define RAM_SIZE     (0x8000)
#define WORD_SIZE    (1)

void usage();
int my_reset_system();

int main(int argc, char *argv[]){

  //----------------------------
  e_platform_t platform;
  e_epiphany_t dev;
  e_loader_diag_t e_verbose;
  unsigned int i,j,k,addr;
  unsigned int data;
  int status=1;//pass

  int row0,col0,rows,cols;
  int verbose=0;

  char result,data_in,expected;
  char high_pattern = 0xff;
  char low_pattern  = 0x00;
  

  if (argc < 5){
    usage();
    exit(1);
  }  
  else{
    row0    = atoi(argv[1]);
    col0    = atoi(argv[2]);
    rows    = atoi(argv[3]);
    cols    = atoi(argv[4]);
  }
  //Open
  e_init(NULL);
  my_reset_system();
  e_get_platform_info(&platform);
  e_open(&dev, 0, 0, platform.rows, platform.cols);
  //e_set_host_verbosity(H_D3);

  printf("-------------------------------------------------------\n");  

  for (i=row0; i<(row0+rows); i++) {
    for (j=col0; j<(col0+cols); j++) {   
      printf("Running host march-C read/write test for core (%d,%d)\n", i,j);      
      //M0: UP{w0}
      for(k=0;k<(RAM_SIZE);k=k+WORD_SIZE){
	addr= k;
	//W0
	data_in=low_pattern;
	e_write(&dev, i, j, k, &data_in, sizeof(data_in));
      }
      //M1: UP{r0,w1,r1,w0,r0,w1}
      for(k=0;k<(RAM_SIZE);k=k+WORD_SIZE){
	addr= k;
	//R0
	expected=low_pattern;
	e_read(&dev, i, j, addr, &result, sizeof(result));
	if(result!=expected){
	  if(verbose){
	    printf("FAIL-PAT-M1: addr=0x%x expected=0x%x result=0x%x\n",addr,expected,result); 
	  }
	  status=0;
	}
	//W1
	data_in=high_pattern;
	e_write(&dev, i, j, addr, &data_in, sizeof(data_in));
	//R1
	expected=high_pattern;	  
	e_read(&dev, i, j, addr, &result, sizeof(result));	
	if(result!=expected){
	  if(verbose){
	    printf("FAIL-PAT-M1: addr=0x%x expected=0x%x result=0x%x\n",addr,expected,result); 
	  }
	  status=0;
	}
	//W0
	data_in=low_pattern;
	e_write(&dev, i, j, addr, &data_in, sizeof(data_in));
	//R0
	expected=low_pattern;
	e_read(&dev, i, j, addr, &result, sizeof(result));	
	if(result!=expected){
	  if(verbose){
	    printf("FAIL-PAT-M1: addr=0x%x expected=0x%x result=0x%x\n",addr,expected,result); 
	  }
	  status=0;
	}
	//W1
	data_in=high_pattern;
	e_write(&dev, i, j, addr, &data_in, sizeof(data_in));
      }
      //M2: UP{r1,w0,w1}		
      for(k=0;k<(RAM_SIZE);k=k+WORD_SIZE){
	addr=(k);
	//R1
	expected=high_pattern;
	e_read(&dev, i, j, addr, &result, sizeof(result));	
	if(result!=expected){
	  if(verbose){
	    printf("FAIL-PAT-M2: addr=0x%x expected=0x%x result=0x%x\n",addr,expected,result);
	  }
	  status=0;
	}
	//W0
	data_in=low_pattern;
	e_write(&dev, i, j, addr, &data_in, sizeof(data_in));
	//W1
	data_in=high_pattern;
	e_write(&dev, i, j, addr, &data_in, sizeof(data_in));
      }
      //M3: DOWN{R1,W0,W1,W0}
      for(k=0;k<(RAM_SIZE);k=k+WORD_SIZE){
	addr=(RAM_SIZE-k-WORD_SIZE);
	//R1
	expected=high_pattern;
	e_read(&dev, i, j, addr, &result, sizeof(result));	
	if(result!=expected){
	  if(verbose){
	    printf("FAIL-PAT-M3: addr=0x%x expected=0x%x result=0x%x\n",addr,expected,result); 	
	  }
	  status=0;
	}
	//W0
	data_in=low_pattern;
	e_write(&dev, i, j, addr, &data_in, sizeof(data_in));
	//W1
	data_in=high_pattern;
	e_write(&dev, i, j, addr, &data_in, sizeof(data_in));
	//W0
	data_in=low_pattern;
	e_write(&dev, i, j, addr, &data_in, sizeof(data_in));
      }
      //M4:DOWN{R0,W1,W0}
      for(k=0;k<(RAM_SIZE);k=k+WORD_SIZE){
	addr=(RAM_SIZE-k-WORD_SIZE);
	//R0
	expected=low_pattern;
	e_read(&dev, i, j, addr, &result, sizeof(result));	
	if(result!=expected){
	  if(verbose){
	    printf("FAIL-PAT-M4: addr=0x%x expected=0x%x result=0x%x\n",addr,expected,result); 
	  }
	  status=0;
	}
	//W1
	data_in=high_pattern;
	e_write(&dev, i, j, addr, &data_in, sizeof(data_in));
	//W0
	data_in=low_pattern;
	e_write(&dev, i, j, addr, &data_in, sizeof(data_in));
      }
      //M5:DOWN{R0,W1,R1}
      for(k=0;k<(RAM_SIZE);k=k+WORD_SIZE){
	addr=(RAM_SIZE-k-WORD_SIZE);
	//R0
	expected=low_pattern;
	e_read(&dev, i, j, addr, &result, sizeof(result));
	if(result!=expected){
	  if(verbose){
	    printf("FAIL-PAT-M5: addr=0x%x expected=0x%x result=0x%x\n",addr,expected,result); 
	  }
	  status=0;
	}
	//W1
	data_in=high_pattern;
	e_write(&dev, i, j, addr, &data_in, sizeof(data_in));
	//R1
	expected=high_pattern;
	e_read(&dev, i, j, addr, &result, sizeof(result));
	if(result!=expected){
	  if(verbose){
	    printf("FAIL-PAT-M5: addr=0x%x expected=0x%x result=0x%x\n",addr,expected,result); 
	  }
	  status=0;
	}
      }
      //M6:DOWN{R1,W0,R0}
      for(k=0;k<(RAM_SIZE);k=k+WORD_SIZE){
	addr=(RAM_SIZE-k-WORD_SIZE);
	//R1
	expected=high_pattern;
	e_read(&dev, i, j, addr, &result, sizeof(result));
	if(result!=expected){
	  if(verbose){
	    printf("FAIL-PAT-M6: addr=0x%x expected=0x%x result=0x%x\n",addr,expected,result); 
	  }
	  status=0;
	}
	//W0
	data_in=low_pattern;
	e_write(&dev, i, j, addr, &data_in, sizeof(data_in));
	//R0
	expected=low_pattern;
	e_read(&dev, i, j, addr, &result, sizeof(result));
	if(result!=expected){
	  if(verbose){
	    printf("FAIL-PAT-M6: addr=0x%x expected=0x%x result=0x%x\n",addr,expected,result); 
	  }
	  status=0;
	}
      }
    }
  }
  //Close
  e_close(&dev);
  e_finalize();

  //Self Check
  if(status){
    return EXIT_SUCCESS;
  }
  else{
    return EXIT_FAILURE;
  }   
}
//////////////////////////////////////////////////////////////////////////
void usage()
{
  printf("-----------------------------------------------\n");
  printf("Function: Runs exhaustive march-c memory test\n");
  printf("Usage:    e-mem-test <row> <col> <rows> <cols> <v>\n");
  printf("Example:  e-mem-test 0 0 4 4 1\n");
  printf("\n");
  printf("Options:\n");
  printf("  row     - target core start row coordinate\n");
  printf("  col     - target core start column coordinate\n");
  printf("  rows    - number of rows to test\n");
  printf("  cols    - number of columns to test\n");
  printf("  v       - 1=verbose mode, 0=quiet\n");
  printf("-----------------------------------------------\n");
  return;
}
//////////////////////////////////////////////////////////////////////////
int my_reset_system()
{
  unsigned int row,col,i,j,data;
  e_epiphany_t dev;
  e_platform_t platform;


  e_init(NULL);
  e_get_platform_info(&platform);
  ee_write_esys(E_SYS_RESET, 0);//reset
  usleep(200000);
  
  //Open all cores
  e_open(&dev, 0, 0, platform.rows, platform.cols);

  //shut down north link
  if(1){
      row=0;
      col=2;

      ee_write_esys(E_SYS_CONFIG, 0x10000000);
      data = 0x000000FFF;
      e_write(&dev, row, col, 0xf0304, &data, sizeof(int));  
      data = 0x000000FFF;
      e_write(&dev, row, col, 0xf0308, &data, sizeof(int));  
      ee_write_esys(E_SYS_CONFIG, 0x00000000);
  }
  
  //Shut down west link (WEST==2,0)
  if(1){
    row=2;
    col=0;
    ee_write_esys(E_SYS_CONFIG, 0xd0000000);    
    data = 0x000000FFF;
    e_write(&dev, row, col, 0xf0304, &data, sizeof(int));      
    data = 0x000000FFF;
    e_write(&dev, row, col, 0xf0308, &data, sizeof(int));      
    ee_write_esys(E_SYS_CONFIG, 0x00000000);
  }

  //Shut down south link (SOUTH==7,2)
  if(1){
    if ((dev.type == E_E64G401)){
      row=7;
      col=2;
    }
    else{
      row=3;
      col=2;
    }

    ee_write_esys(E_SYS_CONFIG, 0x90000000);    
    data = 0x000000FFF;
    e_write(&dev, row, col, 0xf0304, &data, sizeof(int));      
    data = 0x000000FFF;
    e_write(&dev, row, col, 0xf0308, &data, sizeof(int));      
    ee_write_esys(E_SYS_CONFIG, 0x00000000);
  }

   //Change elink clock divider (temporary workaround due to FPGA timing issue)
  if(1){
    //east link register is in a different place in e64
    if ((dev.type == E_E64G401)){
      row=2;
      col=7;
    }
    else{
      row=2;
      col=3;
    }
    //Writing to the east ELINK transmit config register
    ee_write_esys(E_SYS_CONFIG, 0x50000000);
    data = 0x1;
    e_write(&dev, row, col, 0xf0300, &data, sizeof(int));
    ee_write_esys(E_SYS_CONFIG, 0x00000000);
  }  

 //Reset chip one more time (west side))
  if(0){
    row=2;
    col=0;
    ee_write_esys(E_SYS_CONFIG, 0xd0000000);    
    data = 0x000000001;
    e_write(&dev, row, col, 0xf0324, &data, sizeof(int));      
    ee_write_esys(E_SYS_CONFIG, 0x00000000);
  }

  //Enable Clock Gating
  if(1){
    for (i=0; i<platform.rows; i++) {
      for (j=0; j<platform.cols; j++) {
  	//eCore clock gating
	data=0x00400000;
	e_write(&dev, i, j, 0xf0400, &data, sizeof(data));
	//eMesh clock gating
	data=0x00000002;
	e_write(&dev, i, j, 0xf0700, &data, sizeof(data));
      }
    }
  }  

 
  //Close down device
  e_close(&dev);
  return E_OK;
}
