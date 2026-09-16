#NMAKE

CC_FLAGS = /Ox /Os /EHsc /DUNICODE /nologo

nothing:
	@echo Options:
	@echo ========
	@echo balls_2d, balls_3d, fire,
	@echo matrix_code, matrix_trace,
	@echo nyancat, sokoban
	@echo.

balls_2d: cge_balls.cpp
	@cl $(CC_FLAGS) cge_balls.cpp /DBALLS_2D=1 -Febin/balls_2d
	@del *.obj
	
balls_3d: cge_balls.cpp
	@cl $(CC_FLAGS) cge_balls.cpp /DBALLS_2D=0 -Febin/balls_3d
	@del *.obj
	
fire:
	@cl $(CC_FLAGS) cge_fire.cpp -Febin/fire
	@del *.obj

matrix_code:
	@cl $(CC_FLAGS) cge_matrix_code.cpp -Febin/matrix_code
	@del *.obj
	
matrix_trace:
	@cl $(CC_FLAGS) cge_matrix_trace.cpp -Febin/matrix_trace
	@del *.obj
	
nyancat:
	@cl $(CC_FLAGS) cge_nyancat.cpp winmm.lib -Febin/nyancat
	@del *.obj
	
sokoban:
	@cl $(CC_FLAGS) cge_sokoban.cpp -Febin/sokoban
	@del *.obj
	
clean:
	@del bin/*.exe
	
all: balls_2d balls_3d fire matrix_code matrix_trace nyancat sokoban

