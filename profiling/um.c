/* um.c 
        
        Written by Georgia Power and Brooke Thompson for CS40 HW7: Profiling
        
        Optimized UM program.        
*/
#include <stdint.h>
#include <stdio.h>
#include <assert.h>
#include <string.h>
#include <stdlib.h>
#include <sys/stat.h>

/* define integers as sensical UM opcodes */
typedef enum Um_opcode {
        CMOV = 0, SLOAD, SSTORE, ADD, MUL, DIV,
        NAND, HALT, ACTIVATE, INACTIVATE, OUT, IN, LOADP, LV
} Um_opcode;


int main(int argc, char *argv[]) 
{ 
        /* check for correct usage */
        if (argc != 2) {
                fprintf(stderr, "Usage: ./um [filename]\n");
                exit(EXIT_FAILURE);
        }
    
        /* get size of file */
        struct stat info;
        stat(argv[1], &info);
        int filebytes = info.st_size;
        
        /* open file and ensure sure it is valid */
        FILE* program = fopen(argv[1], "r");
    
        if (program == NULL) { 
                fprintf(stderr, "Please enter valid file.\n");
                exit(EXIT_FAILURE);
        }

        /* declare registers and length of the file */
        uint32_t registers[8];
        int segment_length = filebytes / 4;
        
        /* initialize array to hold mapped memory, vars to keep track of 
        mapped memory */
        uint32_t** mapped_memory = (uint32_t**)calloc(segment_length, 
                                                        sizeof(uint32_t*));
        int num_segments = 0;
        
        /* initialize array to hold unmapped IDs and ints to keep track 
        of memory */
    	uint32_t* unmapped_IDs = (uint32_t*)calloc(segment_length, 
                                                        sizeof(uint32_t));
        uint32_t num_unmapped = 0;
        uint32_t num_unmapped_cap = filebytes / 4;

	/* initialize array to hold mapped segment lengths */
        uint32_t* segment_lengths = (uint32_t*)calloc(segment_length, 
                                                         sizeof(uint32_t));

        int mem_cap = segment_length;
         
        /* make and initialize segment 0*/
        uint32_t* segment_0 = (uint32_t*)calloc(segment_length, 
                                                        sizeof(uint32_t));
        for (int i = 0; i < segment_length; i++) { 
                
                /* make 32 bit word out of 4 characters */
                unsigned lsb = 24;
                uint64_t word = 0;
                for (int j = 0; j < 4; j++) {
                        int value = fgetc(program);
                        word = word | (value << lsb);
                        lsb = lsb - 8;       
                }
                /* place word in 0 segment*/
                segment_0[i] = (uint32_t)word;
        }

        /* set segment 0 and store its length */
        mapped_memory[0] = segment_0;
        segment_lengths[0] = segment_length;   

        /* initialize all registers to 0 */
        for (int reg = 0; reg < 8; reg++) { 
                registers[reg] = 0; 
                registers[reg] = 0;
        }
    
        /* set program counter and zero segment for loop */
        uint32_t program_counter = 0;
        uint32_t* ZeroSeg = segment_0;
 
       /* initialize registers and opcode to be
        extracted from current instruction */
        Um_opcode op = 0;
        uint32_t ra;
        uint32_t rb;
        uint32_t rc;
        
        /* machine cycle */
        while (op != HALT) {

                /* get the instruction we are currently executing */
                uint32_t curr = ZeroSeg[program_counter];
                op = curr & 4026531840;
                op = op >> 28;
            
                /* advance program counter */
                program_counter++;
        
                /* switch case based on opcode */
                switch (op) {
                        case LV:
                                ra = (curr & 234881024) >> 25;
                                uint32_t value = (curr & 33554431);
                                registers[ra] = value;
                                break;
                        
                        case CMOV:
                                ra = (uint32_t)((curr & 448) >> 6);
                                rb = (uint32_t)((curr & 56) >> 3);
                                rc = (uint32_t)(curr & 7);
                                if (registers[rc] != 0) {
                                        registers[ra] = registers[rb];
                                }
                                break;

                        case SLOAD:
                                ra = (uint32_t)((curr & 448) >> 6);
                                rb = (uint32_t)((curr & 56) >> 3);
                                rc = (uint32_t)(curr & 7);
                                uint32_t* segment = 
                                                mapped_memory[registers[rb]];
    	                        uint32_t v = segment[registers[rc]]; 
		                registers[ra] = v;
                                break;

                        case SSTORE:
                                ra = (uint32_t)((curr & 448) >> 6);
                                rb = (uint32_t)((curr & 56) >> 3);
                                rc = (uint32_t)(curr & 7);
                                uint32_t* segm = mapped_memory[registers[ra]];
		                segm[registers[rb]] = registers[rc];
                                break;

                        case ADD:
                                ra = (uint32_t)((curr & 448) >> 6);
                                rb = (uint32_t)((curr & 56) >> 3);
                                rc = (uint32_t)(curr & 7);
                                registers[ra] = (registers[rb] + 
                                                                registers[rc]);
                                break;

                        case MUL:
                                ra = (uint32_t)((curr & 448) >> 6);
                                rb = (uint32_t)((curr & 56) >> 3);
                                rc = (uint32_t)(curr & 7);
                                registers[ra]  = (registers[rb] * 
                                                                registers[rc]); 
                                break;

                        case DIV:
                                ra = (uint32_t)((curr & 448) >> 6);
                                rb = (uint32_t)((curr & 56) >> 3);
                                rc = (uint32_t)(curr & 7);
                                registers[ra]  =  (registers[rb] / 
                                                                registers[rc]);
                                break;

                        case NAND: 
                                ra = (uint32_t)((curr & 448) >> 6);
                                rb = (uint32_t)((curr & 56) >> 3);
                                rc = (uint32_t)(curr & 7);
                                registers[ra] =  ~(registers[rb] & 
                                                                registers[rc]);
                                break;

                        case ACTIVATE: 
                                rb = (uint32_t)((curr & 56) >> 3);
                                rc = (uint32_t)(curr & 7);

                                /* get new length from register rc */
                                uint32_t num_words = registers[rc];
    	                        uint32_t* new_segment = (uint32_t*)
                                      calloc((int)num_words, sizeof(uint32_t));

                                /* make new ID for the segment */
                                uint32_t new_id = 0;
                                
                                if (num_unmapped == 0) { 
			                num_segments++;
			                new_id = num_segments;
    	                        }

                                else { 
			                new_id = unmapped_IDs
                                                        [num_unmapped - 1];
         	                        unmapped_IDs[(num_unmapped) - 1] = 0;
			                num_unmapped--;			
    	                        }
	
                                /* reallocate segment holder if we need to */
                                if ((int)new_id >= mem_cap) { 
		
			                int current_length = 
                                                        segment_lengths[0];
                                        int new_num_elems = 4 * 
                                                        (current_length);

                                        mapped_memory = realloc(mapped_memory, 
                                        (new_num_elems * sizeof(uint32_t*)));
			                segment_lengths = 
                                                realloc(segment_lengths, 
                                                (new_num_elems * 
                                                        sizeof(uint32_t)));

			               mem_cap = new_num_elems;
		                }
	
                                /* place new segment in segment holder */
                                mapped_memory[new_id] = new_segment;
		                segment_lengths[new_id] = num_words;
                                
                                /* place new ID in register B*/
		                registers[rb] = new_id;
                                break;

                        case INACTIVATE:
                                rc = (uint32_t)(curr & 7);
    	                        /* get number of segment to be unmapped */
                                uint32_t identifier = registers[rc];
    	                        uint32_t* seg = mapped_memory[identifier];
		                /* unmap */
                                free(seg);
		                mapped_memory[identifier] = NULL;
                                /* reallocate space in unmapped ID holder 
                                if we need */
		                if (identifier > num_unmapped_cap) { 
			                num_unmapped_cap = 2 * 
                                                        num_unmapped_cap;
			                unmapped_IDs = realloc(unmapped_IDs, 
                                        (num_unmapped_cap * sizeof(uint32_t)));
		                }
                                /* set unmapped ID in unmapped ID array */
                                unmapped_IDs[num_unmapped] = identifier;
                                /* set length of unmapped slot to 0 and 
                                increase number of unmapped IDs*/
                                segment_lengths[identifier] = 0;
                                num_unmapped++;
                                break;

                        case OUT:
                                rc = (uint32_t)(curr & 7);
                                uint32_t val = registers[rc];
                                assert (val <= 255);
                                fputc(val, stdout);
                                break;

                        case IN:
                                rc = (uint32_t)(curr & 7);
                                int values; 
                                values = fgetc(stdin);
                                assert (values <= 255);

                                if (values == EOF) { 
                                        values = ~0;
                                }

                                registers[rc] = values;
                                break;

                        case LOADP:
                                rb = (uint32_t)((curr & 56) >> 3);
                                rc = (uint32_t)(curr & 7);
                                if (registers[rb] == 0) { 
                                        program_counter = registers[rc];
                                }
    
                                else {
                                        /* get segment to be copied */
                                        int index_to_dup = registers[rb];
                                        uint32_t* to_dup = mapped_memory
                                                                [index_to_dup];
                                        int new_length = segment_lengths
                                                                [index_to_dup];
                                        /* make segment to be copied into */
                                        uint32_t* copy = (uint32_t*)calloc
                                                (new_length, sizeof(uint32_t));
                                        /* copy */
                                        for (int index = 0; index < new_length; 
                                                                index++) { 
                                                copy[index] = to_dup[index];
                                        }       

                                        /* free current 0 segment and set 
                                        copy to zero segment */
                                        free(mapped_memory[0]);
                                        mapped_memory[0] = copy;
                                        /* advance program counter */
                                        program_counter = registers[rc];
                                }    
                                /* reassign 0 segment for loop */
                                ZeroSeg = mapped_memory[0];
                                break;
                        
                        default:  
                                break;
                }
                 
        }  
 
        /* free mapped segments */
    	int memory_length = num_segments;
	for (int index = 0; index <= memory_length; index++) { 
        	
                uint32_t* segment = mapped_memory[index]; 
        	
                if (segment != NULL) { 
			free(segment);
        	}
    	}
        free(mapped_memory);
        /* close file and free other allocated memory */
        fclose(program);
    	free(unmapped_IDs);
	free(segment_lengths);
    
}
