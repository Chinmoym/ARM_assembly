#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

struct code_line {
	int line_number;
	char instr_line[50];
	char addr_mode[5];
	int no_oprnd;
	char opcode[10];
	int gp_no;
	int gp_op;
	int in_op;
	int rd1;
	int rd2;
	int rm;
	int imm;
	int hex_value;
};

struct lookup {
	char name[15];
	unsigned int gp_no;
	unsigned int gp_op;
	unsigned int in_op;
};

int total_line;
// enum regstr {R0, R1, R2, R3, R4, R5, R6, R7, R8, R9. R10. R11, R12, SP, LR, PC};

//instruction Q
#define INSTR_QSIZE 65536
struct code_line instruction[INSTR_QSIZE];
int front=-1, rear=-1;
//opcode lookup table
#define OPI_QSIZE 12
struct lookup opcode_imm[OPI_QSIZE];
#define OPR_QSIZE 37
struct lookup opcode_reg[OPR_QSIZE];
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
int find_am();
int decode();
int decode_opd();
int generate_hex();

int main (int argc, char * argv[]){

	int err,i;
	
	if (argc < 2){
		printf("Too less parameter: ./program [.s file]\n");
		exit(-1);
	}
	// else if (argc > 2){
	// 	printf("Too many parameter: ./program [.s file]\n");
	// 	exit(-1);
	// }
	else{
		printf("Parsing file %s\n",argv[1]);
		// printf("Options = %s\n",argv[2]);
	}

	//create lookup tables
	err = lookup();
	if (err){
		printf("Failed to create the database. check the lookup files.\n");
		exit(-1);
	}

	// get the instructions
	err = parsing(argv[1]);
	if(err){
		printf("Failed to parse the code\n");
		exit(-1);
	}

	//decode the opcode and destination register
	err = decode();
	if(err){
		printf("Failed to decode the instructions\n");
		exit(-1);
	}

	//find addressing mode of instruction
	err = find_am();
	if(err){
		printf("Failed to find the addressing mode for instructions\n");
		exit(-1);
	}

	//decode operand
	err = decode_opd();
	if(err){
		printf("Failed to decode the operand\n");
		exit(-1);
	}

	//generate the hex value
	err = generate_hex();
	if(err){
		printf("Failed to generate hex value\n");
		exit(-1);
	}

	instruction[0].rd1=-1;
	if (argc > 2){
		if (!strcmp(argv[2],"-d")){
			printf("\n\t\t\t\t****Immediate addressing opcode table****\n");
			printf("instr\tgp_no\tgp_op\tin_op\t\n");
			for (i=0;i<OPI_QSIZE;i++)
				printf("%s\t%x\t%x\t%x\n",opcode_imm[i].name,opcode_imm[i].gp_no,opcode_imm[i].gp_op,opcode_imm[i].in_op);
			printf("\n\t\t\t\t****Register addressing opcode table****\n");
			printf("instr\tgp_no\tgp_op\tin_op\t\n");
			for (i=0;i<OPR_QSIZE;i++)
				printf("%s\t%x\t%x\t%x\n",opcode_reg[i].name,opcode_reg[i].gp_no,opcode_reg[i].gp_op,opcode_reg[i].in_op);
			printf("\n\t\t\t\t****Registers table****\n");
			printf("instr\tgp_no\tgp_op\tin_op\t\n");
			for (i=0;i<REG_QSIZE;i++)
				printf("%s\t%x\t%x\t%x\n",regstr[i].name,regstr[i].gp_no,regstr[i].gp_op,regstr[i].in_op);


			printf("Total number of code lines: %d\n",total_line);
			printf("\n\t\t\t\t****Instruction decode table****\n");
			printf("#ln\tno_oprd\topcode\tgp_no\tgp_op\tin_op\trd1\trm\trd2\timm\taddr_md\thex\t\tinstr\n");
			for(i=0;i<total_line;i++)
				printf("%d\t%d\t%s\t%d\t|%x|\t|%x|\t%d\t%d\t%d\t|%x|\t%s\t|%x|%s\n",
																instruction[i].line_number,
																instruction[i].no_oprnd,
																instruction[i].opcode,
																instruction[i].gp_no,
																instruction[i].gp_op,
																instruction[i].in_op,
																instruction[i].rd1,
																instruction[i].rm,
																instruction[i].rd2,
																instruction[i].imm,
																instruction[i].addr_mode,
																instruction[i].hex_value,
																instruction[i].instr_line);

			printf("\n");
		}

	}
	printf("\n\t\t\t\t****Output**** \n");
	printf("#\tHex\tInstruction\n\n");
	for(i=0;i<total_line;i++)
			printf("%d\t%x %s\n",instruction[i].line_number,
															instruction[i].hex_value,
															instruction[i].instr_line);

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
		line.rd1=-1;
		line.rd2=-1;
		line.rm=-1;
		line.gp_op=0;
		line.gp_no=0;
		line.in_op=0;
		line.imm=0;
		line.hex_value=-1;

		total_line = i;
		strcpy(line.instr_line,opd);
		c=fgetc(fp);
		insert(line);
	}

	close(fp);

	return 0;
}

