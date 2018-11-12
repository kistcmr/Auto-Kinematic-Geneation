#pragma once 

#define RCMD_BUTTON         (0x0001)
#define RCMD_USER           (0x1000)
#define RCMD_RESERVED       (0x2000)

// Reserved commands
#define RESERVED_CMD_GO_HOME		(RCMD_RESERVED + 3)
#define RESERVED_CMD_EMERGENCY_STOP			(RCMD_RESERVED + 4)

#define USER_CMD_SELECT_JOINTS		(RCMD_USER + 1)
#define USER_CMD_SELECT_KEEP_AXIS	(RCMD_USER + 2)

#define USER_CMD_TASK_CTRL			(RCMD_USER + 10)
#define USER_CMD_TASK_MOVE			(RCMD_USER + 11)

// ADDED (since ver. 1.7.2): for direct teaching (by joint compliance control)
#define USER_CMD_DIRECT_TEACHING	(RCMD_USER + 12)

// Command for teaching waypoint
#define USER_CMD_RESET_WAYPOINT			(RCMD_USER + 40)
#define USER_CMD_FINISH_WAYPOINT			(RCMD_USER + 41)

#define USER_CMD_INSERT_WAYPOINT			(RCMD_USER + 31)
#define USER_CMD_DELETE_WAYPOINT		(RCMD_USER + 32)
#define USER_CMD_EXECUTE_WAYPOINT		(RCMD_USER + 33)
#define USER_CMD_SET_WAYPOINT_MODE	(RCMD_USER + 34)