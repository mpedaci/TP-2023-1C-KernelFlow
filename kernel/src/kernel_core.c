#include "kernel_core.h"

// CORE

void start_kernel_core()
{
    recursos_create();
    archives_create();
    all_pcb_create();
    queues_create();
    pthread_mutex_init(&mutex_NEW_to_READY, NULL);
    pthread_mutex_init(&mutex_fs_connection, NULL);
    pthread_create(&thr_core, 0, process_queues, NULL);
}

void end_kernel_core()
{
    core_running = false;
    pthread_join(thr_file, NULL);
    pthread_join(thr_io, NULL);
    pthread_join(thr_core, NULL);
    // AGREGAR DESTRUCTORES
    queues_destroy();
    all_pcb_destroy();
    archives_destroy();
    recursos_destroy();
    pthread_mutex_destroy(&mutex_NEW_to_READY);
    pthread_mutex_destroy(&mutex_fs_connection);
    log_info(logger_aux, "Thread Kernel Core: finalizado");
}

// CONTROLLER

void *process_queues()
{
    while (core_running)
    {
        // NEW -> READY (FIFO)
        move_NEW_to_READY();
        // READY -> EXEC
        move_READY_to_EXEC();

        sleep(1);
    }
    log_info(logger_aux, "Thread Process Queues: finalizado");
    pthread_exit(0);
}

// STATUS MOVES

void move_NEW_to_READY()
{
    pthread_mutex_lock(&mutex_NEW_to_READY);
    while (can_move_NEW_to_READY())
    {
        t_pcb *pcb = get_pcb_from(QNEW);
        bool ts_created = request_t_segment(pcb);
        if (ts_created)
            move_pcb_from_to(pcb, QNEW, QREADY);
        else
        {
            move_pcb_from_to(pcb, QNEW, QEXIT);
            pcb->exit_status = ERROR;
            EXIT(pcb);
        }
    }
    pthread_mutex_unlock(&mutex_NEW_to_READY);
}

void move_READY_to_EXEC()
{
    if (can_execute_process())
    {
        apply_planner_algorithm();
        t_pcb *to_execute = move_fist_from_to(QREADY, QEXEC);
        if (to_execute != NULL)
        {
            if (algorithm_is("HRRN"))
            {
                t_temporal *tiempo_actual = temporal_create();
                to_execute->est_sig_rafaga = calculate_s(to_execute, tiempo_actual);
                temporal_destroy(tiempo_actual);
            }
            EXEC(NULL, false);
        }
    }
}

// CONDICIONES

int get_actual_multiprog()
{
    return get_queue_size(QREADY) + get_queue_size(QEXEC) + get_queue_size(QBLOCK);
}

bool accept_new_process_in_READY()
{
    return get_actual_multiprog() < config_kernel->grado_max_multiprog;
}

bool can_move_NEW_to_READY()
{
    return get_queue_size(QNEW) > 0 && accept_new_process_in_READY();
}

bool can_execute_process()
{
    return !is_queue_empty(QREADY) && is_queue_empty(QEXEC);
}