int find_am(){
	struct code_line line;
	int i;

	for(i=0;i<total_line;i++){
		if (!strcmp(instruction[i].opcode,"PUSH")){//check for push
			instruction[i].addr_mode[0] = 'P';
			instruction[i].addr_mode[1] = 'U';
			instruction[i].addr_mode[2] = 'S';
			instruction[i].addr_mode[3] = '\0';
			instruction[i].addr_mode[4] = '\0';
		}
		else if (!strcmp(instruction[i].opcode,"POP")){//check for pop
			instruction[i].addr_mode[0] = 'P';
			instruction[i].addr_mode[1] = 'O';
			instruction[i].addr_mode[2] = 'P';
			instruction[i].addr_mode[3] = '\0';
			instruction[i].addr_mode[4] = '\0';
		}

		else if (!strcmp(instruction[i].opcode,"LDR")){//check for load
			instruction[i].addr_mode[0] = 'L';
			instruction[i].addr_mode[1] = 'D';
			instruction[i].addr_mode[2] = 'R';
			instruction[i].addr_mode[3] = '\0';
			instruction[i].addr_mode[4] = '\0';
			if (strchr(instruction[i].instr_line,'#'))
				instruction[i].addr_mode[2] = 'I';
		}
		else if (!strcmp(instruction[i].opcode,"STR")){//check for store
			instruction[i].addr_mode[0] = 'S';
			instruction[i].addr_mode[1] = 'T';
			instruction[i].addr_mode[2] = 'R';
			instruction[i].addr_mode[3] = '\0';
			instruction[i].addr_mode[4] = '\0';
			if (strchr(instruction[i].instr_line,'#'))
				instruction[i].addr_mode[2] = 'I';
		}
		else if (instruction[i].opcode[0]=='B'){//branching instr
			instruction[i].addr_mode[0] = 'B';
			instruction[i].addr_mode[1] = 'R';
			instruction[i].addr_mode[2] = 'R';
			instruction[i].addr_mode[3] = '\0';
			instruction[i].addr_mode[4] = '\0';
		}
		else if (strchr(instruction[i].instr_line,'#')){ //immediate
			instruction[i].addr_mode[0] = 'I';
			instruction[i].addr_mode[1] = 'M';
			instruction[i].addr_mode[2] = 'M';
			instruction[i].addr_mode[3] = '\0';
			instruction[i].addr_mode[4] = '\0';			
		}
		else{//register
			instruction[i].addr_mode[0] = 'R';
			instruction[i].addr_mode[1] = 'E';
			instruction[i].addr_mode[2] = 'G';
			instruction[i].addr_mode[3] = '\0';
			instruction[i].addr_mode[4] = '\0';			
		}
	}
	return 0;
}

