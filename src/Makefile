include ../Makefile.inc

SRC= main.c

ROOT_DIR =..

#OBJS = ${SRCS:.c=.o}

#Primeiro alvo
all:$(ELF)

#Gerando arquivo assembly (não passa por este alvo)
assembly: $(SRC)
	$(CC) $(SRC) -Wl,-Map=_$(TARGET).map -o $(TARGET).s $(FLAGS) -S
	
#Gerando objetos
$(ELF):$(SRC)
#	$(CC) -Wall -c $(SRC) $(CC_FLAGS)
	$(CC) $(SRC) -Wl,-Map=$(ROOT_DIR)/bin/_$(TARGET).map -o $(ELF)  $(LD_FLAGS)

	
#Limpando os objetos
clean:
	rm -rf *.o *.elf *.hex *.map *.s *.list

