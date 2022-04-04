#include "cubelang.h"

int debug = 0;
int main(int argc, char** argv){
    if(argc != 2 && argc !=3){
        fprintf(stderr, "Invalid arguments. Expected format:\n\"cubelang [filename.cl] [-d]\"");
        exit(1);
    }
    if(argc ==3 && argv[2][0] == '-' && argv[2][1] == 'd'){
        debug = 1;
    }

    FILE* f = fopen(argv[1], "r");
    int N = 0;

    int16_t* memory = parse(f, &N);

    execute(memory, N);

    return 0;
}

int16_t* parse(FILE* f, int* N){

    int ret = fscanf(f, "%x", N);
    if(ret == 0 || ret == EOF){
        fprintf(stderr, "Invalid Format. Please start program with N.");
        exit(1);
    }

    int16_t* memory = malloc((*N) * (*N) * (*N));
    int j = 0;

    char buf[7];
    int i = 0;
    int c = 0;
    int is_hex = 1;

    //Read single chars into the buffer and add them to memory when a space separates them
    while( (c = fgetc(f)) != EOF ){

        if(isspace(c)){
            if(i != 0){
                buf[i] = '\0';

                if(is_hex){
                    memory[j++] = to_hex(buf);
                }else{
                    memory[j++] = parse_asm(buf);
                }

                memset(buf, 0, sizeof(buf));
                i = 0;
                is_hex = 1;
            }

        }
        else if(i >= 7){
            fprintf(stderr, "Invalid Format. Element %d is too long.");
            exit(1); 
        }
        else{
            buf[i++] = c;

            int c_is_hex = ('0' <= c && c <= '9') || ('a' <= c && c <= 'f') || ('A' <= c && c <= 'F');
            if(!c_is_hex){
                is_hex = 0;
            }
        }
    }

    if(j+1 != (*N) * (*N) * (*N)){
        fprintf(stderr, "Invalid Format. There must be NxNxN elements.");
        exit(1);
    }

    return memory;

}

int16_t to_hex(char* str){
    return (int16_t) strtol(str, NULL, 16);
}

int16_t parse_asm(char* str){
    int l = strlen(str);

    int16_t instmv = dir_to_int( str[l-1] ) & 0x7;
    int16_t datmv = dir_to_int( str[l-2] ) & 0x7;
    int16_t instruction = inst_name_to_int(str + l - 5) & 0x1F;
    int16_t condmv = 0;
    if(l==6){
        condmv = dir_to_int( str[0] ) & 0x7 ;
    }

    if(debug){
        printf("%s : %d %d %d %d : %d\n", str, condmv, instruction, datmv, instmv, ((condmv<<11) + (instruction<<6) + (datmv<<3) + (instmv)));
    }
    
    return ((condmv<<11) + (instruction<<6) + (datmv<<3) + (instmv));
    
}

int16_t dir_to_int(char dir){
    switch (dir){
        case '_':
            return 0;
            break;
        case 'X':
            return 1;
            break;
        case 'x':
            return 2;
            break;
        case 'Y':
            return 3;
            break;
        case 'y':
            return 4;
            break;
        case 'Z':
            return 5;
            break;
        case 'z':
            return 6;
            break;
        default:
            return 0;
            break;
        
    }
}

int16_t inst_name_to_int(char* inst){
    char name[4];
    for(int i = 0; i < 3; i++){
        name[i] = inst[i];
    }
    name[3] = '\0';

    if (strcmp(name, "SAV") == 0){ return 00;}
    else if (strcmp(name, "LOD") == 0){ return 1;}
    else if (strcmp(name, "ADD") == 0){ return 2;}
    else if (strcmp(name, "SUB") == 0){ return 3;}
    else if (strcmp(name, "INC") == 0){ return 4;}
    else if (strcmp(name, "DEC") == 0){ return 5;}
    else if (strcmp(name, "MUL") == 0){ return 6;}
    else if (strcmp(name, "DIV") == 0){ return 7;}
    else if (strcmp(name, "AND") == 0){ return 9;}
    else if (strcmp(name, "MOD") == 0){ return 8;}
    else if (strcmp(name, "ORR") == 0){ return 10;}
    else if (strcmp(name, "NOT") == 0){ return 11;}
    else if (strcmp(name, "XOR") == 0){ return 12;}
    else if (strcmp(name, "SFT") == 0){ return 13;}
    else if (strcmp(name, "MPS") == 0){ return 14;}
    else if (strcmp(name, "MNG") == 0){ return 15;}
    else if (strcmp(name, "MZR") == 0){ return 16;}
    else if (strcmp(name, "RED") == 0){ return 17;}
    else if (strcmp(name, "WRT") == 0){ return 18;}
    else if (strcmp(name, "END") == 0){ return 19;}
    else if (strcmp(name, "NOP") == 0){ return 20;}
}

