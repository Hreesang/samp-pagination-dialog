// -
// Include Guards
// -

#if defined _INC_pagination_dialog_async
    #endinput
#endif
#define _INC_pagination_dialog_async


// -
// Exported Packages
// -

#tryinclude <PawnPlus>
#if !defined _PawnPlus_included
	#error You need PawnPlus to use this library. (https://github.com/IS4Code/PawnPlus)
#endif


// -
// Internal Packages
// -

#tryinclude "pagination_dialog"
#if !defined _INC_pagination_dialog
    #tryinclude <pagination_dialog>
#endif

#if !defined _INC_pagination_dialog
    #error You need the base script of pagination_dialog.
#endif


// -
// Pre-Hooks
// -

#if !defined CHAIN_ORDER
	#define CHAIN_ORDER() 0
#endif

#define CHAIN_HOOK(%0) forward @CO_%0();public @CO_%0(){return CHAIN_ORDER()+1;}
#define CHAIN_NEXT(%0) @CO_%0

#define CHAIN_FORWARD:%0_%2(%1)=%3; \
	forward %0_%2(%1); \
	public %0_%2(%1) <_ALS : _ALS_x0, _ALS : _ALS_x1> { return (%3); } \
	public %0_%2(%1) <> { return (%3); }

#define CHAIN_PUBLIC:%0(%1) %0(%1) <_ALS : _ALS_go>

CHAIN_HOOK(PGDA)
#undef CHAIN_ORDER
#define CHAIN_ORDER CHAIN_NEXT(PGDA)

static stock _PGDA_IncludeStates() <_ALS : _ALS_x0, _ALS : _ALS_x1, _ALS : _ALS_x2, _ALS : _ALS_x3> {}
static stock _PGDA_IncludeStates() <_ALS : _ALS_go> {}


// -
// Declarations
// -

#if !defined PAGINATION_DIALOG_PP_ID
    #define PAGINATION_DIALOG_PP_ID 0x502B // "P+" in ascii hex
#endif

enum _:E_ASYNC_PAGE_DIALOG
{
    bool:E_ASYNC_PAGE_DIALOG_RESPONSE,
    E_ASYNC_PAGE_DIALOG_LISTITEM,
}

static stock Pool:gs_playersTaskPool = INVALID_POOL;


// -
// Static APIs
// -

static stock bool:clearPaginationDialogTask(playerid, bool:deleteTask = true)
{
    if (gs_playersTaskPool == INVALID_POOL || !pool_has(gs_playersTaskPool, playerid)) {
        return false;
    }

    new Task:t = Task:pool_get(gs_playersTaskPool, playerid);
    if (deleteTask) {
        task_delete(t);
    }

    pool_remove(gs_playersTaskPool, playerid);
    if (!pool_size(gs_playersTaskPool)) {
        pool_delete(gs_playersTaskPool);
        gs_playersTaskPool = INVALID_POOL;
    }

    return true;
}


// -
// APIs
// -

stock Task:ShowPaginationDialogAsync(playerid, PAGE_DIALOG_STYLE:style, const caption[], List:items, const button1[], const button2[] = "", const nextButton[] = ">>>", const prevButton[] = "<<<", page = 0, maxItems = MAX_PAGINATION_DIALOG_ITEMS)
{
    if (gs_playersTaskPool == INVALID_POOL) {
        gs_playersTaskPool = pool_new();
    } else {
        if (pool_has(gs_playersTaskPool, playerid)) {
            new Task:t = Task:pool_get(gs_playersTaskPool, playerid);
            task_delete(t);
        }
    }

    new Task:t = task_new();
    pool_set(gs_playersTaskPool, playerid, t);

    ShowPaginationDialog(playerid, PAGINATION_DIALOG_PP_ID, style, caption, items, button1, button2, nextButton, prevButton, page, maxItems);
    return t;
}


// -
// Internals
// -

public OnGameModeInit()
{
	state _ALS : _ALS_go;
	return PGDA_OnGameModeInit();
}

public OnFilterScriptInit()
{
	state _ALS : _ALS_go;
	return PGDA_OnFilterScriptInit();
}

public OnPaginationDialogResponse(playerid, dialogid, bool:response, listitem)
{
    if (dialogid != PAGINATION_DIALOG_PP_ID) {
        return PGDA_OnPaginationDialogResponse(playerid, dialogid, response, listitem);
    }

    if (gs_playersTaskPool == INVALID_POOL || !pool_has(gs_playersTaskPool, playerid)) {
        return PGDA_OnPaginationDialogResponse(playerid, dialogid, response, listitem);
    }

    new data[E_ASYNC_PAGE_DIALOG];
    data[E_ASYNC_PAGE_DIALOG_RESPONSE] = response;
    data[E_ASYNC_PAGE_DIALOG_LISTITEM] = listitem;

    new Task:t = Task:pool_get(gs_playersTaskPool, playerid);
    task_set_result_ms_arr(t, data, 0);
    clearPaginationDialogTask(playerid, .deleteTask = false);
    return 1;
}

public OnPlayerDisconnect(playerid, reason)
{
    clearPaginationDialogTask(playerid);
    return PGDA_OnPlayerDisconnect(playerid, reason);
}


// -
// Hooks
// -

#if defined _ALS_OnFilterScriptInit
	#undef OnFilterScriptInit
#else
	#define _ALS_OnFilterScriptInit
#endif
#define OnFilterScriptInit(%0) CHAIN_PUBLIC:PGDA_OnFilterScriptInit(%0)
CHAIN_FORWARD:PGDA_OnFilterScriptInit() = 1;

#if defined _ALS_OnGameModeInit
	#undef OnGameModeInit
#else
	#define _ALS_OnGameModeInit
#endif
#define OnGameModeInit(%0) CHAIN_PUBLIC:PGDA_OnGameModeInit(%0)
CHAIN_FORWARD:PGDA_OnGameModeInit() = 1;

#if defined _ALS_OnPaginationDialogResponse
    #undef OnPaginationDialogResponse
#else
    #define _ALS_OnPaginationDialogResponse
#endif
#define OnPaginationDialogResponse(%0) CHAIN_PUBLIC:PGDA_OnPaginationDialogResponse(%0)
CHAIN_FORWARD:PGDA_OnPaginationDialogResponse(playerid, dialogid, bool:response, listitem) = 1;

#if defined _ALS_OnPlayerDisconnect
	#undef OnPlayerDisconnect
#else
	#define _ALS_OnPlayerDisconnect
#endif
#define OnPlayerDisconnect(%0) CHAIN_PUBLIC:PGDA_OnPlayerDisconnect(%0)
CHAIN_FORWARD:PGDA_OnPlayerDisconnect(playerid, reason) = 0;
