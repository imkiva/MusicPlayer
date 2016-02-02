$(BIN): $(OBJ)
	@echo "    CCLD  $(BIN)"
	@$(CXX) $(OBJ) $(CXXFLAGS) -o ../$(BIN)


%.o: %.cc
	@echo "    CC    $<"
	@$(CXX) -I. -Iinclude $(CXXFLAGS) -c $< -o $@


.PHONY : clean
clean: 
	@echo -e "  CLEAN\t" $(OBJ)
	@$(RM) $(OBJ) 2>/dev/null