int decode(){
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
				//getting the opcode
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
				//destination register decoding
				else if((str[j]==',' || str[j]==' ' || str[j] =='\0' || str[j] ==';') && ds==0){
					// k--;
					opc[k]='\0';
					k=-1;
					ds=1;
					for(l=0;l<REG_QSIZE;l++){
						// printf("%s %s\n",regstr[l].name,opc);
						if(!strcmp(regstr[l].name,opc)){
							instruction[i].rd1=regstr[l].in_op;
							// printf("ok\n");
							break;
						}
					}
					// instruction[i].rd1 = 
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
	return 0;
}

int decode_opd(){
	int i,j,k,len,l,immd,no_opd=1,found;
	char str[50],str1[10];

	//decode operands 
	for(i=0;i<total_line;i++){
		for(l=0;l<10;l++)
			str1[l] = '\0';
		immd=0;
		no_opd=1;
		found=1;
		strcpy(str,instruction[i].instr_line);
		// printf("%s\n",str);
		//remove spaces from the string
		len = strlen(str);
		for(k=0; k<len; k++)
		{
			if(str[k]==' ' || str[k]=='\t' || str[k]==']')
			{
				for(j=k; j<len; j++)
					str[j]=str[j+1];
				len--;
			}
			if(str[k]=='[' || str[k]==']')
			{
				for(j=k; j<len; j++)
					str[j]=str[j+1];
				len--;
			}
		}
		
		printf("%s\n",str);
		// go till first , or ; or EOL
		j=0;k=0;
		while(str[j]!='\0'){
		 	if(str[j]==',' || str[j]==';'){	
				j++;
				found=0;
				break;
			}
			j++;	
		}
		// printf("%d\n",no_opd);
		
		//go for second , or ; or EOL
		if (found==0){
			if (str[j]!='\0'){
				k=j;
				// printf("ok %d\n",k);
				while(str[k]!='\0'){
					if (str[k]==',' || str[k]==';')
						break;
					k++;
				}
				for (l=j;l<k;l++)
					str1[l-j]=str[l];
				str1[l]='\0';
				// printf("%s\n",str1);
				for (l=0;l<strlen(str1);l++)
					if (str1[l]==' ')
						str1[l]='\0';
				no_opd++;
				if (str1[0]=='#' /*&& str1[strlen(str1)-1]!=']'*/){
					immd=0;
					for(l=1;l<strlen(str1);l++){
						immd=immd*10+(str1[l]-'0');
					}
					instruction[i].imm=immd;
					// printf("%d\n",immd);
				}
				else{
					for(l=0;l<REG_QSIZE;l++){
						if(!strcmp(regstr[l].name,str1)){
							instruction[i].rm=regstr[l].in_op;
							// printf("ok\n");
							break;
						}
					}
				}

			}
			else
				continue;

		
			//go for third operand
			j=k+1;k++;
			if(str[j]!='\0'){
				k=j;
				// printf("ok %d\n",k);
				while(str[k]!='\0'){
					if (str[k]==',' || str[k]==';'||str[k]=='\0')
						break;
					k++;
				}

				for (l=j;l<k;l++)
					str1[l-j]=str[l];
				str1[l]='\0';
				for (l=0;l<strlen(str1);l++)
					if (str1[l]==0x20)
						str1[l]='\0';
				 
				// printf("%s %d %x %x\n",str1,a,str1[a],str1[a-1]);
				no_opd++;
				
				if (str1[0]=='#' /*&& str1[strlen(str1)-1]!=']'*/){
					immd=0;
					// printf("ok %d\n",immd);
					for(l=1;l<strlen(str1);l++){
						immd=immd*10+(str1[l]-'0');
						// printf("ok %d\n",immd);
					}
					instruction[i].imm=immd;
					// printf("%d\n",immd);
				}
				else{
					for(l=0;l<REG_QSIZE;l++){
						if(!strcmp(regstr[l].name,str1)){
							instruction[i].rd2=regstr[l].in_op;
							// printf("ok\n");
							break;
						}
					}
				}
			}
		}
		instruction[i].no_oprnd = no_opd;
	}

	//decode opcode
	for(i=0;i<total_line;i++)
	{
		if(!strcmp(instruction[i].addr_mode,"IMM") || !strcmp(instruction[i].addr_mode,"LDI") || !strcmp(instruction[i].addr_mode,"STI")){
			for(l=0;l<OPI_QSIZE;l++){
				if(!strcmp(opcode_imm[l].name,instruction[i].opcode)){
					instruction[i].in_op=opcode_imm[l].in_op;
					instruction[i].gp_op=opcode_imm[l].gp_op;
					instruction[i].gp_no=opcode_imm[l].gp_no;
					// printf("ok\n");
					break;
				}
			}
		}
		else if(!strcmp(instruction[i].addr_mode,"REG") || !strcmp(instruction[i].addr_mode,"BRR")){
			for(l=0;l<OPR_QSIZE;l++){
				if(!strcmp(opcode_reg[l].name,instruction[i].opcode)){
					instruction[i].in_op=opcode_reg[l].in_op;
					instruction[i].gp_op=opcode_reg[l].gp_op;
					instruction[i].gp_no=opcode_reg[l].gp_no;
					// printf("ok\n");
					break;
				}
			}
		}
		else if(!strcmp(instruction[i].addr_mode,"LDR") || !strcmp(instruction[i].addr_mode,"STR")){
			if(instruction[i].no_oprnd==2){
				for(l=0;l<OPI_QSIZE;l++){
					if(!strcmp(opcode_imm[l].name,instruction[i].opcode)){
						instruction[i].in_op=opcode_imm[l].in_op;
						instruction[i].gp_op=opcode_imm[l].gp_op;
						instruction[i].gp_no=opcode_imm[l].gp_no;
						// printf("ok\n");
						break;
					}
				}
			}
		}
		else{
			instruction[i].in_op=0;
			instruction[i].gp_op=0;
			instruction[i].gp_no=0;
		}

		//instruction[i].hex_value = instruction[i].gp_op;
	}

	return 0;
}

int generate_hex(){
	int i;
	for(i=0;i<total_line;i++)
	{ //rules 
		//gp 1
		if (instruction[i].gp_no==1)
			instruction[i].hex_value = (instruction[i].gp_op + instruction[i].in_op) | (instruction[i].imm*64 | instruction[i].rm*8 | instruction[i].rd1);
		//gp 2
		if (instruction[i].gp_no==2)
			if (!strcmp(instruction[i].addr_mode,"IMM"))
				instruction[i].hex_value = (instruction[i].gp_op + instruction[i].in_op) | (instruction[i].imm*64 | instruction[i].rm*8 | instruction[i].rd1);
			else
				instruction[i].hex_value = (instruction[i].gp_op + instruction[i].in_op) | (instruction[i].rd2*64 | instruction[i].rm*8 | instruction[i].rd1);
		//gp 3
		if (instruction[i].gp_no==3)
			instruction[i].hex_value = (instruction[i].gp_op + instruction[i].in_op) | (instruction[i].imm | instruction[i].rd1*256);
		//gp 4
		if (instruction[i].gp_no==4){
			instruction[i].hex_value = (instruction[i].gp_op + instruction[i].in_op) | (instruction[i].rm*8 | instruction[i].rd1);
			if(instruction[i].rd1==13 || instruction[i].rm==13)//SP
				instruction[i].hex_value |= 1*128;
 		}
 		//gp 5
		if (instruction[i].gp_no==5)
			instruction[i].hex_value = (instruction[i].gp_op + instruction[i].in_op) | (1*64 | instruction[i].rd1*8);
		//gp 7
		if (instruction[i].gp_no==7)
			instruction[i].hex_value = (instruction[i].gp_op + instruction[i].in_op) | (instruction[i].rd2*64 | instruction[i].rm*8 | instruction[i].rd1);
		//gp 9
		if (instruction[i].gp_no==9)
			instruction[i].hex_value = (instruction[i].gp_op + instruction[i].in_op) | ((instruction[i].imm/4)*64 | instruction[i].rm*8 | instruction[i].rd1);
		//gp 13
		// if (instruction[i].gp_no==14)
		// 	instruction[i].hex_value = (instruction[i].gp_op + instruction[i].in_op) | (instruction[i].imm*46 | instruction[i].rm*8 | instruction[i].rd1);
		//gp 14
		// if (instruction[i].gp_no==14)
		// 	instruction[i].hex_value = (instruction[i].gp_op + instruction[i].in_op) | (instruction[i].imm*46 | instruction[i].rm*8 | instruction[i].rd1);

	}
	return 0;
}

int lookup(){
	int i,gpn,gpop,inop;
	char c,str[10];

	//opcode list immediate
	FILE *fl = fopen("opcode.list.immediate","r");
	if(fl==NULL)
		return 1;
	i=0;
	while ((c=fgetc(fl))!=EOF){
		fseek(fl,-1,SEEK_CUR);
		fscanf(fl,"%s",str);
		fscanf(fl,"%d",&gpn);
		fscanf(fl,"%x",&gpop);
		fscanf(fl,"%x",&inop);

		opcode_imm[i].in_op=inop;
		opcode_imm[i].gp_no=gpn;
		opcode_imm[i].gp_op=gpop;
		strcpy(opcode_imm[i].name,str);
		i++;
	}

	close(fl);

	//opcode list register addressing
	fl = fopen("opcode.list.register","r");
	if(fl==NULL)
		return 1;
	
	i=0;
	while ((c=fgetc(fl))!=EOF){
		fseek(fl,-1,SEEK_CUR);
		fscanf(fl,"%s",str);
		fscanf(fl,"%d",&gpn);
		fscanf(fl,"%x",&gpop);
		fscanf(fl,"%x",&inop);
		opcode_reg[i].in_op=inop;
		opcode_reg[i].gp_no=gpn;
		opcode_reg[i].gp_op=gpop;
		strcpy(opcode_reg[i].name,str);
		i++;
	}

	close(fl);

	//register list
	fl = fopen("register.list","r");
	if(fl==NULL)
		return 1;
	
	i=0;
	while ((c=fgetc(fl))!=EOF){
		fseek(fl,-1,SEEK_CUR);
		fscanf(fl,"%s",str);
		fscanf(fl,"%d",&gpn);
		fscanf(fl,"%x",&gpop);
		fscanf(fl,"%x",&inop);
		regstr[i].in_op=inop;
		regstr[i].gp_no=gpn;
		regstr[i].gp_op=gpop;
		strcpy(regstr[i].name,str);
		i++;
	}

	close(fl);
	return 0;
}
