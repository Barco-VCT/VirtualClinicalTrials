##### Check if the modules in ${defaultmodules} and the personnal modules (listed in manual_conf.txt) are public, restricted, or unusable
macro(Find_Modules)
	SET(user_conf_file ${CMAKE_CURRENT_SOURCE_DIR}/build_${COMPILATION_TYPE}/user_conf.txt)
	IF(EXISTS ${user_conf_file})
	#Recompile if there are some changes in the manual configuration file
	configure_file(${user_conf_file} ${user_conf_file} COPYONLY) 
	
	#####read the manual configuration file; Remove the modules with a '-' from the ${defaultmodules} list and add the other modules to the list: ${addedmodules}
	manage_user_conf()
	ELSE()
		message("No configuration file: Cmake will build the minimal configuration of VCT")
	ENDIF()
	SET(allmodules
		${addedmodules}
		${defaultmodules}
	)
	#check if the module exists, and if it exists if it's a public, or restricted or if some mandatory files are missing
	message("\n#################Find modules#################")
	foreach(module ${allmodules})
	message("\n${module}:")
	IF(EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/src/Modules/${module}/${module}.h)
		message("${module}.h exists")
		IF(EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/src/Modules/${module}/cpp/BuildFull.cmake)
			message("BuildFull.cmake exists")
			message("Add ${module} as a public module")
			SET(publicmodules
				${publicmodules}
				${module}
			)
		ELSE()
			message("BuildFull.cmake does not exist")
			IF(EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/src/Modules/${module}/lib/${module}.lib AND EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/src/Modules/${module}/lib/${module}D.lib)
				message("${module}.lib & ${module}D.lib exist")
				message("Add ${module} as a restricted module")
				SET(restrictedmodules
					${restrictedmodules}
					${module}
				)
			ELSE()
				message("${module}.lib or ${module}D.lib does not exist")
				message("unable to add ${module}")
			ENDIF()
		ENDIF()
	ELSE()
		message("${module}/${module}.h does not exist")
		message("unable to add ${module}")
	ENDIF()
endforeach(module)

	SET(allmodules
		${publicmodules}
		${restrictedmodules}
	)
endmacro()

#####read the user's configuration file; Remove the modules with a '-' from the ${defaultmodules} list and add the other modules to the list: ${addedmodules}
macro(manage_user_conf)
##### read the user's configuration file
	file(READ "${user_conf_file}" usermodules)
	STRING(REGEX REPLACE "\n" ";" usermodules "${usermodules}")
	#### sort the modules into "To remove" (with a '-') "To compile in solution and with library" (with the rprefix "lib_") and "to add"
	foreach(module ${usermodules})
		STRING( FIND ${module} "-" POSITION1 )# Find the modules to remove from the default modules list
		STRING( FIND ${module} "lib_" POSITION2 )# Find the modules to compile the libraries and add to the solution
		STRING( FIND ${module} "#" POSITION3) # Find the commented modules
		IF(${POSITION1} MATCHES 0)
			STRING(REGEX REPLACE "-" "" module "${module}")
			SET(Module_to_remove
				${Module_to_remove}
				${module}
			)
		ELSEIF(${POSITION2} MATCHES 0)
			STRING(REGEX REPLACE "lib_" "" module "${module}")
			SET(Libslist
				${Libslist}
				${module}
			)
			SET(addedmodules
				${addedmodules}
				${module}
			)
		ELSEIF(${POSITION3} MATCHES 0)
		# the module is commented so it is not added to any list
		ELSE()
			SET(addedmodules
				${addedmodules}
				${module}
			)
		ENDIF()
	endforeach(module)
#### remove the "To remove" modules from the "defaultmodules" list
	foreach(defaultmodule ${defaultmodules})
	SET(FOUND
		FALSE
		)
		foreach(removedmodule ${Module_to_remove})
			IF(${defaultmodule} MATCHES ${removedmodule})
				SET(FOUND
					TRUE
				)
			ENDIF()
		endforeach(removedmodule)
		IF(FOUND MATCHES FALSE)
			SET(defaultmodules_temp
				${defaultmodules_temp}
				${defaultmodule}
			)
		ELSE()
			message("The default module ${defaultmodule} has been disabled by the user")
		ENDIF()
	endforeach(defaultmodule)
	SET(defaultmodules
		${defaultmodules_temp}
	)
