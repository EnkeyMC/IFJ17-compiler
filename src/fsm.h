#ifndef IFJ17_COMPILER_FSM_H
#define IFJ17_COMPILER_FSM_H

/*
 * Macros to easily create Finite State Machines
 *
 * Example usage:
 *
 * FSM() {
 *      STATE(start_state) {
 *          NEXT_STATE(ex1);
 *      }
 *
 *      STATE(ex1) {
 *          if (var == 42)
 *              NEXT_STATE(start_state);
 *          else
 *              return 42;
 *      }
 * }
 */
#define FSM()
#define STATE(x)            s_##x:
#define NEXT_STATE(x)       goto s_##x

#endif //IFJ17_COMPILER_FSM_H
