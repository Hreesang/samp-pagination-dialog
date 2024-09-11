// -
// Include Guards
// -

#if defined _INC_pagination_dialog
	#endinput
#endif
#define _INC_pagination_dialog


// -
// Exported Packages
// -

#tryinclude <PawnPlus>
#if !defined _PawnPlus_included
	#error You need PawnPlus to use this library. (https://github.com/IS4Code/PawnPlus)
#endif

// -
// Pre-Hooks
// -
// This method was taken from
// https://github.com/oscar-broman/samp-weapon-config/issues/61#issuecomment-268567805
// Thanks to Y_Less.
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

CHAIN_HOOK(PGD)
#undef CHAIN_ORDER
#define CHAIN_ORDER CHAIN_NEXT(PGD)

static stock _PGD_IncludeStates() <_ALS : _ALS_x0, _ALS : _ALS_x1, _ALS : _ALS_x2, _ALS : _ALS_x3> {}
static stock _PGD_IncludeStates() <_ALS : _ALS_go> {}


// -
// Declarations
// -

#if !defined PAGINATION_DIALOG_ID
	#define PAGINATION_DIALOG_ID 1847
#endif

#if !defined DEFAULT_PAGINATION_DIALOG_ITEMS
	#define DEFAULT_PAGINATION_DIALOG_ITEMS -1
#endif

#if !defined MAX_PAGINATION_DIALOG_ITEMS
	#define MAX_PAGINATION_DIALOG_ITEMS DEFAULT_PAGINATION_DIALOG_ITEMS
#endif

#if !defined MAX_PAGINATION_DIALOG_CAPTION
	#define MAX_PAGINATION_DIALOG_CAPTION 64
#endif

#if !defined MAX_PAGINATION_DIALOG_BUTTON
	#define MAX_PAGINATION_DIALOG_BUTTON 64
#endif

enum PAGE_DIALOG_STYLE:E_PAGE_DIALOG_STYLE {
	PAGE_DIALOG_STYLE_LIST = _:DIALOG_STYLE_LIST,
	PAGE_DIALOG_STYLE_TABLIST = _:DIALOG_STYLE_TABLIST,
	PAGE_DIALOG_STYLE_TABLIST_HDRS = _:DIALOG_STYLE_TABLIST_HEADERS,
}

static enum _:E_PAGINATION_DATA {
	List:E_ITEMS,
	E_MAX_PAGES,
	E_CURRENT_PAGE,
	E_MAX_PAGE_ITEMS,
	E_ITEM_START_INDEX,
	E_ITEM_END_INDEX,
	E_CAPTION[MAX_PAGINATION_DIALOG_CAPTION],
	E_BUTTON_1[MAX_PAGINATION_DIALOG_BUTTON],
	E_BUTTON_2[MAX_PAGINATION_DIALOG_BUTTON],
	E_PREV_BUTTON[MAX_PAGINATION_DIALOG_BUTTON],
	E_NEXT_BUTTON[MAX_PAGINATION_DIALOG_BUTTON],
	PAGE_DIALOG_STYLE:E_STYLE,
	E_DIALOG_ID,
	String:E_HEADER,
}

static bool:gs_hasOnPgdDialogRsp = false;
static Pool:gs_playersPaginationPool = INVALID_POOL;


// -
// Static APIs
// -

static stock getMaxPages(List:items, maxItems)
{
	new size = list_size(items);
	new totalPages = size / maxItems;
	new leftover = size % maxItems;

	if (leftover) {
		totalPages++;
	}

	return totalPages;
}

static stock getMaxPageItems(List:items, maxItems) 
{
	const maxDialogInfo = 4096;
	new maxCalculatedItems = 1;
	new totalLen = (MAX_PAGINATION_DIALOG_BUTTON * 2) + 6;

	for (new i = 0, j = list_size(items); i < j; i++) {
		new String:str = list_get_str_s(items, i);
		new strLen = str_len(str) + 3; // '\n' is 3 chars

		if (totalLen + strLen >= maxDialogInfo) {
			break;
		}
		
		totalLen += strLen;
		maxCalculatedItems++;
	}

	if (maxItems == DEFAULT_PAGINATION_DIALOG_ITEMS) {
		return maxCalculatedItems;
	}

	if (maxItems > maxCalculatedItems) {
		return maxCalculatedItems;
	}

	return maxItems;
}

static stock bool:hasNextPage(currentPage, maxPages)
{
	if (currentPage < (maxPages - 1)) {
		return true;
	}

	return false;
}