endmacro()

##### prepare the list of the modules' libraries to be linked to the project
macro(LinkLibraries)
	message(\n"link the libraries:")
	foreach(module ${restrictedmodules})
	message("${module}.lib & ${module}D.lib") 
	SET(FILES_NAME "${FILES_MODULES_${module}}")
	SET ( TARGET_LNK_LIBRARIES
		${TARGET_LNK_LIBRARIES}
		debug ${module}D.lib
		optimized ${module}.lib
	)
	# SET_TARGET_PROPERTIES(TARGET_LNK_LIBRARIES PROPERTIES LINKER_LANGUAGE C)
	SET( LNK_DIRECTORIES
		${LNK_DIRECTORIES}
		${VCT_build_SOURCE_DIR}/vct/src/Modules/${module}/lib/
		)
	endforeach(module)
	LINK_DIRECTORIES(
		${LNK_DIRECTORIES}
	)
endmacro()

##### Exclude from the compilation the files specified by the administrator
macro(Exclude_Files)
#exclude some files from the compilation
foreach(module ${notcompiledmodules})
	#### first exclude the main header of the module
	set_source_files_properties(${CMAKE_CURRENT_SOURCE_DIR}/src/Modules/${module}/${module}.h PROPERTIES HEADER_FILE_ONLY TRUE)
	message("the module: ${module} has been disabled, it will appear in the solution, but won't be compiled")
	#### and then if it is possible, exclude the other files of the modules
	IF(EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/src/Modules/${module}/cpp/BuildFull.cmake)
		include (${CMAKE_CURRENT_SOURCE_DIR}/src/Modules/${module}/cpp/BuildFull.cmake)
		SET(FILES_NAME "${FILES_MODULES_${module}}")
		foreach(f ${FILES_NAME})
			set_source_files_properties(${f} PROPERTIES HEADER_FILE_ONLY TRUE)
		endforeach(f)
	ENDIF()
endforeach(module)
endmacro()


##### Generate the file FactoryModule.cpp from the list of the activated modules
macro(Factory_Generator)
	message("Generate FactoryModule.cpp")
	set(factoryfile ${CMAKE_CURRENT_SOURCE_DIR}/src/Container/FactoryModule.cpp)
	file(WRITE ${factoryfile} )
	file(APPEND ${factoryfile} "//////////////////////////////////////////////////////////////////"\n "//                Simulation                                    //"\n "//                VCT: Virtual Clinical Trials                  //"\n "//////////////////////////////////////////////////////////////////"\n \n)
	file(APPEND ${factoryfile} "#include "\"FactoryModule.h "\"\n")
	foreach(module ${compiledmodules})
		file(APPEND ${factoryfile} "#include "\"../Modules/${module}/${module}.h "\"\n")
	endforeach(module)

	file(APPEND ${factoryfile} "\n// BARCO Libraries\n// Note that this option should be enabled under Project Settings/Preprocessor definitions\n")
	file(APPEND ${factoryfile} "#ifdef _USE_EXT_LIBRARIES_\n#endif\n\n\n")
	file(APPEND ${factoryfile} "using namespace std;\n")
	file(APPEND ${factoryfile} "/*!\n * Construct a Module\n * @param[in] moduleName   Name of the Module to construct (e.g. VideoCardModule, PatternGeneratorModule ...)\n * @param[in] pMevicLogger Pointer to MevicLogger object to assign with the Module to construct\n */\n")
	file(APPEND ${factoryfile} "Module * FactoryModule::construct(string moduleName, MevicLogger * pMevicLogger)\n{\n\tModule * pModule = NULL;\n\n")

	foreach(module ${compiledmodules})
		file(APPEND ${factoryfile} "\tif(moduleName =="\"${module}"\")")
		file(APPEND ${factoryfile} "\n\t\t pModule = new ${module};\n\telse\n")
	endforeach(module)
	
	file(APPEND ${factoryfile} "\t\t pMevicLogger->logError(moduleName+"\" ": not listed in FactoryModule" "\")")
	file(APPEND ${factoryfile} ";\n\t if(NULL != pModule) pModule->SetMevicLogger(pMevicLogger);\n\n\t return pModule;\n}")
