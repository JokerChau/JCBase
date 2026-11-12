#pragma once

typedef enum JC_IOStatus {
	JC_IO__NULL_FILE = -100,
	JC_IO__SUCCESSFUL_OP = 1000,
	JC_IO__WRITE_FAILED,
} JC_IOStatus;