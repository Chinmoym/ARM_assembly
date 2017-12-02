#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

struct code_line {
	int line_number;
	char instr_line[50];
	char addr_mode[5];
	char opcode[10];
	int dest;
	int oprnd1;
	int oprnd2;
	int oprnd3;
	int hex_value;
};

struct lookup {
	char name[15];
	unsigned int value;
};

int total_line;
// enum regstr {R0, R1, R2, R3, R4, R5, R6, R7, R8, R9. R10. R11, R12, SP, LR, PC};

//instruction Q
#define INSTR_QSIZE 65536
struct code_line instruction[INSTR_QSIZE];
int front=-1, rear=-1;
//opcode lookup table
int OP_QSIZE;
//condition  lookup table
int COND_QSIZE;
//register  lookup table
#define REG_QSIZE 16
struct lookup regstr[REG_QSIZE];

int is_empty(){
	if(front==-1||front==rear+1)
		return 1;
	return 0;
}

int is_full(){
	if(rear==INSTR_QSIZE-1)
		return 1;
	return 0;
}

void insert(struct code_line instr){
	if(is_full()){
		printf("Instruction Q full.\n");
		exit(-1);
	}
	if (front==-1)
		front++;

	rear++;
	// printf("%d %d\n",rear,front);
	instruction[rear] = instr;
}

/**/
int lookup();
int parsing(char * file);
void decode();

int main (int argc, char * argv[]){

	int err,i;
	
	if (argc < 2){
		printf("Too less parameter: ./program [.s file]\n");
		exit(-1);
	}
	else if (argc > 2){
		printf("Too many parameter: ./program [.s file]\n");
		exit(-1);
	}
	else{
		printf("Parsing file %s\n",argv[1]);
	}

	err = lookup();
	if (err){
		printf("Failed to create the database. check the lookup files.\n");
		exit(-1);
	}

	err = parsing(argv[1]);
	if(err){
		printf("Failed to parse the code\n");
		exit(-1);
	}

	//
	printf("total number of lines: %d\n",total_line);
	for(i=0;i<total_line;i++)
		printf("%d\t%s\t\t%d\n",instruction[i].line_number,instruction[i].opcode,instruction[i].dest);

	printf("\n");
	return 0;
}

int parsing(char * file){
	int i,j;
	struct code_line line;
	char c,opc[10],opd[50];

	FILE * fp = fopen(file,"r");
	i=0;
	j=0;
	c=fgetc(fp);
	while(c!=EOF){ //for EOF
		i++;
		j=0;
		while(c!='\n'){//read one line
			opd[j++]=toupper(c);
			// printf("%c",c);
			c=fgetc(fp);
		}
		opd[j++] = ' ';
		opd[j]='\0';
		// printf("%d %s\n",i,opd);
		line.line_number = i;
		total_line = i;
		strcpy(line.instr_line,opd);
		c=fgetc(fp);
		insert(line);
	}
	decode();

	return 0;
}

void decode(){
	int i,j,fc,op,ds,k,l;
	char str[50];
	char opc[10];
	for(l=0;l<10;l++)
		opc[l] ='\0';
	for(i=0;i<total_line;i++){
		strcpy(str,instruction[i].instr_line);
		// printf("%s\n",str);
		j=0;
		fc=0;
		k=0;
		op=0;
		ds=0;
		for(l=0;l<10;l++)
			opc[l] =0;
		while(str[j]!='\0'){
			if ((str[j]=='\t' || str[j]==' ' || str[j] =='\0') && fc==0){ //till 1st character other than space abd tab
				j++;
				continue;
			}
			else{
				fc=1;
				
				opc[k] = str[j];
				if((str[j]=='\t' || str[j]==' ' || str[j] =='\0') && op==0){ //get opcode
					op=1;
					opc[k] = '\0';
					strcpy(instruction[i].opcode,opc);
					//printf("%d\t%s",i,instruction[i].opcode);
					//printf("\t\t\topc=%s\n",opc);
					k=-1;
					for(l=0;l<10;l++)
						opc[l] =0;
				}
				else if((str[j]==',' || str[j]==' ' || str[j] =='\0') && ds==0){
					// k--;
					opc[k]='\0';
					k=-1;
					ds=1;
					for(l=0;l<REG_QSIZE;l++){
						// printf("%s %s\n",regstr[l].name,opc);
						if(!strcmp(regstr[l].name,opc)){
							instruction[i].dest=regstr[l].value;
							// printf("ok\n");
							break;
						}
					}
					// instruction[i].dest = 
					// printf("opc=%s\n",opc);
					for(l=0;l<10;l++)
						opc[l] =0;
				}
				j++;
				k++;
				// printf("opc=%s\n",opc);

			}
		}
	}

}

int lookup(){
	int i,val;
	char c,str[10];

	//opcode list
	FILE *fl = fopen("opcode.list","r");
	if(fl==NULL)
		return 1;
	
	fscanf(fl,"%d",&OP_QSIZE);
	struct lookup opcode[OP_QSIZE];
	i=0;
	while ((c=fgetc(fl))!=EOF){
		fseek(fl,-1,SEEK_CUR);
		fscanf(fl,"%s",str);
		fscanf(fl,"%d",&val);
		opcode[i].value=val;
		strcpy(opcode[i].name,str);
		i++;
	}

	close(fl);

	// for (i=0;i<OP_QSIZE;i++)
	// 	printf("%s %x %d\n",opcode[i].name,opcode[i].value,i);

	//register list
	fl = fopen("register.list","r");
	if(fl==NULL)
		return 1;
	
	// fscanf(fl,"%d",&REG_QSIZE);
	// struct lookup regstr[REG_QSIZE];
	i=0;
	while ((c=fgetc(fl))!=EOF){
		fseek(fl,-1,SEEK_CUR);
		fscanf(fl,"%s",str);
		fscanf(fl,"%x",&val);
		regstr[i].value=val;
		strcpy(regstr[i].name,str);
		i++;
	}

	close(fl);

	// for (i=0;i<REG_QSIZE;i++)
	// 	printf("%s %x\n",regstr[i].name,regstr[i].value);

	return 0;
}
