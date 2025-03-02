#pragma once

#define COMCALL0(OBJ, FUNC_NAME) ((OBJ)->lpVtbl->FUNC_NAME(OBJ))
#define COMCALL(OBJ, FUNC_NAME, ...) ((OBJ)->lpVtbl->FUNC_NAME(OBJ, __VA_ARGS__))