int ptr_to_index(volptr ptr, int N, int N2){
    return ptr.x + ptr.y*N + ptr.z*N2;
}

void execute(int16_t* mem, int N){
    int N2 = N*N;

    volptr ip = {0, 0, 0};
    volptr dp = {0, 0, N-1};
    int16_t R = 0;


    while(1){

        int ii = ptr_to_index(ip, N, N2);
        int di = ptr_to_index(dp, N, N2);
        
        int16_t full_instruction = mem[ii];

        int16_t instmv = (full_instruction>>0) & 0x7;
        int16_t datmv  = (full_instruction>>3) & 0x7;
        int16_t instruction = (full_instruction>>6) & 0x1F;
        int16_t condmv = (full_instruction>>11) & 0x7;

        if(debug){
            printf("(%d, %d, %d): %d : %d %d %d %d\n", ip.x, ip.y, ip.z, full_instruction, condmv, instruction, datmv, instmv);
            printf("(%d, %d, %d): %d\n\n", dp.x, dp.y, dp.z, mem[di]);
        }

        int instptr_moved = 0;
        switch(instruction){
            case 0:
                //SAV  
                mem[di] = R;
                break;	
            case 1:
                //LOD  
                R = mem[di];
                break;	
            case 2:
                //ADD  
                R += mem[di];
                break;	
            case 3:
                //SUB  
                R -= mem[di];
                break;	
            case 4:
                //INC  
                R++;
                break;	
            case 5:
                R--;
                break;	
            case 6:
                //MUL  
                R *= mem[di];
                break;	
            case 7:
                //DIV  
                R /= mem[di];
                break;	
            case 8:
                //MOD  
                R &= mem[di];
                break;	
            case 9:
                //AND  
                R &= mem[di];
                break;	
            case 10:
                //ORR  
                R |= mem[di];
                break;	
            case 11:
                //NOT  
                R = ~R;
                break;	
            case 12:
                //XOR  
                R = mem[di];
                break;	
            case 13:
                //SFT  
                if(mem[di] >= 0){
                    R = R<<mem[di];;
                }
                else{
                    R = R>>(-mem[di]);
                }
                instptr_moved = 1;
                break;	
            case 14:
                //MPS  
                if(R > 0){
                    move_ptr(&ip, condmv, N);
                }
                else{
                    move_ptr(&ip, instmv, N);
                }
                instptr_moved = 1;
                break;	
            case 15:
                //MNG  
                if(R < 0){
                    move_ptr(&ip, condmv, N);
                }
                else{
                    move_ptr(&ip, instmv, N);
                }
                instptr_moved = 1;
                break;	
            case 16:
                //MZR  
                if(R == 0){
                    move_ptr(&ip, condmv, N);
                }
                else{
                    move_ptr(&ip, instmv, N);
                }
                instptr_moved = 1;
                break;	
            case 17:
                R = (char) getc(stdin);
                break;	
            case 18:
                //WRT  
                putc((char) R, stdout);
                fflush(stdout);
                break;	
            case 19:
                //END  
                exit(1);
                break;
            case 20:
                //NOP
                break;
            default:
                //ERR  
                fprintf(stderr, "Error, Unknown instruction: %d", instruction);
                exit(1);
        }

        if(!instptr_moved){
            move_ptr(&ip, instmv, N);
        }
        move_ptr(&dp, datmv, N);
    }

}

void move_ptr(volptr* ptr, int16_t dir, int N){
    switch(dir){
        case 0:
            return;
        case 1:
            ptr->x = (ptr->x+1 +N)%N;
            return;
        case 2:
            ptr->x = (ptr->x-1 +N)%N;
            return;
        case 3:
            ptr->y = (ptr->y+1 +N)%N;
            return;
        case 4:
            ptr->y = (ptr->y-1 +N)%N;
            return;
        case 5:
            ptr->z = (ptr->z+1 +N)%N;
            return;
        case 6:
            ptr->z = (ptr->z-1 +N)%N;
            return;
        default:
            fprintf(stderr ,"\"%d\" is an invalid direction.", dir);
            exit(1);
    }
}