endmacro()


##### Make the project to compile the libraries of the modules specified in the variable ${Libslist}
macro(Compile_Libraries)
	message("\nLibraries to compile:")
	foreach(module ${Libslist})
		message("${module}")
		IF(EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/src/Modules/${module}/${module}.h AND EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/src/Modules/${module}/cpp/BuildFull.cmake)
			SET(lib "${module}Lib")
			SET(FILES_NAME "${FILES_MODULES_${module}}")
			ADD_LIBRARY ( ${lib} 
				${FILES_NAME}
			)
			SET_TARGET_PROPERTIES ( ${lib} PROPERTIES 
				#ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/src/Modules/${module}/lib
				OUTPUT_NAME "${module}"
			)
			SET_TARGET_PROPERTIES ( ${lib} PROPERTIES 
				COMPILE_FLAGS "/Z7"
			)
			STRING(REGEX REPLACE "/" "\\\\" CURRENT_DIR_LIB ${CMAKE_CURRENT_SOURCE_DIR})
			ADD_CUSTOM_COMMAND(
				TARGET ${lib} 
				POST_BUILD
				COMMAND if not exist ${CURRENT_DIR_LIB}\\src\\Modules\\${module}\\lib\\ mkdir ${CURRENT_DIR_LIB}\\src\\Modules\\${module}\\lib\\
				# Compile the debug library in debug mode
				COMMAND if $(ConfigurationName) == Debug (
				copy /Y .\\$(ConfigurationName)\\${module}.lib ${CURRENT_DIR_LIB}\\src\\Modules\\${module}\\lib\\${module}D.lib
				) ELSE (
				# Compile the release library in release mode
					copy /Y .\\$(ConfigurationName)\\${module}.lib ${CURRENT_DIR_LIB}\\src\\Modules\\${module}\\lib
				) ###############FINALIZE THIS FOR ALL THE MODULES / THEN UPDATE THE EXTERNALS WITH THE CORRECT LIBRARIES PATHS AND NAME / AND THEN TRY THIS OUT !!!!!!!!!!
				)
			SET_PROPERTY(TARGET ${lib}         PROPERTY FOLDER "Libraries")
			# ADD_DEPENDENCIES ( VCT ${lib} )
		ELSE()
			message("${module}.h or BuildFull.cmake is missing, unable to make a library project")
		ENDIF()
	endforeach(module)
	message("\n")
endmacro()

##### Write the list of the compiled modules in a text file to be compared with the list of the test unit, in order to avoid to run a test unit of a module that is not currently on VCT
macro(Generate_list_of_modules_for_test_units)
set(listfile ${CMAKE_CURRENT_SOURCE_DIR}/../test_units/night_build/svn_files/list_modules.txt)
	file(WRITE ${listfile})
	foreach(module ${compiledmodules})
		file(APPEND ${listfile} "${module}"\n)
	endforeach()
endmacro()

##### suppress the redundancy in a module list: the argument is the NAME of the list to compute, not the list itself
macro(suppress_redundancy Li)
# copy the list whose name is in argument of this macro
set(Lis "${${Li}}")
SET(listtemp )
	foreach(module ${Lis})
		SET(FOUND
			FALSE
		)
		foreach(moduletemp ${listtemp})
			IF("${module}" STREQUAL "${moduletemp}")
				SET(FOUND 
					TRUE
				)
			ENDIF()
		endforeach(moduletemp)
		IF(FOUND MATCHES FALSE)
			SET(listtemp
				${listtemp}
				${module}
			)
		ENDIF()
	endforeach(module)
	# copy the list without redundancy in the list whose name is in argument of this macro
	SET(${Li}
		${listtemp}
	)
endmacro()
macro(test)
#
endmacro()

