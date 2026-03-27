After cloning/downloading the repository, copy freeglut.dll from Ants_AI/Ants_AI/Libraries/lib/freeglut.dll to Ants_AI/Debug.

If the program still does not run, create a new visual studio project and follow these instructions ([proj_dir] is your project's name):
	1. copy the Libraries folder and the .cpp and .h files to [proj_dir]/[proj_dir]
	1. copy glad.c to [proj_dir]
	3. create a folder [proj_dir]/Debug
	4. copy freeglut.dll from Libraries/lib to [proj_dir]/Debug
	5. create a folder [proj_dir]/saved_generations
	6. In project>properties switch Configuration (dropdown selectors at the top) to Debug and Win32
	7. In project>properties:
		> linker > input > additional dependencies
			--> change to "glew32s.lib;SOIL.lib;freeglut.lib;%(AdditionalDependencies)"

		> configuration properties > vc++ directories > include directories
			--> change to "$(ProjectDir)/Libraries/include;$(IncludePath)"

		> configuration properties > vc++ directories > library directories
			--> change to "$(ProjectDir)/Libraries/lib;$(LibraryPath)"


If it still does not work, send me a message. Try looking online for another method to setup the libraries used in this project.