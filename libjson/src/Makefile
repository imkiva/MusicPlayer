$(LIB): $(OBJ)
	@echo "    AR    $(LIB)"
	@$(AR) -rs ../$(LIB) $(OBJ)


%.o: %.cpp
	@echo "    CC    $<"
	@$(CXX) -I. -Iinclude -I../include $(CXXFLAGS) -c $< -o $@


.PHONY : clean
clean: 
	@echo -e "  CLEAN\t" $(OBJ)
	@$(RM) $(OBJ) 2>/dev/null
