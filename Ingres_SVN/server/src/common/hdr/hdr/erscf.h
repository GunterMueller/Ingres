#define _SC_CLASS 9
#define E_SC0000_OK                    0x00090000
#define E_SC0001_OP_CODE               0x00090001
#define E_SC0002_BAD_SEMAPHORE         0x00090002
#define E_SC0003_INVALID_SEMAPHORE     0x00090003
#define E_SC0004_NO_MORE_MEMORY        0x00090004
#define E_SC0005_LESS_THAN_REQUESTED   0x00090005
#define E_SC0006_NOT_ALLOCATED         0x00090006
#define E_SC0007_INFORMATION           0x00090007
#define E_SC0008_INFO_TRUNCATED        0x00090008
#define E_SC0009_BUFFER                0x00090009
#define E_SC000A_NO_BUFFER             0x0009000a
#define E_SC000B_NO_REQUEST            0x0009000b
#define E_SC000C_BUFFER_LENGTH         0x0009000c
#define E_SC000D_BUFFER_ADDR           0x0009000d
#define E_SC000E_AFCN_ADDR             0x0009000e
#define E_SC000F_IN_USE                0x0009000f
#define E_SC0010_NO_PERM               0x00090010
#define E_SC0011_NESTED_DISABLE        0x00090011
#define E_SC0012_NOT_SET               0x00090012
#define E_SC0013_TIMEOUT               0x00090013
#define E_SC0014_INTERRUPTED           0x00090014
#define E_SC0015_BAD_PAGE_COUNT        0x00090015
#define E_SC0016_BAD_PAGE_ADDRESS      0x00090016
#define E_SC0017_BAD_SESSION           0x00090017
#define E_SC0018_BAD_FACILITY          0x00090018
#define E_SC0019_BAD_FUNCTION_CODE     0x00090019
#define E_SC0020_WRONG_PAGE_COUNT      0x00090020
#define E_SC0021_BAD_ADDRESS           0x00090021
#define E_SC0022_BAD_REQUEST_BLOCK     0x00090022
#define E_SC0023_INTERNAL_MEMORY_ERROR 0x00090023
#define E_SC0024_INTERNAL_ERROR        0x00090024
#define E_SC0025_NOT_YET_AVAILABLE     0x00090025
#define E_SC0026_SCB_ALLOCATE          0x00090026
#define E_SC0027_BUFFER_LENGTH         0x00090027
#define E_SC0028_SEMAPHORE_DUPLICATE   0x00090028
#define E_SC0029_HOLDING_SEMAPHORE     0x00090029
#define E_SC002B_EVENT_REGISTERED      0x0009002b
#define E_SC002C_EVENT_NOT_REGISTERED  0x0009002c
#define E_SC002D_SET_SESS_AUTH_IN_XACT 0x0009002d
#define E_SC002E_SETROLE_IN_XACT       0x0009002e
#define E_SC002F_SETGROUP_IN_XACT      0x0009002f
#define E_SC0030_CHANGESECAUDIT_IN_XAC 0x00090030
#define E_SC0031_SET_SESS_AUTH_ID_INVA 0x00090031
#define E_SC0100_MULTIPLE_MEM_INIT     0x00090100
#define E_SC0101_NO_MEM_INIT           0x00090101
#define E_SC0102_MEM_CORRUPT           0x00090102
#define E_SC0103_MEM_PROTECTION        0x00090103
#define E_SC0104_CORRUPT_POOL          0x00090104
#define E_SC0105_CORRUPT_FREE          0x00090105
#define E_SC0106_BAD_SIZE_REDUCE       0x00090106
#define E_SC0107_BAD_SIZE_EXPAND       0x00090107
#define E_SC0108_UNEXPECTED_EXCEPTION  0x00090108
#define E_SC0109_MULTI_DB_ADD          0x00090109
#define E_SC010A_RETRY                 0x0009010a
#define E_SC010B_SCB_DBCB_LINK         0x0009010b
#define E_SC010C_DB_ADD                0x0009010c
#define E_SC010D_DB_LOCATION           0x0009010d
#define E_SC010E_DB_DELETE             0x0009010e
#define E_SC010F_INCOMPLETE_DB_DELETE  0x0009010f
#define E_SC0110_ALLOC_TOO_BIG         0x00090110
#define E_SC0120_DBLIST_ERROR          0x00090120
#define E_SC0121_DB_OPEN               0x00090121
#define E_SC0122_DB_CLOSE              0x00090122
#define E_SC0123_SESSION_INITIATE      0x00090123
#define E_SC0124_SERVER_INITIATE       0x00090124
#define E_SC0125_POOL_ADDITION         0x00090125
#define E_SC0126_SC0M_CHECK            0x00090126
#define E_SC0127_SERVER_TERMINATE      0x00090127
#define E_SC0128_SERVER_DOWN           0x00090128
#define E_SC0129_SERVER_UP             0x00090129
#define E_SC012A_BAD_MESSAGE_FOUND     0x0009012a
#define E_SC012B_SESSION_TERMINATE     0x0009012b
#define E_SC012C_ADF_ERROR             0x0009012c
#define E_SC012D_DB_NOT_FOUND          0x0009012d
#define E_SC012E_QEP_INVALID           0x0009012e
#define E_SC012F_RETRY                 0x0009012f
#define E_SC0130_LO_DBP_MISMATCH       0x00090130
#define E_SC0131_LO_STAR_MISMATCH      0x00090131
#define E_SC0132_LO_REPEAT_MISMATCH    0x00090132
#define E_SC0200_SEM_INIT              0x00090200
#define E_SC0201_SEM_WAIT              0x00090201
#define E_SC0202_SEM_RELEASE           0x00090202
#define E_SC0203_SEM_FREE              0x00090203
#define E_SC0204_MEMORY_ALLOC          0x00090204
#define E_SC0205_BAD_SCS_OPERATION     0x00090205
#define E_SC0206_CANNOT_PROCESS        0x00090206
#define E_SC0207_UNEXPECTED_ERROR      0x00090207
#define E_SC0208_BAD_PSF_HANDLE        0x00090208
#define E_SC0209_PSF_REQ_DISTRIBUTED   0x00090209
#define E_SC020A_UNRECOGNIZED_QMODE    0x0009020a
#define E_SC020B_UNRECOGNIZED_TEXT     0x0009020b
#define E_SC020C_INPUT_OUT_OF_SEQUENCE 0x0009020c
#define E_SC020D_NO_DISTRIBUTED        0x0009020d
#define E_SC020E_UNUSED_MSG            0x0009020e
#define E_SC020F_LOST_LOCK             0x0009020f
#define E_SC0210_SCS_SQNCR_ERROR       0x00090210
#define E_SC0211_CLF_ERROR             0x00090211
#define E_SC0212_ADF_ERROR             0x00090212
#define E_SC0213_DMF_ERROR             0x00090213
#define E_SC0214_OPF_ERROR             0x00090214
#define E_SC0215_PSF_ERROR             0x00090215
#define E_SC0216_QEF_ERROR             0x00090216
#define E_SC0217_QSF_ERROR             0x00090217
#define E_SC0218_RDF_ERROR             0x00090218
#define E_SC0219_SCF_ERROR             0x00090219
#define E_SC021A_ULF_ERROR             0x0009021a
#define E_SC021B_BAD_COPY_STATE        0x0009021b
#define E_SC021C_SCS_INPUT_ERROR       0x0009021c
#define E_SC021D_TOO_MANY_CURSORS      0x0009021d
#define E_SC021E_CURSOR_NOT_FOUND      0x0009021e
#define E_SC021F_BAD_SQNCR_CALL        0x0009021f
#define E_SC0220_SESSION_ERROR_MAX     0x00090220
#define E_SC0221_SERVER_ERROR_MAX      0x00090221
#define E_SC0222_BAD_SCDNOTE_STATUS    0x00090222
#define E_SC0223_BAD_SCDNOTE_FACILITY  0x00090223
#define E_SC0224_MEMORY_FREE           0x00090224
#define E_SC0225_BAD_GCA_INFO          0x00090225
#define E_SC0226_BAD_GCA_TYPE          0x00090226
#define E_SC0227_MEM_NOT_FREE          0x00090227
#define E_SC0228_INVALID_TRACE         0x00090228
#define E_SC0229_INVALID_ATTN_CALL     0x00090229
#define E_SC022A_SERVER_LOAD           0x0009022a
#define E_SC022B_PROTOCOL_VIOLATION    0x0009022b
#define E_SC022C_PREMATURE_MSG_END     0x0009022c
#define E_SC022D_BLOCK_NOT_MD_ASSOC    0x0009022d
#define E_SC022E_WRONG_BLOCK_TYPE      0x0009022e
#define E_SC022F_BAD_GCA_READ          0x0009022f
#define E_SC0230_INVALID_CHG_PROT_LEN  0x00090230
#define E_SC0231_INVALID_CHG_PROT_CODE 0x00090231
#define E_SC0232_INVALID_STARTUP_CODE  0x00090232
#define E_SC0233_SESSION_START         0x00090233
#define E_SC0234_SESSION_END           0x00090234
#define E_SC0235_AVERAGE_ROWS          0x00090235
#define E_SC0236_FAST_COMMIT_INVALID   0x00090236
#define E_SC0237_FAST_COMMIT_ADD       0x00090237
#define E_SC0238_FAST_COMMIT_EXIT      0x00090238
#define E_SC0239_DBMS_TASK_ERROR       0x00090239
#define E_SC023A_GCA_LISTEN_FAIL       0x0009023a
#define E_SC023B_INCONSISTENT_MSG_QUEU 0x0009023b
#define E_SC023C_SXF_ERROR             0x0009023c
#define E_SC023D_SXF_BAD_FLUSH         0x0009023d
#define E_SC023E_SXF_BAD_WRITE         0x0009023e
#define E_SC023F_QRYTEXT_WRITE         0x0009023f
#define E_SC0240_DBMS_TASK_INITIATE    0x00090240
#define E_SC0241_VITAL_TASK_FAILURE    0x00090241
#define E_SC0242_ALTER_MAX_SESSIONS    0x00090242
#define E_SC0243_WRITE_BEHIND_ADD      0x00090243
#define E_SC0244_WRITE_BEHIND_EXIT     0x00090244
#define E_SC0245_CANNOT_REGISTER       0x00090245
#define E_SC0246_SCA_SHOW_ERROR        0x00090246
#define E_SC0247_SCA_OPEN_ERROR        0x00090247
#define E_SC0248_SCA_POSITION_ERROR    0x00090248
#define E_SC0249_SCA_READ_ERROR        0x00090249
#define E_SC024A_SCA_CLOSE_ERROR       0x0009024a
#define E_SC024B_SCA_ALLOCATE          0x0009024b
#define E_SC024C_SCA_DEALLOCATE        0x0009024c
#define E_SC024D_SCA_ADDING            0x0009024d
#define E_SC024E_SCA_NOT_ADDED         0x0009024e
#define E_SC024F_SCA_IMP_ARGUMENTS     0x0009024f
#define E_SC0250_COPY_OUT_OF_SEQUENCE  0x00090250
#define E_SC0251_COPY_CLOSE_ERROR      0x00090251
#define E_SC0252_COPY_SYNC_ERROR       0x00090252
#define E_SC0253_BAD_COPY_ERRMSG       0x00090253
#define E_SC0254_SERVER_INIT_ADD       0x00090254
#define E_SC0255_INCOMPLETE_SEND       0x00090255
#define E_SC0256_SORT_THREAD_EXIT      0x00090256
#define E_SC0260_XENCODE_ERROR         0x00090260
#define E_SC0261_XENCODE_BAD_PARM      0x00090261
#define E_SC0262_XENCODE_BAD_RESULT    0x00090262
#define E_SC0263_SCA_RISK_CONSISTENCY  0x00090263
#define E_SC0264_SCA_ILLEGAL_MAJOR     0x00090264
#define E_SC0265_SCA_STATE             0x00090265
#define E_SC0266_USER_SHUTDOWN         0x00090266
#define E_SC0267_SCA_ID_UNKNOWN        0x00090267
#define E_SC0268_SCA_RISK_INVOKED      0x00090268
#define E_SC0269_SCA_INCOMPATIBLE      0x00090269
#define E_SC026A_SCA_REGISTER_ERROR    0x0009026a
#define E_SC026B_INVALID_CURSOR_MODE   0x0009026b
#define E_SC0270_NO_EVENT_MESSAGE      0x00090270
#define E_SC0271_EVENT_THREAD          0x00090271
#define E_SC0272_EVENT_THREAD_ADD      0x00090272
#define E_SC0273_EVENT_RETRY           0x00090273
#define E_SC0274_NOTIFY_EVENT          0x00090274
#define E_SC0275_IGNORE_ASTATE         0x00090275
#define E_SC0276_CX_RAISE_FAIL         0x00090276
#define E_SC0280_NO_ALERT_INIT         0x00090280
#define E_SC0281_ALERT_ALLOCATE_FAIL   0x00090281
#define E_SC0282_RSES_ALLOCATE_FAIL    0x00090282
#define E_SC0283_AINST_ALLOCATE_FAIL   0x00090283
#define E_SC0284_BUCKET_INIT_ERROR     0x00090284
#define E_SC0285_EVENT_MEMORY_ALLOC    0x00090285
#define E_SC0286_EVENT_SEM_INIT        0x00090286
#define E_SC0287_AINST_SESSION_ID      0x00090287
#define E_SC0288_XSEV_SM_DESTROY       0x00090288
#define E_SC0289_XSEV_ALLOC            0x00090289
#define E_SC028A_XSEV_CRLOCKLIST       0x0009028a
#define E_SC028B_XSEV_ATTACH           0x0009028b
#define E_SC028C_XSEV_EALLOCATE        0x0009028c
#define E_SC028D_XSEV_REGISTERED       0x0009028d
#define E_SC028E_XSEV_NOT_REGISTERED   0x0009028e
#define E_SC0290_XSEV_SIGNAL           0x00090290
#define E_SC0291_XSEV_NOT_CONNECT      0x00090291
#define E_SC0292_XSEV_SEM_INIT         0x00090292
#define E_SC0293_XSEV_VERSION          0x00090293
#define E_SC0294_XSEV_CONFIG_LOC       0x00090294
#define E_SC0295_XSEV_CONFIG_OPEN      0x00090295
#define E_SC0296_XSEV_CONFIG_READ      0x00090296
#define E_SC0297_XSEV_CONFIG_VALUE     0x00090297
#define E_SC0298_EVENT_CONFIG          0x00090298
#define E_SC0299_EVENTS_DISABLED       0x00090299
#define E_SC029A_EVENT_CLUSTER_INIT    0x0009029a
#define E_SC029B_XSEV_REQLOCK          0x0009029b
#define E_SC029C_XSEV_RLSLOCK          0x0009029c
#define E_SC029D_XSEV_PREVERR          0x0009029d
#define E_SC029E_XSEV_TSTLOCK          0x0009029e
#define E_SC029F_XSEV_EVCONNECT        0x0009029f
#define E_SC02C0_CSP_MAIN_ADD          0x000902c0
#define E_SC02C1_CSP_MAIN_EXIT         0x000902c1
#define E_SC02C2_CSP_MSGMON_ADD        0x000902c2
#define E_SC02C3_CSP_MSGMON_EXIT       0x000902c3
#define E_SC02C4_CSP_MSGTHR_ADD        0x000902c4
#define E_SC02C5_CSP_MSGTHR_EXIT       0x000902c5
#define E_SC02C6_BAD_RAD_PARAM         0x000902c6
#define E_SC0300_EXDB_LOCK_REQ         0x00090300
#define E_SC0301_LOC_EXISTS            0x00090301
#define E_SC0302_AUDIT_INIT            0x00090302
#define E_SC0303_SCA_CREATE            0x00090303
#define E_SC0304_SCA_MODIFY            0x00090304
#define E_SC0305_SCA_QTXT_CREATE       0x00090305
#define E_SC0306_SCA_DBP_CREATE        0x00090306
#define E_SC0307_SCA_CONVERTING        0x00090307
#define E_SC0308_SCA_CONVERTED         0x00090308
#define E_SC0309_SCA_NOT_CONVERTED     0x00090309
#define E_SC030A_SCA_ALTER             0x0009030a
#define E_SC030B_SCA_BEGIN_XACT        0x0009030b
#define E_SC030C_SCA_COMMIT            0x0009030c
#define E_SC030D_SCA_ABORT             0x0009030d
#define E_SC030E_SCA_ADD_DB            0x0009030e
#define E_SC030F_SCA_OPEN_DB           0x0009030f
#define E_SC0310_SCA_CLOSE_DB          0x00090310
#define E_SC0311_SCA_DELETE_DB         0x00090311
#define E_SC0312_SCA_NO_LICENSE        0x00090312
#define E_SC0313_NOT_LICENSED          0x00090313
#define E_SC0314_RECOVER_THREAD_ADD    0x00090314
#define E_SC0315_RECOVERING_ORPHAN_DX  0x00090315
#define E_SC0316_LOC_NOT_FOUND         0x00090316
#define E_SC0317_LOC_NOT_AUTHORIZED    0x00090317
#define E_SC0318_SCA_USER_TRACE        0x00090318
#define E_SC0319_SERVER_CRASH_TEST     0x00090319
#define E_SC031A_LO_PARAM_ERROR        0x0009031a
#define E_SC031B_SETLG_XCT_INPROG      0x0009031b
#define E_SC031C_ONERR_XCT_INPROG      0x0009031c
#define E_SC031D_RECOVERY_ADD          0x0009031d
#define E_SC031E_RECOVERY_EXIT         0x0009031e
#define E_SC031F_LOGWRITER_ADD         0x0009031f
#define E_SC0320_LOGWRITER_EXIT        0x00090320
#define E_SC0321_CHECK_DEAD_ADD        0x00090321
#define E_SC0322_CHECK_DEAD_EXIT       0x00090322
#define E_SC0323_GROUP_COMMIT_ADD      0x00090323
#define E_SC0324_GROUP_COMMIT_EXIT     0x00090324
#define E_SC0325_FORCE_ABORT_ADD       0x00090325
#define E_SC0326_FORCE_ABORT_EXIT      0x00090326
#define E_SC0327_RECOVERY_SERVER_NOCNC 0x00090327
#define W_SC0328_REMOVE_SESSION        0x00090328
#define E_SC0329_AUDIT_THREAD_ADD      0x00090329
#define E_SC032A_AUDIT_THREAD_EXIT     0x0009032a
#define E_SC032B_BAD_PM_FILE           0x0009032b
#define E_SC032C_NO_PM_FILE            0x0009032c
#define E_SC032D_C2_AUDITING_ON        0x0009032d
#define E_SC032E_C2_NEEDS_KME          0x0009032e
#define E_SC032F_CP_TIMER_ADD          0x0009032f
#define E_SC0330_CP_TIMER_EXIT         0x00090330
#define E_SC0331_C2_INVALID_MODE       0x00090331
#define E_SC0332_B1_NEEDS_C2           0x00090332
#define E_SC0333_B1_NO_SECLABELS       0x00090333
#define E_SC0334_BAD_SECURE_LEVEL      0x00090334
#define E_SC0335_NO_PROC_SEC_LABEL     0x00090335
#define E_SC0336_BAD_SESS_SEC_LABEL    0x00090336
#define E_SC0337_SEC_LABEL_TOO_LOW     0x00090337
#define E_SC0338_SEC_LABEL_TOO_HIGH    0x00090338
#define E_SC0339_SECURE_INVALID_PARM   0x00090339
#define E_SC033A_DB_SEC_LABEL          0x0009033a
#define E_SC033B_SET_LABEL_INVLD       0x0009033b
#define E_SC033C_SET_SESSION_ERR       0x0009033c
#define E_SC033D_TZNAME_INVALID        0x0009033d
#define E_SC033E_SET_PRIORITY_ERR      0x0009033e
#define E_SC033F_INVLD_PRIORITY        0x0009033f
#define E_SC0340_TERM_THREAD_ADD       0x00090340
#define E_SC0341_TERM_THREAD_EXIT      0x00090341
#define E_SC0342_IDLE_LIMIT_INVALID    0x00090342
#define E_SC0343_CONNECT_LIMIT_INVALID 0x00090343
#define I_SC0344_SESSION_DISCONNECT    0x00090344
#define E_SC0345_SERVER_CLOSED         0x00090345
#define E_SC0346_DBMS_PARAMETER_ERROR  0x00090346
#define W_SC0347_CRASH_SESSION         0x00090347
#define W_SC0348_CRASH_SERVER          0x00090348
#define E_SC0349_GET_DB_ALARMS         0x00090349
#define E_SC034A_FIRE_DB_ALARMS        0x0009034a
#define E_SC034B_ALARM_QRY_IIEVENT     0x0009034b
#define E_SC034C_GET_SESSION_ROLE      0x0009034c
#define E_SC034D_ROLE_NOT_AUTHORIZED   0x0009034d
#define E_SC034E_PRIV_PROPAGATE_ERROR  0x0009034e
#define E_SC034F_NO_FLAG_PRIV          0x0009034f
#define E_SC0350_B1_NOT_LICENCED       0x00090350
#define E_SC0351_IIROLE_GET_ERROR      0x00090351
#define E_SC0352_IIROLE_ERROR          0x00090352
#define E_SC0353_SESSION_ROLE_ERR      0x00090353
#define E_SC0354_EXTPWD_GCA_COMPLETION 0x00090354
#define E_SC0355_EXTPWD_GCA_NOPEER     0x00090355
#define E_SC0356_EXTPWD_GCA_CSSUSPEND  0x00090356
#define E_SC0357_EXTPWD_GCA_FASTSELECT 0x00090357
#define E_SC0358_EXTPWD_GCA_FORMAT     0x00090358
#define E_SC0359_EXTPWD_GCM_ERROR      0x00090359
#define E_SC035A_EXTPWD_NO_AUTH_MECH   0x0009035a
#define E_SC035B_ROLE_CHECK_ERROR      0x0009035b
#define E_SC035C_USER_CHECK_EXT_PWD    0x0009035c
#define E_SC035D_SET_ROLE_IN_XACT      0x0009035d
#define E_SC035E_LOAD_ROLE_DBPRIV      0x0009035e
#define E_SC035F_SET_ROLE_NODBACCESS   0x0009035f
#define E_SC0360_SEC_WRITER_THREAD_ADD 0x00090360
#define E_SC0361_SEC_WRITER_THREAD_EXI 0x00090361
#define E_SC0362_SECLABEL_PARAM_SLTYPE 0x00090362
#define E_SC0363_DBP_PARAM_INCONSISTEN 0x00090363
#define E_SC0364_READ_AHEAD_EXIT       0x00090364
#define E_SC0365_IOMASTER_OPENDB       0x00090365
#define E_SC0366_WRITE_ALONG_EXIT      0x00090366
#define E_SC0367_WRITE_ALONG_ADD       0x00090367
#define E_SC0368_READ_AHEAD_ADD        0x00090368
#define E_SC0369_IOMASTER_BADCPU       0x00090369
#define E_SC0370_IOMASTER_SERVER_NOCNC 0x00090370
#define E_SC0371_NO_TX_FLAG            0x00090371
#define E_SC0372_NO_MESSAGE_AREA       0x00090372
#define E_SC0373_PSF_RESOLVE_ERROR     0x00090373
#define W_SC0374_NO_ACCESS_TO_TABLE    0x00090374
#define W_SC0375_INVALID_LOCK_MODE     0x00090375
#define E_SC0376_INVALID_GET_FLAG      0x00090376
#define E_SC0377_NULL_RECORD_ID        0x00090377
#define E_SC0378_NO_KEY_VALUES         0x00090378
#define E_SC0379_INVALID_KEYSIZE       0x00090379
#define E_SC037A_INVALID_OPCODE        0x0009037a
#define E_SC037B_BAD_DATE_CUTOFF       0x0009037b
#define E_SC037C_REP_QMAN_EXIT         0x0009037c
#define E_SC037D_REP_QMAN_ADD          0x0009037d
#define E_SC037E_LK_CALLBACK_THREAD_AD 0x0009037e
#define E_SC037F_LK_CALLBACK_THREAD_EX 0x0009037f
#define E_SC0380_SET_LOCKMODE_ROW      0x00090380
#define E_SC0381_SETLG_READONLY_DB     0x00090381
#define E_SC0382_LICENSE_THREAD_ADD    0x00090382
#define E_SC0383_LICENSE_THREAD_EXIT   0x00090383
#define E_SC0384_CLEANUP_THREAD_ADD    0x00090384
#define E_SC0385_CLEANUP_THREAD_EXIT   0x00090385
#define E_SC0386_LOGTRC_THREAD_ADD     0x00090386
#define E_SC0387_LOGTRC_THREAD_EXIT    0x00090387
#define E_SC0388_NO_LICENSE            0x00090388
#define E_SC0389_LOCK_CREATE           0x00090389
#define E_SC0390_BAD_LOCK_REQUEST      0x00090390
#define E_SC0391_BAD_LOCK_RELEASE      0x00090391
#define E_SC0392_CUT_INIT              0x00090392
#define E_SC0393_RAAT_NOT_SUPPORTED    0x00090393
#define E_SC0394_CLUSTER_NOT_SUPPORTED 0x00090394
#define E_SC0395_RDF_DDB_FAILURE       0x00090395
#define E_SC0396_DIST_DEADLOCK_THR_ADD 0x00090396
#define E_SC0397_DIST_DEADLOCK_THR_EXI 0x00090397
#define E_SC0398_GLC_SUPPORT_THR_ADD   0x00090398
#define E_SC0399_GLC_SUPPORT_THR_EXIT  0x00090399
#define E_SC039A_SET_SESSION_READWRITE 0x0009039a
#define E_SC039B_VAR_LEN_INCORRECT     0x0009039b
#define E_SC0500_INVALID_LANGCODE      0x00090500
#define E_SC0501_INTERNAL_ERROR        0x00090501
#define E_SC0502_GCA_ERROR             0x00090502
#define E_SC0503_MKSYSCF_THREAD_ADD    0x00090503
#define E_SC0504_CLEANUP_THREAD_ADD    0x00090504
#define E_SC0505_CLEANUP_THREAD_INIT   0x00090505
#define E_SC0506_COMPILE_THREAD_ADD    0x00090506
#define E_SC0507_STALONE_UTIL_START    0x00090507
#define E_SC0508_LICENSE_OBTAIN_FAIL   0x00090508
#define E_SC0509_LICENSE_RELEASE_FAIL  0x00090509
#define E_SC050A_LICENSE_VERIFY_FAIL   0x0009050a
#define E_SC050C_PROBELOG_OPEN_FAIL    0x0009050c
#define E_SC050D_INVALID_PROBENUM      0x0009050d
#define E_SC050E_PROBES_INTERNAL_ERROR 0x0009050e
#define E_SC050F_PROBES_MISPLACED_END  0x0009050f
#define E_SC0510_PROBES_STARTUP        0x00090510
#define E_SC0511_PROBES_SHUTDOWN       0x00090511
#define E_SC0514_BAD_SYSTEM_STORE      0x00090514
#define E_SC0515_NO_SYSTEM_STORE       0x00090515
#define E_SC0516_BAD_SERVER_TYPE       0x00090516
#define E_SC0517_SYSTEM_STORE_EXISTS   0x00090517
#define E_SC0519_CONN_LIMIT_EXCEEDED   0x00090519
#define E_SC051A_SERVER_IS_RUNNING     0x0009051a
#define E_SC051B_LOGTRC_THREAD_ADD     0x0009051b
#define E_SC051C_LOGTRC_THREAD_INIT    0x0009051c
#define E_SC051D_LOAD_CONFIG           0x0009051d
#define I_SC051E_IIMONITOR_CMD         0x0009051e
