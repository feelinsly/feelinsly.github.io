/****************************************************************************
 Module
     EF_FrameworkPrivate.h
 Description
     header file for private definitions in EF_Framework.c
 Notes
     Pupose is to clean up the EF_Framework.c file, since users will edit it
 History
 When           Who     What/Why
 -------------- ---     --------
 10/17/11 12:24 jec      started coding
*****************************************************************************/
#ifndef EF_FrameworkPrivate_H
#define EF_FrameworkPrivate_H

typedef boolean InitFunc_t( void );
typedef boolean PostFunc_t( EF_Event );
typedef boolean QEmptyFunc_t( void );
typedef boolean RunFunc_t( void );
typedef unsigned char StateQueryFunc_t( void );

typedef InitFunc_t * pInitFunc;
typedef PostFunc_t * pPostFunc;
typedef QEmptyFunc_t * pQEmptyFunc;
typedef RunFunc_t * pRunFunc;
typedef StateQueryFunc_t * pStateQueryFunc;

#define NULL_INIT_FUNC ((pInitFunc)0)




typedef struct EF_SMDesc_t {
    InitFunc_t *InitFunc;    // State machine Initialization function
    PostFunc_t *PostFunc;    // State machine post to queue function
    QEmptyFunc_t *QEmptyFunc; // func to test State machine queue
    RunFunc_t *RunFunc;    // State machine Initialization function
}EF_SMDesc;

#endif // EF_FrameworkPrivate_H