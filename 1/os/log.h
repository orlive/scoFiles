#define ERROR_LOG {char tmp[20];sprintf(tmp,"<%d>",err_log++);log_print(tmp);}
int err_log = 0;
