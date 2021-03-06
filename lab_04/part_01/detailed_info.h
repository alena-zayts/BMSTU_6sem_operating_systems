#define ENVIRON_DETAILED_INFO "Данный файл содержит исходное окружение, которое было установлено при запуске текущего процесса (вызове execve()). Если после вызова execve() окружение процесса будет модифицировано, этот файл не отразит внесенных изменений.\nНекоторые переменные окружения:\nLS_COLORS - используется для определения цветов, с которыми будут выведены имена файлов при вызове ls.\nLESSCLOSE, LESSOPEN – определяют пре- и пост- обработчики файла, который открывается при вызове less.\nXDG_MENU_PREFIX, XDG_VTNR, XDG_SESSION_ID, XDG_SESSION_TYPE, XDG_DATA_DIRS, XDG_SESSION_DESKTOP, XDG_CURRENT_DESKTOP, XDG_RUNTIME_DIR, XDG_CONFIG_DIRS, DESKTOP_SESSION – переменные, необходимые для вызова xdg-open, использующейся для открытия файла или URL в пользовательском приложении.\nLANG – язык и кодировка пользователя.\nDISPLAY – указывает приложениям, куда отобразить графический пользовательский интерфейс.\nGNOME_SHELL_SESSION_MODE, GNOME_TERMINAL_SCREEN, GNOME_DESKTOP_SESSION_ID, GNOME_TERMINAL_SERVICE, GJS_DEBUG_OUTPUT, GJS_DEBUG_TOPICS, GTK_MODULES, GTK_IM_MODULE, VTE_VERSION – переменные среды рабочего стола GNOME.\nCOLORTERM – определяет поддержку 24-битного цвета.\nUSER – имя пользователя, от чьего имени запущен процесс,\nUSERNAME – имя пользователя, кто инициировал запуск процесса.\nSSH_AUTH_SOCK - путь к сокету, который агент использует для коммуникации с другими процессами.\nTEXTDOMAINDIR, TEXTDOMAIN – директория и имя объекта сообщения, получаемого при вызове gettext.\nPWD – путь к рабочей директории.\nHOME – путь к домашнему каталогу текущего пользователя.\nSSH_AGENT_PID - идентификатор процесса ssh-agent.\nTERM – тип запущенного терминала.\nSHELL – путь к предпочтительной оболочке командной строки.\nSHLVL – уровень текущей командной оболочки.\nLOGNAME – имя текущего пользователя.\nPATH - список каталогов, в которых система ищет исполняемые файлы.\n_ - полная командная строка процесса\nOLDPWD - путь к предыдущему рабочему каталогу.\n"

#define EXE_DETAILED_INFO "Эта символическая ссылка может быть разыменована обычным образом; попытка открыть его откроет исполняемый файл. Можно даже ввести/proc/[pid]/exe, чтобы запустить другую копию того же исполняемого файла, который запускается процессом [pid]. Если путь не связан, символическая ссылка будет содержать строку '(удалено)', добавленную к исходному имени пути. В многопоточном процессе содержимое этой символической ссылки недоступно, если основной поток уже завершился.\n"

#define FD_DETAILED_INFO "Данная поддиректория содержит одну запись для каждого файла, который открыт процессом. Имя каждой такой записи соответствует номеру файлового дескриптора и является символьной ссылкой на реальный файл. Так, 0 - это стандартный ввод, 1 - стандартный вывод, 2 - стандартный вывод сообщений об ошибках и т. д.\nДля файловых дескрипторов сокетов и программных каналов записи будут являться символьными ссылками, содержащими тип файла с inode. Вызов readlink() для них вернет строку следующего формата: type:[inode].\nДля файловых дескрипторов, не имеющих соответствующего inode, символьная ссылка будет иметь следующий вид: anon_inode:<file-type>.\nДля мультипоточных процессов содержимое данной поддиректории может быть недоступно, если главный поток завершил свое выполнение.\nПрограммы, которые принимают имя файла в качестве аргумента командной строки, но не используют стандартный ввод, и программы, которые пишут в файл, но не используют стандартный вывод, могут использовать стандартный ввод и вывод в качестве аргументов командной строки с помощью файлов из /proc/[pid]/fd.\n"

#define MAPS_DETAILED_INFO "Девайсы идентифицируются старшим и младшим (major and minor) номерами устройств. Одно и то же устройство может иметь один и тот же major, но разные minor.\n    Начальный адрес участка памяти-конечный участок участка памяти\n    Права доступа(r - доступно для чтения, w - доступно для изменения, x - доступно для выполнения, s - разделяемый, p - private (copy on write))\n    смещение распределения\n    старший:младший номер устройства\n      inode\n"

#define PAGEMAP_DETAILED_INFO "Он содержит одно 64-разрядное значение для каждой виртуальной страницы, причем биты установлены следующим образом:\n     63 Если задано, страница присутствует в оперативной памяти.\n      62 Если задано, страница находится в пространстве подкачки\n      61 (начиная с Linux 3.5) Страница представляет собой страницу, сопоставленную с файлом, или общую анонимную страницу.\n     60-57 (начиная с Linux 3.11) Ноль\n      56 (начиная с Linux 4.2) Страница отображается исключительно на карте. 55 (начиная с Linux 3.11)\n      PTE имеет мягкое загрязнение\n      54-0 Если страница присутствует в оперативной памяти (бит 63), то эти биты обеспечивают номер фрейма страницы, который можно использовать для индексации /proc/kpageflags и /proc/kpagecount. Если страница присутствует в режиме подкачки (бит 62), то биты 4-0 задают тип подкачки, а биты 54-5 кодируют смещение подкачки.\nФайл /proc/[pid]/pagemap присутствует только в том случае, если включена опция конфигурации ядра CONFIG_PROC_PAGE_MONITOR\n"