static stock bool:hasPrevPage(currentPage)
{
	if (currentPage > 0) {
		return true;
	}

	return false;
}

static stock void:executeDialogResponse(playerid, dialogid, bool:response, listitem)
{
	if (gs_hasOnPgdDialogRsp) {
		CallLocalFunction("OnPaginationDialogResponse", "iiii", playerid, dialogid, response, listitem);
	}
}

static stock void:parseDialogCaption(const paginationData[E_PAGINATION_DATA], dest[], len = sizeof dest)
{
	new const pageParse[] = "{page}";
	new const maxPagesParse[] = "{maxPages}";

	dest[0] = EOS;
	strcat(dest, paginationData[E_CAPTION], len);

	new startPageParsePos = strfind(dest, pageParse);
	if (startPageParsePos != -1) {
		strdel(dest, startPageParsePos, (startPageParsePos + sizeof pageParse) - 1);
		
		new string[11];
		valstr(string, paginationData[E_CURRENT_PAGE] + 1);
		strins(dest, string, startPageParsePos, len);
	}

	new startMaxPagesParsePos = strfind(dest, maxPagesParse);
	if (startMaxPagesParsePos != -1) {
		strdel(dest, startMaxPagesParsePos, (startMaxPagesParsePos + sizeof maxPagesParse) - 1);
		
		new string[11];
		valstr(string, paginationData[E_MAX_PAGES]);
		strins(dest, string, startMaxPagesParsePos, len);
	}
}


// -
// APIs
// -

forward OnPaginationDialogResponse(playerid, dialogid, bool:response, listitem);

stock AddPaginationDialogItemStr(List:items, ConstStringTag:str)
{
	if (!list_valid(items)) {
		return -1;
	}

	return list_add_str_s(items, str);
}

stock AddPaginationDialogItem(List:items, const string[])
{
	if (!list_valid(items)) {
		return -1;
	}

	return list_add_str(items, string);
}


stock bool:OpenPaginationDialogPage(playerid, page)
{	
	if (gs_playersPaginationPool == INVALID_POOL || !pool_has(gs_playersPaginationPool, playerid)) {
		return false;
	}

	new paginationData[E_PAGINATION_DATA];
	pool_get_arr(gs_playersPaginationPool, playerid, paginationData);
	new size = list_size(paginationData[E_ITEMS]);

	paginationData[E_ITEM_START_INDEX] = page * paginationData[E_MAX_PAGE_ITEMS];
	paginationData[E_ITEM_END_INDEX] = (paginationData[E_ITEM_START_INDEX] + paginationData[E_MAX_PAGE_ITEMS]) - 1;
	
	new maxSize = size - 1;
	if (paginationData[E_ITEM_END_INDEX] > maxSize) {
		paginationData[E_ITEM_END_INDEX] = maxSize;
	}

	static dialogInfo[4096];
	dialogInfo[0] = EOS;

	for (new i = paginationData[E_ITEM_START_INDEX]; i <= paginationData[E_ITEM_END_INDEX]; i++) {
		new string[256];
		list_get_str(paginationData[E_ITEMS], i, string);

		if (i != 0) {
			strcat(dialogInfo, "\n");
		}

		strcat(dialogInfo, string);
	}

	new bool:hasNext = hasNextPage(page, paginationData[E_MAX_PAGES]);
	new bool:hasPrev = hasPrevPage(page);

	if (hasPrev) {
		new buttonString[MAX_PAGINATION_DIALOG_BUTTON + 3];
		format(buttonString, sizeof buttonString, "%s\n", paginationData[E_PREV_BUTTON]);
		strins(dialogInfo, buttonString, 0);
	}
	
	if (hasNext) {
		new buttonString[MAX_PAGINATION_DIALOG_BUTTON + 3];
		format(buttonString, sizeof buttonString, "\n%s", paginationData[E_NEXT_BUTTON]);
		strcat(dialogInfo, buttonString);
	}

	if (str_valid(paginationData[E_HEADER])) {
		new header[256];
		str_get(paginationData[E_HEADER], header);
		
		strcat(header, "\n");
		strins(dialogInfo, header, 0);
	}
	
	paginationData[E_CURRENT_PAGE] = page;
	pool_set_arr(gs_playersPaginationPool, playerid, paginationData);

	#if defined DIALOG_STYLE
		new DIALOG_STYLE:style = DIALOG_STYLE:paginationData[E_STYLE];
	#else
		new style = _:paginationData[E_STYLE];
	#endif

	new caption[64];
	parseDialogCaption(paginationData, caption);

	ShowPlayerDialog(
		playerid,
		PAGINATION_DIALOG_ID,
		style,
		caption,
		dialogInfo,
		paginationData[E_BUTTON_1],
		paginationData[E_BUTTON_2]
	);
	return true;
}

