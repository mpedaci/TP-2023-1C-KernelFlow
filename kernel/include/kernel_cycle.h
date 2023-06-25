#ifndef KERNEL_CYCLE_H
#define KERNEL_CYCLE_H

#include <stdbool.h>

#include "kernel_structs.h"
#include "kernel_vars.h"
#include "communication.h"
#include "queue_controller.h"
#include "kernel_instructions.h"

#include "types.h"
#include "types_controllers.h"

// NEW -> READY

bool request_t_segment(t_pcb *pcb);

// EXEC -> READY | BLOCK | EXIT

void EXEC(t_pcb *pcb, bool is_second_execution);
void send_pcb_to_cpu(t_pcb *pcb);
t_pcontexto_desalojo *receive_pcontext_from_cpu();
void update_pcb(t_pcb *pcb, t_pcontexto_desalojo *pcontexto);
void process_pcontexto_desalojo(t_pcb *pcb, t_pcontexto_desalojo *pcontexto_response);

// EXIT

void EXIT(t_pcb *pcb);

#endif /* KERNEL_CYCLE_H */