#ifndef IFJ17_COMPILER_FSM_H
#define IFJ17_COMPILER_FSM_H

#define FSM()
#define STATE(x)            s_##x:
#define NEXT_STATE(x)       goto s_##x

#endif //IFJ17_COMPILER_FSM_H