stock bool:ShowPaginationDialog(playerid, dialogid, PAGE_DIALOG_STYLE:style, const caption[], List:items, const button1[], const button2[] = "", const nextButton[] = ">>>", const prevButton[] = "<<<", page = 0, maxItems = MAX_PAGINATION_DIALOG_ITEMS)
{
	if (!IsPlayerConnected(playerid)) {
		return false;
	}

	if (!list_valid(items) || list_size(items) <= 0) {
		return false;
	}

	if (gs_playersPaginationPool == INVALID_POOL) {
		gs_playersPaginationPool = pool_new();
	} else {
		if (pool_has(gs_playersPaginationPool, playerid)) {
			new paginationData[E_PAGINATION_DATA];
			pool_get_arr(gs_playersPaginationPool, playerid, paginationData);
			list_delete(paginationData[E_ITEMS]);

			if (str_valid(paginationData[E_HEADER])) {
				str_delete(paginationData[E_HEADER]);
			}
		}
	}

	new paginationData[E_PAGINATION_DATA];
	
	if (style == PAGE_DIALOG_STYLE_TABLIST_HDRS) {
		new header[256];
		list_get_str(items, 0, header);
		paginationData[E_HEADER] = str_acquire(str_new(header));
		list_remove(items, 0);
	} else {
		paginationData[E_HEADER] = INVALID_STRING;
	}
	
	paginationData[E_ITEMS] = items;
	paginationData[E_MAX_PAGE_ITEMS] = getMaxPageItems(items, maxItems);
	paginationData[E_MAX_PAGES] = getMaxPages(items, paginationData[E_MAX_PAGE_ITEMS]);
	paginationData[E_CURRENT_PAGE] = page;
	strcat(paginationData[E_CAPTION], caption, MAX_PAGINATION_DIALOG_CAPTION);
	strcat(paginationData[E_BUTTON_1], button1, MAX_PAGINATION_DIALOG_BUTTON);
	strcat(paginationData[E_BUTTON_2], button2, MAX_PAGINATION_DIALOG_BUTTON);
	strcat(paginationData[E_PREV_BUTTON], prevButton, MAX_PAGINATION_DIALOG_BUTTON);
	strcat(paginationData[E_NEXT_BUTTON], nextButton, MAX_PAGINATION_DIALOG_BUTTON);
	paginationData[E_STYLE] = style;
	paginationData[E_DIALOG_ID] = dialogid;

	if (paginationData[E_STYLE] == PAGE_DIALOG_STYLE_TABLIST) {
		new curTabs = 0;
		new pos = -1;
		
		while ((pos = strfind(paginationData[E_PREV_BUTTON], "\t", false, pos)) != -1) {
			curTabs++;
		}

		new String:firstItemStr = list_get_str_s(paginationData[E_ITEMS], 0);
		new firstItemTabs = 0;
		pos = -1;
		new String:findValueStr = str_new_static("\t");

		while ((pos = str_find(firstItemStr, findValueStr, pos)) != -1) {
			firstItemTabs++;
		}

		for (new i = curTabs; i <= firstItemTabs; i++) {
			strcat(paginationData[E_PREV_BUTTON], "\t ", MAX_PAGINATION_DIALOG_BUTTON);
		}
	}

	pool_set_arr(gs_playersPaginationPool, playerid, paginationData);
	return OpenPaginationDialogPage(playerid, page);
}

stock bool:ClosePaginationDialog(playerid)
{
	if (gs_playersPaginationPool == INVALID_POOL || !pool_has(gs_playersPaginationPool, playerid)) {
		return false;
	}

	new paginationData[E_PAGINATION_DATA];
	pool_get_arr(gs_playersPaginationPool, playerid, paginationData);
	list_delete(paginationData[E_ITEMS]);

	if (str_valid(paginationData[E_HEADER])) {
		str_delete(paginationData[E_HEADER]);
	}

	pool_remove(gs_playersPaginationPool, playerid);

	if (!pool_size(gs_playersPaginationPool)) {
		pool_delete(gs_playersPaginationPool);
		gs_playersPaginationPool = INVALID_POOL;
	}

	#if defined HidePlayerDialog
		HidePlayerDialog(playerid);
	#else
		ShowPlayerDialog(playerid, -1, DIALOG_STYLE_LIST, "", "", "", "");
	#endif

	return true;
}