char *STAT_DETAILED_INFO[] = {
    "pid - ID процесса",
    "comm - Имя файла",
    "state - Состояние процесса",
    "ppid - ID родительского процесса",
    "pgrp - ID группы процесса",
    "session - ID сессии процесса",
    "tty_nr - управляющий терминал процесса",
    "tpgid - ID внешней группы процессов управляющего терминала",
    "flags - Флаги ядра процесса",
    "minflt - Количество минорных ошибок процесса (Минорные ошибки не включают ошибки загрузки страниц памяти с "
    "диска)",
    "cminflt - Количество минорных ошибок дочерних процессов (Минорные ошибки не включают ошибки загрузки страниц "
    "памяти с диска)",
    "majflt - Количество Мажорных ошибок процесса",
    "cmajflt - Количество Мажорных ошибок дочерних процессов процесса",
    "utime - Количество времени, в течение которого этот процесс исполнялся в пользовательском режиме",
    "stime - Количество времени, в течение которого этот процесс исполнялся в режиме ядра",
    "cutime - Количество времени, в течение которого дети этого процесса исполнялись в "
    "пользовательском режиме",
    "cstime - Количество времени, в течение которого дети этого процесса были запланированы в режиме "
    "ядра",
    "priority - Приоритет процесса",
    "nice - уровень nice процесса, то есть пользовательский приоритет процесса (-20 - высокий, 19 - низкий)",
    "num_threads - Количество потоков",
    "itrealvalue - Время в тиках до следующего SIGALRM отправленного в процесс из-за интервального таймера",
    "starttime - Время, прошедшее с момента загрузки системы, когда процесс был запущен",
    "vsize - Объем виртуальной памяти в байтах",
    "rss - Resident Set (Memory) Size: Количество страниц процесса в физической памяти",
    "rsslim - Текущий предел в байтах для RSS процесса",
    "startcode - Адрес, над которым может работать текст программы",
    "endcode - Адрес, до которого может работать текст программы",
    "startstack - Адрес начала стека",
    "kstkesp - Текущее значение ESP (Stack pointer)",
    "kstkeip - Текущее значение EIP (instruction pointer)",
    "pending - Битовая карта отложенных сигналов, отображаемое в виде десятичного числа",
    "blocked - Битовая карта заблокированных сигналов, отображаемое в виде десятичного числа",
    "sigign - Битовая карта игнорированных сигналов, отображаемое в виде десятичного числа",
    "sigcatch - Битовая карта пойманных сигналов, отображаемое в виде десятичного числа",
    "wchan - Адрес ядрёной функции, где процесс находится в состоянии сна",
    "nswap - Количество страниц, поменявшихся местами",
    "cnswap - Накопительный своп для дочерних процессов",
    "exit_signal - Сигнал, который будет послан родителю, когда процесс будет завершен",
    "processor - Номер процессора, на котором происходило последнее выполнение",
    "rt_priority - Приоритет планирования в реальном времени - число в диапазоне от 1 до 99 для процессов, "
    "запланированных в соответствии с политикой реального времени",
    "policy - Политика планирования",
    "blkio_ticks - Общие блочные задержки ввода/вывода",
    "gtime - Гостевое время процесса",
    "cgtime - Гостевое время дочерних процессов",
    "start_data - Адрес, над которым размещаются инициализированные и неинициализированные данные программы (BSS)",
    "end_data - Адрес, под которым размещаются инициализированные и неинициализированные данные программы (BSS)",
    "start_brk - Адрес, выше которого куча программ может быть расширена с помощью brk (станавливает конец "
    "сегмента данных в значение, указанное в аргументе end_data_segment, когда это значение является приемлимым, "
    "система симулирует нехватку памяти и процесс не достигает своего максимально возможного размера сегмента "
    "данных)",
    "arg_start - Адрес, над которым размещаются аргументы командной строки программы (argv)",
    "arg_end - Адрес, под которым размещаются аргументы командной строки программы (argv)",
    "env_start - Адрес, над которым размещена программная среда",
    "env_end - Адрес, под которым размещена программная среда",
    "exit_code - Состояние выхода потока в форме, сообщаемой waitpid"};


char *STATM_DETAILED_INFO[] = {"size - общее число страниц выделенное процессу в виртуальной памяти",
                         "resident - количество страниц, загруженных в физическую память",
                         "shared - количество общих резедентных страниц",
                         "trs - количество страниц кода",
                         "lrs - количество страниц библиотеки",
                         "drs - количество страниц данных/стека",
                         "dt - dirty pages - данные для записи находятся в кэше страниц, но требуется к "
                         "первоначальной записи на носитель"};

char *IO_DETAILED_INFO[] = {"символы для чтения - количество байтов, которые этот процесс запрашивал считать из хранилища (сумма байтов, которые процесс запрашивал у read(2) и аналогичных системных вызовов)",
"символы для записи - количество байтов, которые этот процесс передал или должен передать для записи",
"количество системных вызовов чтения (read(2), pread(2) и т.д.)",
"количество системных вызовов записи (write(2), pwrite(2) и т.д.)",
"прочитанные байты - количество байтов, которые этот процесс действительно считал",
"записанные байты - количество байтов, которые этот процесс действительно записал",
"байты, отмененные на запись - (дело в усечении). Если процесс записывает 1 МБ в файл, а затем удаляет файл, он фактически не будет выполнять запись. Но это будет учтено как отправка 1 МБ для записи. Другими словами: это поле представляет количество байтов для записи, которые не были фактически записаны из-за усечения кэша. Процесс может привести к 'отрицательному' вводу-выводу. Если процесс усекает некоторый 'грязный' кэш страниц, некоторый ввод-вывод, который был учтен другой задачей (в ее write_bytes), не будет выполняться."};
