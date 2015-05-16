#pragma once

// Max number
#define MAX_TABLE_NUMBER   16
#define MAX_COLUMN_NUMBER  32
#define MAX_RECORD         16384

// Lock type
#define LOCK_FREE       0
#define LOCK_SHARE      1
#define LOCK_EXCLUSIVE  2

// Column Type
#define COLUMN_TYPE_INT   0
#define COLUMN_TYPE_STR   1
#define COLUMN_TYPE_WSTR  2
#define COLUMN_TYPE_BIN   3
#define COLUMN_TYPE_FLOAT 4

// Size of name
#define TABLE_NAME_SIZE  16
#define COLUMN_NAME_SIZE 16