// -
// Internals
// -

public OnGameModeInit()
{
	state _ALS : _ALS_go;
	gs_hasOnPgdDialogRsp = (funcidx("OnPaginationDialogResponse") != -1);	
	return PGD_OnGameModeInit();
}

public OnFilterScriptInit()
{
	state _ALS : _ALS_go;
	gs_hasOnPgdDialogRsp = (funcidx("OnPaginationDialogResponse") != -1);
	return PGD_OnFilterScriptInit();
}

public OnDialogResponse(playerid, dialogid, response, listitem, inputtext[])
{
	if (dialogid != PAGINATION_DIALOG_ID) {
		return PGD_OnDialogResponse(playerid, dialogid, response, listitem, inputtext);
	}

	if (gs_playersPaginationPool == INVALID_POOL || !pool_has(gs_playersPaginationPool, playerid)) {
		return PGD_OnDialogResponse(playerid, dialogid, response, listitem, inputtext);
	}

	new paginationData[E_PAGINATION_DATA];
	pool_get_arr(gs_playersPaginationPool, playerid, paginationData);
	new startIdx = paginationData[E_ITEM_START_INDEX];

	new bool:hasNext = hasNextPage(paginationData[E_CURRENT_PAGE], paginationData[E_MAX_PAGES]);
	new bool:hasPrev = hasPrevPage(paginationData[E_CURRENT_PAGE]);
	new endIdx = paginationData[E_ITEM_END_INDEX];
	new bool:isPageTransition = false;

	if (hasPrev) {
		if (listitem == 0) {
			// Even the response is false, but if the listitem is on transition,
			// OnPaginationDialogResponse won't be called.
			isPageTransition = true;
			if (response) {
				OpenPaginationDialogPage(playerid, paginationData[E_CURRENT_PAGE] - 1);
			}
		}
	}

	if (hasNext) {
		if (listitem == (endIdx - startIdx) + (hasPrev ? 2 : 1)) {
			isPageTransition = true;
			if (response) {
				OpenPaginationDialogPage(playerid, paginationData[E_CURRENT_PAGE] + 1);
			}
		}
	}

	if (isPageTransition) {
		if (!response) {
			new pageDialogID = paginationData[E_DIALOG_ID];
			ClosePaginationDialog(playerid);
			executeDialogResponse(playerid, pageDialogID, false, -1);
		}
		return 1;
	}
	
	new idx = startIdx + (hasPrev ? (listitem - 1) : listitem);
	new pageDialogID = paginationData[E_DIALOG_ID];

	ClosePaginationDialog(playerid);
	executeDialogResponse(playerid, pageDialogID, bool:!!response, idx);
	return 1;
}

public OnPlayerDisconnect(playerid, reason)
{
	ClosePaginationDialog(playerid);
	return PGD_OnPlayerDisconnect(playerid, reason);
}


// -
// Hooks
// -

#if defined _ALS_OnFilterScriptInit
	#undef OnFilterScriptInit
#else
	#define _ALS_OnFilterScriptInit
#endif
#define OnFilterScriptInit(%0) CHAIN_PUBLIC:PGD_OnFilterScriptInit(%0)
CHAIN_FORWARD:PGD_OnFilterScriptInit() = 1;

#if defined _ALS_OnGameModeInit
	#undef OnGameModeInit
#else
	#define _ALS_OnGameModeInit
#endif
#define OnGameModeInit(%0) CHAIN_PUBLIC:PGD_OnGameModeInit(%0)
CHAIN_FORWARD:PGD_OnGameModeInit() = 1;

#if defined _ALS_OnDialogResponse
	#undef OnDialogResponse
#else
	#define _ALS_OnDialogResponse
#endif
#define OnDialogResponse(%0) CHAIN_PUBLIC:PGD_OnDialogResponse(%0)
CHAIN_FORWARD:PGD_OnDialogResponse(playerid, dialogid, response, listitem, inputtext[]) = 0;

#if defined _ALS_OnPlayerDisconnect
	#undef OnPlayerDisconnect
#else
	#define _ALS_OnPlayerDisconnect
#endif
#define OnPlayerDisconnect(%0) CHAIN_PUBLIC:PGD_OnPlayerDisconnect(%0)
CHAIN_FORWARD:PGD_OnPlayerDisconnect(playerid, reason) = 0;

