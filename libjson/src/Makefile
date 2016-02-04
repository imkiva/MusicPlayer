$(LIB): $(OBJ)
	@echo "    AR    $(LIB)"
	@$(AR) -rs ../$(LIB) $(OBJ)


%.o: %.cpp
	@echo "    CC    $<"
	@$(CXX) -I. -Iinclude -I../include $(CXXFLAGS) -c $< -o $@


.PHONY : clean
clean: 
	@for i in $(OBJ); do echo -e "  CLEAN\t" $$i; $(RM) $$i; done
