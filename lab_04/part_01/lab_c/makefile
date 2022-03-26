# ./lab_c.exe func_tests/pos_02_in.txt func_tests/out.txt Added 10

CC := gcc
INCPATH := ../inc/
OUTPATH := ./out/
SRCPATH := ./src/
UNIPATH := ./unit_tests/
SRCS := $(wildcard *.c)
INC := -I $(INCPATH)
CFLAGS := -std=c99 -Wall -Werror -Wextra -Wfloat-equal -pedantic -Wfloat-conversion -Wvla


#2 варианта сборки
.PHONY : debug 
debug : CFLAGS += -g3
debug : lab_c.exe

.PHONY : release
release : CFLAGS += -DNDEBUG -g0
release : lab_c.exe

# Общие объектные файлы
OBJS := $(OUTPATH)insert.o $(OUTPATH)my_getdelim.o $(OUTPATH)pop_front.o $(OUTPATH)remove_duplicates.o $(OUTPATH)sort.o $(OUTPATH)task.o $(OUTPATH)task_list.o

# Объектные файлы, нужные только в модульном тестировании
OBJSUNIT := $(OUTPATH)check_insert.o $(OUTPATH)check_pop_front.o $(OUTPATH)check_remove_duplicates.o $(OUTPATH)check_sort.o  $(OUTPATH)check_task.o $(OUTPATH)check_task_list.o $(OUTPATH)check_my_getdelim.o

#сборка основной программы
lab_c.exe : $(OBJS) $(OUTPATH)app.o
	$(CC) $^ -o $@
run: lab_c.exe
	./lab_c.exe func_tests/pos_02_in.txt func_tests/out.txt Added 10
#сборка модульных тестов
unit_tests.exe : $(OBJS) $(OBJSUNIT) $(OUTPATH)check_main.o 
	$(CC) $^ -o $@ -lcheck

#компоновка тестирующих модулей
$(OUTPATH)check_%.o : $(UNIPATH)check_%.c
	cd out && $(CC) $(CFLAGS) $(INC) -c ../$<
	
#компоновка основных модулей
$(OUTPATH)%.o : $(SRCPATH)%.c
	cd out && $(CC) $(CFLAGS) $(INC) -c ../$<
	
# сборка и прогон модульных тестов	
.PHONY : unit 	
unit : unit_tests.exe
	./unit_tests.exe

# прогон функциональных тестов
.PHONY : func  	
func : app.exe
	cd func_tests && ./run_tests_10_01.bat
    
 
#зависиммости
%.d : %.c
	$(CC) -M $< > $@

include $(SRCS:.c=.d)

#очистка
.PHONY : clean 
clean :
	$(RM) $(OUTPATH)*.o *.exe *.d

# сборка и прогон модульных тестов с проверкой drmemory
.PHONY : check_mem_unit 
check_mem_unit : unit_tests.exe
	drmemory -lib_blacklist "*" -batch -- ./unit_tests.exe
	
# прогон функциональных тестов с проверкой drmemory	
.PHONY : check_mem_func 
check_mem_func : app.exe
	echo "negatives"
	drmemory -lib_blacklist "*" -batch -- ./app.exe func_tests\\pos_in.txt func_tests\\out.txt Test; true
	drmemory -lib_blacklist "*" -batch -- ./app.exe func_tests\\pos_in.txt func_tests\\out.txt Test 2 toomany; true
	drmemory -lib_blacklist "*" -batch -- ./app.exe func_tests\\pos_in.txt func_tests\\out.txt Test a; true
	drmemory -lib_blacklist "*" -batch -- ./app.exe func_tests\\pos_in.txt func_tests\\out.txt Test .1; true
	drmemory -lib_blacklist "*" -batch -- ./app.exe func_tests\\pos_in.txt func_tests\\out.txt Test -1; true
	drmemory -lib_blacklist "*" -batch -- ./app.exe func_tests\\neg_06_in.txt func_tests\\out.txt Test 1; true
	drmemory -lib_blacklist "*" -batch -- ./app.exe func_tests\\neg_07_in.txt func_tests\\out.txt Test 1; true
	drmemory -lib_blacklist "*" -batch -- ./app.exe func_tests\\neg_09_in.txt func_tests\\out.txt Test 1; true
	drmemory -lib_blacklist "*" -batch -- ./app.exe func_tests\\neg_10_in.txt func_tests\\out.txt Test 1; true
	drmemory -lib_blacklist "*" -batch -- ./app.exe func_tests\\neg_11_in.txt func_tests\\out.txt Test 1; true
	drmemory -lib_blacklist "*" -batch -- ./app.exe func_tests\\neg_12_in.txt func_tests\\out.txt Test 1; true
	echo "positives"
	drmemory -lib_blacklist "*" -batch -- ./app.exe func_tests\\pos_01_in.txt func_tests\\out.txt Added 10
	drmemory -lib_blacklist "*" -batch -- ./app.exe func_tests\\pos_03_in.txt func_tests\\out.txt Added 10
	drmemory -lib_blacklist "*" -batch -- ./app.exe func_tests\\pos_04_in.txt func_tests\\out.txt Added 10
	drmemory -lib_blacklist "*" -batch -- ./app.exe func_tests\\pos_05_in.txt func_tests\\out.txt Added 10
	drmemory -lib_blacklist "*" -batch -- ./app.exe func_tests\\pos_06_in.txt func_tests\\out.txt Added 10
	drmemory -lib_blacklist "*" -batch -- ./app.exe func_tests\\pos_07_in.txt func_tests\\out.txt Test 1
	
	