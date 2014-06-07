#if defined(__STDC__) || defined(__cplusplus)
# define P_(s) s
#else
# define P_(s) ()
#endif



/* gl_tools.c */
GSString_t *glink_pack_string_t P_((char *cstring, GSString_t *ps));
GSLongInt_t glink_unpack_string_t P_((GSString_t *ps, char *cstring, GSUlong_t size));
GSMeasurement_t *glink_pack_measurement P_((GSMeasurement_t *pMeasurement, GSDP_t value, char *type, char *unit));
GSUnit_t *glink_pack_unit P_((GSUnit_t *pUom, char *type, char *unit));


/* gs_alloc.c */
char *gs_shm_alloc P_((u_int size, GSKey_t *key, int *mid));
int gs_shm_free P_((char *ptr));
int gs_shm_remove P_((int id));
char *gs_alloc P_((unsigned nbytes));
void gs_free P_((char *aptr));
char *gs_init_pool P_((int size, key_t key));
int gs_term_pool P_((void));

/* gs_faccess.c */
int gs_faccess P_((char *file, char *mode, char *arg));

/* gs_chk_debug.c */
int gs_chk_debug P_((void));
int gs_set_debug P_((int mode));

/* gs_connect.c */
GSHandle_t *gs_connect P_((int *ac, char ***av));
int gsz_attach_slot P_((GSKey_t *key, GSSlot_t **pslot));

/* gs_cm_do_exec.c */
int gs_cm_process_msg P_((GSGBL_t *pcm, GSMessage_t *pmsg, GSMessage_t *preplymsg));
int gs_do_exec P_((GSMessage_t *pmsg, GSMessage_t *preply));
int gs_cfgstr P_((char *entry, char *item, char *outbuf, int *outlen));
int gs_cfgnum P_((char *entry, char *item, int *result));
int gs_status_msg P_((GSMessage_t *pmsg, int status));
int gs_data_msg P_((GSMessage_t *pmsg, char *data, int len));

/* gs_config.c */
int gs_cgetent P_((char *bp, char *name));
int gs_cgetnum P_((char *id));
int gs_cgetflag P_((char *id));
char *gs_cgetstr P_((char *id, char **area));

/* gs_cf_name.c */
char *gs_cf_name P_((char *cfgfile));

/* gs_get_handle.c */
GSHandle_t *gs_get_handle P_((void));

/* gs_slot.c */
int gs_create_slot P_((GSKey_t *key, GSSlot_t **pslot));
int gs_attach_slot P_((GSKey_t *key, GSSlot_t **pslot));
int gs_detach_slot P_((GSSlot_t *pslot));
int gs_delete_slot P_((GSSlot_t *pslot));

/* gs_home.c */
char *gs_home P_((char *homedir));

/* gs_init.c */
GSHandle_t *gs_initialize P_((char *dest, char *context, int *st));

/* gs_ipc_key.c */
GSKey_t *gs_ipc_key P_((char *file));

/* gs_log.c */
int gs_log P_((char *va_alist,...));

/* gs_logfile.c */
char *gs_logfile P_((char *logfile));

/* gs_log_init.c */
char *gs_log_init P_((char *errfile));

/* gs_inq_ctxt.c */
int gs_inquire_context P_((char *dest, GSContextRequirements_t ***context_req, unsigned int *count));

/* gs_inq_dest.c */
int gs_inquire_destinations P_((char ***dest_list, unsigned int *count));

/* gs_receive.c */
int gs_receive P_((GSHandle_t *h, GSTCB_t **tcb));

/* gs_semlib.c */
int gs_sem_create P_((key_t key, int initval));
int gs_sem_open P_((key_t key));
int gs_sem_rm P_((int id));
int gs_sem_close P_((int id));
int gs_sem_wait P_((int id));
int gs_sem_signal P_((int id));
int gs_sem_op P_((int id, int value));
void gs_sem_dump P_((char *str, int id));

/* gs_send.c */
int gs_send P_((GSHandle_t *h, GSTCB_t *tcb, GSMessage_t *result));

/* gs_sendstr.c */
int gs_sendstr P_((GSHandle_t *h, char *str));

/* gs_subs.c */
int gsz_octal P_((char *str));
int gs_semrm P_((int sid));
int gs_semtran P_((int key));
void gs_semcall P_((int sid, int op));
void gs_semP P_((int sid));
void gs_semV P_((int sid));
int send_data P_((void));
void gs_print_args P_((int argc, char **argv));
GSString_t *gs_mk_string P_((char *string, GSString_t *pstring_t));
GSVector_t *gs_mk_vector P_((char *data, unsigned int dsize, GSVectorType_t vtype, unsigned int len, GSVector_t *pvec));
char *gs_strdup P_((char *cp));
char **gs_tcp_append P_((char ***ptcp, char *cp));
int gs_tcp_len P_((char **tcp));
int gsz_delete_slot P_((GSSlot_t *pslot));
int gsz_detach_slot P_((GSSlot_t *pslot));
int getopt P_((int argc, char **argv, char *opts));

/* gs_talk.c */
int gs_talk P_((GSHandle_t *h, GSSlot_t *pslot, GSMessage_t *msg, GSMessage_t *reply));
int gs_rec_talk P_((GSHandle_t *h, GSSlot_t *pslot, GSMessage_t *msg, GSMessage_t *reply));

/* gs_finder_rec.c */

/* gs_terminate.c */
int gs_terminate P_((GSHandle_t *h));

/* gs_cm.c */
proc *gs_findproc P_((int pid));
void gs_addproc P_((int pid, int type, key_t key, int shmid));
void gs_delproc P_((int pid));

/* gs_test.c */
int start_cm P_((void));
int term_cm P_((void));
int check_cm P_((void));

#undef P_
