struct open_params {
	string file<>;
	int flags;
	int mode;
};

struct creat_params {
	string file<>;
	int mode;
};

struct lseek_params {
	int fd;
	int offset;
	int whence;
};

struct write_params {
	int fd;
	char bytes<>;
};

struct read_params {
	int fd;
	int max;
};

struct read_retval {
	int status;
	char bytes<>;
};

program EPNFS {
	version EPNFS_V1 {
		int EPL_OPEN(open_params) = 1;
		int EPL_CREAT(creat_params) = 2;
		int EPL_LSEEK(lseek_params) = 3;
		int EPL_CLOSE(int) = 4;
		int EPL_WRITE(write_params) = 5;
		read_retval EPL_READ(read_params) = 6;
	} = 1;
} = 0x331410ff;
