#define   TNC_TX_PROCESS     100
#define   TNC_RX_PROCESS     101
#define   TNC_TX_ERROR       102
#define   TNC_RX_ERROR       103
#define   TNC_TX_SCHED       104
#define   TNC_RX_SCHED       105
#define   TNC_TX_MAINT       106
#define   TNC_RX_MAINT       107

#define   ERROR_PROCESS      200
#define   COMS_RX_PROCESS    201

#define   SCHED_RX_PROCESS   301
#define   SCHED_TX_PROCESS   301


#define   GPIB_TX_VAL1       400
#define   GPIB_TX_VAL2       401
#define   GPIB_TX_VAL11      402
#define   GPIB_TX_VAL22      403

int msqid_tnc_tx_error,        msqid_tnc_tx_error;
int msqid_tnc_tx_sched,        msqid_tnc_rx_sched;
int msqid_tnc_tx_maint,        msqid_tnc_rx_maint;
int msqid_process_tx_maint,    msqid_process_rx_maint;
int msqid_gpib_tx_error,       msqid_gpib_rx_error;
int msqid_gpib_tx_process,     msqid_gpib_rx_process;
int ipc_error;

#define FF 12
