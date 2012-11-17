ROOT_DIR =.

include Makefile.inc

#Primeiro alvo
all:$(HEX)
#	msp430-strip $(ROOT_DIR)/$(SRC_DIR)/$(ELF)
	cp $(ROOT_DIR)/$(SRC_DIR)/$(ELF) ./bin
	msp430-strip $(ROOT_DIR)/$(SRC_DIR)/$(ELF)
	cp $(ROOT_DIR)/$(SRC_DIR)/$(ELF) ./bin/striped_$(ELF)
#	$(CC) $(OBJ) -Wl,-Map=_$(TARGET).map -o $(ELF) $(LD_FLAGS) $(WAR) 
	$(OD) -S -g ./bin/$(ELF) > ./bin/_$(TARGET).list

$(ROOT_DIR)/$(LIB_DIR)/lib$(NAME).a: MODULES
#	@echo -n "Creating lib ... "
	$(AR) $(AR_FLAGS) $(ROOT_DIR)/$(LIB_DIR)/lib$(NAME).a $(ROOT_DIR)/obj/*.o
#	@echo "Done."

MODULES: 
	cd $(ROOT_DIR)/modules/ ; $(MAKE)

#Gerar arquivo HEX
$(HEX):$(ELF)
	$(CP) -O ihex $(ROOT_DIR)/$(SRC_DIR)/$(ELF) $(ROOT_DIR)/bin/$(HEX)

#Gerando arquivo ELF
$(ELF): $(ROOT_DIR)/$(LIB_DIR)/lib$(NAME).a
#	msp430-strip $(OBJ) -o $(OBJ)
#	$(CC) $(OBJ) -Wl,-Map=_$(TARGET).map -o $(ELF) $(FLAGS) $(WAR) 
#	$(DUMP) -S -g $(ELF) > _$(TARGET).list
	cd $(SRC_DIR) ; $(MAKE)

#Gerando arquivo assembly (não passa por este alvo)
assembly: $(SRC)
	$(CC) $(SRC) -Wl,-Map=_$(TARGET).map -o $(TARGET).s $(FLAGS) -S
	
#Gerando objetos
$(OBJ):$(SRC)
	$(CC) -Wall -c $(SRC) $(FLAGS)

#Gerando documentação	
docs: $(HDR) $(SRC) 
	$(DOXYGEN) $(DOXYGENFLAGS)
	cd ./doc/gen/latex; $(MAKE) ; rm -f *.ps *.dvi *.aux *.toc *.idx *.ind *.ilg *.log *.out *.brf *.blg *.bbl
	
#Gravando programa
flash-target: $(ELF)
	mspdebug rf2500 "prog ./bin/teste.elf"
	
#Limpando os objetos
clean:
	clear
	rm -rf *.o *.elf *.hex *.map *.s *.list ./obj/* ./lib/*
	cd $(SRC_DIR) ; $(MAKE) clean
	cd $(ROOT_DIR)/modules/ ; $(MAKE) clean
	

