$(BIN): $(OBJ)
	@echo "    CCLD  $(BIN)"
	@$(CXX) $(OBJ) $(CXXFLAGS) -o ../$(BIN)


%.o: %.cc
	@echo "    CC    $<"
	@$(CXX) -I. -Iinclude $(CXXFLAGS) -c $< -o $@


.PHONY : clean
clean: 
	@for i in $(OBJ); do echo -e "  CLEAN\t" $$i; $(RM) $$i; done
